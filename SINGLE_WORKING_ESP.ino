#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <SoftwareSerial.h>

// Wi-Fi credentials
const char* WIFI_SSID = "Link";
const char* WIFI_PASSWORD = "jan12345";

const char* FIREBASE_HOST = "https://cycle-data-67d83-default-rtdb.firebaseio.com";
const char* FIREBASE_AUTH = "tfEvrH2bwTaBj8zlbAM6sB6pg4VAIhe1ceEMO0r7";

// Define RX pin for the RFID reader
#define RFID_RX_PIN D1

// Station identifiers
const String STAND_ID = "A";
const String CYCLE_ID = "CYCLE_1";
const String SLOT_ID = "1";

// Firebase keys for easier referencing
const String FIREBASE_PATH = "/CYCLES/";
const String CYCLE_STATUS_KEY = "CYCLE_STATUS";
const String STAND_STATUS_KEY = "STAND_STATUS";

// Firebase objects
FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

// Predefined RFID card ID
const String AUTHORIZED_RFID_ID = "4900F775DB10";  // Replace with the actual RFID tag ID
String lastScannedID = "";

// Last printed standStatus
String lastPrintedStandStatus = "";

// Initialize the RFID reader
SoftwareSerial rfidReader(RFID_RX_PIN, -1); // Only RX pin is used

void setup() {
  Serial.begin(9600);
  rfidReader.begin(9600);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Initialize Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  // Initial setup to ensure STAND_STATUS is correct when CYCLE_STATUS is UNLOCKED
  updateStandStatusFromFirebase();  // Call to ensure initial STAND_STATUS is correct
}

void loop() {
  String scannedID = readRFID();

  if (!scannedID.isEmpty()) {
    lastScannedID = scannedID;

    if (scannedID == AUTHORIZED_RFID_ID) {
      updateCycleStatus("LOCKED");  // Lock the cycle on RFID scan
      updateStandStatusFromFirebase();  // Update STAND_STATUS to reflect lock
    } else {
      Serial.println("Unauthorized RFID scanned");
    }
  }

  updateStandStatusFromFirebase();  // Regularly check Firebase for CYCLE_STATUS updates

  delay(1000);
}

String readRFID() {
  String id = "";
  int count = 0;
  char input[13];

  // Check if data is available from the RFID reader
  if (rfidReader.available()) {
    while (rfidReader.available() && count < 12) {
      input[count] = rfidReader.read();
      count++;
      delay(5);
    }
    input[count] = '\0';
    id = String(input); // Convert char array to String
  }
  return id;
}

void updateCycleStatus(String status) {
  if (Firebase.setString(firebaseData, FIREBASE_PATH + CYCLE_STATUS_KEY, status)) {
    // Serial.println("CYCLE_STATUS updated successfully to " + status);
  } else {
    // Serial.println("Failed to update CYCLE_STATUS: " + firebaseData.errorReason());
  }
}

void updateStandStatusFromFirebase() {
  if (Firebase.getString(firebaseData, FIREBASE_PATH + CYCLE_STATUS_KEY)) {
    String cycleStatus = firebaseData.stringData();
    String standStatus;

    // Set STAND_STATUS based on the CYCLE_STATUS value
    if (cycleStatus == "LOCKED") {
      standStatus = STAND_ID + "_" + SLOT_ID + "_" + CYCLE_ID;  // A_1_CYCLE_1
    } else {
      standStatus = STAND_ID + "_" + SLOT_ID + "_0";  // A_1_0
    }

    // Only print if the standStatus has changed
    if (standStatus != lastPrintedStandStatus) {
      Serial.print(standStatus);  // Send the status to the Arduino (A_1_0 or A_1_CYCLE_1)
      lastPrintedStandStatus = standStatus;  // Update last printed standStatus

      // Update STAND_STATUS in Firebase only if it has changed
      if (Firebase.getString(firebaseData, FIREBASE_PATH + STAND_STATUS_KEY) && firebaseData.stringData() != standStatus) {
        if (Firebase.setString(firebaseData, FIREBASE_PATH + STAND_STATUS_KEY, standStatus)) {
          // Serial.println("STAND_STATUS updated successfully to " + standStatus);
        } else {
          // Serial.println("Failed to update STAND_STATUS: " + firebaseData.errorReason());
        }
      }
    }
  } else {
    // Serial.println("Failed to retrieve CYCLE_STATUS: " + firebaseData.errorReason());
  }
}