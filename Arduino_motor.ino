// Define stepper motor control pins
const int StepPin = 2; // Step pin for CNC Shield
const int DirPin = 5;  // Direction pin for CNC Shield

// Pin for receiving data from ESP8266
#define rx 0 // RX pin for Arduino (connected to TX of ESP8266)

int count = 0;
String id;
char input[13];

void setup() {
  Serial.begin(9600);   // Start serial communication with Arduino  
  pinMode(StepPin, OUTPUT);
  pinMode(DirPin, OUTPUT);
}

void loop() {
  // Read data directly from the serial (ESP8266)
  if (Serial.available()) {
    count = 0;
    while (Serial.available() && count < 12) {  // Read up to 12 characters from RFID
      input[count] = Serial.read();
      count++;
      delay(5);
    }

    input[count] = '\0'; // Null-terminate the string
    id = String(input);   // Convert the input array to a string

    // Print received ID for debugging
    Serial.println("Received RFID ID: " + id);

    // Process the RFID ID
    if (id == "4C003D07F781") { // Example RFID tag for detection (Cycle 1)
      Serial.println("Detected RFID: " + id);
      rotateMotor(180, LOW);  // Rotate counterclockwise
    }
    else {
      Serial.println("Removed RFID: " + id);
      rotateMotor(180, HIGH);  // Rotate clockwise
    }
  }
 
  delay(500);  // Reduce delay to allow faster reading
}

void rotateMotor(int degrees, int direction) {
  int steps = degrees / 1.8; // Calculate steps based on the stepper motor's step angle
  digitalWrite(DirPin, direction); // Set motor direction
  Serial.println("Rotating motor " + String(degrees) + " degrees " + (direction == LOW ? "counterclockwise" : "clockwise"));

  for (int x = 0; x < steps; x++) {
    digitalWrite(StepPin, HIGH);
    delayMicroseconds(500); // Adjust for speed
    digitalWrite(StepPin, LOW);
    delayMicroseconds(500);
  }

  Serial.println("Rotation complete.");
  delay(1000); // Delay after completing rotation
}
