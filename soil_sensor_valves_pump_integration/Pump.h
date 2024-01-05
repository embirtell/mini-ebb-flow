#ifndef MY_PUMP_H
#define MY_PUMP_H

class Pump {
  private:
    char pin;

  public:
    void setup(char pin);
    void blink(int ms);
    void blink();
    void open();
    void close();
};
#endif
