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
    // Check if data is available in Serial Monitor
    if (Serial.available() > 0) {
        // Read data from Serial Monitor
        String inputData = Serial.readStringUntil('\n');
        inputData.trim();  // Remove any extra whitespace

        // Send data to Firebase
        if (Firebase.setString(firebaseData, "/data", inputData)) {
            Serial.println("Data sent successfully to Firebase!");
        } else {
            Serial.println("Failed to send data");
            Serial.println("Reason: " + firebaseData.errorReason());
        }

        // Delay to allow Firebase to process the data
        delay(2000);

        // Retrieve data from Firebase
        if (Firebase.getString(firebaseData, "/rahul")) {
            if (firebaseData.dataType() == "string") {
                String receivedValue = firebaseData.stringData();
                Serial.print("Received data from Firebase: ");
                Serial.println(receivedValue);
            }
        } else {
            Serial.println("Failed to retrieve data");
            Serial.println("Reason: " + firebaseData.errorReason());
        }

        Serial.println("Enter a new value to send to Firebase:");
    }
}
