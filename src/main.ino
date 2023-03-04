#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 200
#define MAXPRESSURE 1000

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP=8,XM=A2,YP=A3,YM=9; //320x480 ID=0x9486
const int TS_LEFT=130,TS_RT=900,TS_TOP=950,TS_BOT=90;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Adafruit_GFX_Button btn_0, btn_1, btn_2, btn_3, btn_4, btn_5, btn_6, btn_7, btn_8, btn_9;

// include the SD library:
#include <SPI.h>
#include <SD.h>

// set up variables using the SD utility library functions:
// Sd2Card card;
// SdVolume volume;
// SdFile root;
File myFile;

// change this to match your SD shield or module;
// Arduino Ethernet shield: pin 4
// Adafruit SD shields and modules: pin 10
// Sparkfun SD shield: pin 8
// MKRZero SD: SDCARD_SS_PIN
const int chipSelect = 10;

int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool hold = false;
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();

    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);

    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
        Serial.println(String(p.x) + "\t" + String(p.y) + "\t" + String(pixel_x) + "\t" + String(pixel_y));
        tft.drawCircle(pixel_x, pixel_y, 1, TFT_RED);
        // delay(100);
    }
    else
    {
        hold = false;
    }
    return pressed;
}

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define font_multiplier 2
int line_height = 0;
int header_height = 0;
int footer_height = 0;
int terminal_height = 0;
int max_line = 0;

int line = 0;

String data = "";
char character;
bool inv = false;

int scroll = 0;

bool scroll_toggle = false;

// 7,8 as capslock icon
// 17 as backspace icon
// 16 as enter icon
// 23 as change page icon

// 24,25,26,27 as arrow keys icon
// 64 addsign icon
// 35 hashtag icon

