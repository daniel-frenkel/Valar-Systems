class time_store{
  public:
    char day_sel[7];
    char active;
    int hour;
    int minute;
    int second;
    int type; // 0=time, 1=sunrise, 2=sunset
    long offset; // timezone offset
};

/*
struct resetStallguard {

  sendData(0x34 + 0x80, 0x000); // Disable stallguard /
  sendData(0x34 + 0x80, 0x400); // Enable stallguard /
  sendData(0x14 + 0x80,  MOVE_VELOCITY - 100); // writing value 0x00088888 = 559240 = 0.0 to address 11 = 0x14(TCOOLTHRS)
};
*/
