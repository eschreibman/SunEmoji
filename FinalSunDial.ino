/*
 * Tufts Comp 50 Wearable Devices Final Project
 * Code contributions from: Eliza Schreibman, Jake Mingolla, Reema Al-Marzoog, 
 * Melissa Blotner, Chris Gregg, and other libraries credited below.
 * 
 * SunDial- A wearables device designed to help users moniter their sun intake
 * and build better skin care habits. A timer (displayed on an oLED screen) fills up
 * over time as the user is exposed to more UV light. If there is a higher amount of UV
 * exposure detected, the timer fills up faster. Timer is reset via button or IOS app.
 * 
 * Major thanks to Chris, Kate, and Raewyn for all their help!
 * 
 */


/**************************  BUZZER CONSTANTS  ************************/
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

/*****************************  SCREEN CONSTANTS *************************************/


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

/*************************************  MAIN CODE  ************************************************/

#define SIGNAL A0
#define S0 0
#define BUTTON 1
#define SPEAKER 2

long timer_start;
long prev_time = 0;
bool reset = false;
long curr_time;
int count = 0;
int UVindex;
//determines how often the UV is read and sent to the IOS
int max_count = 25; //for actual, 1000


void setup(void) {
        Serial.begin(9600);
        //reading the output signal off of the MUX
        pinMode(SIGNAL, INPUT);
        //pin that switches between channels (UV reading and screen writing)
        pinMode(S0, OUTPUT);
        //input button
        pinMode(BUTTON, INPUT_PULLUP);
        attachPinChangeInterrupt(BUTTON, resetTimer, FALLING);
        //buzzer
        pinMode(SPEAKER, OUTPUT);

        //initialize timer
        timer_start = millis();
        //start off displaying on the screen
        digitalWrite(S0, HIGH);
        
        // Instructs the display to draw with a pixel on.
        u8g.setColorIndex(1); 
        // Assigns into the width and the height the values set by the display.
        width = u8g.getWidth();
        height = u8g.getHeight(); 
}

void loop(void) {
        // get the current time
        curr_time = millis(); 

        //get the current UV reading from the bean
        readUV();

        //check if timer has been reset preemptivly from the ISO app
        if (Serial.available() > 0) {
                String data = Serial.readString();
                if (data.startsWith("reset")){
                        //reset start time and count
                        timer_start = millis();
                        count = 0;
                        //confirm reset send to IOS
                        Serial.println("reset_confirm");
                        }
        }

        //if timer reaches zero
        if( Timer(UVindex, timer_start, curr_time) ){
                //send the IOS app an alert to trigger push notifications
                Serial.println("timer_at_zero");
                 
                while(!reset){ 
                        //alert the user by changing the screen and app
                        drawAlert();
                        //play a melody to alter user through auidory queue
                        playMelody();
                            
                        //wait for either button press on device or IOS reset
                        if (Serial.available() > 0) {
                                String data = Serial.readString();
                                if (data.startsWith("reset")){
                                        reset = true;
                                        //confirm reset send to IOS
                                        Serial.println("reset_confirm");
                                }
                        }     
                }
                
                //sucessfully reset
                //turn off speaker
                digitalWrite(SPEAKER, LOW);
                reset = false;
                //reset start time and count
                timer_start = millis();
                count = 0;
        } 


}

/******************************  UV TIMER  *********************************/

/* Reads off the UV sensor every few seconds
 *  Sends that reading to the IOS app for graphing
 */
void readUV() {
        //read from the bean and send the reading to the IOS app every 100 readings
        if(count == 0){
                digitalWrite(S0, LOW); //read in on the UV sensor
                int UVreading = analogRead(SIGNAL);
                UVindex = (UVreading * 3.0 / 102.4); // using 3V instead of 5
                
                String index = "index:" + (String)UVindex;
        
                Serial.println(index);
                digitalWrite(S0, HIGH); //go back to display
        }
        count ++;
        //reset the count after 100 readings
        if(count > max_count){
                count = 0;
        }
}


