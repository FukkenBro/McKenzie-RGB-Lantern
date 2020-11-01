#include <Arduino.h>
#include <FastLED.h>
#include <IRremote.h> // библиотека для ИК
#include <avr/sleep.h> // библиотека для сна

//FastLED
#define LED_COUNT 3
#define LED_DT 2
#define MAX_BRIGHTNES 255
struct CRGB leds[LED_COUNT];

//ИНТЕРФЕЙСЫ: ========================================================

// const int RECV = 3;

// Define IR Receiver and Results Objects
IRrecv irrecv(3);
decode_results results;

// Иннициаллизация функций ========================================================
void animation1(byte speedMultiplier);
void animation2(byte hueDelta, byte speedMultiplier);
byte *Wheel(byte WheelPos);
void heat();
void fadeOut();

//глобальные переменные
// задержка для анимации RGB светодиодов [ms]
#define THIS_DELAY 300
byte *c;
byte cHue;
byte cSat;
byte cVal;

// закрасить все диоды ленты в один цвет
void one_color_all(int cred, int cgrn, int cblu)
{
  for (int i = 0; i < LED_COUNT; i++)
  {
    leds[i].setRGB(cred, cgrn, cblu);
  }
}

void showHSV(byte hue, byte sat, byte val){
  for (int i = 0; i < LED_COUNT; i++)
  {
    leds[i].setHSV(hue, sat, val);
  }
  LEDS.show();
}

void shiftHue(byte hue){
while (cHue!=hue){
  cHue++;
  showHSV(cHue,255,255);
  delay(10);
}
}

void setup()
{
  delay(100);
  Serial.begin(9600);
  one_color_all(0, 0, 0); // погасить все светодиоды
  LEDS.show();
  LEDS.setBrightness(MAX_BRIGHTNES);                  // ограничить максимальную яркость
  LEDS.addLeds<WS2811, LED_DT, GRB>(leds, LED_COUNT); // настрйоки для нашей ленты
  irrecv.enableIRIn();
  //  heat();
}

void loop()
{
  if (irrecv.decode(&results)){
        switch(results.value){
        // 1 FFA25D
          case 0xFFA25D:
          Serial.println("hue 0");
shiftHue(0);
        break;
        // 2 FF629D
          case 0xFF629D:
shiftHue(28);
        break;
        // 3 FFE21D
          case 0xFFE21D:
shiftHue(56);
        break;
        // 4 FF22DD
          case 0xFF22DD:
shiftHue(84);
        break;
        // 5 FF02FD
          case 0xFF02FD:
shiftHue(112);
        break;
        // 6 FFC23D
          case 0xFFC23D:
shiftHue(140);
        break;
        // 7 FFE01F
          case 0xFFE01F:
shiftHue(168);
        break;
        // 8 FFA857
          case 0xFFA857:
shiftHue(196);
        break;
        // 9 FF906F
          case 0xFF906F:
  shiftHue(224);
        break;
        // * FF6897
          case 0xFF6897:

        break;
        // 0 FF9867
          case 0xFF9867:

        break;
        // # FFB04F
          case 0xFFB04F:

        break;
        // UP FF18E7
          case 0xFF18E7:

        break;
        // LEFT FF10EF
          case 0xFF10EF:

        break;
        // OK FF38C7
          case 0xFF38C7:

        break;
        // RIGHT FF5AA5
          case 0xFF5AA5:

        break;
        // DOWN FF4AB5
          case 0xFF4AB5:

        break;
        // REPEAT FFFFFF
        case 0xFFFFFF:

        break;
    }
    Serial.println(results.value, HEX);
    irrecv.resume(); 
  }
}

//FUNCTIONS =======================================================================
void animation1(byte speedMultiplier)
{
  uint16_t i, j;
  for (j = 0; j < 256 * 1; j++)
  { // 5 cycles of all colors on wheel
    for (i = 0; i < LED_COUNT; i++){
      c = Wheel(((i * 256 / LED_COUNT) + j) & 255);
      one_color_all(*c, *(c + 1), *(c + 2));
    }
    FastLED.show();
    delay(THIS_DELAY / speedMultiplier);
  }
}

void animation2(byte hueDelta, byte speedMultiplier)
{
  uint16_t i, j;
  for (j = 0; j < 256 * 5; j++)
  { // 5 cycles of all colors on wheel
    for (i = 0; i < LED_COUNT; i++){
      c = Wheel(((i * 256 / hueDelta) + j) & 255);
      leds[i].setRGB(*c, *(c + 1), *(c + 2));
    }
    FastLED.show();
    delay(THIS_DELAY / speedMultiplier);
  }
}

byte *Wheel(byte WheelPos)
{
  static byte c[3];
  if (WheelPos < 85)
  {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  }
  else
  {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }
  return c;
}

void heat()
{
  for (int i = 0; i <= 255; i++)
  {
    for (int j = 0; j < LED_COUNT; j++)
    {
      one_color_all(0, 0, i);
    }
    FastLED.show();
    delay(5);
  }
}


