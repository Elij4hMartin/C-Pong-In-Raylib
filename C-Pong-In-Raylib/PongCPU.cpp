#include "raylib.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

struct Ball
{
    float x, y;
    float speedX, speedY;
    float initSpeed;
    float radius;
    
    Sound ping = LoadSound("./ping.wav");  // Load WAV audio file
    Sound drop = LoadSound("./ballDrop.wav");  // Load WAV audio file
    
    void Hide()
    {
        x = -100;
        y = GetScreenHeight()/6;
    }
    
    void Draw()
    {
        DrawCircle((int) x, (int) y, radius, WHITE);
    }
    
    void Play()
    {
        PlaySound(ping);      // Play WAV sound
        SetSoundPitch(ping, 1.0f);
    }
    
    void Reset()
    {
        PlaySound(drop);      // Play WAV sound
        
        //Randomize direction
        x = GetScreenWidth() / 2.0f;
        y = GetScreenHeight() / 2.0f; 
        int randomNumber1 = 1 + rand()%2;
        int randomNumber2 = 1 + rand()%2;
        if (randomNumber1 == 1)
        {
            speedX = initSpeed;
        } else {
            speedX = -initSpeed;
        }
        if (randomNumber2 == 1)
        {
            speedY = initSpeed;
        } else {
            speedY = -initSpeed;
        }
    }
};

struct Paddle
{
    float x, y;
    float speed;
    float width, height;
    
    Rectangle GetRect()
    {
        return Rectangle{(int) x - width/2, (int) y - height/2, width, height};
    }
    
    void Draw()
    {
        DrawRectangleRec(GetRect(), WHITE);
    }
};

struct ScoreBoard
{
    int x, y;
    int score = 0;
    int size;
    
    void Draw()
    {
        DrawText(TextFormat("%d", score), x, y, size, GRAY);
    }
};


