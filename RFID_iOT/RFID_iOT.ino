/*
 * Write personal data of a MIFARE RFID card using a RFID-RC522 reader
 * Uses MFRC522 - Library to use ARDUINO RFID MODULE KIT 13.56 MHZ WITH TAGS SPI W AND R BY COOQROBOT. 
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * Hardware required:
 * Arduino
 * PCD (Proximity Coupling Device): NXP MFRC522 Contactless Reader IC
 * PICC (Proximity Integrated Circuit Card): A card or tag using the ISO 14443A interface, eg Mifare or NTAG203.
 * The reader can be found on eBay for around 5 dollars. Search for "mf-rc522" on ebay.com. 
 */

#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

#define RST_PIN         9           // Configurable, see typical pin layout above
#define SS_PIN          10          // Configurable, see typical pin layout above
                      //RX, TX
SoftwareSerial mySerial(4,5);

MFRC522 rfid(SS_PIN, RST_PIN);   // Create rfid instance

byte nuidPICC[7];

void printDec(byte *buffer, byte bufferSize){
    
    for(byte i=0; i < bufferSize; i++){
      Serial.print(buffer[i] < 0x10 ? " 0" : "");
      Serial.print(buffer[i], DEC);
    }

    for(byte i=0; i < bufferSize; i++){
      mySerial.print(buffer[i] < 0x10 ? " 0" : "");
      mySerial.print(buffer[i], DEC);
    }
  }

  void printHex(byte *buffer, byte bufferSize){
   for(byte i=0; i< rfid.uid.size; i++){ //For dependiendo ls bytes de memoria de cada tarjeta
    if(rfid.uid.uidByte[i] < 0x10){ //Genera espacio entre cada byte o rellene con 0 en caso de ser menor a 9 el byte de memoria
      Serial.print(" 0");
    }else{
      Serial.print(" ");
    }
     Serial.print(rfid.uid.uidByte[i], HEX);//muestra cada byte en formato hexadecimal
    }
  }

void setup() {
  Serial.begin(9600);        // Initialize serial communications with the PC
  mySerial.begin(9600);
  SPI.begin();               // Init SPI bus
  rfid.PCD_Init();        // Init MFRC522 card

  MFRC522::MIFARE_Key key;
  
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
  
  Serial.println(F("This code scan the MIRAFE Classic NUID"));
  Serial.print(F("Using the following key: "));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);
}

void loop() {

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print(F("PICC type: "));    
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  //Check is the PICC of Classic MIFARE type
  if(piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K){
     Serial.println(F("Your tag is not of type MIFARE Classic"));
      return;
    }
    
  Serial.println(F("A new card has been detected"));
  
  //Store NUID into nuidPICC array
  for (byte i = 0; i < rfid.uid.size; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }

  Serial.println(F("The NUID tag is: "));
  Serial.print(F("In hex: "));
  printHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();
  Serial.print(F("In dec: "));
  printDec(rfid.uid.uidByte, rfid.uid.size);
  Serial.println();

  
  rfid.PICC_HaltA(); // Halt PICC
  rfid.PCD_StopCrypto1();  // Stop encryption on PCD
  delay(2000);

}
