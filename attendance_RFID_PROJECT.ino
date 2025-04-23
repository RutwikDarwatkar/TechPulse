#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define the pins for the RFID reader
#define SS_PIN 10  // Slave Select pin
#define RST_PIN 9  // Reset pin

// Create instances for RFID and LCD
MFRC522 rfid(SS_PIN, RST_PIN); // Create MFRC522 instance
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set I2C address for LCD (adjust if necessary)

// Define a structure to hold staff information
struct Staff {
    String cardID;
    String name;
    String staffID;
};

// Create an array of staff members (modify as needed)
Staff staffMembers[] = {
  //{"CARD_Address", "Person_Name", "Person_ID"},
    {"70b6755", "Rutwik Darwatkar", "S001"},
    {"a2366f51", "Shubham Kamble", "S002"},
    {"abcdef", "Charlie Brown", "S003"},
    {"fedcba", "Diana Prince", "S004"}
};

void setup() {
    // Initialize serial communication
    Serial.begin(9600);
    SPI.begin(); // Initialize SPI bus
    rfid.PCD_Init(); // Initialize RFID reader
    lcd.init(); // Initialize LCD
    lcd.backlight(); // Turn on the backlight

    // Display initialization message
    lcd.setCursor(0, 0);
    lcd.print("RFID Attendance");
    lcd.setCursor(0, 1);
    lcd.print("System Ready");
    delay(2000); // Display for 2 seconds
}

void loop() {
    // Check if a new card is present
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        String cardID = "";
        
        // Read the card ID
        for (byte i = 0; i < rfid.uid.size; i++) {
            cardID += String(rfid.uid.uidByte[i], HEX);
        }
        
        // Search for the staff member in the array
        String staffName = "Unknown";
        String staffID = "N/A";
        bool found = false;

        for (int i = 0; i < sizeof(staffMembers) / sizeof(staffMembers[0]); i++) {
            if (staffMembers[i].cardID == cardID) {
                staffName = staffMembers[i].name;
                staffID = staffMembers[i].staffID;
                found = true;
                break;
            }
        }

        // Display information on LCD
        lcd.clear();
        lcd.setCursor(0, 0);
        if (found) {
            lcd.print("Name: " + staffName);
            lcd.setCursor(0, 1);
            lcd.print("ID: " + staffID);
            Serial.println("Attendance recorded for: " + staffName + " (" + staffID + ")");
        } else {
            lcd.print("Unauthorized Card");
            Serial.println("Unauthorized access: Card ID - " + cardID);
        }

        // Send data to PLX-DAQ for Excel logging
        Serial.print("Attendance,");
        Serial.print(found ? staffName : "Unauthorized");
        Serial.print(",");
        Serial.println(found ? staffID : "N/A");

        delay(2000); // Delay to allow reading on LCD
        rfid.PICC_HaltA(); // Halt PICC (card)
    }
}
