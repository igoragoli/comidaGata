#include <timer.h>
#include <MFRC522.h>
#include <SPI.h>
#include <Servo.h>

// RFID RST and SS pins
#define RST_PIN 9
#define SS_PIN 10

// Servo pins
#define SRVL_PIN 4
#define SRVR_PIN 2

// Constants
#define CLOSED_L 90
#define OPEN_L 180
#define CLOSED_R 90
#define OPEN_R 0
#define TIME 10000

// Variables
bool state = 0;
bool outoftime = 0;
Timer timer;
MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo servo_l, servo_r;

// void timeout(void): is called after TIME ms, and outoftime becomes true, representing
// that some time has already passed and the bowl can close.
void timeout(void){
  outoftime = 1;
}

void setup(){
  SPI.begin();
  mfrc522.PCD_Init();
  servo_l.attach(SRVL_PIN);
  servo_r.attach(SRVR_PIN);
  timer.setCallback(timeout);
}

void loop(){
  if (state == 0) {
    // Bowl is closed
      if (mfrc522.PICC_IsNewCardPresent() || mfrc522.PICC_ReadCardSerial()){
        // Open the bowl
        servo_l.write(OPEN_L);
        servo_r.write(OPEN_R);
        delay(1000);
        state = 1;
      }
  }
  else if (state == 1) {
    // Bowl is open
    outoftime = 0;
    timer.setTimeout(TIME);
    timer.start();
    do {
      if (mfrc522.PICC_IsNewCardPresent() || mfrc522.PICC_ReadCardSerial()){
        timer.start();
      }
      timer.update();
    } while(!outoftime);
    // Close the bowl
    servo_l.write(CLOSED_L);
    servo_r.write(CLOSED_R);
    delay(1000);
    state = 0;
    }
}
