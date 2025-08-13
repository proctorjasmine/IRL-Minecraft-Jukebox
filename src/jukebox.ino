#include "DFRobotDFPlayerMini.h"
#include "Arduino.h"
#include <SPI.h>
#include <MFRC522.h>

// ======================SPI CONNECTION for RC522 RFID MODULE ======================

#define RST_PIN         9          // GPIO 9 serving as reset pin
#define CS_PIN          10         // FSPICS0 on ESP32 C3 ; Input and output CS Signal
#define MISO            2          // FSPIQ on ESP32 C3 ; master in - slave out
#define MOSI            7          // FSPID on ESP32 C3 ; master out - slave in
#define CLK             6          // FSPICLK on ESP C3 ; shared clock

// ======================CONNECTION for DFPlayer mini module ======================

#define DF_RX           19
#define DF_TX           18

// ======================CONNECTION for Potentiometer volume control ======================

#define POT_VOL         8

// =======================================================================================
#define FPSerial Serial1
DFRobotDFPlayerMini myDFPlayer;

String lastUID = "";

unsigned long lastSeen = 0;
bool cardPresent = false;
int mapped_volume = 0;
int current_volume = 0;


MFRC522 mfrc522(CS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
	// Initialize serial communications with the PC
  Serial.begin(115200);


  // Initalize connection with RFID Scanner
	FPSerial.begin(9600, SERIAL_8N1, DF_RX, DF_TX);
	SPI.begin(CLK, MISO, MOSI, CS_PIN);			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522

  // Intialize connection to DFPlayer Mini
	Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));

	if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }

	Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.volume(20);  //Set volume to initial value of 20

	Serial.println("place card on sensor...");

}

void loop() {
	static unsigned long lastCardTime = 0;

  //volume adjustment from potentiometer reading
  mapped_volume = map(analogRead(POT_VOL), 0, 4095, 0, 30);
  //change volume only if new volume is detected
  if(mapped_volume != myDFPlayer.readVolume()){
      myDFPlayer.volume(mapped_volume);
  }


  // Try to read a card
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Build UID string
    String currentUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      currentUID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
      currentUID.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    currentUID.toUpperCase();
    currentUID.trim();

    //print the read UID Tag

    //Serial.print("UID tag: ");
    //Serial.println(currentUID);

    // Check if it's a new card
    if (currentUID != lastUID) {
      lastUID = currentUID;

      if (currentUID == "33 DA 7C FA") {
        Serial.println("New card detected: playing song 1");
        myDFPlayer.play(1);
      }
      else if (currentUID == "E3 C9 2F 1C") {
        Serial.println("New card detected: playing song 2");
        myDFPlayer.play(2);
      }
			else if (currentUID == "41 1F 86 39") {
        Serial.println("New card detected: playing song 3");
        myDFPlayer.play(3);
      }
			else if (currentUID == "A1 1E 86 39") {
        Serial.println("New card detected: playing song 4");
        myDFPlayer.play(4);
      }
			else if (currentUID == "E1 DB 85 39") {
        Serial.println("New card detected: playing song 5");
        myDFPlayer.play(5);
      }
			else if (currentUID == "61 DD 85 39") {
        Serial.println("New card detected: playing song 6");
        myDFPlayer.play(6);
      }
			else if (currentUID == "A1 DC 85 39") {
        Serial.println("New card detected: playing song 7");
        myDFPlayer.play(7);
      }
			else if (currentUID == "71 20 86 39") {
        Serial.println("New card detected: playing song 8");
        myDFPlayer.play(8);
      }
			else if (currentUID == "11 21 86 39") {
        Serial.println("New card detected: playing song 9");
        myDFPlayer.play(9);
      }
      else {
        Serial.println("Access denied — pausing");
        myDFPlayer.pause();
      }
    }

    // if card is still present
    cardPresent = true;

    //update the lastcardtime
    lastCardTime = millis();
  }

  // If the card hasn't been seen for .5 seconds, stop playback
  if (cardPresent && (millis() - lastCardTime > 500)) {
    Serial.println("Card removed — stopping playback");

		//===================fading mechanism to revisit====================
		// for (int i = myDFPlayer.readVolume(); i > 0; i--){
		// 	myDFPlayer.volume(i);
		// 	delay(100);
		// }
    //==================================================================
		myDFPlayer.stop();
    cardPresent = false;
    lastUID = "";
  }

  delay(100); // reduce polling rate
}


