//#define TILT_PIN  2     // the number of tilt pin
//
//void TiltSensorSetup() {
//  pinMode(TILT_PIN, INPUT);
//}
//
//void TiltSensorLoop() {
//  // read tilt sensor
//  tiltSensor = digitalRead(TILT_PIN);
//  //set Tilt State
//  if (tiltCounts>TILT_THRESHOLD) setTiltState();
//  //timed reset of Tilt State
//  else if(tiltState && millis()-lastTiltTime>=RESET_TILT_STATE_TIMER) resetTiltState();
//  else if (tiltSensor == true) tiltCounts++;
//}
//
//void setTiltState(){
//  Serial.println("TILT");
//  lastTiltTime=millis();
//  tiltState=true;
//}
//
//void resetTiltState(){
//   tiltState=false;
//   tiltCounts=0;
//}

