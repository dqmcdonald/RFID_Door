#include <LowPower.h>


/*
 * Control Solenoids with a RFID reader
 *
 *
 * MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT.
 * The library file MFRC522.h has a wealth of useful info. Please read it.
 * The functions are documented in MFRC522.cpp.
 *
 * Based on code Dr.Leong   ( WWW.B2CQSHOP.COM )
 * Created by Miguel Balboa (circuitito.com), Jan, 2012.
 * Rewritten by Søren Thing Andersen (access.thing.dk), fall of 2013 (Translation to English, refactored, comments, anti collision, cascade levels.)
 * Released into the public domain.
 *
 */

#include <SPI.h>
#include <MFRC522.h>


#define SS_PIN 5
#define RST_PIN 14
#define SOLENOID_PIN 16

MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance.

void setup() {
  Serial.begin(9600);	// Initialize serial communications with the PC
  SPI.begin();			// Init SPI bus
  mfrc522.PCD_Init();	// Init MFRC522 card
  Serial.println("Scan PICC to see UID and type...");
  pinMode( SOLENOID_PIN, OUTPUT );
  digitalWrite( SOLENOID_PIN, LOW ); 
}

void loop() {
 //  LowPower.powerDown(SLEEP_2S, ADC_OFF, BOD_OFF);  
 
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }


  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
 
  if( mfrc522.uid.uidByte[0] == 0xF4 ) {
    
    digitalWrite( SOLENOID_PIN, HIGH );
    Serial.println(mfrc522.uid.uidByte[0]);
    Serial.println(mfrc522.uid.uidByte[1]);
    delay( 2000 );
     mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
    Serial.println(mfrc522.uid.uidByte[0]);
     
    
    digitalWrite(SOLENOID_PIN, LOW );
  }
  else {
   // Dump debug info about the card. PICC_HaltA() is automatically called.
    mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
    Serial.println(mfrc522.uid.uidByte[0]);
     
    
  }
  
  
}




