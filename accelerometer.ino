#define THRESHOLD 35
#define DURATION 1
#define RESET_TILT_STATE_TIMER 1000
boolean tiltState =false;
unsigned long int lastTiltTime=0;


#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

void AccSetup() 
{
  
  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_16G))
  {
    delay(500);
  }

  mpu.setAccelPowerOnDelay(MPU6050_DELAY_3MS);

  mpu.setIntFreeFallEnabled(false);  
  mpu.setIntZeroMotionEnabled(false);
  mpu.setIntMotionEnabled(false);
  
  mpu.setDHPFMode(MPU6050_DHPF_5HZ);

  mpu.setMotionDetectionThreshold(THRESHOLD);
  mpu.setMotionDetectionDuration(DURATION);
  
  //checkSettings();
}

void accLoop()
{
  Vector rawAccel = mpu.readRawAccel();
  Activites act = mpu.readActivites();

  if (act.isActivity) setTiltState();
  else if(tiltState && millis()-lastTiltTime>=RESET_TILT_STATE_TIMER) resetTiltState();
  
}

void setTiltState(){
//  Serial.println("TILT");
  //resetCapacitives();
  lastTiltTime=millis();
  tiltState=true;
}

void resetTiltState(){
  resetCapacitives();
   tiltState=false;
}

////##########################################################################################################################################################################
//void accSetup2() 
//{
//  while(!mpu.begin(MPU6050_SCALE_2000DPS, MPU6050_RANGE_2G))
//  {
//    delay(500);
//  }
//
//  // If you want, you can set accelerometer offsets
//  // mpu.setAccelOffsetX();
//  // mpu.setAccelOffsetY();
//  // mpu.setAccelOffsetZ();
//  
//}
//
//void accLoop2()
//{
////  Vector rawAccel = mpu.readRawAccel();
//  Vector normAccel = mpu.readNormalizeAccel();
//
////  Serial.print(" Xraw = ");
////  Serial.print(rawAccel.XAxis);
////  Serial.print(" Yraw = ");
////  Serial.print(rawAccel.YAxis);
////  Serial.print(" Zraw = ");
////  Serial.println(rawAccel.ZAxis);
//  
////  Serial.print(" Xnorm = ");
////  Serial.print(normAccel.XAxis);
////  Serial.print(" Ynorm = ");
////  Serial.print(normAccel.YAxis);
////  Serial.print(" Znorm = ");
////  Serial.println(normAccel.ZAxis);
//// if (normAccel.XAxis>THRESH || normAccel.YAxis>THRESH || normAccel.ZAxis>THRESH) setTiltState();
////  else if(tiltState && millis()-lastTiltTime>=RESET_TILT_STATE_TIMER) resetTiltState();
//}

