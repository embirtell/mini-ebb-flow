#ifndef MY_PUMP_H
#define MY_PUMP_H

class Pump {
  private:
    byte pin;

  public:
    Pump(byte pin);

    void blink(int ms);
    void blink();
    void open();
    void close();
};
#endif
