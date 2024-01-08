#ifndef MY_PUMP_H
#define MY_PUMP_H

class Pump {
  private:
    char pin;
    bool running;

  public:
    void setup(char pin);
    bool getRunning();
    void blink(int ms);
    void blink();
    void open();
    void close();
};
#endif
