// #include <Arduino.h>
// #include <avr/sleep.h> // библиотека для сна

// //ИНТЕРФЕЙСЫ: ========================================================
// #define OUTPUT_PIN 10 // пин, к которому подключена нагрузка (PM пин таймера№1 PIN9 или PIN10)
// #define LED_PIN 9     // пин, к которому подключен светодиод индикатор
// #define SENSOR_PIN 0  // Аналоговый пин для подключения фотосенсора

// //НАСТРОИВАЕМЫЕ ПАРАМЕТРЫ:=========================================================
// #define HOLD_DURATION 150   //время удержания кнопки для регистрации длинного нажатия (default - 150)
// #define TIMER_DELAY 1800000 // задержка выключения [ms] (default 1 800 000ms = 30min)
// #define THIS_DELAY 300      // задержка для анимации RGB светодиодов [ms]
// #define MAX_VAL 1023        // 10bit - 1023, 8bit - 255;
// #define MIN_VAL 0           // задержка для анимации RGB светодиодов [ms]

// //ИННЦИАЛИЗАЦИЯ ПЕРЕМЕННЫХ: =========================================================
// int potVal;    //loop() показания потенциометра
// int senVal;    //loop() показания фотосенсора
// bool prevFlag; //
// int prevBr;    //
// int br;        //

// unsigned long brDelay = 500; // mode2()
// unsigned long brStart;       // mode2()

// unsigned long start = 0;     // debounce() стартовая позиция для отсчёта прерывания на основе millis() в методе debounce();
// #define DEBOUNCE_DELAY 50    // debounce() гистерезис функции debounce
// volatile int pressTimer = 0; // debounce()

// unsigned long shutDown = 0; // poll() время выключения

// #define PULSE_BRIGHTNES 100 // pulse() Яркость мигающего диода таймера
// unsigned long pulseDelay;   // pulse()
// unsigned long pulseStart;   // pulse()
// int pulseFade = 0;         // pulse()
// int pulseFadeStep = 0;     // pulse()
// int mode = 1;               // pulse()

// // Button input related values
// static const int STATE_NORMAL = 0; // interrupt() no button activity
// static const int STATE_SHORT = 1;  // interrupt() short button press
// static const int STATE_LONG = 2;   // interrupt() long button press
// volatile int resultButton = 0;     // interrupt() global value set by checkButton()

// //FLAGS: ==========================================================================
// // флаг для buttonRoutine()
// volatile bool modeFlag;
// bool glowFlag = false;
// bool timerFlag = false;
// bool pulseFlag = false;
// byte buttonState;

// // Иннициаллизация функций ========================================================
// void interrupt();
// bool debounce();
// void heat(int heatTo);
// void fadeOut(int fadeFrom);
// void pwrDown();
// void pulse();
// void poll();
// void blink();
// void kill();
// void onButtonClick();
// void onButtonHold();
// void mode1();
// void mode2();
// void pwm10bit(int pin,int value);

// //SETUP ===========================================================================
// void setup()
// {
//   delay(100);
//   Serial.begin(9600);
//   pinMode(BUTTON_PIN, INPUT_PULLUP); // defaul HIGH
//   pinMode(LED_PIN, OUTPUT);
//   pinMode(OUTPUT_PIN, OUTPUT);
//   // // Настройка ШИМ на пинах 9 и 10
//   // 10bit 15 625Hz
//   TCCR1A = TCCR1A & 0xe0 | 3;
//   TCCR1B = TCCR1B & 0xe0 | 0x09;
//   attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), interrupt, CHANGE); // triggers when LOW
//   potVal = analogRead(POT_PIN);
//   br = map(potVal, 0, 1023, 0, MAX_VAL);
//   prevBr = br;
//   heat(br);
// }

// //LOOP ============================================================================
// void loop()
// {
//   analog.update();
//   if (timerFlag == true)
//   {
//     pulse();
//     poll();
//   }

//   if (modeFlag)
//   {
//     mode1();
//     delay(10);
//   }
//   else if (!modeFlag)
//   {
//     // (defaul режим)
//     mode2();
//     delay(10);
//   }
// }

// //FUNCTIONS =======================================================================

