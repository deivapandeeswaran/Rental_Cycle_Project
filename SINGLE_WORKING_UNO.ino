#include <SoftwareSerial.h>

#define rx 5
#define tx 4

const int StepX = 2;
const int DirX = 5;
const int StepY = 3;
const int DirY = 6;
const int StepZ = 4;
const int DirZ = 7;

// State variables to track lock/unlock status for slots
bool slot1Locked = true;  // Start as locked
bool slot2Locked = true;  // Start as locked

void setup() {
    // Start serial communication
    Serial.begin(9600);  // For communication with ESP8266
    pinMode(StepX, OUTPUT);
    pinMode(DirX, OUTPUT);
    pinMode(StepY, OUTPUT);
    pinMode(DirY, OUTPUT);
    pinMode(StepZ, OUTPUT);
    pinMode(DirZ, OUTPUT);
}

void loop() {
    delay(500);

    // Check if there is data available from the ESP8266
    if (Serial.available()) {
        String command = Serial.readStringUntil('\n'); // Read the incoming command
        Serial.println(command);

        // Handle commands for Slot 1
        if (command == "A_1_CYCLE_1") {
            rotateMotor1CounterClockwise(360);  // Rotate motor to unlock slot
            Serial.println("Unlocked Slot 1");
            slot1Locked = false;  // Update state to unlocked
        } 
        else if (command == "A_1_0") {
            rotateMotor1Clockwise(360);  // Rotate motor to lock slot
            Serial.println("Locked Slot 1");
            slot1Locked = true;  // Update state to locked
        }

        
    }
}

// Function to rotate motor 1 clockwise (locks Slot 1)
void rotateMotor1Clockwise(int steps) {
    digitalWrite(DirX, HIGH);  // Set direction to clockwise
    for (int x = 0; x < steps; x++) {
        digitalWrite(StepX, HIGH);
        delayMicroseconds(500);  // Adjust speed as needed
        digitalWrite(StepX, LOW);
        delayMicroseconds(500);
    }
}

// Function to rotate motor 1 counterclockwise (unlocks Slot 1)
void rotateMotor1CounterClockwise(int steps) {
    digitalWrite(DirX, LOW);  // Set direction to counterclockwise
    for (int x = 0; x < steps; x++) {
        digitalWrite(StepX, HIGH);
        delayMicroseconds(500);  // Adjust speed as needed
        digitalWrite(StepX, LOW);
        delayMicroseconds(500);
    }
}

// Function to rotate motor 2 clockwise (locks Slot 2)

