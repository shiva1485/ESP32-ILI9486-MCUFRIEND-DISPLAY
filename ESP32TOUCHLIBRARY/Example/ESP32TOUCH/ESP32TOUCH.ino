//This example is tested only for ESP32 with 3.5 inch ili9486 tft MCUFRIEND DISPLAY.
//To calibrate your touchscreen use the ESP32calibration in the ESP32TOUCH library (or) Touch_calibrate_native of MCUFRIENDS default example.
//This library is compatible with the MCUFRIEND_kbv library.

//Check the connections.txt folder to known the connections of esp32 to ili9486 tft MCUFRIEND DISPLAY.
//Works for pretty much all display models of MCUFRIEND.

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <ESP32TOUCH.h>//This is the touch library for ESP32.

const int XP = 12, YP = 33, XM = 15, YM = 13;  //for ili9486 tft MCUFRIEND DISPLAY to esp32.
const int TS_LEFT = 175, TS_RT = 913, TS_TOP = 941, TS_BOT = 91;//here calibration values of touchscreen are placed.

//my hacked version
#define TouchScreen ESP32TOUCH //class name defined in the ESP32touch library(Make pins work connected to the tft).

#define MINPRESSURE 200
#define MAXPRESSURE 1000

TouchScreen ts(XP, YP, XM, YM, 300);   //Initialization the touch function pins
TSPoint p;                            //class name defined in the ESP32touch library(gets values of touched point).

int pixel_x, pixel_y;// These return the pressed values on the screen(calculated values).

bool touch (void) //Here pressed values of the screen is calculated.
{
  TSPoint p;

  do {
    p = ts.getPoint();//reads the values pressed in the tft.
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
  } while ((p.z < MINPRESSURE ) || (p.z > MAXPRESSURE));//condition for when to read tft screen pressed values.

  pixel_x = map(p.x, TS_LEFT, TS_RT, 0, 320);//gets the calculated x values
  pixel_y = map(p.y, TS_TOP, TS_BOT, 0, 480);//gets the calculated y values
}

//Colours
#define WHITE 0xFFFF
#define RED   0xF800
#define BLUE  0x001F
#define GREEN 0x07E0
#define BLACK 0x0000

void setup()
{
  tft.begin(0x9486);
  tft.fillScreen(BLUE);
  tft.println("Waiting for Serial");
  delay(1000);
  Serial.begin(9600);
  while (!Serial);
  tft.fillScreen(BLUE);
  tft.fillRect(0, 156, 66, 55, BLACK);
}

void loop()
{
  touch();//using the touch function.
  if (touch())
  {
    tft.fillScreen(RED);
    tft.print("working");
  }

  /*if (pixel_x > 13 && pixel_x < 79 && pixel_y > 156 && pixel_y < 211)
    {
    tft.fillScreen(RED);
    tft.print("working");
    Serial.println ("working");
    }*/
}
