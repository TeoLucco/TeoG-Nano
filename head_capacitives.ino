void headCapacitiveSetup() {
  headSensor[0] = new CapacitiveSensor(HEAD_BUTTON_0S, HEAD_BUTTON_0R);
  headSensor[1] = new CapacitiveSensor(HEAD_BUTTON_1S, HEAD_BUTTON_1R);
  headSensor[2] = new CapacitiveSensor(HEAD_BUTTON_2S, HEAD_BUTTON_2R);
  headSensor[3] = new CapacitiveSensor(HEAD_BUTTON_3S, HEAD_BUTTON_3R);
  for (int i = 0; i < N_HEAD_SENSORS; i++)                          
    headSensor[i]->set_CS_AutocaL_Millis(HEAD_SCALIBRATION_TIMER);
}

int FindMax(long ARRAY[], byte START, byte END) {
  long MAXIMUM, LOCATION;
  MAXIMUM = ARRAY[START];
  LOCATION = START;
  for (byte i = START + 1; i < END; i++)
  {
    if (ARRAY[i] > MAXIMUM)
    {
      MAXIMUM = ARRAY[i];
      LOCATION = i;
    }
  }
  return LOCATION;
}

void headCapacitiveLoop() {
  boolean checkMax = false;
  if (millis() - releaseButtonTime > 1000) {

    for (int i = 0; i < N_HEAD_SENSORS; i++) {
      headSensorValue[i] = headSensor[i]->capacitiveSensor(12);
      if (headSensorValue[i] > headThreshold && checkMax == false)
        checkMax = true;
    }
    if (checkMax) {
      pressedButton = FindMax(headSensorValue, 0, N_HEAD_SENSORS);
      releaseButtonTime = millis();
      resetHeadCapacitives();
    }
    else pressedButton = -1;
    checkMax = false;
  } else pressedButton = -1;
  delay(10);
}

void resetHeadCapacitives() {
  for (int i = 0; i < N_HEAD_SENSORS; i++) {
    headSensor[i]->reset_CS_AutoCal();
  }
}

