#include "Arduino.h"

#include "ESP32TOUCH.h"

#define NUMSAMPLES 3  //.kbv
#if defined(__STM32F1__) || defined(ESP32)   //Maple core
#define ADC_ADJUST >>2
#else
#define ADC_ADJUST
#endif

TSPoint::TSPoint(void) {
  x = y = 0;
}

TSPoint::TSPoint(int16_t x0, int16_t y0, int16_t z0) {
  x = x0;
  y = y0;
  z = z0;
}

bool TSPoint::operator==(TSPoint p1) {
  return  ((p1.x == x) && (p1.y == y) && (p1.z == z));
}

bool TSPoint::operator!=(TSPoint p1) {
  return  ((p1.x != x) || (p1.y != y) || (p1.z != z));
}

static void insert_sort(int array[], uint8_t size) {
  uint8_t j;
  int save;

  for (int i = 1; i < size; i++) {
    save = array[i];
    for (j = i; j >= 1 && save < array[j - 1]; j--)
      array[j] = array[j - 1];
    array[j] = save;
  }
}

TSPoint ESP32TOUCH::getPoint(void) {
  int x, y, z;
  int samples[NUMSAMPLES];
  uint8_t i, valid;

  valid = 1;

  pinMode(_yp, INPUT);
  pinMode(_ym, INPUT);

  digitalWrite(_yp, LOW);
  digitalWrite(_ym, LOW);

  pinMode(_xp, OUTPUT);
  pinMode(_xm, OUTPUT);
  digitalWrite(_xp, HIGH);
  digitalWrite(_xm, LOW);

  for (i = 0; i < NUMSAMPLES; i++) {
    samples[i] = analogRead(_yp) ADC_ADJUST;
  }
  insert_sort(samples, NUMSAMPLES);
  x = (1023 - samples[NUMSAMPLES / 2]); //choose median

  pinMode(_xp, INPUT);
  pinMode(_xm, INPUT);
  digitalWrite(_xp, LOW);
  digitalWrite(_xm, LOW);     //.kbv for Due

  pinMode(_yp, OUTPUT);
  digitalWrite(_yp, HIGH);
  pinMode(_ym, OUTPUT);
  digitalWrite(_ym, LOW);     //.kbv for Due

  for (i = 0; i < NUMSAMPLES; i++) {
    samples[i] = analogRead(_xm) ADC_ADJUST;
  }

  insert_sort(samples, NUMSAMPLES);

  y = (1023 - samples[NUMSAMPLES / 2]);

  // Set X+ to ground
  pinMode(_xp, OUTPUT);
  digitalWrite(_xp, LOW);

  // Set Y- to VCC
  pinMode(_ym, OUTPUT);     //.kbv
  digitalWrite(_ym, HIGH);

  // Hi-Z X- and Y+
  digitalWrite(_xm, LOW);   //.kbv
  pinMode(_xm, INPUT);      //.kbv
  digitalWrite(_yp, LOW);
  pinMode(_yp, INPUT);

  int z1 = analogRead(_xm) ADC_ADJUST;
  int z2 = analogRead(_yp) ADC_ADJUST;

  z = (1023 - (z2 - z1));

  return TSPoint(x, y, z);  //XM, YP still in ANALOG mode
}

ESP32TOUCH::ESP32TOUCH(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym) {
  _yp = yp;
  _xm = xm;
  _ym = ym;
  _xp = xp;
  _rxplate = 0;
  pressureThreshhold = 10;
}


ESP32TOUCH::ESP32TOUCH(uint8_t xp, uint8_t yp, uint8_t xm, uint8_t ym,
                       uint16_t rxplate) {
  _yp = yp;
  _xm = xm;
  _ym = ym;
  _xp = xp;
  _rxplate = rxplate;

  pressureThreshhold = 10;
}

int ESP32TOUCH::readTouchX(void) {
  pinMode(_yp, INPUT);
  pinMode(_ym, INPUT);
  digitalWrite(_yp, LOW);
  digitalWrite(_ym, LOW);

  pinMode(_xp, OUTPUT);
  digitalWrite(_xp, HIGH);
  pinMode(_xm, OUTPUT);
  digitalWrite(_xm, LOW);

  return (1023 - (analogRead(_yp)) ADC_ADJUST);
}


int ESP32TOUCH::readTouchY(void) {
  pinMode(_xp, INPUT);
  pinMode(_xm, INPUT);
  digitalWrite(_xp, LOW);
  digitalWrite(_xm, LOW);

  pinMode(_yp, OUTPUT);
  digitalWrite(_yp, HIGH);
  pinMode(_ym, OUTPUT);
  digitalWrite(_ym, LOW);

  return (1023 - (analogRead(_xm)) ADC_ADJUST);
}


uint16_t ESP32TOUCH::pressure(void) {
  // Set X+ to ground
  pinMode(_xp, OUTPUT);
  digitalWrite(_xp, LOW);

  // Set Y- to VCC
  pinMode(_ym, OUTPUT);
  digitalWrite(_ym, HIGH);

  // Hi-Z X- and Y+
  digitalWrite(_xm, LOW);
  pinMode(_xm, INPUT);
  digitalWrite(_yp, LOW);
  pinMode(_yp, INPUT);

  int z1 = analogRead(_xm) ADC_ADJUST;
  int z2 = analogRead(_yp) ADC_ADJUST;

  return (1023 - (z2 - z1));
}
