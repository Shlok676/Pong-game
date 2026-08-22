#include <raylib.h>

Color Green = Color{38, 185, 154, 255};
Color Dark_Green = Color{20, 160, 133, 255};
Color Light_Green = Color{129, 204, 184, 255};
Color Yellow = Color{243, 213, 91, 255};

int player_score = 0;
int cpu_score = 0;

class Ball{
    public:
        float x, y;
        float speed_x, speed_y; 
        int radius;
        float base_speed;       

        void Draw(){
            DrawCircle(x, y, radius, Yellow);
        }
        void Update(Sound BounceSfx){
            x += speed_x;
            y += speed_y;

            if (y + radius >= GetScreenHeight() || y - radius <= 0) {
                speed_y *= -1;
                PlaySound(BounceSfx);
            }

            // CPU wins
            if (x + radius >= GetScreenWidth()) {
                cpu_score++;
                ResetBall();
            }

            if (x - radius <= 0) {
                player_score++;
                ResetBall();
            }
        }

        void ResetBall() {
            x = GetScreenWidth() / 2.0f;
            y = GetScreenHeight() / 2.0f;

            base_speed = 7.0f; 

            speed_x = (GetRandomValue(0, 1) == 0) ? -base_speed : base_speed;
            speed_y = (GetRandomValue(0, 1) == 0) ? -base_speed : base_speed;
        }
};

class Paddle{
    protected:
        void LimitMovement() {
            if (y <= 0) {
                y = 0;
            }
            if (y + height >= GetScreenHeight()) {
                y = GetScreenHeight() - height;
            }
        }

    public:
        float x, y;
        float width, height;
        int speed;

        void Draw(){
            DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, WHITE);
        }

        void Update() { 
            if (IsKeyDown(KEY_UP)) {
                y = y - speed;
            }
            if (IsKeyDown(KEY_DOWN)) {
                y = y + speed;
            }
            if (IsKeyDown(KEY_W)) {
                y = y - speed;
            }
            if (IsKeyDown(KEY_S)) {
                y = y + speed;
            }

            LimitMovement();
        }
};

class CpuPaddle : public Paddle{
    public:
        void Update(int ball_y, int ball_speed_x){
            if (ball_speed_x < 0) {
                if (y + height / 2 > ball_y) {
                    y = y - speed;
                }
                if (y + height / 2 <= ball_y) {
                    y = y + speed;
                }
            } else {
                float center = GetScreenHeight() / 2.0f - height / 2.0f;
                if (y + height / 2 < center) {
                    y += speed / 2;
                }
                if (y + height / 2 > center) {
                    y -= speed / 2;
                }
            }

            LimitMovement();
        }
};

Ball ball;
Paddle player;
CpuPaddle cpu;

int main(){

    int screen_width = 1280;
    int screen_height = 800;

    SetConfigFlags(FLAG_MSAA_4X_HINT); 

    InitWindow(screen_width, screen_height, "Pong");
    InitAudioDevice();
    
    ChangeDirectory(GetApplicationDirectory());

    Sound bounceSound = LoadSound("Assets/bounce.mp3");
    
    SetTargetFPS(60);

    Image windowIcon = LoadImage("Assets/ping-pong.exe");
    SetWindowIcon(windowIcon);
    UnloadImage(windowIcon);

    ball.radius = 20;
    ball.base_speed = 7.0f; 
    ball.ResetBall(); 

    player.width = 25;
    player.height = 120;
    player.x = screen_width - player.width - 10;
    player.y = screen_height / 2 - player.height / 2;
    player.speed = 6;

    cpu.height = 120;
    cpu.width = 25;
    cpu.x = 10;
    cpu.y = screen_height / 2 - cpu.height / 2;
    cpu.speed = 6;

    while (WindowShouldClose() == false){

        // Updating

        ball.Update(bounceSound);
        player.Update();
        cpu.Update(ball.y, ball.speed_x);

        if (IsKeyDown(KEY_R)){
            ball.ResetBall();
            player.x = screen_width - player.width - 10;
            player.y = screen_height / 2 - player.height / 2;
            cpu.x = 10;
            cpu.y = screen_height / 2 - cpu.height / 2;
            player_score = 0;
            cpu_score = 0;
        }

        // Checking for collision

        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{player.x, player.y, player.width, player.height})){
            if (ball.speed_x > 0) { 
                float paddle_center_y = player.y + (player.height / 2.0f);
                float relative_intersect_y = (ball.y - paddle_center_y) / (player.height / 2.0f);
            
                ball.base_speed *= 1.05f; 
                
                ball.speed_x = -ball.base_speed;
                ball.speed_y = relative_intersect_y * ball.base_speed; 
                
                PlaySound(bounceSound);
            }
        }

        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu.x, cpu.y, cpu.width, cpu.height})){
            if (ball.speed_x < 0) { 
                float paddle_center_y = cpu.y + (cpu.height / 2.0f);
                float relative_intersect_y = (ball.y - paddle_center_y) / (cpu.height / 2.0f);
                
                ball.base_speed *= 1.05f; 
                
                ball.speed_x = ball.base_speed; 
                ball.speed_y = relative_intersect_y * ball.base_speed; 
                
                PlaySound(bounceSound);
            }
        }

        // Drawing
        BeginDrawing();
        ClearBackground(Dark_Green);

        DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, Green);
        DrawCircle(screen_width / 2, screen_height / 2, 150, Light_Green);

        DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);
        ball.Draw();
        player.Draw();
        cpu.Draw();

        DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
        DrawText("/", screen_width / 4 + 40, 20, 80, WHITE);
        DrawText(TextFormat("%i", 10), screen_width / 4 + 120, 20, 80, WHITE);
        
        DrawText(TextFormat("%i", player_score), 2.5 * screen_width / 4 - 20, 20, 80, WHITE);
        DrawText("/", 2.5  * screen_width / 4 + 40, 20, 80, WHITE);
        DrawText(TextFormat("%i", 10), 2.5 * screen_width / 4 + 120, 20, 80, WHITE);

        if (cpu_score >= 10){
            DrawText("Computer Wins", screen_width / 2 - 308, screen_height / 3, 80, WHITE);
            DrawText("Press R to Restart", screen_width / 2 - 400, screen_height / 2, 80, WHITE);
            ball.x = screen_width / 2;
            ball.y = screen_height / 2;
            ball.base_speed = 0;
        }
        if (player_score >= 10){
            DrawText("Player Wins", screen_width / 2 - 260, screen_height / 3, 80, WHITE);
            DrawText("Press R to Restart", screen_width / 2 - 400, screen_height / 2, 80, WHITE);
            ball.x = screen_width / 2;
            ball.y = screen_height / 2;
            ball.base_speed = 0;
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