// //Адаптивная яркость
// void mode1()
// {
//   potVal = analogRead(POT_PIN);
//   senVal = constrain(map(analogRead(SENSOR_PIN), 0, 1023, 1023, 0) - 100, 0, 1023);
//   // Serial.print("MODE1  ");
//   // Serial.print("Potentiometer val = ");
//   // Serial.println(potVal);
//   // Serial.print("ModeFlag = ");
//   // Serial.println(modeFlag);
//   // Serial.print("Sensor val = ");
//   // Serial.println(senVal);
//   if (senVal >= potVal)
//   {
//     //Если темно
//     glowFlag = true;
//     br = MAX_VAL;
//   }
//   else if (senVal < potVal)
//   {
//     //Если светло
//     glowFlag = false;
//     br = 0;
//   }
//   // Serial.print("\nglowFlag vs prevGlowFlag ");
//   // Serial.print(glowFlag);
//   // Serial.print(" vs ");
//   // Serial.print(prevFlag);
//   // Serial.print("\n");
//   if (prevFlag != glowFlag)
//   {
//     if (!prevFlag)
//     {
//       heat(MAX_VAL);
//     }
//     else if (prevFlag)
//     {
//       fadeOut(MAX_VAL);
//     }
//   }
//   pwm10bit(OUTPUT_PIN, br);
//   prevFlag = glowFlag;
//   prevBr = br;

//   // Serial.print("Brightness = ");
//   // Serial.println(br);
//   // Serial.println(" ");
// }

// //Ручная настройка яркости (defaul режим)
// void mode2()
// {  //body
//     potVal = analog.getValue();
//     br = potVal;
//     if (br <= 3)
//     {
//       br = 0;
//     }
//     pwm10bit(OUTPUT_PIN,br);
//     Serial.println(br);
// }

// ////

// void heat(int heatTo)
// {
//   for (int i = 0; i < heatTo; i++)
//   {
//     pwm10bit(OUTPUT_PIN, i);
//     delay(1);
//   }
// }

// void fadeOut(int fadeFrom)
// {
//   for (int i = fadeFrom; i > 0; i--)
//   {
//     pwm10bit(OUTPUT_PIN, i);
//     delay(1);
//   }
// }

// void blink()
// {
//   int blinkTimes = 2;
//   for (int j = 0; j < blinkTimes; j++)
//   {
//     for (size_t i = 0; i <= 125; i++)
//     {
//       pwm10bit(OUTPUT_PIN, i);
//       delay(1);
//     }
//     for (int i = 125; i > 50; i--)
//     {
//       pwm10bit(OUTPUT_PIN, i);
//       delay(2);
//     }
//   }
// }

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////

// void interrupt()
// {
//   /*
//   * This function implements software debouncing for a two-state button.
//   * It responds to a short press and a long press and identifies between
//   * the two states. Your sketch can continue processing while the button
//   * function is driven by pin changes.
//   */

//   const unsigned long LONG_DELTA = 500ul;               // hold seconds for a long press
//   const unsigned long DEBOUNCE_DELTA = 50ul;            // debounce time
//   static int lastButtonStatus = HIGH;                   // HIGH indicates the button is NOT pressed
//   int buttonStatus;                                     // button atate Pressed/LOW; Open/HIGH
//   static unsigned long longTime = 0ul, shortTime = 0ul; // future times to determine if button has been poressed a short or long time
//   boolean Released = true, Transition = false;          // various button states
//   boolean timeoutShort = false, timeoutLong = false;    // flags for the state of the presses

//   buttonStatus = digitalRead(BUTTON_PIN); // read the button state on the pin "BUTTON_PIN"
//   timeoutShort = (millis() > shortTime);  // calculate the current time states for the button presses
//   timeoutLong = (millis() > longTime);

//   if (buttonStatus != lastButtonStatus)
//   { // reset the timeouts if the button state changed
//     shortTime = millis() + DEBOUNCE_DELTA;
//     longTime = millis() + LONG_DELTA;
//   }

//   Transition = (buttonStatus != lastButtonStatus);   // has the button changed state
//   Released = (Transition && (buttonStatus == HIGH)); // for input pullup circuit

//   lastButtonStatus = buttonStatus; // save the button status

