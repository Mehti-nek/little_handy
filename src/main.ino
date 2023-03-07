#include <Arduino.h>
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

int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();

    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);

    bool pressed = (p.z > MINPRESSURE);
    if (pressed) {
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
        // Serial.println(String(p.x) + "\t" + String(p.y) + "\t" + String(pixel_x) + "\t" + String(pixel_y));
        tft.drawCircle(pixel_x, pixel_y, 1, TFT_RED);
        // delay(100);
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
byte data_length = 0;
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
byte keys[10][10] = {
                    // alphanumeric char
                    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'},
                    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p'},
                    {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 17},
                    {7, 'z', 'x', 'c', 'v', 'b', 'n', 'm', 24, 16}, 
                    {23, 35, 64, 32, 32, 32, 32, 27, 25, 26},
                    // other char and emojie
                    {1, 2, 3, 4, 5, 6, 11, 12, 14, 15},
                    {18, 19, 20, 21, 36, 22, 28, 29, 30, 31},
                    {37, 42, 43, 45, 61, 94, 95, 38,124,126},
                    {92, 47, 91, 93, 60, 62, 40, 41,123,125},
                    {23, 44, 96, 39, 46, 58, 59, 63, 33, 34}};


void setup(void)
{
    Serial.begin(9600);

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

    drawkeypad();
}



void loop(void)
{
    // character = Keypad();
    // terminal_print(Keypad(), RED);
    // Serial.println(Keypad());
    
    while(Serial.available()>0)
    {
        character = Serial.read();
        terminal_print(character, GREEN);
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

byte return_char = 0;

int key_row = -1;
int key_colum = -1;

char Keypad()
{
    if(Touch_getXY())
    {
        if (pixel_y - (header_height + terminal_height) > 0)
        {
            key_colum = pixel_x / 32;
            key_row = (pixel_y - (header_height + terminal_height)) / 32;

            key_x = (key_colum * 32) + 1;
            key_y = (header_height + terminal_height) + (key_row * 32) + 1;
            color = TFT_DARKGREY;

            i0 = key_row + (sign * 5);
            tft.fillRoundRect(key_x, key_y, key_w, key_h, 10, color);
            tft.setTextSize(3);
            tft.setTextColor(WHITE);
            tft.setCursor(key_x + 7, key_y + 5);

            if(isAlpha(char(keys[i0][key_colum])) && capslock) tft.print(char(keys[i0][key_colum] - 32));
            else tft.print(char(keys[i0][key_colum]));

            if(isAlpha(char(keys[i0][key_colum])) && capslock) return_char = char(keys[i0][key_colum] - 32);
            else return_char = char(keys[i0][key_colum]);

            switch (keys[key_row][key_colum])
            {
                case 7:
                    keys[key_row][key_colum] = 8;
                    capslock = !capslock;
                    break;

                case 8:
                    keys[key_row][key_colum] = 7;
                    capslock = !capslock;
                break;

                case 16:
                    return_char = 13;
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
    else
    {
        key_colum = -1;
        key_row = -1;
        return_char = 0;
    }

    terminal_print(return_char, RED);
    // return return_char;

    // Serial.print(key_colum);
    // Serial.print("\t");
    // Serial.println(key_row);

    // // tft.fillRect(0, tft.height() - footer_height, tft.width(), footer_height, TFT_LIGHTGREY);
    // return_char = 0;
    // for (byte i = 0; i < 5; i++)
    // {
    //     for (byte j = 0; j < 10; j++)
    //     {
    //         key_x = (j * 32) + 1;
    //         key_y = (header_height + terminal_height) + (i * 32) + 1;
    //         color = TFT_DARKGREY;

    //         if (Touch_getXY())
    //         {
    //             if ((key_x < pixel_x) && (key_x > (int16_t)(pixel_x - key_w)) && (key_y < pixel_y) && (key_y > (int16_t)(pixel_y - key_h)))
    //             {
    //                 if(isAlpha(char(keys[i0][j])) && capslock) return_char = char(keys[i0][j] - 32);
    //                 else return_char = char(keys[i0][j]);
    //                 // hold = true;
    //                 color = TFT_BLACK;
    //                 switch (keys[i][j])
    //                 {
    //                     case 7:
    //                         keys[i][j] = 8;
    //                         capslock = !capslock;
    //                         break;

    //                     case 8:
    //                         keys[i][j] = 7;
    //                         capslock = !capslock;
    //                     break;

    //                     case 16:
    //                         return_char = 13;
    //                         Serial.println("> ");
    //                     break;

    //                     case 17:
    //                         Serial.println("< ");
    //                     break;

    //                     case 23:
    //                         sign = !sign;
    //                     break;

    //                     default:
    //                     break;
    //                 }
    //             }
    //         }
    //         i0 = i + (sign * 5);
    //         tft.fillRoundRect(key_x, key_y, key_w, key_h, 10, color);
    //         tft.setTextSize(3);
    //         tft.setTextColor(WHITE);
    //         tft.setCursor(key_x + 7, key_y + 5);

    //         if(isAlpha(char(keys[i0][j])) && capslock) tft.print(char(keys[i0][j] - 32));
    //         else tft.print(char(keys[i0][j]));
    //     }
    // }
}

void drawkeypad()
{
    return_char = 0;
    for (byte i = 0; i < 5; i++)
    {
        for (byte j = 0; j < 10; j++)
        {
            key_x = (j * 32) + 1;
            key_y = (header_height + terminal_height) + (i * 32) + 1;
            color = TFT_DARKGREY;


            i0 = i + (sign * 5);
            tft.fillRoundRect(key_x, key_y, key_w, key_h, 10, color);
            tft.setTextSize(3);
            tft.setTextColor(WHITE);
            tft.setCursor(key_x + 7, key_y + 5);

            if(isAlpha(char(keys[i0][j])) && capslock) tft.print(char(keys[i0][j] - 32));
            else tft.print(char(keys[i0][j]));
        }
    }
}

void terminal_print(char character_in, int color)
{
    tft.setTextSize(font_multiplier);
    tft.setTextColor(color);
    switch (character_in)
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

                data_length = data.length();

                // first line command start with >
                tft.print("> ");
                for (byte i = 0; i < data_length; i++)
                {
                    tft.print(data[i]);
                    // if the data is biger than n it 
                    if (i%22 == 0 && i != 0)
                    {
                        scroll += line_height;
                        tft.vertScroll(header_height, terminal_height, scroll);
                        tft.println();
                        tft.print("- ");
                    }
                }
                scroll += line_height;
                tft.vertScroll(header_height, terminal_height, scroll);

            }
            else
            {
                line = 0;
                scroll = 0;
                // tft.vertScroll(header_height, terminal_height, scroll);
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
            if (int(character) != 10 || int(character) != 0)
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