/* Calculates the rate at which the timer should fill up
 * based on the current UV reading. The current times are small
 * to make testing easier, but we have commented the actual times
 * based on data found on the internet (e.g. when the UV index is
 * below 2 you need to reapply sunscreen every 2 hours).
 * Returns true when the timer/screen has filled up all the way, false otherwise
*/
bool Timer(int UV, long start, long curr){
        long timeLimit = 0;
        if( UV < 2){
                //2 minutes
                timeLimit = 40000; //real life time: 120 minutes
        } else if (UV < 4){
                //1 minute
                timeLimit = 35000; //3600000 //real life time: 60 minutes
        } else if (UV < 6){
                //30 seconds
                timeLimit = 30000;//1800000; //real life time: 30 minutes
        } else if (UV < 7){
                //20 seconds
                timeLimit = 20000; //real life time: 20 minutes
        } else if (UV < 9){
                //10 seconds
                timeLimit = 10000; //real life time: 15 minutes
        } else {
                //10 seconds
                timeLimit = 10000; //real life time: 10 minutes
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

/* Interupt function that resets the timer logic
 * if the user pushes the physical (not IOS) button
 */
void resetTimer() {
        //reset the timer
        timer_start = millis();
        //stops the draw alert
        reset = true;
        //tell IOS button on device was pressed
        Serial.println("pbutton_pressed");
}

/******************************  SCREEN  *********************************/

/* Draws a step of the bar given a current step value.
 * This value must be between MIN and MAX - if it is not,
 it is set to within the bounds.*/
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


/* Draws a 2d array of suns on the screen and controls their speed and position so they 'wrap'
 around the end of the screen and back to the beginning.*/
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
                                // First, compute the original x by multiplying the width 
                                // per column by the column counter and add the X_OFFSET.
                                x = ( col * (width / cols) + X_OFFSET );
                                // If the row is odd (i.e. modding it by 2 produces a 
                                // result of 1), add 8 pixels to the x position.
                                if (row % 2) {
                                        x += 8;
                                }
                                // Prevent x from exceeding the width of the device. 
                                // Note that this does not stop all parts of the sun from 
                                // being rendered on the screen and it is still possible 
                                // for part of a sun to be on the screen and part off. 
                                // This gives the effect that the sun 'jumps' from the end 
                                // back to the beginning.
                                x = (x % width);
                                
                                // Allow for partial suns to be rendered at the start to 
                                // continue the illusionof the 'jump'.
                                x -= 16;
                                
                                // y is static and is simply the row counter multiplied 
                                // with the height per row.
                                y = row * (height / rows);
                                
                                // Finally, draw the bitmap for the new computed position.
                                u8g.drawBitmapP(x, y, 4, 8, sun);
                        }
                }
        // Draw the outermost frame for continuity.
        u8g.drawFrame(0, 0, width, height);
        } while (u8g.nextPage() );
}


/***************************  BUZZER  ************************************/
/*
 * Melody
 * created 21 Jan 2010, modified 30 Aug 2011
 * by Tom Igoe
 * This example code is in the public domain. http://www.arduino.cc/en/Tutorial/Tone
 */

// notes in the melody:
int melody[] = {
        NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
        4, 8, 8, 4, 4, 4, 4, 4
};

void playMelody() {
        // iterate over the notes of the melody:
        for (int thisNote = 0; thisNote < 8; thisNote++) {
                // to calculate the note duration, take one second
                // divided by the note type.
                //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
                int noteDuration = 1000 / noteDurations[thisNote];
                tone(SPEAKER, melody[thisNote], noteDuration);
                
                // to distinguish the notes, set a minimum time between them.
                // the note's duration + 30% seems to work well:
                int pauseBetweenNotes = noteDuration * 1.30;
                delay(pauseBetweenNotes);
                // stop the tone playing:
                noTone(8);
        }
}

/*
 * GraphicsTest.pde
 *
 * >>> Before compiling: Please remove comment from the constructor of the
 * >>> connected graphics display (see below).
 *
 * Universal 8bit Graphics Library, https://github.com/olikraus/u8glib/
 *
 * Copyright (c) 2012, olikraus@gmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list
 *  of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or other
 * materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
