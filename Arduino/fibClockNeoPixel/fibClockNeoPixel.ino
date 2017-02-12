//*****************************************************************************
// fibClock.ino - Code for Fibonacci clock operation
//*****************************************************************************
//Connections - 
//DS1307 : Vcc- 5V ,GND- GND, SDA- A5, SCL- A4
//NeoPixels : Vcc- 5V, GND- GND, DIN- Digital Pin 6  

#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            6

// How many NeoPixels are attached to the Arduino?
#define NUM_OF_FIB_PIXELS      5
#define BRIGHTNESS 50

#define NONE 0
#define HOURS 1
#define MINUTES 2
#define BOTH 3


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel fibLEDs = Adafruit_NeoPixel(5, PIN, NEO_GRB + NEO_KHZ800);

int fibNumArray[5] = {5, 3, 2, 1, 1};
int fibFlagArray[5] = {0, 0, 0, 0, 0};
int hour_counter =  0;
int minute_counter = 0;

//********************************************************
//
// This initializes the flags of fibonacci array to zero
//
//********************************************************
void initFibFlagArray()
{
  int index = 0;
  for (index = 0; index < 5; index++){
    fibFlagArray[index] = NONE;
  }
}

/**
  * This function converts the given 'number' (0-11) into its fibonacci series representation
  * and updates the array 'fibFlagArray'
  *
  * 'unit' - This parameter is used to update the flag in the structure
  *        - unit == 1 => for hours
  *        - unit == 2 => for minutes
 **/
void convertNumberToFibFlagArray(int number, int unit)
{
  int index = 0;
  int sum = 0;
  
  while(index < 5){
    sum += fibNumArray[index];

    if(sum > number){
      sum -= fibNumArray[index];
    } 
    else if(sum == number){
      fibFlagArray[index] += unit;
      break;
    }
    else{
      fibFlagArray[index] += unit;
    }
    index++;
  }
  return;
}

//*****************************************************************************
//
// The function to update the LED pins based on the values of fibFlagArray
//
//*****************************************************************************
void updateFibLEDs(void)
{
  int index = 0;
  
  for(index = 0; index < 5; index++){
    Serial.print(fibNumArray[index]);
    Serial.print("\t");
    Serial.print(fibFlagArray[index]);
    Serial.println("");
    switch(fibFlagArray[index]){

    case NONE :
          fibLEDs.setPixelColor(index, 0);
          break;
    case HOURS :
          fibLEDs.setPixelColor(index, fibLEDs.Color(BRIGHTNESS, 0, 0));
          break;
    case MINUTES :
          fibLEDs.setPixelColor(index, fibLEDs.Color(0, BRIGHTNESS, 0));
          break;
    case BOTH :
          fibLEDs.setPixelColor(index, fibLEDs.Color(0, 0, BRIGHTNESS));
          break;
    }
  }
  fibLEDs.show();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial) ; // wait for serial
  delay(200);
  
  hour_counter = 0;
  minute_counter = -5;

  fibLEDs.begin();
  fibLEDs.show(); // Initialize all pixels to 'off'
  Serial.println("-- LEDs Initialized --");
}

void loop() {
  tmElements_t tm;
  
  if (RTC.read(tm)) {
    Serial.print("Ok, Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
    
    if(abs(tm.Minute - minute_counter) >= 5){
      initFibFlagArray();

      if(tm.Hour == 0){
        convertNumberToFibFlagArray(12 , 1);
      }
      else if(tm.Hour > 12 ){
        convertNumberToFibFlagArray(tm.Hour - 12 , 1);
      }
      else {
        convertNumberToFibFlagArray(tm.Hour, 1);
      }
      
      convertNumberToFibFlagArray(tm.Minute/5, 2);
      updateFibLEDs();
      
      minute_counter = (tm.Minute/5) * 5;
    }
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
  delay(1000); 
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}
