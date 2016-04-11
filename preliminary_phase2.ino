/*

  GraphicsTest.pde

  >>> Before compiling: Please remove comment from the constructor of the
  >>> connected graphics display (see below).

  Universal 8bit Graphics Library, https://github.com/olikraus/u8glib/

  Copyright (c) 2012, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification,
  are permitted provided that the following conditions are met:

    Redistributions of source code must retain the above copyright notice, this list
    of conditions and the following disclaimer.

    Redistributions in binary form must reproduce the above copyright notice, this
    list of conditions and the following disclaimer in the documentation and/or other
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


*/


#include "U8glib.h"
#include <PinChangeInt.h>

U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI

// Store the width and the height of the screen.
int width;
int height;

// For the drawStep() function, the value to draw must be a float between MIN and MAX.
float MAX = 1.0;
float MIN = 0.0;

// For the drawAlert(), these constants store information about how to render the alert
// message of dancing suns.
int X_SPEED = 4;   // The pixels advanced per frame of the alert.
int X_OFFSET = 0;  // The current offset in the x postion
int rows = 3;      // The number of rows of suns to draw.
int cols = 8;      // The number of columns of suns to draw.

// Bitmap storing the sun image.
const uint8_t sun[] U8G_PROGMEM = {
  0x00, 0x02, 0x40, 0x00,
  0x00, 0x03, 0x0c, 0x00,
  0x00, 0x19, 0xfd, 0x80,
  0x00, 0x03, 0xfc, 0x00,
  0x00, 0x07, 0xfe, 0x80,
  0x00, 0x0b, 0xfd, 0x00,
  0x00, 0x02, 0x70, 0x80,
  0x00, 0x02, 0x64, 0x00

};

/**********************************************************************************/

#define SIGNAL A0
#define S0 0
#define BUTTON 1
#define HAPTIC 2

long orig_time;
long timer_start;
long prev_time = 0;
bool reset = false;
long curr_time;

void setup(void) {
        Serial.begin(9600);
        //reading off of MUX
        pinMode(SIGNAL, INPUT);
        //pin which switches to change between channels
        pinMode(S0, OUTPUT);
        //input button
        pinMode(BUTTON, INPUT_PULLUP);
        //haptic output pin
        pinMode(HAPTIC, OUTPUT);
        attachPinChangeInterrupt(BUTTON, resetTimer, FALLING);

        orig_time = millis();
        timer_start = orig_time;
        digitalWrite(S0, HIGH); //start off displaying on the screen

        u8g.setColorIndex(1); // Instructs the display to draw with a pixel on.
        // Assigns into the width and the height the values set by the display.
        width = u8g.getWidth();
        height = u8g.getHeight();
}

void loop(void) {
        
        curr_time = millis()-timer_start; // the time since the last update

        int curr_UV = readUV();

        //TODO need to be able to handle the timer being reset
        //preemptivly from the ISO app

        //if timer reaches zero
        if( Timer(curr_UV, timer_start, curr_time) ){
                //not yet reset
                while(!reset){ 
                        //alert the user by changing the screen and app
                        drawAlert();
                        digitalWrite(HAPTIC, HIGH); //TODO buzzes until you reset, want it to not do that
                        //wait for either button press on device or IOS reset
                        if (Serial.available() > 0) {
                                String data = Serial.readString();
                                if (data.startsWith("reset")){
                                        reset = true;
                                }
                        }
                }
                
                //sucessfully reset
                reset = false;
                //reset start time
                timer_start = millis();
                curr_time = millis();
        }
}




int readUV() {
        digitalWrite(S0, LOW); //read in on the UV sensor
        int UVreading = analogRead(SIGNAL);
        int UVindex = (UVreading * 3.0 / 102.4); // using 3V instead of 5

        String reading = "reading:" + (String) UVreading;
        String index = ":index:" + (String)UVindex;
        
        Serial.println(reading + index);
        digitalWrite(S0, HIGH); //go back to display

        return UVindex;
}


void resetTimer() {// will be called via interrupt when button is pressed
        //reset the timer
        timer_start = millis();
        reset = true;
}

bool Timer(int UV, long start, long curr){
        
        long timeLimit = 0;
        if( UV < 2){
                //made to be only 10 seconds for TESTING
                timeLimit = 20000;//7200000; //120 minutes
        } else if (UV < 4){
                timeLimit = 3600000; //60 minutes
        } else if (UV < 6){
                timeLimit = 1800000; //30 minutes
        } else if (UV < 7){
                timeLimit = 1200000; //20 minutes
        } else if (UV < 9){
                timeLimit = 900000; //15 minutes
        } else {
                timeLimit = 600000; //10 minutes
        }
        //if the difference in time is greater than time limit
        if ((curr - start) > timeLimit){
                //time to put on more sunscreen!
                return true;
        } else {
                //take the current percentage of time to fill up the bar on screen
                float bar = ((float)(curr-start)/(float)timeLimit);
                drawStep(bar);
                reset = false;
                return false;
        }
}



// Draws a step of the bar given a current step value.
// This value must be between MIN and MAX - if it is not,
// it is set to within the bounds.
void drawStep(float val) {
  if (val > MAX) {
    val = MAX;
  } else if (val < MIN) {
    val = MIN;
  }
  
  u8g.firstPage();
  
  // Find the interpolated width of the bar given the float value.
  int w = (int) (width * val);

  // Every available frame, render the encapsulating frame as well as the box
  // that is w pixels wide.
  do {
    u8g.drawFrame(0, 0, width, height);
    u8g.drawBox(0, 0, w, height);
  } while (u8g.nextPage() );
}


// Draws a 2d array of suns on the screen and controls their speed and position so they 'wrap'
// around the end of the screen and back to the beginning.
void drawAlert() {

  // Advance the current offset to render with by the speed.
  X_OFFSET += X_SPEED;
  
  u8g.firstPage();

  // Store the values of x and y outside the loop 
  // to render each sun in case the compiler sucks.
  int x;
  int y;

  // For each available frame, render each sun. A sun's position can be calculated
  // given the row and column of the sun as well as the X_OFFSET.
  do {
    for (int row = 0; row < rows; row++) {
      for (int col = 0; col < cols; col++) {
        // First, compute the original x by multiplying the width per column by the
        // column counter and add the X_OFFSET.
        x = ( col * (width / cols) + X_OFFSET );

        // If the row is odd (i.e. modding it by 2 produces a result of 1), add
        // 8 pixels to the x position.
        if (row % 2) {
          x += 8;
        }

        // Prevent x from exceeding the width of the device. Note that this does not
        // stop all parts of the sun from being rendered on the screen and it is still
        // possible for part of a sun to be on the screen and part off. This gives the effect
        // that the sun 'jumps' from the end back to the beginning.
        x = (x % width);

        // Allow for partial suns to be rendered at the start to continue the illusion
        // of the 'jump'.
        x -= 16;

        // y is static and is simply the row counter multiplied with the height per row.
        y = row * (height / rows);

        // Finally, draw the bitmap for the new computed position.
        u8g.drawBitmapP(x, y, 4, 8, sun);
      }
    }

    // Draw the outermost frame for continuity.
    u8g.drawFrame(0, 0, width, height);
    
  } while (u8g.nextPage() );
}



