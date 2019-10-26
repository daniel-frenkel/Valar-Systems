// GPS code here
float lslat=-1; // NEVER use these variables in the code above, only below.  They are a security risk otherwise.
float lslon=-1;

float lat;
float lon;

BLYNK_WRITE(V127) {
  GpsParam gps(param);
  lslat=gps.getLat();
  lslon=gps.getLon();
  DEBUG_STREAM.print("Lat = ");
  DEBUG_STREAM.print(lslat);
  DEBUG_STREAM.print(", Lon = ");
  DEBUG_STREAM.println(lslon);
}
BLYNK_WRITE(V126) {
  if(param.asInt()!=0){
    DEBUG_STREAM.println();
    DEBUG_STREAM.print("Lat: ");
    DEBUG_STREAM.println(lslat, 7);
    DEBUG_STREAM.print("Lon: ");
    DEBUG_STREAM.println(lslon, 7);
    if(lslat==-1||lslon==-1){
      DEBUG_STREAM.println("Invalid!");
    }else{
      lon=lslon;
      lat=lslat;
      preferences.putFloat("lat", lat);
      preferences.putFloat("lon", lon);
    }
  }
}
