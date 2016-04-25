#define UVpin A0
//float UVindex = 0; 

int UVreading = 0;
float UVindex = 0;
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

        UVreading++;
        UVindex++;

        String reading = "reading:"+(String)UVreading;
        

        char buff[5];
        
        String thing = dtostrf(UVindex, 5, 2, buff);

        String index = ":index:"+(String)thing;
        
        Serial.println(reading + index);     

        if(UVreading % 10 == 0){
          UVreading = 0;
          UVindex = 0;
        }
        
        delay(2000);
}
