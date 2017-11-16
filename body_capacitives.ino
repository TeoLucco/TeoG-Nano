boolean touched[N_BODY_SENSORS] = {false, false, false};
int touchedCount = 0;

void bodyCapacitiveSetup() {
  bodySensor[0] = new CapacitiveSensor(BODY_SX_S, BODY_SX_R); //sx
  bodySensor[1] = new CapacitiveSensor(BODY_DX_S, BODY_DX_R); //dx
  bodySensor[2] = new CapacitiveSensor(BODY_FRONT_S, BODY_FRONT_R); //front
  for (int i = 0; i < N_BODY_SENSORS; i++) {
    bodySensor[i]->set_CS_AutocaL_Millis(BODY_SCALIBRATION_TIMER);
    capacitiveState[i] = no_touch;
  }
}

void bodyCapacitiveLoop() {
  updateCapacitiveStates();
  accLoop();
  if(millis()-bodyStartTime>RESTABILIZATION_TIME){
    checkInteractions();
  }
}
unsigned long int lastMeasurement = 0;
boolean print = false;
boolean calibration[N_BODY_SENSORS] = {false, false, false};
#define FREQUENCY 20
#define SCALIBRATION_VALUE 400
unsigned long int startTime[4]={0,0,0,0};
boolean overTheLimit[4]={false,false,false,false};

void updateCapacitiveStates() {
  if (millis() - lastMeasurement > FREQUENCY) {
    lastMeasurement = millis();
    print = true;
    for (int i = 0; i < N_BODY_SENSORS; i++) {
      bodySensorValue_nf[i] = bodySensor[i]->capacitiveSensor(10);
//      if (bodySensorValue_nf[i] >= SCALIBRATION_VALUE && !overTheLimit[i]) {
//        startTime[i] = millis();
//        overTheLimit[i] = true;
//      }
//      if (bodySensorValue_nf[i] < SCALIBRATION_VALUE && overTheLimit[i])
//        overTheLimit[i] = false;
//      if (overTheLimit[i] && millis() - startTime[i] > BODY_SCALIBRATION_TIMER) {
//        //Serial.println("AUTOCALIBRAZIONE");
//        bodySensor[i]->reset_CS_AutoCal();
//        overTheLimit[i] = false;
//      }
    }
    filtering();
    touchedCount = 0;
    for (int i = 0; i < N_BODY_SENSORS; i++) {
      updateBodyState(i);
    }
  }
}

void filtering() {
  left_body_f.input(bodySensorValue_nf[0]);
  right_body_f.input(bodySensorValue_nf[1]);
  front_body_f.input(bodySensorValue_nf[2]);
  bodySensorValue[0] = left_body_f.output();
  bodySensorValue[1] = right_body_f.output();
  bodySensorValue[2] = front_body_f.output();
}

void updateBodyState(int i) {
  touched[i] = false;
  previousDynamicCapacitiveState[i] = capacitiveState[i];
  if (bodySensorValue[i] < lowBodyThreshold[i])
    updateState(no_touch, i);
  else if (bodySensorValue[i] >= lowBodyThreshold[i] && bodySensorValue[i] < highBodyThreshold[i])
    updateState(soft_touch, i);
  else if (bodySensorValue[i] >= highBodyThreshold[i])
    updateState(strong_touch, i);
  if (capacitiveState[i] == soft_touch || capacitiveState[i] == strong_touch) {
    touched[i] = true;
    touchedCount++;
  }
}

