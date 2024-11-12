#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <SoftwareSerial.h>  // Include SoftwareSerial for multiple RFID readers

// Wi-Fi credentials
const char* WIFI_SSID = "Deiva";
const char* WIFI_PASSWORD = "123123.dp";

const char* FIREBASE_HOST = "https://fmtc-cycle-proj-be-default-rtdb.firebaseio.com";
const char* FIREBASE_AUTH = "AIzaSyCrrLIgXyu47LhNRDGcQ2INmivGCcWr-g0";

// Define RX pins for each RFID reader
#define SLOT1_RX D1
#define SLOT2_RX D2
#define SLOT3_RX D3
#define SLOT4_RX D4

// Define the station identifier
const String STAND_ID = "A"; 
const String CYCLE_STATUS = "LOCKED";  // Constant for station identifier

// Initialize Firebase objects
FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

// RFID variables
String lastIdSLOT1 = "";
String lastIdSLOT2 = "";
String lastIdSLOT3 = "";
String lastIdSLOT4 = "";

// Variables to store the latest retrieved data
String prevSTATUS = "", prevSTAND_ID = "", prevSLOT_ID = "";

// Software serial ports for RFID readers
SoftwareSerial slot1Reader(SLOT1_RX, -1);  // Only RX pin is defined
SoftwareSerial slot2Reader(SLOT2_RX, -1);
SoftwareSerial slot3Reader(SLOT3_RX, -1);
SoftwareSerial slot4Reader(SLOT4_RX, -1);

void setup() {
  Serial.begin(9600);

  // Initialize software serial for each RFID reader
  slot1Reader.begin(9600);
  slot2Reader.begin(9600);
  slot3Reader.begin(9600);
  slot4Reader.begin(9600);

  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Firebase configuration
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Increase Firebase response buffer size
  firebaseData.setBSSLBufferSize(1024, 1024); // Adjust buffer sizes
  firebaseData.setResponseSize(4096);         // Increase response buffer size

  // Initialize Firebase
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Retrieve and print Firebase data if changed
  retrieveAndPrintFirebaseData();

  // Read and upload RFID data for each slot
  readAndUploadRFID(slot1Reader, lastIdSLOT1, "1", "cycle1");
  readAndUploadRFID(slot2Reader, lastIdSLOT2, "2", "cycle2");
  readAndUploadRFID(slot3Reader, lastIdSLOT3, "3", "cycle3");
  readAndUploadRFID(slot4Reader, lastIdSLOT4, "4", "cycle4");

  delay(1000);  // Avoid rapid polling
}

// Function to read RFID and upload data to Firebase for a specific slot
void readAndUploadRFID(SoftwareSerial &reader, String &lastId, String SLOT_ID, String CYCLE_ID) {
  String id = "";
  int count = 0;
  char input[13];

  // Check if data is available in the RFID reader
  if (reader.available()) {
    while (reader.available() && count < 12) {  // Read 12 characters from RFID
      input[count] = reader.read();
      count++;
      delay(5);
    }
    input[count] = '\0';  // Null-terminate the string
    id = String(input);   // Convert the input array to a string
  }

  // Only process if the current ID is different from the last processed one
  if (id != "" && id != lastId) {
    Serial.print(STAND_ID + "_" + SLOT_ID + "_" + CYCLE_STATUS);

    // Map RFID IDs to corresponding cycles
    if (id == "4C003D07F781") CYCLE_ID = "cycle1";
    else if (id == "4C003D13A2C0") CYCLE_ID = "cycle2";
    else if (id == "4C003D055226") CYCLE_ID = "cycle3";
    else if (id == "4C003D22095A") CYCLE_ID = "cycle4";
    else if (id == "43002DFB8217") CYCLE_ID = "cycle5";
    else if (id == "42003C1F6302") CYCLE_ID = "cycle6";
    else if (id == "4B005ACD36EA") CYCLE_ID = "cycle7";
    else if (id == "43002DFD198A") CYCLE_ID = "cycle8";
    else if (id == "4C003817FE9D") CYCLE_ID = "cycle9";
    else if (id == "4900F775DB10") CYCLE_ID = "cycle10";
    else if (id == "4B005A763E59") CYCLE_ID = "cycle11";
    else if (id == "41008FA692FA") CYCLE_ID = "cycle12";
    else if (id == "4900B6A590CA") CYCLE_ID = "cycle13";
    else {
      Serial.println("Unknown card");
      return;
    }

    String STAND_STATUS = STAND_ID + "_" + SLOT_ID + "_" + CYCLE_ID;

    // Update Firebase only if a new ID is detected
    uploadDataToFirebase(STAND_STATUS, STAND_ID, SLOT_ID, CYCLE_ID);

    // Store the last processed ID
    lastId = id;
  }
}

void retrieveAndPrintFirebaseData() {
  String STATUS, STAND_ID, SLOT_ID;
  int retries = 3;  // Retry attempts for Firebase calls

  // Retrieve STATUS with retries
  while (retries > 0) {
    if (Firebase.getString(firebaseData, "CYCLE_ID")) {
      STATUS = firebaseData.stringData();
      break;
    } else {
      Serial.println("Failed to retrieve CYCLE_ID, retrying...");
      retries--;
      delay(1000);
    }
  }

  retries = 3;  // Reset retries for next call

  // Retrieve STAND_ID with retries
  while (retries > 0) {
    if (Firebase.getString(firebaseData, "STAND_ID")) {
      STAND_ID = firebaseData.stringData();
      break;
    } else {
      Serial.println("Failed to retrieve STAND_ID, retrying...");
      retries--;
      delay(1000);
    }
  }

  retries = 3;  // Reset retries for next call

  // Retrieve SLOT_ID with retries
  while (retries > 0) {
    if (Firebase.getInt(firebaseData, "SLOT_ID")) {
      SLOT_ID = String(firebaseData.intData());
      break;
    } else {
      Serial.println("Failed to retrieve SLOT_ID, retrying...");
      retries--;
      delay(1000);
    }
  }

  // Print only if there is a change
  if (STATUS != prevSTATUS || STAND_ID != prevSTAND_ID || SLOT_ID != prevSLOT_ID) {
    prevSTATUS = STATUS;
    prevSTAND_ID = STAND_ID;
    prevSLOT_ID = SLOT_ID;

    if (!STATUS.isEmpty() && !STAND_ID.isEmpty() && !SLOT_ID.isEmpty()) {
      Serial.print(STAND_ID + "_" + SLOT_ID + "_" + STATUS);
    }
  }
}

void uploadDataToFirebase(String STAND_STATUS, String station, String SLOT_ID, String CYCLE_ID) {
  // Define the Firebase path dynamically based on the station, slot, and cycle
  String path = "/stations/" + station + "/" + SLOT_ID + "/" + CYCLE_ID;

  // Create a FirebaseJson object to upload structured data
  FirebaseJson jsonData;

  // Set the structured data in the FirebaseJson object
  jsonData.set("CYCLE_STATUS", "UNLOCKED");
  jsonData.set("SLOT_ID", SLOT_ID); 
  jsonData.set("STAND_ID", station);
  jsonData.set("CYCLE", CYCLE_ID); 
  jsonData.set("STAND_STATUS", STAND_STATUS);

  if (Firebase.setJSON(firebaseData, path, jsonData)) {
    Serial.println("Uploaded data to: " + path);
  } else {
    Serial.println("Failed to upload data: " + firebaseData.errorReason());
  }
}
