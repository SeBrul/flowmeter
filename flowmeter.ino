#include <Adafruit_ZeroTimer.h>

#include <power.h>
#include <compiler.h>
#include <interrupt_sam_nvic.h>
#include <status_codes.h>
#include <i2s.h>
#include <pinmux.h>
#include <clock_feature.h>
#include <parts.h>
#include <gclk.h>
#include <Adafruit_ASFcore.h>
#include <system_interrupt_features.h>
#include <interrupt.h>
#include <system.h>
#include <system_interrupt.h>
#include <reset.h>
#include <clock.h>

//#include "LiquidCrystal.h"
//LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// which pin to use for reading the sensor? can use any pin!
#define FLOWSENSORPIN 2

// count how many pulses!
volatile uint16_t pulses = 0;
// track the state of the pulse pin
volatile uint8_t lastflowpinstate;
// you can try to keep time of how long it is between pulses
volatile uint32_t lastflowratetimer = 0;
// and use that to calculate a flow rate
volatile float flowrate;

void ISR_timer(struct tc_module *const module_inst) 
{ uint8_t x = digitalRead(FLOWSENSORPIN);
  
  if (x == lastflowpinstate) {
   lastflowratetimer++;
   return; // nothing changed! 
 }
 
  if (x == HIGH) {
   //low to high transition!
   pulses++;
}

//// Interrupt is called once a millisecond, looks for any pulses from the sensor!
//SIGNAL(TIMER2_COMPA_vect) {
//  uint8_t x = digitalRead(FLOWSENSORPIN);
//  
//  if (x == lastflowpinstate) {
//   lastflowratetimer++;
//   return; // nothing changed! 
// }
// 
//  if (x == HIGH) {
//   //low to high transition!
//   pulses++;
// }
 
 lastflowpinstate = x;
 flowrate = 1000.0;
 flowrate /= lastflowratetimer; // in hertz
 lastflowratetimer = 0;
}
//void useInterrupt(boolean v) {
// if (v) {
//   // Timer0 is already used for millis() - we'll just interrupt somewhere
//   // in the middle and call the "Compare A" function above
//   OCR0A = 0xAF;
//   TIMSK0 |= _BV(OCIE0A);
// } else {
//   // do not call the interrupt function COMPA anymore
//   TIMSK0 &= ~_BV(OCIE0A);
// }
//}

void setup() {
  Serial.begin(9600);
  Serial.print("Flow sensor test!");
  //lcd.begin(16, 2);
  
  pinMode(FLOWSENSORPIN, INPUT);
  digitalWrite(FLOWSENSORPIN, HIGH);
  lastflowpinstate = digitalRead(FLOWSENSORPIN);
//  useInterrupt(true);
}

void loop() // run over and over again
{
 //lcd.setCursor(0, 0);
 //lcd.print("Pulses:"); lcd.print(pulses, DEC);
 //lcd.print(" Hz:");
 //lcd.print(flowrate);
 //lcd.print(flowrate);
 Serial.print("Freq: "); Serial.println(flowrate);
 Serial.print("Pulses: "); Serial.println(pulses, DEC);
 
 // if a plastic sensor use the following calculation
 // Sensor Frequency (Hz) = 7.5 * Q (Liters/min)
 // Liters = Q * time elapsed (seconds) / 60 (seconds/minute)
 // Liters = (Frequency (Pulses/second) / 7.5) * time elapsed (seconds) / 60
 // Liters = Pulses / (7.5 * 60)
 float liters = pulses;
 liters /= 7.5;
 liters /= 60.0;

/*
// if a brass sensor use the following calculation
float liters = pulses;
liters /= 8.1;
liters -= 6;
liters /= 60.0;
*/
 Serial.print(liters); Serial.println(" Liters");
// lcd.setCursor(0, 1);
// lcd.print(liters); lcd.print(" Liters ");

 delay(100);
}