void checkInteractions() {
  switch (touchState) {
    case nothing: waitTouch(); break;
    case hug:     checkHug(); break;
    case hitPat0:    checkHitPat0(); break;
    case hitPat1:    checkHitPat1(); break;
    case hitPat2:    checkHitPat2(); break;
  }
}
unsigned long int lastTilt=0;
unsigned long int startTouchingTime = 0;
long maxValue = 0;
void waitTouch() {
  hugsCount = 0;
  maxValue = 0;
  if (!tiltState && ((bodySensorValue[2] >= lowBodyThreshold[2] && bodySensorValue[1] <  lowBodyThreshold[1] && bodySensorValue[0] >= lowBodyThreshold[0]) ||
      (bodySensorValue[2] >= lowBodyThreshold[2] && bodySensorValue[1] >= lowBodyThreshold[1] && bodySensorValue[0] <  lowBodyThreshold[0]) ||
      (bodySensorValue[2] <  lowBodyThreshold[2] && bodySensorValue[1] >= lowBodyThreshold[1] && bodySensorValue[0] >= lowBodyThreshold[0]) ||
      (bodySensorValue[2] >= lowBodyThreshold[2] && bodySensorValue[1] >= lowBodyThreshold[1] && bodySensorValue[0] >= lowBodyThreshold[0]))) {
    touchState = hug;
    startTouchingTime = millis();
  }
  else if ((capacitiveState[0] == soft_touch || capacitiveState[0] == strong_touch) && capacitiveState[1] == no_touch && capacitiveState[2] == no_touch) {
    startTouchingTime = millis();
    maxValue = 0;
    touchState = hitPat0;
  }
  else if (capacitiveState[0] == no_touch && (capacitiveState[1] == soft_touch || capacitiveState[1] == strong_touch) && capacitiveState[2] == no_touch) {
    startTouchingTime = millis();
    maxValue = 0;
    touchState = hitPat1;
  }
  else if (capacitiveState[0] == no_touch && capacitiveState[1] == no_touch && (capacitiveState[2] == soft_touch || capacitiveState[2] == strong_touch)) {
    startTouchingTime = millis();
    maxValue = 0;
    touchState = hitPat2;
  }else if (tiltState && capacitiveState[0] == no_touch && capacitiveState[1] == no_touch && capacitiveState[2] == no_touch){
    //if(millis()-lastTilt>2000){
      //lastTilt=millis();
      resetTiltState();
      hitComplete(3);
     //}
  }
}

void checkHug() {
//  if(touchedCount>1){for (int i = 0; i < N_BODY_SENSORS; i++) bodySensor[i]->resetLastCall();}else{resetCapacitives();}
    if (  ((millis() - stateStartTime[2] >= HUGTIME * (hugsCount + 1) && bodySensorValue[2] >= lowBodyThreshold[2])
            && (millis() - stateStartTime[0] >= HUGTIME * (hugsCount + 1) && bodySensorValue[0] >= lowBodyThreshold[0]))
        ||((millis() - stateStartTime[2] >= HUGTIME * (hugsCount + 1) && bodySensorValue[2] >= lowBodyThreshold[2])
            && (millis() - stateStartTime[1] >= HUGTIME * (hugsCount + 1) && bodySensorValue[1] >= lowBodyThreshold[1]))
        ||((millis() - stateStartTime[0] >= HUGTIME * (hugsCount + 1) && bodySensorValue[0] >= lowBodyThreshold[0])
            && (millis() - stateStartTime[1] >= HUGTIME * (hugsCount + 1) && bodySensorValue[1] >= lowBodyThreshold[1]))){
//  if ( millis() - startTouchingTime >= HUGTIME && (   (bodySensorValue[2] >= lowBodyThreshold[2] && bodySensorValue[0] >= lowBodyThreshold[0])
//       || (bodySensorValue[2] >= lowBodyThreshold[2] && bodySensorValue[1] >= lowBodyThreshold[1])
//       || (bodySensorValue[1] >= lowBodyThreshold[1] && bodySensorValue[0] >= lowBodyThreshold[0]) )) {
    hugsCount++;
    startTouchingTime = millis();
    Serial.write(9);
    Serial.write(-1);
//        Serial.println();
//        Serial.println();
//        Serial.print("HUG ");
//        Serial.println(hugsCount);
//        Serial.println();
//        Serial.println();
  }
  if (capacitiveState[2] == no_touch && capacitiveState[1] == no_touch && capacitiveState[0] == no_touch)
    touchState = nothing;
  if (hugsCount == 8) resetCapacitives();
}


void checkHitPat0() {
  //if (bodySensorValue[0] > maxValue) maxValue = bodySensorValue[0];
  if ((previousDynamicCapacitiveState[0] == soft_touch || previousDynamicCapacitiveState[0] == strong_touch) && previousDynamicCapacitiveState[1] == no_touch && previousDynamicCapacitiveState[2] == no_touch &&
      capacitiveState[0] == no_touch && capacitiveState[1] == no_touch && capacitiveState[2] == no_touch) {
    patHitComplete(0);
  } else if(millis()-startTouchingTime>MAX_PAT_TIME){
    resetCapacitives();
    touchState = nothing;
  }else if (tiltState){
//    if(millis()-lastTilt>2000){
//      lastTilt=millis();
      resetTiltState();
      hitComplete(0);
     //}
  } else patHitStatusExitCond();
}

