#define UVpin A0
//float UVindex = 0; 
void setup() {
        Serial.begin(57600);
        Serial.setTimeout(25);
        pinMode(UVpin, INPUT);
}

void loop() {
        /*
        int UVreading = analogRead(UVpin);
        UVindex = (UVreading * 3.0 / 102.4); // using 3V instead of 5V 
        Serial.print("The reading is ");
        Serial.println(UVreading);
        Serial.print(" and the UV index is ");
        Serial.println(UVindex);
        */

        int UVreading = 10;
        int UVindex = 20;

        String reading = "reading:"+(String)UVreading;
        String index = ":index:"+(String)UVindex;
        
        Serial.println(reading + index);
        //Serial.println(index);
        

        
        
        delay(2000);
}
