#include <Arduino.h>
#include <FastLED.h>
#include <IRremote.h>   // библиотека для ИК
#include <avr/sleep.h>  // библиотека для сна

//FastLED
#define LED_COUNT 3
#define LED_DT 9
#define MAX_BRIGHTNES 255
struct CRGB leds[LED_COUNT];

//ИНТЕРФЕЙСЫ: ========================================================

// Define IR Receiver and Results Objects
const int RECV_PIN = 2;
IRrecv irrecv(RECV_PIN);
decode_results results;

// Иннициаллизация функций ========================================================
boolean isResumable(long code);
void execute(int code);
boolean validate(long code);
void resetIr();
void animation1(byte speedMultiplier);
void animation2(byte hueDelta, byte speedMultiplier);
void customAnimation(int speedDelay);
byte *Wheel(byte WheelPos);
void heat();
void fadeOut();

//глобальные переменные
// задержка для анимации RGB светодиодов [ms]
#define THIS_DELAY 300
byte *c;
byte cHue = 255;
byte cSat = 255;
byte cVal = 255;
int lastCommand;
//skip()

unsigned long codes[] = {
    0xFFA25D,
    0xFF629D,
    0xFFE21D,
    0xFF22DD,
    0xFF02FD,
    0xFFC23D,
    0xFFC23D,
    0xFFE01F,
    0xFFA857,
    0xFF906F,
    0xFF6897,
    0xFF9867,
    0xFFB04F,
    0xFF18E7,
    0xFF10EF,
    0xFF38C7,
    0xFF5AA5,
    0xFF4AB5,

    0xFFFFFFFF};
int codesLenght = 1;
// resumable Codes (brightness,sat adjustments and timer wont break the animation)
unsigned long rsmCodes[] = {
    //*
    0xFF6897,
    //#
    0xFFB04F,
    //UP
    0xFF18E7,
    //LEFT
    //0xFF10EF,
    //OK
    // 0xFF38C7,
    //RIGHT
    //0xFF5AA5,
    //DOWN
    0xFF4AB5,

    //REPEAT
    0xFFFFFFFF};
int rsmCodesLenght = 1;

unsigned int checkpoint;
long int codes[] = {
    0xFFA25D,
    0xFF629D,
    0xFFE21D,
    0xFF22DD,
    0xFF02FD,
    0xFFC23D,
    0xFFE01F,
    0xFFA857,
    0xFF906F,
    0xFF6897,
    0xFF9867,
    0xFFB04F,
    0xFF4AB5,
    0xFF18E7,
    0xFF10EF,
    0xFF38C7,
    0xFF5AA5};
int codesSize = sizeof(codes) / sizeof(codes[0]);

int getSize(unsigned long array[])
{
  int size = 1;
  for (size_t i = 0;; i++)
  {
    if (array[i] != 0xFFFFFFFF)
    {
      size++;
    }
    else
    {
      return size;
    }
  }
}

// закрасить все диоды ленты в один цвет
void one_color_all(int cred, int cgrn, int cblu) {
  for (int i = 0; i < LED_COUNT; i++) {
    leds[i].setRGB(cred, cgrn, cblu);
  }
}

void showHSV(byte hue, byte sat, byte val) {
  for (int i = 0; i < LED_COUNT; i++) {
    leds[i].setHSV(hue, sat, val);
  }
  LEDS.show();
}

void colorShift(byte hue, byte sat, byte val) {
  if (cHue != hue || cSat != sat || cVal != val) {
    int dirHue = 0;
    if (((hue - cHue + 256) % 256) < 128) {
      dirHue = 1;
    } else {
      dirHue = (-1);
    }
    while (cHue != hue || cSat != sat || cVal != val) {
      if (cHue != hue) {
        cHue += dirHue;
      }
      if (cSat != sat) {
        if (cSat > sat) {
          cSat--;
        } else if (cSat < sat) {
          cSat++;
        }
      }
      if (cVal != val) {
        if (cVal > val) {
          cVal--;
        } else if (cVal < val) {
          cVal++;
        }
      }
      showHSV(cHue, cSat, cVal);
      delay(15);
    }
  }
}

