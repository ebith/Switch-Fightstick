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
/*
struct controller {
  uint32_t keys;

  struct {
    uint16_t x;
    uint16_t y;
  } circlePad;

  struct {
    uint16_t x;
    uint16_t y;
  } cStick;
};
*/

typedef enum {
  SWITCH_Y       = 0x01,
  SWITCH_B       = 0x02,
  SWITCH_A       = 0x04,
  SWITCH_X       = 0x08,
  SWITCH_L       = 0x10,
  SWITCH_R       = 0x20,
  SWITCH_ZL      = 0x40,
  SWITCH_ZR      = 0x80,
  SWITCH_MINUS  = 0x100,
  SWITCH_PLUS   = 0x200,
  SWITCH_LCLICK  = 0x400,
  SWITCH_RCLICK  = 0x800,
  SWITCH_HOME    = 0x1000,
  SWITCH_CAPTURE = 0x2000,
  SWITCH_RELEASE = 0x00,
} JoystickButtons_t;

// Joystick HID report structure. We have an input and an output.
typedef struct {
  uint16_t Button; // 16 buttons; see JoystickButtons_t for bit mapping
  uint8_t  HAT;    // HAT switch; one nibble w/ unused nibble
  uint8_t  LX;     // Left  Stick X
  uint8_t  LY;     // Left  Stick Y
  uint8_t  RX;     // Right Stick X
  uint8_t  RY;     // Right Stick Y
  uint8_t  VendorSpec;
} USB_JoystickReport_Input_t;
