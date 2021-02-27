#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
Encoder EncA(2, 3);
int motor = 9;
int motorback = 10;
float error = 0;
float wanted = 100; //770 = 1 tur
float ti = 99999999;
float td = 0;
float Kc = 0;
float Ki = 0;
float Kd = 0;
float integral;
float derivative;
int Ku;
float lasterror;
int pos = 0;
float last = 0;
float tim2 = 0;
int now = 0;
float timchang = 0;
int counter1 = 0;
float lastpos = 0;
float datas[]={0, 0, 0};
int sercount = 0;
int startcounter = 0;
void setup() {
  Serial.begin(115200);
  pinMode(motor, OUTPUT);
  pinMode(motorback, OUTPUT);
  // plx daq setup
  //Serial.println("CLEARDATA");
  //Serial.println("LABEL,Real Time,Timer, Position");
  //Serial.println("RESETTIMER");
  // plx daq setup end
}

void loop() {
  if (Serial.available() >= 1 && startcounter == 0){
    Kc = Serial.read();
    ti = 99999999;
    td = 0;
    startcounter++;
  }
  pos = EncA.read();
  error = wanted - pos;
  now = millis();
  timchang = now - last;
  //plx-daq code start
  //Serial.print("DATA,TIME,");
  //Serial.print(now);
  //Serial.print(",");
  //Serial.println(pos);
  //plx-daq code end
  integral += error * timchang * 0.001;
  Ki = error + (integral / ti);
  derivative = ((error - lasterror) / (timchang)) * 0.001;
  Kd = (derivative * td);
  Ku = (int)(Kc * (Ki + Kd));
  if (Ku >= 255) {
    Ku = 255;
  }
  if (-255 >= Ku) {
    Ku = -255;
  }
  if (Ku >= 0) {
    analogWrite(motor, Ku);
    analogWrite(motorback, 0);
  }
  if (0 >= Ku) {
    Ku = -Ku;
    analogWrite(motor, 0);
    analogWrite(motorback, Ku);
  }
  last = now;
  //tim2 += 5;
  //if (tim2 == 200) {
  //integral = 0;
  //}
  if (now >= 10000 && counter1 == 0) {
    wanted = 150;
    counter1 = 1;
  }
  if (now >= 10000) {
    Serial.print(now);
    Serial.print(",");
    Serial.println(pos);
  }
  lasterror = error;
  lastpos = pos;
  if (Serial.available() >= 3){
    for(int zz = 0; zz < 3; zz++){
      datas[zz] = Serial.read();
    } 
  }
  if (datas[2] != 0){
    integral = 0;
    wanted = 100;
    lasterror = 0;
    Kc = datas[0];
    ti = datas[1];
    td = datas[2];
  }
}


