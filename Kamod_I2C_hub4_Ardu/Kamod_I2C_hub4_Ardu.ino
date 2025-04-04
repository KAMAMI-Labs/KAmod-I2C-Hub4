//example code for KAmod I2C Hub4
//control 4x MCP23008 with the same I2C address
//ino board: ESP32-WROOM-DA Module

#include <Wire.h>
//#include <Adafruit_MCP23X17.h>
#include <Adafruit_MCP23X08.h>

#define DEBUG False

//LED
#define LED_PIN         2 
#define MY_DELAY        2000
#define TEST_LED_ON     1
#define TEST_LED_OFF    0

//I2C
#define I2C_SDA         33
#define I2C_SCL         32
#define I2C_FREQ        100000
#define MCP23008_ARD    0x20

#define TCA9546_ARD     0x70
#define TCA9546_CH0     (1<<0)
#define TCA9546_CH1     (1<<1)
#define TCA9546_CH2     (1<<2)
#define TCA9546_CH3     (1<<3)

TwoWire I2Cbus = TwoWire(0);
Adafruit_MCP23X08 MCPIO;
int i;
int sel;
int success;

//--------------------------------------------------
bool i2cHub(uint8_t ch_mask){
  I2Cbus.beginTransmission(TCA9546_ARD);
  I2Cbus.write(&ch_mask, 1);
  return I2Cbus.endTransmission(true);
}

//--------------------------------------------------
bool i2cCheck(uint8_t adr){
  I2Cbus.beginTransmission(adr);
  return I2Cbus.endTransmission(true);
}

//--------------------------------------------------
void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  delay(1000); // wait for native usb
  Serial.println(); Serial.println(); Serial.println();
  Serial.println("Hello. KAmod I2C Hub4 test");

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  delay(500);
  while(!I2Cbus.begin(I2C_SDA, I2C_SCL, I2C_FREQ)){
    Serial.println("I2C Bus init failed...");
  }

  success = 0;
  sel = 3;

  while(1){
    sel++;
    if (sel > 3) {
      sel = 0;
      delay(500);
    }
    i2cHub(1<<sel);
    delay(20);

    if (!i2cCheck(MCP23008_ARD)){
      delay(20);
      if (MCPIO.begin_I2C(MCP23008_ARD, &I2Cbus)) {
        Serial.print("MCP init OK at channel ");
        success++;
      } else {
        Serial.print("MCP init FAIL at channel ");
      }
    } else {
      Serial.print("MCP Detect Failed at channel ");
    }
    Serial.print(sel);
    Serial.println();

    if ((sel == 3) && (success > 0)) break;
  }

  Serial.println("Setup finished");
  sel = 3;
  i = 8;
  success = 0;
}

//-------------------------------------------------
void loop() {
  i++;
  if (i  >= 8){
    i = 0;
    Serial.println(); Serial.println();
    while(1){
      sel++;
      if (sel > 3) {
        sel = 0;
        delay(500); 
      }

      i2cHub(1<<sel);
      if (!i2cCheck(MCP23008_ARD)){
        Serial.println(); Serial.println("##################");
        Serial.print("Select I2C channel ");
        Serial.print(sel); Serial.println();
        Serial.print("Out: ");
        success++;
        break;
      } else {
        Serial.print("MCP Detect Failed at channel ");
        Serial.print(sel); Serial.println();
        success = 0;
      }
      delay(50);
    }
  }

  if (i & 1){
    digitalWrite(LED_PIN, TEST_LED_ON);
  } else {
    digitalWrite(LED_PIN, TEST_LED_OFF);
  }
  
  Serial.print(i); Serial.print(" ");
  MCPIO.pinMode(i, OUTPUT);
  MCPIO.digitalWrite(i, TEST_LED_ON);
  delay(200);
  MCPIO.pinMode(i, OUTPUT);
  MCPIO.digitalWrite(i, TEST_LED_OFF);
  delay(100);

  if (success >= 12) {
    Serial.println(); Serial.println(); Serial.println();
    Serial.println("### Verify OK 3 times ###");
    Serial.println("### THATS ALL ###");
    while(1){};
  }
}
