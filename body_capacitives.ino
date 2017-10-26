
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
  checkInteractions();
}
unsigned long int lastMeasurement=0;
boolean print=false;
boolean calibration[N_BODY_SENSORS]={false,false,false};
#define FREQUENCY 20
void updateCapacitiveStates() {
  if(millis()-lastMeasurement>FREQUENCY){
    lastMeasurement=millis();
    print=true;
  for (int i = 0; i < N_BODY_SENSORS; i++) {
    bodySensorValue_nf[i] = bodySensor[i]->capacitiveSensor(10);
    calibration[i]=bodySensor[i]->getCalibrate();
  }
  filtering();
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
  previousDynamicCapacitiveState[i] = capacitiveState[i];
  if (bodySensorValue[i] < lowBodyThreshold[i])
    updateState(no_touch, i);
  else if (bodySensorValue[i] >= lowBodyThreshold[i] && bodySensorValue[i] < highBodyThreshold[i])
    updateState(soft_touch, i);
  else if (bodySensorValue[i] >= highBodyThreshold[i])
    updateState(strong_touch, i);
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

unsigned long int startTouchingTime=0;
long maxValue = 0;
void waitTouch() {
  hugsCount = 0;
  maxValue=0;
  if ((bodySensorValue[2] >= lowBodyThreshold[2] && bodySensorValue[1] < lowBodyThreshold[1] && bodySensorValue[0] >= lowBodyThreshold[0]) ||
      (bodySensorValue[2] >= lowBodyThreshold[2] && bodySensorValue[1] >= lowBodyThreshold[1] && bodySensorValue[0] <= lowBodyThreshold[0]) ||
      (bodySensorValue[2] >= lowBodyThreshold[2] && bodySensorValue[1] >= lowBodyThreshold[1] && bodySensorValue[0] >= lowBodyThreshold[0])) {
    touchState = hug;
  }
  else if ((capacitiveState[0] == soft_touch || capacitiveState[0] == strong_touch) && capacitiveState[1] == no_touch && capacitiveState[2] == no_touch) {
    startTouchingTime = millis();
    maxValue = 0;
    touchState = hitPat0;
  }
  else if (capacitiveState[0] == no_touch && (capacitiveState[1] == soft_touch ||capacitiveState[1] == strong_touch) && capacitiveState[2] == no_touch) {
    startTouchingTime = millis();
    maxValue = 0;
    touchState = hitPat1;
  }
  else if (capacitiveState[0] == no_touch && capacitiveState[1] == no_touch && (capacitiveState[2] == soft_touch || capacitiveState[2] == strong_touch)) {
    startTouchingTime = millis();
    maxValue = 0;
    touchState = hitPat2;
  }
}

void checkHug() {
  if ((millis() - stateStartTime[2] >= HUGTIME * (hugsCount + 1) && bodySensorValue[2] >= lowBodyThreshold[2]) &&
      ((millis() - stateStartTime[0] >= HUGTIME * (hugsCount + 1) && bodySensorValue[0] >= lowBodyThreshold[0]) ||
       (millis() - stateStartTime[1] >= HUGTIME * (hugsCount + 1) && bodySensorValue[1] >= lowBodyThreshold[1]) )) {
    hugsCount++;
    //Serial.write(1);
    //Serial.println(hugsCount);
  }
  if (capacitiveState[2] == no_touch && capacitiveState[1] == no_touch && capacitiveState[0] == no_touch)
    touchState = nothing;
}


void checkHitPat0() {
  if (bodySensorValue[0] > maxValue) maxValue = bodySensorValue[0];
  if ((previousDynamicCapacitiveState[0] == soft_touch || previousDynamicCapacitiveState[0] == strong_touch) && previousDynamicCapacitiveState[1] == no_touch && previousDynamicCapacitiveState[2] == no_touch &&
      capacitiveState[0] == no_touch && capacitiveState[1] == no_touch && capacitiveState[2] == no_touch) {
    patHitComplete(0);
  } else patHitStatusExitCond();
}

void checkHitPat1() {
  if (bodySensorValue[1] > maxValue) maxValue = bodySensorValue[1];
  if ((previousDynamicCapacitiveState[1] == soft_touch || previousDynamicCapacitiveState[1] == strong_touch) && previousDynamicCapacitiveState[0] == no_touch && previousDynamicCapacitiveState[2] == no_touch &&
      capacitiveState[1] == no_touch && capacitiveState[0] == no_touch && capacitiveState[2] == no_touch) {
    patHitComplete(1);
  } else patHitStatusExitCond();
}

void checkHitPat2() {
  if (bodySensorValue[2] > maxValue) maxValue = bodySensorValue[2];
  if ((previousDynamicCapacitiveState[2] == soft_touch || previousDynamicCapacitiveState[2] == strong_touch) && previousDynamicCapacitiveState[0] == no_touch && previousDynamicCapacitiveState[1] == no_touch &&
      capacitiveState[2] == no_touch && capacitiveState[0] == no_touch && capacitiveState[1] == no_touch) {
    patHitComplete(2);
  } else patHitStatusExitCond();
}

void patHitComplete(int i){
if (millis() - startTouchingTime > MAX_HIT_TIME) {
      patComplete(i);
    } else {
      if (maxValue > highBodyThreshold[i]) hitComplete(i);
      else patComplete(i);
    }
}

void patComplete(int i) {
  touchState = nothing;
  resetCapacitives();
  //Serial.println();
  Serial.print(",PAT");
  //Serial.println();
  //Serial.write(2);
  //Serial.write(i);

}

void hitComplete(int i) {
  touchState = nothing;
  resetCapacitives();
  //Serial.println();
  Serial.print(",HIT");
  //Serial.println();
  //Serial.write(3);
  //Serial.write(i);
}

void patHitStatusExitCond() {
  if ((bodySensorValue[2] >= lowBodyThreshold[2] && bodySensorValue[1] < lowBodyThreshold[1] && bodySensorValue[0] >= lowBodyThreshold[0]) ||
      (bodySensorValue[2] >= lowBodyThreshold[2] && bodySensorValue[1] >= lowBodyThreshold[1] && bodySensorValue[0] <= lowBodyThreshold[0]) ||
      (bodySensorValue[2] >= lowBodyThreshold[2] && bodySensorValue[1] >= lowBodyThreshold[1] && bodySensorValue[0] >= lowBodyThreshold[0])) {
    touchState = hug;
    startTouchingTime=millis();
  } else if (capacitiveState[2] == no_touch && capacitiveState[1] == no_touch && capacitiveState[0] == no_touch)
    touchState = nothing;

}

void resetCapacitives() {
  for (int i = 0; i < N_BODY_SENSORS; i++) {
    bodySensor[i]->reset_CS_AutoCal();
    calibration[i]=bodySensor[i]->getCalibrate();
  }
}



