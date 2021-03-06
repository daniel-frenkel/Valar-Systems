void setZero(){
      XACTUAL=0;
      sendData(0x21+0x80, 0);      // XACTUAL=0
      sendData(0x2D+0x80, 0);      // XTARGET=0
      preferences_local.putInt("XACTUAL", 0);
      Blynk.virtualWrite(V46, "CLOSED");
      Blynk.virtualWrite(V23, 0);
      Serial.println("XACTUAL: ");
      Serial.println(sendData(0x21+0x80, 0));
}
