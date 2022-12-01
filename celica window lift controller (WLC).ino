/*
Controller to window lifters (2 auto Left + Right) for Toyota Celica T230

Sketch for arduino nano - AtMega 328/128


*/

#define relayLeftUp 16      //    Relay output LEFT UP (релейный выход левый вверх)
#define relayLeftDown 17    //  Relay output LEFT DOWN (релейный выход левый вниз)
#define relayRightUp 18     //   Relay output RIGHT UP (релейный выход правый вверх)
#define relayRightDown 19   // Relay output RIGHT DOWN (релейный выход правый вниз)
#define light_pin 8         //MOSFET output for LIGHT button (транзистор подсветки верхней платы)

#define ignition_pin 15     //   IGNITION INPUT (разрешение на работу кнопок от зажигания)
#define outsource_pin 5     //   Signal to close from Alarm System (пин от внешнего управления)
#define switchleft_pin 7    //  LEFT switch  (концевик левый)
#define switchright_pin 6   // RIGHT switch (концевик правывй)
#define leftAuto_pin 9      
#define leftUp_pin 10
#define leftDown_pin 11
#define rightAuto_pin 12
#define rightUp_pin 13
#define rightDown_pin 14



//******ADC (инициализация входов АЦП для контроля силы тока, для ограничения подъема)
#define left_hall A6 //      Датчик холла левый (-20а-0-20а 0в-2.5-5в) ACS712 20A
#define right_hall A7 //     Датчик холла правый (-20а-0-20а 0в-2.5-5в) ACS712 20A

/*Паспортные данные датчиков ХОЛЛА и АЦП Arduino. Не калибровочные значения!!!
      ACS712 30A 66mV/A
       CUR   VOLT     DAC
      -35A - 0.124V   40
      -30A - 0.52V    108
      -25A - 0.85V    175
      -20A - 1.18V    242
      -15A - 1.51V    310
      -10A - 1.84V    377
      -5A  - 2.17V    444
       0A  - 2.5V     512
       5A  - 2.83V    579
       10A - 3.16V    646
       15A - 3.49V    713
       20A - 3.82V    781
       25A - 4.15V    848
       30A - 4.48V    915
       35A - 4.81V    1023

      ACS712 20A 100mV/A
       CUR   VOLT     DAC
      -25A - 0V       0
      -20A - 0,5V     102
      -15A - 1V       205
      -10A - 1,5V     307
      -5A  - 2V       409
       0A  - 2,5V     512
       5A  - 3V       614
       10A - 3,5V     716
       15A - 4V       819
       20A - 4,5V     921
       25A - 5V       1023

      ACS712 5A 185mV/A
       CUR   VOLT     DAC
      -10A - 0,65V    146
      -5A  - 1,575V   329
       0A  - 2,5V     512
       5A  - 3,425V   694
       10A - 4,35V    1023

// для активации, на вход МК нужно подтянуть к земле)(PIN --- КНОПКА --- GND)
*/

#include <VirtualButton.h>
#include <GyverPower.h>

VButton ignSignal;
VButton LeftUp;
VButton LeftDown;
VButton LeftAuto;
VButton RightUp;
VButton RightDown;
VButton RightAuto;
VButton AutoCloseWind;
      



byte powermode = 0;

boolean unlock = 0;

byte autoClose = 0;
int curLeft = 0;
int curRight = 0;
//int curMax = 900; //АЦП для ограничения по току минимум 0 (0в), максимум 1023 (5в) 900~=4.18В
  
uint8_t stateLeft = 0;
uint8_t stateRight = 0;

unsigned long currentmillis = 0;
unsigned long timeLeft = 0;
unsigned long timeRight = 0;
unsigned long timerADC = 0;
unsigned long leftStart = 0;
unsigned long rightStart = 0;
unsigned long timerIgn = 0;


const static unsigned long timeAuto = 4500; //время отрабатывания авторежима, мсек
const static unsigned long timeAfterMax = 200; //время отрабатывания после ограничения по току, мсек
const static unsigned long timeButtonPush = 8000; //время отключения непрерывной работы кнопки, мсек
const static unsigned long timeAfterPower = 300000; // время удержания состояния работы после выключения IGN, мсек


void setup() {
  power.hardwareDisable(PWR_UART1|PWR_UART2|PWR_UART3|PWR_I2C|PWR_SPI|PWR_USB|PWR_TIMER2|PWR_TIMER3|PWR_TIMER4|PWR_TIMER5);
  power.setSleepMode(IDLE_SLEEP);

  
  Serial.begin(9600);
  buttons_init();
  relays_init();
//***************************
 //******Условие обработки зажатой кнопки, мсек
  LeftUp.setHoldTimeout(20); // Левый вверх
  LeftDown.setHoldTimeout(20); // Левый вниз
  LeftAuto.setHoldTimeout(20); // Левый авто
  RightUp.setHoldTimeout(20); // Правый вверх
  RightDown.setHoldTimeout(20); // Правый вниз
  RightAuto.setHoldTimeout(20);// Правый авто
  ignSignal.setHoldTimeout(300); // IGNition ON
 //***************************
  

//******СБРОС ВСЕХ РЕЛЕЙНЫХ ВЫВОДОВ 
 digitalWrite(relayLeftDown, LOW);
 digitalWrite(relayLeftUp, LOW);
 digitalWrite(relayRightDown, LOW);
 digitalWrite(relayRightUp, LOW);
}

void loop() {
  delay(5);
  checkInputs();
  work();
  if(unlock){

  digitalWrite(light_pin, HIGH);

    left(LeftUp.hold(), LeftDown.hold(), LeftAuto.hold());
    right(RightUp.hold(), RightDown.hold(), RightAuto.hold());

  if(LeftUp.press()||LeftDown.press())
    leftStart = millis();
  if(millis() - leftStart > timeButtonPush)
    stateLeft = 0;

  if(RightUp.press()||RightDown.press())
    rightStart = millis();
  if(millis() - rightStart > timeButtonPush)
    stateRight = 0;
  }

if(ignSignal.hold())
powermode = 1;

if(ignSignal.release())
powermode = 2;
//******Режим включения зажигания: 0 - Зажигание ВЫКЛ, 1 - Зажигание ВКЛ, 2 - Режим перехода из ВКЛ в ВЫКЛ (выдержка времени)
switch (powermode){
  case 0:
        power.sleep(SLEEP_FOREVER);
 //     Serial.println("*STANDBY*");
      break;
   case 1:
    unlock = 1;
    //Serial.println("-WORKED-");
    timerIgn = millis();
      break;
   case 2:
//    Serial.println("*POWER SAVE MODE*");
    if (millis() - timerIgn > timeAfterPower){
      Serial.println("<IGN TIME IS OVER>");
      digitalWrite(relayLeftDown, LOW);
      digitalWrite(relayLeftUp, LOW); 
      digitalWrite(relayRightDown, LOW);
      digitalWrite(relayRightUp, LOW); 
      digitalWrite(light_pin, LOW);
      unlock = 0;
      powermode = 0;
      power.sleep(SLEEP_FOREVER);
      }
      break;
  }
}
