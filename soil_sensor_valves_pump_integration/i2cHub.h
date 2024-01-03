class I2CHub {
  private:
    char addr;
    char currentPort;
  
  public:
    I2CHub(char addr);
    void setPort(char port);
    char getCurrentPort();
};