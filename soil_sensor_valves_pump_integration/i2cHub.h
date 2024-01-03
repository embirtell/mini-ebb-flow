class I2CHub {
  private:
    char addr;
    char currentPort;
  
  public:
    void setup(char addr);
    void setPort(char port);
    char getCurrentPort();
};