void colorShift(byte hue, byte sat, byte val, byte del) {
  if (cHue != hue || cSat != sat || cVal != val) {
    int dirHue = 0;
    if (((hue - cHue + 256) % 256) < 128) {
      dirHue = 1;
    } else {
      dirHue = (-1);
    }
    while (cHue != hue || cSat != sat || cVal != val) {
      if (cHue != hue) {
        cHue += dirHue;
      }
      if (cSat != sat) {
        if (cSat > sat) {
          cSat--;
        } else if (cSat < sat) {
          cSat++;
        }
      }
      if (cVal != val) {
        if (cVal > val) {
          cVal--;
        } else if (cVal < val) {
          cVal++;
        }
      }
      showHSV(cHue, cSat, cVal);
      delay(del);
    }
  }
}

void blink(int count) {
  int val = cVal;
  for (size_t i = 0; i < count; i++) {
    colorShift(cHue, cSat, 128, 0);
    colorShift(cHue, cSat, val, 0);
  }
}

void checkLimits(int value) {
  if (value == 0 || value == 255) {
    blink(1);
  }
}

void checkLimits(int value, int min, int max) {
  if (value == min || value == max) {
    blink(1);
  }
}

//FUNCTIONS =======================================================================
void animation1(byte speedMultiplier) {
  uint16_t i, j;
  for (j = 0; j < 256 * 1; j++) {  // 5 cycles of all colors on wheel
    for (i = 0; i < LED_COUNT; i++) {
      c = Wheel(((i * 256 / LED_COUNT) + j) & 255);
      one_color_all(*c, *(c + 1), *(c + 2));
    }
    FastLED.show();
    delay(THIS_DELAY / speedMultiplier);
  }
}

void animation2(byte hueDelta, byte speedMultiplier) {
  uint16_t i, j;
  for (j = 0; j < 256 * 5; j++) {  // 5 cycles of all colors on wheel
    for (i = 0; i < LED_COUNT; i++) {
      c = Wheel(((i * 256 / hueDelta) + j) & 255);
      leds[i].setRGB(*c, *(c + 1), *(c + 2));
    }
    FastLED.show();
    delay(THIS_DELAY / speedMultiplier);
  }
}

byte *Wheel(byte WheelPos) {
  static byte c[3];
  if (WheelPos < 85) {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }
  return c;
}

void heat() {
  for (int i = 0; i <= 255; i++) {
    for (int j = 0; j < LED_COUNT; j++) {
      one_color_all(0, 0, i);
    }
    FastLED.show();
    delay(5);
  }
}

boolean skip(int millisToSkip, unsigned long lastCheck) {
  if (millis() >= lastCheck + millisToSkip) {
    return true;
  }
  return false;
}

void receiver()
{
  // irrecv.decode(&results);
  int input = results.value;
  if (input != 0xFFFFFFFF) {
    lastCommand = input;
  }
  Serial.print('\n');
  Serial.print(lastCommand, HEX);
  resetIr();
  execute(lastCommand);
}