//   if (!Transition)
//   { //without a transition, there's no change in input
//     // if there has not been a transition, don't change the previous result
//     resultButton = STATE_NORMAL | resultButton;
//     return;
//   }

//   if (timeoutLong && Released)
//   {                                           // long timeout has occurred and the button was just released
//     resultButton = STATE_LONG | resultButton; // ensure the button result reflects a long press
//     onButtonHold();
//   }
//   else if (timeoutShort && Released)
//   {                                            // short timeout has occurred (and not long timeout) and button was just released
//     resultButton = STATE_SHORT | resultButton; // ensure the button result reflects a short press
//     onButtonClick();
//   }
//   else
//   {                                             // else there is no change in status, return the normal state
//     resultButton = STATE_NORMAL | resultButton; // with no change in status, ensure no change in button status
//   }
// }
// ////////////////////////////////////////////////////////////////////////////////////////////////////////////

// void pulse()
// {
//   if (pulseDelay != 0)
//   {
//     // мерцание
//     unsigned long current = millis();
//     if (abs(current - pulseStart) > pulseDelay)
//     {
//       if (pulseFlag == false)
//       {
//         pulseFade = constrain((pulseFade + pulseFadeStep), 0, 1018);
//         pwm10bit(LED_PIN, constrain((PULSE_BRIGHTNES - pulseFade), 1, 1023));
//         pulseFlag = true;
//       }
//       else
//       {
//         pwm10bit(LED_PIN, 0);
//         pulseFlag = false;
//         pulseStart = millis();
//       }
//     }
//   }
// }

// void poll()
// {
//   if (timerFlag == true && shutDown != 0)
//   { // если таймер влючен
//     if (millis() >= shutDown)
//     { // если время срабатывания таймера настало
//       pwrDown();
//     }
//   }
//   // Serial.print("До выключения (сек.) ");
//   // Serial.println((shutDown - millis()) / 1000);
// }

// void pwrDown()
// {
//   fadeOut(br);
//   pwm10bit(OUTPUT_PIN, 0);
//   pwm10bit(LED_PIN, 0);
//   timerFlag = false;
//   delay(1000);
//   kill();
// }

// void kill()
// {
//   buttonState = 3;
//   set_sleep_mode(SLEEP_MODE_PWR_DOWN);
//   sleep_enable();
//   sleep_cpu();
// }

// void onButtonClick()
// {
//   if (buttonState == 0)
//   {
//     pulseFade = 0;
//     buttonState += 1;
//     timerFlag = true;
//     shutDown = millis() + TIMER_DELAY;
//     pulseDelay = 500;
//     pulseFadeStep = 2;
//   }
//   else if (buttonState == 1)
//   {
//     pulseFade = 0;
//     buttonState += 1;
//     timerFlag = true;
//     shutDown = millis() + TIMER_DELAY * 2;
//     pulseDelay = 1000;
//     pulseFadeStep = 10;
//   }
//   else if (buttonState == 2)
//   {
//     // cброс всех флажков
//     timerFlag = false;
//     pwm10bit(LED_PIN, 0);
//     shutDown = 0;
//     pulseDelay = 0;
//     buttonState = 0;
//     pulseFade = 0;
//     pulseFadeStep = 0;
//   }
//   else if (buttonState == 3)
//   {
//     // cброс всех флажков
//     timerFlag = false;
//     pwm10bit(LED_PIN, 0);
//     shutDown = 0;
//     pulseDelay = 0;
//     buttonState = 0;
//     pulseFade = 0;
//     pulseFadeStep = 0;
//     asm volatile("  jmp 0");
//   }
//   else
//   {
//     // cброс всех флажков
//     timerFlag = false;
//     pwm10bit(LED_PIN, 0);
//     shutDown = 0;
//     pulseDelay = 0;
//     buttonState = 0;
//     pulseFade = 0;
//     pulseFadeStep = 0;
//     asm volatile("  jmp 0");
//   }
// }

// void onButtonHold()
// {
//   modeFlag = !modeFlag;
//   blink();
// }

// void pwm10bit(int pin, int value){
//   if(value==255){
//     value--;
//   }
//   analogWrite(pin,value);
// }