int main () 
    {
    
    const int WindowWidth = 1280;
    const int WindowHieght = 720;
    
    InitWindow(WindowWidth, WindowHieght, "PONG");
    SetTargetFPS(60);
    
    InitAudioDevice();      // Initialize audio device
    Sound rub = LoadSound("./ballRub.wav");  // Load WAV audio file
    
    //Camera
    Camera2D camera = { 0 };
    camera.target = Vector2{0, 0};
    camera.offset = Vector2{0, 0};
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    
    //Screen shake
    float intensity = 0.0f;
    float multiplier = 2.0f;
    float playTime = 2.0f*PI/16;
    float speed = 4.0f;
    float screenShake = false;
    float timer = 0;
    
    //Reset
    float resetTimer = 0.0f;
    float resetTime = 0.75f;
    bool boardSet = false;
    
    float range = 0;
    float newRange = 0;
    
    //Scoreboard
    ScoreBoard leftBoard;
    leftBoard.size = 200;
    leftBoard.x = GetScreenWidth()/4;
    leftBoard.y = GetScreenHeight()*2/6;
    
    ScoreBoard rightBoard;
    rightBoard.size = 200;
    rightBoard.x = GetScreenWidth()*3/4 - rightBoard.size/2;
    rightBoard.y = GetScreenHeight()*2/6;
    
    //Ball
    Ball ball;
    ball.radius = GetScreenHeight()/50;
    ball.x = GetScreenWidth() / 2.0f;
    ball.y = GetScreenHeight() / 2.0f; 
    ball.initSpeed = 370;
    
    //Paddles
    Paddle leftPaddle;
    leftPaddle.x = 50;
    leftPaddle.y = GetScreenHeight() / 2;
    leftPaddle.width = GetScreenHeight()/30;
    leftPaddle.height = GetScreenHeight()/8;
    leftPaddle.speed = 700;
    
    Paddle rightPaddle;
    rightPaddle.x = GetScreenWidth() - 50;
    rightPaddle.y = GetScreenHeight() / 2;
    rightPaddle.width = GetScreenHeight()/30;
    rightPaddle.height = GetScreenHeight()/8;
    rightPaddle.speed = 700;
    
    PlaySound(rub);
    
    while (!WindowShouldClose()) 
    {
        ball.x += ball.speedX * GetFrameTime();
        ball.y += ball.speedY * GetFrameTime();
        
        intensity = sqrt(pow(ball.speedX,2) + pow(ball.speedY, 2))/ball.initSpeed*multiplier;
        
        if ((timer < playTime) && screenShake)
        {
            timer += 1.0f * GetFrameTime();
            camera.target = Vector2{intensity*sin(1/(2*timer*speed))*cos(4*timer*speed)*sin(2*timer*speed),
            intensity*sin(1/(2*timer*speed))*cos(4*timer*speed)*sin(2*timer*speed)};
        } else {
            timer = 0;
            screenShake = false;
        }
        
        //Check for screen collisions
        if (ball.y + ball.radius >= GetScreenHeight())
        {
            ball.speedY *= -1;
            ball.y = GetScreenHeight() - ball.radius;
            screenShake = true;
            timer = 0;
            ball.Play();      // Play WAV sound
        }
        if (ball.y - ball.radius <= 0)
        {
            ball.speedY *= -1;
            ball.y = 0 + ball.radius;
            screenShake = true;
            timer = 0;
            ball.Play();      // Play WAV sound
        }
        
        //Check for scores
        if ((ball.x < 0) && (leftBoard.score < 10) && (rightBoard.score < 10) && (boardSet))
        {
            rightBoard.score++;
            if ((rightBoard.score < 10) && (leftBoard.score < 10))
            {
                ball.Reset();
            }
        }
        if ((ball.x > GetScreenWidth()) && (leftBoard.score < 10) && (rightBoard.score < 10) && (boardSet))
        {
            leftBoard.score++;
            if ((rightBoard.score < 10) && (leftBoard.score < 10))
            {
                ball.Reset();
            }
        }

        
        if (IsKeyDown(KEY_W))
        {
            leftPaddle.y -= leftPaddle.speed * GetFrameTime();
            if (leftPaddle.y - leftPaddle.height/2 <= 0)
            {
                leftPaddle.y = 0 + leftPaddle.height/2;
            } 
        }
        if (IsKeyDown(KEY_S))
        {
            leftPaddle.y += leftPaddle.speed * GetFrameTime();
            if (leftPaddle.y + leftPaddle.height/2 >= GetScreenHeight())
            {
                leftPaddle.y = GetScreenHeight() - leftPaddle.height/2;
            } 
        }
        if (rightPaddle.y > ball.y)
        {
            range = rightPaddle.y - ball.y;
            newRange = (((range) * (9.5f)) / (GetScreenHeight()));
            rightPaddle.y -= rightPaddle.speed * GetFrameTime() * newRange; 
            if (rightPaddle.y - rightPaddle.height/2 <= 0)
            {
                rightPaddle.y = 0 + rightPaddle.height/2;
            }
        }
        if (rightPaddle.y < ball.y)
        {
            range = ball.y - rightPaddle.y;
            newRange = (((range) * (9.5f)) / (GetScreenHeight()));
            rightPaddle.y += rightPaddle.speed * GetFrameTime() * newRange; 
            if (rightPaddle.y + rightPaddle.height/2 >= GetScreenHeight())
            {
                rightPaddle.y = GetScreenHeight() - rightPaddle.height/2;
            }            
        }
        if (IsKeyPressed(KEY_Z))
        {
            boardSet = false;
            leftBoard.score = 0;
            rightBoard.score = 0;
            PlaySound(rub);
        }
        
        //Check for paddle collisions
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, leftPaddle.GetRect())) 
        {
            if (ball.speedX < 0)
            {
                ball.speedX *= -1.1f;
                ball.speedY = (ball.y - leftPaddle.y) / (leftPaddle.height/2) * ball.speedX;
                screenShake = true;
                timer = 0;
                ball.Play();      // Play WAV sound
            }
        }
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, rightPaddle.GetRect())) 
        {
            if (ball.speedX > 0)
            {
                ball.speedX *= -1.1f;
                ball.speedY = (ball.y - rightPaddle.y) / (rightPaddle.height/2) * - ball.speedX;
                screenShake = true;
                timer = 0;
                ball.Play();      // Play WAV sound
            }
        }
        
        //Set play area
        if ((resetTimer < resetTime) && (!boardSet)) 
        {
        resetTimer += 1.0f*GetFrameTime();
        ball.Hide();
        if (resetTimer > resetTime)
        {
            ball.Reset();
        }
        } else {
            resetTimer = 0.0f;
            boardSet = true;
        }
        
        //Draw play area
        BeginDrawing();
            ClearBackground(BLACK);
            BeginMode2D(camera);
            
            
            //Draw play area
            for (int i = 0; i < 10; i++) 
            {
                DrawRectangle(GetScreenWidth()/2 - GetScreenHeight()/20/2, i*GetScreenHeight()/10, GetScreenHeight()/20, GetScreenHeight()/20,  GRAY);
            }
            
            leftBoard.Draw();
            rightBoard.Draw();
            if (leftBoard.score >= 10)
            {
                DrawText("WINNER", leftPaddle.x + MeasureText("WINNER", leftBoard.size/3)/4, leftPaddle.y - leftPaddle.height/2  + leftBoard.size/12, leftBoard.size/3, RED);
                ball.Hide();
                DrawText("Press [Z] to reset", GetScreenWidth()/4, GetScreenHeight()/4, leftBoard.size/3, ORANGE);
            }
            if (rightBoard.score >= 10)
            {
                DrawText("WINNER", rightPaddle.x - MeasureText("WINNER", rightBoard.size/3)*5/4, rightPaddle.y - rightPaddle.height/2  + leftBoard.size/12, rightBoard.size/3, RED);
                ball.Hide();
                DrawText("Press [Z] to reset", GetScreenWidth()/4, GetScreenHeight()/4, leftBoard.size/3, ORANGE);
            }
            
            
            ball.Draw();
            leftPaddle.Draw();
            rightPaddle.Draw();
            
            //DrawFPS(10, 10);
            EndMode2D();
        EndDrawing();
    }
    
    CloseWindow(); 
    
    return 0;
}
