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
