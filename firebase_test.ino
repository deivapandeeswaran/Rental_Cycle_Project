#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>  // Firebase ESP8266 library

// Define RX and TX pins for RFID
#define rx 5
#define tx 4

// Wi-Fi credentials
const char* ssid = "Deiva";
const char* password = "123123.dp";

// Firebase credentials
#define FIREBASE_HOST "https://cycle-project-bd789-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "709dJJINWwouAl8sRD41YNUqxZdMSwQncnB5oMrs"

// Initialize Firebase objects
FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;  // Create a FirebaseAuth object

// RFID variables
int count = 0;
String id;
String lastId = "";  // Store the last processed ID
char input[13];
SoftwareSerial myserial(rx, tx);

void setup() {
  Serial.begin(9600);   // Start serial communication with Arduino  
  myserial.begin(9600);  // Start software serial communication (RFID reader)

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Firebase configuration
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Initialize Firebase with authentication
  Firebase.begin(&firebaseConfig, &firebaseAuth);  // Pass firebaseAuth instead of firebaseData
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (myserial.available()) {
    count = 0;
    while (myserial.available() && count < 12) {  // Read 12 characters from RFID
      input[count] = myserial.read();
      count++;
      delay(5);
    }
    input[count] = '\0';  // Null-terminate the string
    id = String(input);   // Convert the input array to a string
  }

  // Only process if the current ID is different from the last processed one
  if (id != "" && id != lastId) {
    if (id == "4C003D07F781") {  // Example RFID tag
      Serial.println("CYCLE 1");
      uploadDataToFirebase("CYCLE 1");
    } 
    else if (id == "4C003D13A2C0") {
      Serial.println("CYCLE 2");
      uploadDataToFirebase("CYCLE 2");
    } 
    else if (id == "4C003D055226") {
      Serial.println("CYCLE 3");
      uploadDataToFirebase("CYCLE 3");
    } 
    else if (id == "4C003D22095A") {
      Serial.println("CYCLE 4");
      uploadDataToFirebase("CYCLE 4");
    }

    lastId = id;  // Store the last processed ID
  }

  delay(1000);  // Avoid rapid looping
}

// Function to upload data to Firebase
void uploadDataToFirebase(String cycle) {
  if (Firebase.setString(firebaseData, "/rfid_data", cycle)) {  // Upload cycle to Firebase
    Serial.println("Data uploaded successfully: " + cycle);
  } else {
    Serial.println("Failed to upload data: " + firebaseData.errorReason());
  }
}