// 32 as space
// 9 ,10, 13, 32,
byte keys[10][10] = {{'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
                    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
                    {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 17},
                    {7, 'z', 'x', 'c', 'v', 'b', 'n', 'm', 24, 16}, 
                    {23, 35, 64, 32, 32, 32, 32, 27, 25, 26},

                    {1, 2, 3, 4, 5, 6, 11, 12, 14, 15},
                    {18, 19, 20, 21, 36, 22, 28, 29, 30, 31},
                    {37, 42, 43, 45, 61, 94, 95, 38,124,126},
                    {92, 47, 91, 93, 60, 62, 40, 41,123,125},
                    {23, 44, 96, 39, 46, 58, 59, 63, 33, 34}};


void setup(void)
{
    Serial.begin(9600);

    // xTaskCreate(TaskUI, "UI", 128, NULL, 2, &TaskUI_Handler);
    // xTaskCreate(TaskSerial, "Serial", 128, NULL, 2, &TaskSerial_Handler);
    // xTaskCreate(TaskKeypad, "Keypad", 256, NULL, 2, &TaskKeypad_Handler);

    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    Serial.println("Calibrate for your Touch Panel");
    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft.begin(ID);
    tft.setRotation(0);            //PORTRAIT
    tft.fillScreen(BLACK);
    // on_btn.initButton(&tft,  60, 200, 100, 40, WHITE, CYAN, BLACK, "ON", 2);
    // off_btn.initButton(&tft, 180, 200, 100, 40, WHITE, CYAN, BLACK, "OFF", 2);
    // on_btn.drawButton(false);
    // off_btn.drawButton(false);
    // tft.fillRect(40, 80, 160, 80, RED);

    tft.setTextSize(font_multiplier);
    line_height = font_multiplier * 8;


    header_height = line_height * 2;
    footer_height = line_height * 10;

    terminal_height = tft.height() - header_height - footer_height;
    max_line = terminal_height / line_height;

    tft.fillRect(0, 0, tft.width(), header_height, TFT_BLUE);

    Serial.println(terminal_height);
    Serial.println(footer_height);
    // for (int i = 0; i < max_line; i++)
    // {
    //     tft.setCursor(0, i*line_height + header_height);
    //     tft.print(i);
    //     delay(10);
    // }

    // scroll = line_height;
}



void loop(void)
{
    Keypad();
    while(Serial.available()>0)
    {
        character = Serial.read();
        // Serial.println(int(character));
        // tft.println(character)

        switch (character)
        {
            case 8:
                break;

            case 9:
                break;

            case 13:
                    if ((scroll / line_height) < max_line)
                    {
                        tft.fillRect(0, ((scroll / line_height) * line_height) + header_height, tft.width(), line_height, TFT_BLACK);
                        tft.setCursor(0, ((scroll / line_height) * line_height) + header_height);
                        tft.print("> " + data);

                        Serial.print(data);
                        // while (scroll < end)
                        // {
                        //     scroll += 1;
                        //     tft.vertScroll(header_height, terminal_height, scroll);
                        //     delay(5);
                        // }
                        // end += line_height;

                        scroll += line_height;
                        tft.vertScroll(header_height, terminal_height, scroll);
                    }
                    else
                    {
                        line = 0;
                        scroll = 0;
                        tft.vertScroll(header_height, terminal_height, scroll);
                    }

                    // tft.fillRect(0, scroll + header_height, tft.width(), line_height, TFT_BLACK);
                    
                    // tft.print(data);
                    // tft.println("");
                    // tft.println(proccess(data));
                    // tft.print(">");

                    Serial.print(scroll);
                    Serial.print("\t");
                    Serial.print(scroll / line_height);
                    Serial.print("\t");
                    Serial.print(scroll_toggle);
                    Serial.print("\t");
                    Serial.println(line);
                    data = "";
                    line++;
                break;

            case 27:
                inv = !inv;
                tft.invertDisplay(inv);
                break;
            
            default:
                if (int(character) != 10)
                {
                    data += character;
                }
                break;
        }
        
        // if (character == 27)
        // {
        //     /* code */
        // }
        
        // if (character == 13)
        // {
        //     tft.println("");
        //     tft.print(">");
        // }
        // else
        // {
        //     data += character;
        //     tft.print(data);
        // }
    }

    // Touch_getXY();

    // bool down = Touch_getXY();
    // on_btn.press(down && on_btn.contains(pixel_x, pixel_y));
    // off_btn.press(down && off_btn.contains(pixel_x, pixel_y));
    // if (on_btn.justReleased())
    //     on_btn.drawButton();
    // if (off_btn.justReleased())
    //     off_btn.drawButton();
    // if (on_btn.justPressed()) {
    //     on_btn.drawButton(true);
    //     tft.fillRect(40, 80, 160, 80, GREEN);
    // }
    // if (off_btn.justPressed()) {
    //     off_btn.drawButton(true);
    //     tft.fillRect(40, 80, 160, 80, RED);
    // }
}

// String keywords[5] = {"who are you", "hi", "mkdir", "setup", "conf"};

// String ans = "";
// byte code = 0;
// String proccess(String d)
// {
//     for (byte i = 0; i < 5; i++)
//     {
//         if (d.compareTo(keywords[i])) code = i;
//         // d.compareTo(keywords[i]);
//         Serial.println(keywords[i]);
//     }

//     switch (code)
//     {
//         case 1: ans = "juts a little tool!"; break;
//         case 2: ans = "hello"; break;
//         case 3: ans = "new dir"; break;
//         case 4: ans = "boud rate"; break;
//         case 5: ans = "AT + KJHKJH"; break;
//     }
//     return ans;
// }

int key_x = 0;
int key_y = 0;
int key_w = 30;
int key_h = 30;

int color = TFT_DARKGREY;

bool capslock = false;
bool sign = false;

byte i0 = 0;
void Keypad()
{
    // tft.fillRect(0, tft.height() - footer_height, tft.width(), footer_height, TFT_LIGHTGREY);

    for (byte i = 0; i < 5; i++)
    {
        for (byte j = 0; j < 10; j++)
        {
            key_x = (j * 32) + 1;
            key_y = (header_height + terminal_height) + (i * 32) + 1;
            color = TFT_DARKGREY;

            if (Touch_getXY() && !hold)
            {
                if ((key_x < pixel_x) && (key_x > (int16_t)(pixel_x - key_w)) && (key_y < pixel_y) && (key_y > (int16_t)(pixel_y - key_h)))
                {
                    hold = true;
                    color = TFT_BLACK;
                    switch (keys[i][j])
                    {
                        case 7:
                            keys[i][j] = 8;
                            capslock = !capslock;
                            break;

                        case 8:
                            keys[i][j] = 7;
                            capslock = !capslock;
                            break;

                        case 16:
                            Serial.println("> ");
                            break;

                        case 17:
                            Serial.println("< ");
                            break;

                        case 23:
                            sign = !sign;
                            break;

                        default:
                            break;
                    }
                }
            }
            i0 = i + (sign * 5);
            tft.fillRoundRect(key_x, key_y, key_w, key_h, 10, color);
            tft.setTextSize(3);
            tft.setCursor(key_x + 7, key_y + 5);

            if(isAlpha(char(keys[i0][j])) && capslock) tft.print(char(keys[i0][j] - 32));
            else tft.print(char(keys[i0][j]));
        }
    }
}