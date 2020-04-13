struct packet {
  struct packetHeader {
    unsigned char command;
    unsigned char keyboardActive;
  };
  struct packetHeader packetHeader;
  
  union {
    // CONNECT
    struct connectPacket {
    };
    struct connectPacket connectPacket;
    
    // KEYS
    struct keysPacket {
      unsigned int keys;
      
      struct {
        short x;
        short y;
      } circlePad;
      
      struct {
        unsigned short x;
        unsigned short y;
      } touch;
      
      struct {
        short x;
        short y;
      } cStick;
    };
    struct keysPacket keysPacket;
    
    // SCREENSHOT
    struct screenshotPacket {
      unsigned short offset;
      unsigned char data[4000]; //SCREENSHOT_CHUNK instead of 4000
    };
    struct screenshotPacket screenshotPacket;
  } myunion;
};
