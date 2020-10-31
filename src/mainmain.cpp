#include <Arduino.h>
#include <FastLED.h>
#include <IRremote.h> // библиотека для ИК
#include <avr/sleep.h> // библиотека для сна

//ИНТЕРФЕЙСЫ: ========================================================
#define LED = 2
// const int RECV = 3;

// Define IR Receiver and Results Objects
IRrecv irrecv(3);
decode_results results;

void setup()
{
     // Serial Monitor @ 9600 baud
  Serial.begin(9600);
  // Enable the IR Receiver
  irrecv.enableIRIn();
}

    void loop()
{
  if (irrecv.decode(&results)){
        switch(results.value){
        // 1 FFA25D
          case 0xFFA25D: 
        break;
        // 2 FF629D
          case 0xFF629D:
        break;
        // 3 FFE21D
          case 0xFFE21D:
        break;
        // 4 FF22DD
          case 0xFF22DD:
        break;
        // 5 FF02FD
          case 0xFF02FD:
        break;
        // 6 FFC23D
          case 0xFFC23D:
        break;
        // 7 FFE01F
          case 0xFFE01F:
        break;
        // 8 FFA857
          case 0xFFA857:
        break;
        // 9 FF906F
          case 0xFF906F:
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
    }
    irrecv.resume(); 
  }
}



