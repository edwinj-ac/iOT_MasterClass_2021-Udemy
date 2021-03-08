/*
 * Typical pin layout used:
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

 */
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

}

void loop() {
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID: ");
  for(byte i=0; i< mfrc522.uid.size; i++){ //For dependiendo ls bytes de memoria de cada tarjeta
    if(mfrc522.uid.uidByte[i] < 0x10){ //Genera espacio entre cada byte o rellene con 0 en caso de ser menor a 9 el byte de memoria
      Serial.print(" 0");
    }else{
      Serial.print(" ");
    }
    Serial.print(mfrc522.uid.uidByte[i], HEX);//muestra cada byte en formato hexadecimal
  }

  Serial.println();
  mfrc522.PICC_HaltA(); //Detiene la comunicación

}