void checkHitPat1() {
  //if (bodySensorValue[1] > maxValue) maxValue = bodySensorValue[1];
  if ((previousDynamicCapacitiveState[1] == soft_touch || previousDynamicCapacitiveState[1] == strong_touch) && previousDynamicCapacitiveState[0] == no_touch && previousDynamicCapacitiveState[2] == no_touch &&
      capacitiveState[1] == no_touch && capacitiveState[0] == no_touch && capacitiveState[2] == no_touch) {
    patHitComplete(1);
  } else if(millis()-startTouchingTime>MAX_PAT_TIME){
    resetCapacitives();
    touchState = nothing;
  }else if (tiltState){
//    if(millis()-lastTilt>2000){
//      lastTilt=millis();
      resetTiltState();
      hitComplete(1);
     //}
  } else patHitStatusExitCond();
}

void checkHitPat2() {
  //if (bodySensorValue[2] > maxValue) maxValue = bodySensorValue[2];
  if ((previousDynamicCapacitiveState[2] == soft_touch || previousDynamicCapacitiveState[2] == strong_touch) && previousDynamicCapacitiveState[0] == no_touch && previousDynamicCapacitiveState[1] == no_touch &&
      capacitiveState[2] == no_touch && capacitiveState[0] == no_touch && capacitiveState[1] == no_touch) {
    patHitComplete(2);
  } else if(millis()-startTouchingTime>MAX_PAT_TIME){
    resetCapacitives();
    touchState = nothing;
  }else if (tiltState){
//    if(millis()-lastTilt>2000){
//      lastTilt=millis();
      resetTiltState();
      hitComplete(2);
     //}
  }else patHitStatusExitCond();
}

void patHitComplete(int i) {
//  touchingTime = millis() - startTouchingTime;
//  if (touchingTime > MAX_HIT_TIME) {
    if(tiltState){
      resetTiltState();
      hitComplete(i);
    }else patComplete(i);
//  } else {
//    if (tiltState || maxValue > highBodyThreshold[i]){
//      resetTiltState();
//      hitComplete(i);
//    }
//    else patComplete(i);
//  }
}

void patComplete(int i) {
  touchState = nothing;
  resetCapacitives();
//  Serial.print(",PAT " + String(i) + "," + String(maxValue) + "," + String(touchingTime));
//    Serial.println();
//    Serial.print("PAT");
//    Serial.println();
    Serial.write(2);
    Serial.write(i);

}

void hitComplete(int i) {
  touchState = nothing;
  resetCapacitives();
//  Serial.print(",HIT " + String(i) + "," + String(maxValue) + "," + String(touchingTime));
//    Serial.println();
//    Serial.print(",HIT");
//    Serial.println();
    Serial.write(3);
    Serial.write(i);
}

void patHitStatusExitCond() {
  if ((bodySensorValue[2] >= lowBodyThreshold[2] && bodySensorValue[1] <  lowBodyThreshold[1] && bodySensorValue[0] >= lowBodyThreshold[0]) ||
      (bodySensorValue[2] >= lowBodyThreshold[2] && bodySensorValue[1] >= lowBodyThreshold[1] && bodySensorValue[0] <  lowBodyThreshold[0]) ||
      (bodySensorValue[2] <  lowBodyThreshold[2] && bodySensorValue[1] >= lowBodyThreshold[1] && bodySensorValue[0] >= lowBodyThreshold[0]) ||
      (bodySensorValue[2] >= lowBodyThreshold[2] && bodySensorValue[1] >= lowBodyThreshold[1] && bodySensorValue[0] >= lowBodyThreshold[0])) {
    touchState = hug;
    startTouchingTime = millis();
  } else if (capacitiveState[2] == no_touch && capacitiveState[1] == no_touch && capacitiveState[0] == no_touch)
    touchState = nothing;


}

void resetCapacitives() {
  for (int i = 0; i < N_BODY_SENSORS; i++) {
    bodySensor[i]->reset_CS_AutoCal();
    calibration[i] = bodySensor[i]->getCalibrate();
  }
}



