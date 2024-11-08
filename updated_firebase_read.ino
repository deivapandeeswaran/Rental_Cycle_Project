#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

// Define Firebase objects
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

// Replace with your Firebase and WiFi credentials
const char* WIFI_SSID = "Deniston";
const char* WIFI_PASSWORD = "1234@1234";
const char* FIREBASE_HOST = "cycle-project-2c901-default-rtdb.asia-southeast1.firebasedatabase.app";
const char* FIREBASE_AUTH = "9jJ2iYWNM9DRiaHzRnD4NRL2m2DhtolLvx7tcYrW";

void setup() {
    // Initialize Serial Monitor
    Serial.begin(115200);
    
    // Connect to WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nConnected to WiFi");

    // Set Firebase project host and authentication
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;

    // Initialize Firebase
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    // Check if connected to Firebase
    if (Firebase.ready()) {
        Serial.println("Connected to Firebase!");
    } else {
        Serial.println("Failed to connect to Firebase");
    }

    Serial.println("Enter a value to send to Firebase:");
}

void loop() {

        // Retrieve data from Firebase
        if (Firebase.getString(firebaseData, "/CYCLE_ID")) {
            if (firebaseData.dataType() == "string") {
                String CYCLE_ID = firebaseData.stringData();
                Serial.print("Received CYCLE_ID: ");
                Serial.println(CYCLE_ID);
            }
        } else {
            Serial.println("Failed to retrieve CYCLE_ID data");
            Serial.println("Reason: " + firebaseData.errorReason());
        }
                // Retrieve data from Firebase
        if (Firebase.getString(firebaseData, "/CYCLE_STATUS")) {
            if (firebaseData.dataType() == "string") {
                String CYCLE_STATUS = firebaseData.stringData();
                Serial.print("Received CYCLE_STATUS: ");
                Serial.println(CYCLE_STATUS);
            }
        } else {
            Serial.println("Failed to retrieve CYCLE_STATUS data");
            Serial.println("Reason: " + firebaseData.errorReason());
        }
                
        if (Firebase.getString(firebaseData, "/STAND_ID")) {
            if (firebaseData.dataType() == "string") {
                String STAND_ID = firebaseData.stringData();
                Serial.print("Received STAND_ID: ");
                Serial.println(STAND_ID);
            }
        } else {
            Serial.println("Failed to retrieve STAND_STATUS data");
            Serial.println("Reason: " + firebaseData.errorReason());
        }
                // Retrieve data from Firebase
        
        if (Firebase.getString(firebaseData, "/STAND_STATUS")) {
            if (firebaseData.dataType() == "string") {
                String STAND_STATUS = firebaseData.stringData();
                Serial.print("Received STAND_STATUS: ");
                Serial.println(STAND_STATUS);
            }
        } else {
            Serial.println("Failed to retrieve STAND_STATUS data");
            Serial.println("Reason: " + firebaseData.errorReason());
        }

            Serial.println("..............................................");


    }
