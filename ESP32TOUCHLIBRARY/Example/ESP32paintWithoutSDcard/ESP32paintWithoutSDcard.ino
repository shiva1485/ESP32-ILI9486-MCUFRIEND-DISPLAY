#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <ESP32TOUCH.h>

#define MINPRESSURE 200
#define MAXPRESSURE 1000

#define TouchScreen ESP32TOUCH

const int XP = 12, YP = 33, XM = 15, YM = 13; //320x480 ID=0x9486
const int TS_LEFT = 119, TS_RT = 910, TS_TOP = 955, TS_BOT = 84;

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool touch (void)
{
  TSPoint p;

  do {
    p = ts.getPoint();
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
  } while ((p.z < MINPRESSURE ) || (p.z > MAXPRESSURE));
  pixel_x = map(p.x, TS_LEFT, TS_RT, 0, 320);
  pixel_y = map(p.y, TS_TOP, TS_BOT, 0, 480);
}

int PENRADIUS = 3;
int currentcolor;
int xpos;
int ypos;

#define BLACK   0x0000
#define BLUE    0x051D
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

void setup(void)
{
  tft.begin(0x9486);
  Serial.begin(9600);
  tft.setRotation(0);
  tft.fillScreen(BLACK);
  currentcolor = GREEN;

  tft.setCursor(140, 0);
  tft.setTextSize(2);
  tft.setTextColor(CYAN);
  tft.print("NOTES");

  tft.setCursor(261, 9);
  tft.setTextSize(1);
  tft.setTextColor(YELLOW);
  tft.print("CLEAR");
}

void loop()
{
  touch();

  xpos = pixel_x;
  ypos = pixel_y;

  if (((ypos - PENRADIUS) > 35) && ((ypos + PENRADIUS) < 480))//drawing function
  {
    tft.fillCircle(xpos, ypos, PENRADIUS, currentcolor);
  }

  else if (pixel_x > 262 && pixel_x < 315 && pixel_y > 7 && pixel_y < 32)//clear button
  {
    tft.fillRect(0, 35, 320, 445, BLACK);
  }
}
