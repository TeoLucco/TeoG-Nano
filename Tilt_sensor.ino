#define TILT_PIN  2     // the number of tilt pin
#define RESET_TILT_STATE_TIMER 500

boolean tiltSensor = false;         // variable for reading the pushbutton status
boolean tiltState =false;
unsigned int tiltCounts=0;
unsigned long int lastTiltTime=0;
void TiltSensorSetup() {
  pinMode(TILT_PIN, INPUT);
}

void TiltSensorLoop() {
  // read tilt sensor
  tiltSensor = digitalRead(TILT_PIN);
  //set/reset Tilt State
  if (tiltSensor == true) setTiltState();
  else resetTiltState();
}

void setTiltState(){
  Serial.println("TILT");
  lastTiltTime=millis();
  tiltState=true;
  tiltCounts++;
}


void resetTiltState(){
  if(tiltState && millis()-lastTiltTime>=RESET_TILT_STATE_TIMER){
    tiltState=false;
    tiltCounts=0;
  }
}
