#include <LowPower.h>
#include <EEPROM.h>

/*
   Control Solenoids with a RFID reader


   MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT.
   The library file MFRC522.h has a wealth of useful info. Please read it.
   The functions are documented in MFRC522.cpp.

   Based on code Dr.Leong   ( WWW.B2CQSHOP.COM )
   Created by Miguel Balboa (circuitito.com), Jan, 2012.
   Rewritten by Søren Thing Andersen (access.thing.dk), fall of 2013 (Translation to English, refactored, comments, anti collision, cascade levels.)
   Released into the public domain.

*/

#include <SPI.h>
#include <MFRC522.h>


#define SS_PIN 5
#define RST_PIN 14
#define SOLENOID_PIN 16
#define BUZZER_PIN 8
#define LEARN_BUTTON_PIN 9
#define EXIT_BUTTON_PIN 17
#define LED_ORANGE_PIN 6
#define LED_GREEN_PIN 7
#define OPEN_DURATION 4000

#define LED_OFF    0
#define LED_ORANGE 1
#define LED_GREEN  2

#define MAX_CARDS 10


byte num_cards = 0;
byte card_ids[MAX_CARDS * 4];
bool found_card = false;

MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance.

void writeCardIDs();
void readCardIDs();

// Sets the BiColor LED state
void setLED( int led_state );

void openDoor();


void setup() {
  Serial.begin(9600);	// Initialize serial communications with the PC
  SPI.begin();			// Init SPI bus
  mfrc522.PCD_Init();	// Init MFRC522 card
  Serial.println("Scan PICC to see UID and type...");
  pinMode( SOLENOID_PIN, OUTPUT );
  pinMode(BUZZER_PIN, OUTPUT );
  digitalWrite( SOLENOID_PIN, LOW );

  pinMode( LEARN_BUTTON_PIN, INPUT );
  digitalWrite( LEARN_BUTTON_PIN, HIGH );

  pinMode( EXIT_BUTTON_PIN, INPUT );
  digitalWrite( EXIT_BUTTON_PIN, HIGH );

  pinMode( LED_ORANGE_PIN, OUTPUT );
  pinMode( LED_GREEN_PIN, OUTPUT );

  readCardIDs();
}

void loop() {
  //LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);

  setLED(LED_GREEN);
  delay(200);
  setLED(LED_OFF);
  delay(200);


  // Check to see if the exit button is pressed:
  int exit_val = digitalRead( EXIT_BUTTON_PIN );
  if ( exit_val == 0 ) {
    openDoor();
  }


  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }


  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  int val = digitalRead( LEARN_BUTTON_PIN );
  if ( val == 0 ) {

    int idx = 0;
    for ( idx = 0; idx < 4; idx++ ) {
      card_ids[num_cards * 4 + idx] =
        mfrc522.uid.uidByte[idx];
    }
    num_cards = num_cards + 1;
    if ( num_cards == MAX_CARDS )
      num_cards = 0;

    writeCardIDs();
    tone( BUZZER_PIN, 880, 1000);
    delay(2000);
    tone( BUZZER_PIN, 440, 1000);
    delay(2000);
    tone( BUZZER_PIN, 880, 1000);
    delay(2000);
    return;
  }

  // Check all the known cards:
  int crd = 0;

  found_card = false;
  for ( crd = 0; crd < num_cards; crd++ ) {
    if ( ( mfrc522.uid.uidByte[0] ==
           card_ids[crd * 4] ) &&
         ( mfrc522.uid.uidByte[1] ==
           card_ids[crd * 4 + 1] ) &&
         ( mfrc522.uid.uidByte[2] ==
           card_ids[crd * 4 + 2] ) &&
         ( mfrc522.uid.uidByte[3] ==
           card_ids[crd * 4 + 3] )  )

      found_card = true;
  }





  if ( found_card ) {

    openDoor();

  }
  else {
    // Dump debug info about the card. PICC_HaltA() is automatically called.
    mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
    Serial.println(mfrc522.uid.uidByte[0]);
    setLED( LED_ORANGE );
    tone( BUZZER_PIN, 440, 250 );
    delay(500);
    tone( BUZZER_PIN, 220, 250 );
    delay(500);
    tone( BUZZER_PIN, 440, 250 );
    delay(500);
    setLED( LED_OFF );
  }





}

void writeCardIDs( )
{

  int address = 0;
  EEPROM.write(address, num_cards);
  address = 1;
  int idx = 0;
  for ( idx = 0; idx < num_cards * 4; idx++) {
    EEPROM.write(address, card_ids[idx]);
    address++;
  }


}

void readCardIDs( )
{
  int address = 0;
  num_cards = EEPROM.read(address);
  address = 1;
  int idx = 0;
  for ( idx = 0; idx < num_cards * 4; idx++) {
    card_ids[idx] = EEPROM.read(address);
    address++;
  }
  Serial.print("Read IDs for ");
  Serial.print(num_cards);
  Serial.print(" cards");
}

// Sets the BiColor LED state
void setLED( int led_state )
{
  switch ( led_state ) {

    case LED_OFF:
      digitalWrite( LED_ORANGE_PIN, LOW );
      digitalWrite( LED_GREEN_PIN, LOW );
      break;

    case LED_ORANGE:
      digitalWrite( LED_ORANGE_PIN, HIGH );
      digitalWrite( LED_GREEN_PIN, LOW );
      break;

    case LED_GREEN:
      digitalWrite( LED_ORANGE_PIN, LOW );
      digitalWrite( LED_GREEN_PIN, HIGH );
      break;


    default:
      break;
  }


}

void openDoor(void ) {

  digitalWrite( SOLENOID_PIN, HIGH );
  //    Serial.println(mfrc522.uid.uidByte[0], HEX);
  //   Serial.println(mfrc522.uid.uidByte[1], HEX);
  setLED( LED_GREEN );
  tone( BUZZER_PIN, 220, OPEN_DURATION );
  delay( OPEN_DURATION );
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  Serial.println(mfrc522.uid.uidByte[0]);
  digitalWrite(SOLENOID_PIN, LOW );
  setLED( LED_OFF );
}














