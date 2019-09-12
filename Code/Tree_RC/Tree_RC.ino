#include <SPI.h>
#include <MFRC522.h>
#include <RCSwitch.h>
#include <Wire.h>
#include <SparkFun_APDS9960.h>
#include "knownTags.h"
#include "nbt.h"

#define RST_PIN   PA5     // Configurable, see typical pin layout above
#define SS_PIN    PA1    // Configurable, see typical pin layout above
#define RF_PIN    PA2
#define BUTTON_PIN  PB12
#define FRONT_LED_PIN PC14

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
RCSwitch mySwitch = RCSwitch();
SparkFun_APDS9960 apds = SparkFun_APDS9960();
uint16_t ambient_light = 0;
uint16_t red_light = 200;
uint16_t green_light = 200;
uint16_t blue_light = 0;

uint8_t ambient_light_reduced = 0;
uint8_t red_light_reduced = 200;
uint8_t green_light_reduced = 200;
uint8_t blue_light_reduced = 0;

int btn_cnt = 0;

static nbt_t button_mode_nbt;
static nbt_t button_nbt;

bool lastButtonState = true;
int last_time = millis();

void setup() {
  Serial.begin(9600);  // Initialize serial communications with the PC
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522 card
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FRONT_LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  mySwitch.enableTransmit(RF_PIN);
  // Initialize APDS-9960 (configure I2C and initial values)
  if ( apds.init() ) {
    Serial.println(F("APDS-9960 initialization complete"));
  } else {
    Serial.println(F("Something went wrong during APDS-9960 init!"));
  }
  
  // Start running the APDS-9960 light sensor (no interrupts)
  if ( apds.enableLightSensor(false) ) {
    Serial.println(F("Light sensor is now running"));
  } else {
    Serial.println(F("Something went wrong during light sensor init!"));
  }

  digitalWrite(LED_BUILTIN, LOW);
  NBT_init(&button_mode_nbt, 500);
  NBT_init(&button_nbt, 20);
}

void loop() {
  //Button Modes
  if (NBT_handler(&button_mode_nbt)){
  }

  if (NBT_handler(&button_nbt)){
    bool buttonState = digitalRead(BUTTON_PIN);
    
    if (!buttonState){
      digitalWrite(FRONT_LED_PIN, HIGH);
      Serial.println("BUTTON PRESSED");
      readRGBValues();
      if (btn_cnt==0){
        last_time = millis();
      }
    }
    else{
      digitalWrite(FRONT_LED_PIN, LOW);
      if ((millis() - last_time)>400){
        if (btn_cnt==4){
            Serial.println("BUTTON MODE 4");
        }
        else if (btn_cnt==5){
            Serial.println("BUTTON MODE 5");
        }
        btn_cnt = 0;
      }      
    }
    
    if ((buttonState != lastButtonState) && (millis() - last_time)<400) {
      Serial.println("SHORT BUTTON PRESSED");
      btn_cnt++;
    }

    lastButtonState = buttonState;
  }
  
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }

  // Dump UID
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  } 
  Serial.println();

  for (int k = 0; k<NR_KNOWN_KEYS; k++){
      byte x[7] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
      for (byte i = 0; i < 7; i++) {
            x[i] = knownKeys[k][i];
      }
      if (memcmp(mfrc522.uid.uidByte ,x, sizeof(x)) == 0) {
          int send_value = k << 24 | blue_light_reduced << 16 | green_light_reduced << 8 | red_light_reduced << 0;
          mySwitch.send(send_value, 32);
          Serial.print("R: ");Serial.print(red_light_reduced);Serial.print("\t");
          Serial.print("G: ");Serial.print(green_light_reduced);Serial.print("\t");
          Serial.print("B: ");Serial.print(blue_light_reduced);Serial.print("\t");
          Serial.print("LED num: ");Serial.print(k);Serial.print("\t");
          Serial.println(send_value);
          digitalWrite(LED_BUILTIN, HIGH);
          delay(200);
          digitalWrite(LED_BUILTIN, LOW);
          delay(200);
      }  
  }

}



void readRGBValues(){
  // Read the light levels (ambient, red, green, blue)
  if (  !apds.readAmbientLight(ambient_light) ||
        !apds.readRedLight(red_light) ||
        !apds.readGreenLight(green_light) ||
        !apds.readBlueLight(blue_light) ) {
    Serial.println("Error reading light values");
  } else {
//    Serial.print("Ambient: ");
//    Serial.print(ambient_light);
//    Serial.print(" Red: ");
//    Serial.print(red_light);
//    Serial.print(" Green: ");
//    Serial.print(green_light);
//    Serial.print(" Blue: ");
//    Serial.print(blue_light);

    ambient_light_reduced = (uint8_t)ambient_light;
    red_light_reduced = (uint8_t)red_light;
    green_light_reduced = (uint8_t)green_light;
    blue_light_reduced = (uint8_t)blue_light;

//    Serial.print("\tAmbient: ");
//    Serial.print(ambient_light_reduced);
//    Serial.print(" Red: ");
//    Serial.print(red_light_reduced);
//    Serial.print(" Green: ");
//    Serial.print(green_light_reduced);
//    Serial.print(" Blue: ");
//    Serial.println(blue_light_reduced);
  }
}
