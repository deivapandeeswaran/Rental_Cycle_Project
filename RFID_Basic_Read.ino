#include <SoftwareSerial.h>   
#define rx 5                               
#define tx 4         
int count = 0; 
String id;
String lastId = "";  // Store the last processed ID
char input[13]; 
SoftwareSerial myserial(rx, tx); 
                          
void setup()
{
  Serial.begin(9600);   // Start serial at baud rate of 9600 bits/sec   
  myserial.begin(9600);  // Start software serial communication
}

void loop()
{
   if(myserial.available())
   {
      count = 0;
      while(myserial.available() && count < 12)          // Read 12 characters and store them in input array
      {
         input[count] = myserial.read();
         count++;
         delay(5);
      }

      id = String(input);  // Convert the input array to a string
   }

   // Only process if the current ID is different from the last processed one
   if(id != "" && id != lastId)
   {
      if(id == "4C003D07F781")
      {
         Serial.println("CYCLE 1");
      }
      else if(id == "4C003D13A2C0")
      {
         Serial.println("CYCLE 2");
      }
      else if(id == "4C003D055226")
      {
         Serial.println("CYCLE 3");
      }
      else if(id == "4C003D22095A")
      {
         Serial.println("CYCLE 4");
      }

      lastId = id;  // Store the last processed ID
   }

   delay(1000);  // Add a delay to avoid rapid looping
}
