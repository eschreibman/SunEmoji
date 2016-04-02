/* 
  This sketch uses the Bean library to blink the on-board RGB LED. 
  
  Notes:
    - This is not a low-power sketch 
    - A Bean with a low battery might show a faint blue and green LED color
  
  This example code is in the public domain.
*/
#include <PinChangeInt.h> 

bool time_to_measure = false;
bool send_value = false;
unsigned long echo_time;
void setup() {
  pinMode(0,INPUT); // echo (green)
  pinMode(2,OUTPUT); // trigger (yellow)
  attachPinChangeInterrupt(0, toggled, CHANGE);
  Serial.begin(57600);
  Serial.setTimeout(25);
}

void loop() {
    if (send_value){
      // send data over serial connection
      char buffer[1000]; // bad chris
      sprintf(buffer,"%ld",echo_time);
      Serial.write((uint8_t*)buffer, strlen(buffer));
      Bean.setLed(0, 0, 255);
      send_value = false;  
    }
    else {
      Bean.setLed(0,0,0);
      Serial.write((uint8_t*)"onionize",8);
    }
    // trigger
    digitalWrite(2,HIGH);
    Bean.sleep(200); // keep on to trigger
    digitalWrite(2,LOW); // untrigger
    Bean.sleep(1000); // wait for response
}

void toggled(){
    if (time_to_measure) {
      // need to get return delay
      echo_time = micros()-echo_time;
      send_value = true;
    }
    else {
      echo_time = micros();
    }
    time_to_measure = !time_to_measure;
}