void execute(int code)
{
  Serial.print("\n");
  Serial.print("code:");
  Serial.println(code, HEX);
  switch (code)
  {
    // 1 FFA25D
  case 0xFFA25D:
    colorShift(0, 255, 255);
    break;
    // 2 FF629D
  case 0xFF629D:
    colorShift(8, 255, 255);
    break;
    // 3 FFE21D
  case 0xFFE21D:
    colorShift(17, 255, 255);
    break;
    // 4 FF22DD
  case 0xFF22DD:
    colorShift(59, 255, 255);
    break;
    // 5 FF02FD
  case 0xFF02FD:
    colorShift(82, 255, 255);
    break;
    // 6 FFC23D
  case 0xFFC23D:
    colorShift(131, 255, 255);
    break;
  // 7 FFE01F
  case 0xFFE01F:
    colorShift(155, 255, 255);
    break;
    // 8 FFA857
  case 0xFFA857:
    colorShift(205, 255, 255);
    break;
    // 9 FF906F
  case 0xFF906F:
    colorShift(233, 255, 255);
    break;
    // * FF6897
  case 0xFF6897:
  {
    int sat = constrain(cSat - 10, 0, 255);
    checkLimits(sat);
    colorShift(cHue, sat, cVal);
    break;
  }
    // 0 FF9867
  case 0xFF9867:
    break;
    // # FFB04F
  case 0xFFB04F:
  {
    int sat = constrain(cSat + 10, 0, 255);
    checkLimits(sat);
    colorShift(cHue, sat, cVal);
    break;
  }
    // UP FF18E7
  case 0xFF18E7:
  {
    int val = constrain(cVal + 10, 40, 255);
    checkLimits(val, 40, 255);
    colorShift(cHue, cSat, val);
    break;
  }
    // LEFT FF10EF
  case 0xFF10EF:
  {
    byte hue = cHue - 1;
    Serial.println("------");
    Serial.println(cHue);
    Serial.println(hue);
    colorShift(hue, cSat, cVal);
    break;
  }
    // OK FF38C7
  case 0xFF38C7:
    customAnimation(10);
    break;
    // RIGHT FF5AA5
  case 0xFF5AA5:
  {
    byte hue = cHue + 1;
    Serial.println("++++++");
    Serial.println(cHue);
    Serial.println(hue);
    colorShift(hue, cSat, cVal);
    break;
  }
    // DOWN FF4AB5
  case 0xFF4AB5:
  {
    int val = constrain(cVal - 10, 40, 255);
    checkLimits(val, 40, 255);
    colorShift(cHue, cSat, val);
    break;
  }
  }
}

bool pollIr()
{
  if (irrecv.decode(&results))
  {
    if (results.value == 0)
    {
      return false;
    }

    return true;
  }
  return false;
}

void customAnimation(int speedDelay) {
  unsigned long timer = millis();
  while (!pollIr()) {
    for (size_t i = 0; i < LED_COUNT;) {
      if (skip(speedDelay, timer)) {
        showHSV(cHue, cSat, cVal);
        cHue++;
        i++;
        timer = millis();
      }
    }
  }
  if (validate(results.value))
  {
    if (isResumable(results.value))
    {
      receiver();
      return customAnimation(speedDelay);
    }
    receiver();
    return;
  }
  else
  {
    return customAnimation(speedDelay);
  }
}

boolean validate(long code)
{
  for (size_t i = 0; i < codesLenght; i++)
  {
    if (code == codes[i])
    {
      return true;
    }
  }
  Serial.print("Invalid code: ");
  Serial.println(results.value, HEX);
  resetIr();
  return false;
}

boolean isResumable(long code)
{
  for (size_t i = 0; i < rsmCodesLenght; i++)
  {
    if (code == rsmCodes[i])
    {
      return true;
    }
  }
  Serial.print("NOT RESUMABLE: ");
  Serial.println(results.value, HEX);
  // должно ли это тут быть ?
  // resetIr();
  return false;
}

void resetIr()
{
  results.value = 0;
  irrecv.resume();
}

void setup() {
  delay(100);
  Serial.begin(9600);
  one_color_all(cHue, cSat, cVal);  // погасить все светодиоды
  LEDS.show();
  LEDS.setBrightness(MAX_BRIGHTNES);                   // ограничить максимальную яркость
  LEDS.addLeds<WS2811, LED_DT, GRB>(leds, LED_COUNT);  // настрйоки для нашей ленты
  irrecv.enableIRIn();
  //innitialize and count number of codes
  codesLenght = getSize(codes);
  rsmCodesLenght = getSize(rsmCodes);
}

void loop()
{
  Serial.println("welcome to the main Loop");
  if (pollIr())
  {
    if (validate(results.value))
    {
      receiver();
    }
  }
  delay(1000);
}
