#include "CO2Sensor.h"

#include <MQUnifiedsensor.h>


#define Board "ESP-32"
#define Pin_MQ5 34
#define Voltage_Resolution 5
#define Type_MQ5 "MQ-5"
#define ADC_Bit_Resolution 12
#define RatioMQ5CleanAir 6.5
#define Pin_MQ136 35
#define Type_MQ136 "MQ-136"
#define RatioMQ136CleanAir 3.6
#define Pin_CO2 32

MQUnifiedsensor MQ5(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin_MQ5, Type_MQ5);
MQUnifiedsensor MQ136(Board, Voltage_Resolution, ADC_Bit_Resolution, Pin_MQ136, Type_MQ136);
CO2Sensor co2Sensor (Pin_CO2, 0.99, 100);

void setup() {

  Serial.begin(9600);
  delay(10);

  MQ5_Calibration();
  MQ136_Calibration();
  co2Sensor.calibrate();

}

void loop() {
  displayMQ5();
  delay(500);
  displayMQ136();
  delay(500);
  displayCO2();
  delay(500);
}

void MQ5_Calibration() {
  MQ5.setRegressionMethod(1);
  MQ5.setA(177.65);
  MQ5.setB(-2.56);

  MQ5.init();

  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ5.update(); 
    calcR0 += MQ5.calibrate(RatioMQ5CleanAir);
    Serial.print(".");
  }
  MQ5.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}
  MQ5.serialDebug(true);
}

void MQ136_Calibration() {
  MQ136.setRegressionMethod(1);
  MQ136.setA(36.737); MQ136.setB(-3.536);

  MQ136.init();

  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for(int i = 1; i<=10; i ++)
  {
    MQ136.update(); 
    calcR0 += MQ136.calibrate(RatioMQ136CleanAir);
    Serial.print(".");
  }
  MQ136.setR0(calcR0/10);
  Serial.println("  done!.");
  
  if(isinf(calcR0)) {Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply"); while(1);}
  if(calcR0 == 0){Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply"); while(1);}

  MQ136.serialDebug(true);
}

void displayMQ5() {
  MQ5.update();
  Serial.print("MQ-5: ");
  Serial.print(MQ5.readSensor());
  Serial.println(" PPM");
}

void displayMQ136() {
  MQ136.update();
  Serial.print("MQ-136: ");
  Serial.print(MQ136.readSensor());
  Serial.println(" PPM");
}

void displayCO2() {
  int ppm = co2Sensor.read();

  Serial.print("CO2 Concentration: ");
  Serial.print(ppm);
  Serial.println("PPM");
  delay(1000);
}
