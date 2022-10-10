#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define UP_BUTTON 2
#define DOWN_BUTTON 3

const unsigned long PADDLE_RATE = 33;
const unsigned long BALL_RATE = 30;
const uint8_t PADDLE_HEIGHT = 10;

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 
Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void drawCourt() {OLED.drawRect(0, 0, 128, 32, WHITE);}

uint8_t ball_x = 32, ball_y = 10;
uint8_t ball_dir_x = 1, ball_dir_y = 1;
unsigned long ball_update;
unsigned long paddle_update;

const uint8_t PLAYER_X = 110;
uint8_t player_y = 10;

void setup() {
    //Serial.begin(9600);
    OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
    //OLED.display();
      
    unsigned long start = millis();
    
    pinMode(UP_BUTTON, INPUT);
    pinMode(DOWN_BUTTON, INPUT);
    digitalWrite(UP_BUTTON, 1);
    digitalWrite(DOWN_BUTTON, 1);
    OLED.clearDisplay();
    drawCourt();

    //while(millis() - start < 2000);
    //OLED.display();
    
    ball_update = millis();
    paddle_update = ball_update;
}

void loop() {
    bool update = false;
    unsigned long time = millis();

    static bool up_state = false;
    static bool down_state = false;

    if(digitalRead(UP_BUTTON) == LOW){
      up_state = true;
    }
    if(digitalRead(DOWN_BUTTON) == LOW){
      down_state = true;
    }
    //up_state = up_state | (digitalRead(UP_BUTTON) == LOW);
    //down_state = down_state | (digitalRead(DOWN_BUTTON) == LOW);

    if(time > ball_update) {
        uint8_t new_x = ball_x + ball_dir_x;
        uint8_t new_y = ball_y + ball_dir_y;

        // Check if we hit the vertical walls
        if(new_x == 0 || new_x == 127) {
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        }

        // Check if we hit the horizontal walls.
        if(new_y == 0 || new_y == 31) {
            ball_dir_y = -ball_dir_y;
            new_y += ball_dir_y + ball_dir_y;
        }


        // Check if we hit the player paddle
        if(new_x == PLAYER_X && new_y >= player_y && new_y <= player_y + PADDLE_HEIGHT)
        {
            ball_dir_x = -ball_dir_x;
            new_x = new_x + ball_dir_x + ball_dir_x;
        }

        OLED.drawPixel(ball_x, ball_y, BLACK);
        OLED.drawPixel(new_x, new_y, WHITE);
        ball_x = new_x;
        ball_y = new_y;

        ball_update += BALL_RATE;

        update = true;
    }

    if(time > paddle_update) {
        paddle_update += PADDLE_RATE;

        // Player paddle
        OLED.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, BLACK);
        if(up_state) {
            player_y -= 1;
        }
        if(down_state) {
            player_y += 1;
        }
        
        up_state = down_state = false;
        if(player_y < 1) player_y = 1;
        if(player_y + PADDLE_HEIGHT > 31) player_y = 31 - PADDLE_HEIGHT;
        OLED.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, WHITE);

        update = true;
    }

    if(update)
        OLED.display();
}
