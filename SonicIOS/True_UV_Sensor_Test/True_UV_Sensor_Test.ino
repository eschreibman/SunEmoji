#define UVpin A0
float UVindex = 0; 
void setup() {
        Serial.begin(9600);
        pinMode(UVpin, INPUT);
}

void loop() {
        int UVreading = 10;//analogRead(UVpin);
        UVindex = (UVreading * 3.0 / 102.4); // using 3V instead of 5V 
        Serial.print("The reading is ");
        Serial.println(UVreading);
        Serial.print(" and the UV index is ");
        Serial.println(UVindex);
        
        delay(2000);
}
