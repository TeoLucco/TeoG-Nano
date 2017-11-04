void SerialLoop() {
  if (print) {
    print = false;
    if (workingCapacitives == body) {
      Serial.println("");
      Serial.print(millis()); Serial.print(",");
      for(int i=0;i<N_BODY_SENSORS-1;i++){
      Serial.print(bodySensorValue[i]); Serial.print(","); //Serial.print(calibration[i]); Serial.print(",");  
      } 
      Serial.print(bodySensorValue[N_BODY_SENSORS-1]);
      //Serial.print(millis() - startTouchingTime); 
    }
  }
  if (workingCapacitives == head) {
      Serial.print(headSensorValue[0]); Serial.print("    "); Serial.print(headSensorValue[1]); Serial.print("    ");
      Serial.print(headSensorValue[2]); Serial.print("    "); Serial.print(headSensorValue[3]); Serial.print("    "); Serial.println(pressedButton);
    }
  reciveSerial();
  sendSerial();
}

void reciveSerial() {
  if (Serial.available()) {
    int b = Serial.read();
    switch (b) {
      case 0: workingCapacitives = noOne; break;
      case 1: workingCapacitives = head;resetHeadCapacitives(); break;
      case 2: resetCapacitives();workingCapacitives = body; break;
      case 3: workingCapacitives = both; break;
    }
  }
}

void sendSerial() {
  if (workingCapacitives == head && pressedButton != -1) {
    Serial.write(4);
    Serial.write(pressedButton);
  }
  //i vari abbracci , carezze e colpi vengono scritti su seriale direttamente dalle funzioni di body_capacitives
  //scrivo 1 per abbraccio, 2 per carezza, 3 per colpo e 4 per bottone
}

