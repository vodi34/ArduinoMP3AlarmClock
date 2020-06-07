#include <Adafruit_GFX.h>       // Core graphics library
#include <MCUFRIEND_kbv.h>      // Hardware-specific library
MCUFRIEND_kbv tft;

#include <DS3231M.h>            // Include the DS3231M RTC library
DS3231M_Class DS3231M;          // Create an instance of the DS3231M class


#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);


/* 
 *  Tutorial für tft
 *  https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives
 */

#include <Fonts/FreeSans9pt7b.h>
//#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>
#include <FreeDefaultFonts.h>

#define BLACK   0x0000
#define RED     0xF800
#define GREEN   0x07E0
#define WHITE   0xFFFF
#define GREY    0x8410

// 'play', 50x50
// created with : http://javl.github.io/image2cpp/
// dont forget to invert image colors
//
const uint8_t PlayButton[] PROGMEM = {
// 'play', 50x50px
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 
0x1c, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x03, 0x80, 
0x00, 0x00, 0xe0, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x03, 
0x80, 0x00, 0x00, 0x00, 0x70, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x06, 0x00, 0x00, 
0x00, 0x00, 0x1c, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 
0x06, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x18, 0x00, 0x0c, 0x00, 0x00, 0x03, 0x00, 
0x30, 0x00, 0x0e, 0x00, 0x00, 0x03, 0x00, 0x30, 0x00, 0x0f, 0x80, 0x00, 0x03, 0x00, 0x60, 0x00, 
0x0d, 0xc0, 0x00, 0x01, 0x80, 0x60, 0x00, 0x0c, 0x70, 0x00, 0x01, 0x80, 0x60, 0x00, 0x0c, 0x38, 
0x00, 0x01, 0x80, 0x60, 0x00, 0x0c, 0x1e, 0x00, 0x01, 0x80, 0x60, 0x00, 0x0c, 0x07, 0x00, 0x01, 
0x80, 0x60, 0x00, 0x0c, 0x03, 0xc0, 0x00, 0x80, 0x60, 0x00, 0x0c, 0x00, 0xe0, 0x00, 0xc0, 0x60, 
0x00, 0x0c, 0x00, 0xe0, 0x00, 0xc0, 0x60, 0x00, 0x0c, 0x03, 0xc0, 0x00, 0x80, 0x60, 0x00, 0x0c, 
0x07, 0x00, 0x01, 0x80, 0x60, 0x00, 0x0c, 0x1e, 0x00, 0x01, 0x80, 0x60, 0x00, 0x0c, 0x3c, 0x00, 
0x01, 0x80, 0x60, 0x00, 0x0c, 0x70, 0x00, 0x01, 0x80, 0x60, 0x00, 0x0d, 0xe0, 0x00, 0x01, 0x80, 
0x30, 0x00, 0x0f, 0x80, 0x00, 0x03, 0x00, 0x30, 0x00, 0x0f, 0x00, 0x00, 0x03, 0x00, 0x10, 0x00, 
0x0c, 0x00, 0x00, 0x03, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x1c, 0x00, 0x00, 0x00, 
0x00, 0x06, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x1c, 
0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x38, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x30, 0x00, 0x01, 
0xc0, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x70, 0x00, 
0x00, 0x03, 0x80, 0x00, 0x00, 0x38, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x1e, 0x00, 0x00, 0x1e, 
0x00, 0x00, 0x00, 0x07, 0xc0, 0x00, 0xf8, 0x00, 0x00, 0x00, 0x01, 0xfe, 0x1f, 0xe0, 0x00, 0x00, 
0x00, 0x00, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00
};

// 'play', 50x50
// created with : http://javl.github.io/image2cpp/
// dont forget to invert image colors
//
const uint8_t AlarmButton[] PROGMEM = {
// 'alarm', 50x50px
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x06, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x27, 0xd9, 0x00, 0x00, 0x00, 0x00, 0x00, 0x50, 0x02, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xa0, 0x01, 0x40, 0x00, 0x00, 0x00, 0x01, 0x40, 0x00, 0xa0, 0x00, 0x00, 0x00, 0x02, 
0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x05, 0x18, 0x00, 
0x28, 0x00, 0x00, 0x00, 0x02, 0x0e, 0x18, 0x10, 0x00, 0x00, 0x00, 0x0a, 0x06, 0x38, 0x14, 0x00, 
0x00, 0x00, 0x0a, 0x03, 0xf0, 0x14, 0x00, 0x00, 0x00, 0x0a, 0x01, 0xa0, 0x14, 0x00, 0x00, 0x00, 
0x0a, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x14, 0x00, 0x00, 0x00, 0x02, 0x00, 
0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x28, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x80, 0x00, 0x50, 0x00, 0x00, 
0x00, 0x01, 0x40, 0x00, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x80, 0x01, 0x40, 0x00, 0x00, 0x00, 0x00, 
0x48, 0x04, 0x80, 0x00, 0x00, 0x00, 0x00, 0x23, 0xf1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x0c, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


//global RTC variables
//const uint8_t  LED_PIN             =     13; ///< Built-in Arduino green LED pin
//const uint32_t SERIAL_SPEED        = 115200; ///< Set the baud rate for Serial I/O
//const uint8_t  SPRINTF_BUFFER_SIZE =     32; ///< Buffer size for sprintf()

static uint8_t secs=61, min=61, hour=61, t=61;
String H="", M="", S="", T = "", D="";
boolean h_skip, m_skip, d_skip;
    
void setup(void)
{
    // init serial bus for MP3 player
    mySoftwareSerial.begin(9600);
    
    Serial.begin(115200);         // Serial Monitor Setup
    Serial.println( "starting with setup for Carl's MP3 Alarm Clock" );
  
    #ifdef  __AVR_ATmega32U4__  // If this is a 32U4 processor, then wait for the serial interface to initialize
        Serial.println( "AVR ATmega32U4 found");
        delay(3000);
    #endif
    
    //-- start init RTC
    Serial.println( "start init DS3231M RTC module" );
    while ( !DS3231M.begin() ) {
        Serial.println( "DS3231MM notok" );
        delay(3000);
    } // of loop until device is located

    DS3231M.pinSquareWave();  // Make INT/SQW pin toggle at 1Hz
    DS3231M.adjust();         // Set to library compile Date/Time
    Serial.println( "DS3231M now ok :-)  init done");

    Serial.println( "start init TFT" );
    uint16_t ID = tft.readID();
    Serial.print("found TFT.ID=0x");
    Serial.println(ID, HEX);
    
    if (ID == 0xD3D3) ID = 0x9481; //force ID if write-only display
    tft.begin(ID);
    tft.setRotation(3); 
    
    //tft.setFont(&FreeSevenSegNumFont);
    tft.setTextColor(BLACK);  
    tft.fillScreen(WHITE); // löscht den Screen komplett
    Serial.println( "TFT now availiable" );
    
    /*
    buildTimeStrings();
    showmsgXY(140, 180, 1, H );
    showmsgXY(220, 180, 1, M );
    showmsgXY(300, 180, 1, S );
    showTemperature();
    showDate();
    */
    
    tft.setFont(&FreeSans9pt7b);
    showmsgXY( 8, 300, 1, "MP3 Alarm Clock V0.1 (c) Carl Dietzel 2020" );

    tft.drawBitmap( 180,190, PlayButton, 50,50, BLACK);
    tft.drawBitmap( 240,190, AlarmButton, 50,50, BLACK);

    // if all the init's are okay we set LED to GREEN - not blinking
    pinMode(13,OUTPUT);    // Make the LED light an output pin

    Serial.println( "Initializing DFPlayer ... (May take some seconds)");
    if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
      Serial.println( "Unable to begin:");
      Serial.println( "1.Please recheck the connection!");
      Serial.println( "2.Please insert the SD card!");
      while(true);
    }

    
    Serial.println( "DFPlayer Mini init done :-)" );

    myDFPlayer.volume(20);  //Set volume value. From 0 to 30
 
    myDFPlayer.volumeUp(); //Volume Up
    myDFPlayer.volumeDown(); //Volume Down

    //----Set different EQ----
    myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
    //  myDFPlayer.EQ(DFPLAYER_EQ_POP);
    //  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
    //  myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);
    //  myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
    //  myDFPlayer.EQ(DFPLAYER_EQ_BASS);
  
    //myDFPlayer.play(1);  //Play the first mp3
  
    delay( 5000 );
  
}


void loop(void)  {  
    
  DateTime now = DS3231M.now(); // get the current time from device
  Serial.print( now.hour() ); 
  Serial.print( ":" );
  Serial.print( now.minute() );
  Serial.print( ":" );
  Serial.println( now.second() );
  
  // Output if seconds have changed
  if ( secs != now.second() ) {
    // Use sprintf() to pretty print the date/time with leading zeros 
    // char output_buffer[SPRINTF_BUFFER_SIZE]; ///< Temporary buffer for sprintf()
    // sprintf(output_buffer,"%04d-%02d-%02d %02d:%02d:%02d", now.year(), now.month(),now.day(), now.hour(), now.minute(), now.second());
    // Serial.println(output_buffer);
    secs = now.second(); // Set the counter variable
      
    if( min != now.minute() ) {
        Serial.print( "min :" ) ;
        Serial.println( min );
        Serial.print( "now.minute()" );
        Serial.println( now.minute() );
        Serial.println( "DFPlayer next()" );
        myDFPlayer.next();
        //printDetail(myDFPlayer.readType(), myDFPlayer.read()); 
        min = now.minute();
        m_skip = true;
        showTemperature();
    }
    
    if( hour != now.hour() ) {
        hour = now.hour();
        h_skip = true;
        showDate( now );
    }        
    showTime();
  } // of if the seconds have changed

  delay(675);
}

void showDate( DateTime now ) {
    tft.setFont(&FreeSerif12pt7b);
    deleteMsg( 350,50,1, D );
    //deleteMsg( 350,75,1, "Montag" );

    D = String( String( now.day() ) + "." + String( now.month() ) + "." + now.year());
    showmsgXY( 350,50, 1, D );
    //showmsgXY( 350,75,1, "Montag" );
}

void showTemperature() {
    tft.setFont(&FreeSerif12pt7b);
    deleteMsg( 50,50,1, T );
    T = String(DS3231M.temperature()/100.0);
    showmsgXY( 50,50, 1, T + " °C" );
}
void showTime() {
    //Serial.println( "TFT.setFont: FreeSevenSegNumFont" );
    tft.setFont(&FreeSevenSegNumFont);

    // clean screen from old message 
    deleteMsg( 300, 180, 1, S );
    if( m_skip ) deleteMsg( 220, 180, 1, M );
    if( h_skip ) deleteMsg( 140, 180, 1, H );
    
    buildTimeStrings();
    showmsgXY(300, 180, 1, S );
    if( m_skip) showmsgXY(220, 180, 1, M );
    if( h_skip) showmsgXY(140, 180, 1, H );

}

void showmsgXY(int x, int y, int sz, String msg)
{
    tft.setCursor(x, y);
    tft.setTextSize(sz);
    tft.println(msg);
}

void deleteMsg( int x, int y, int sz, String msg ) {
    int16_t x1, y1;
    uint16_t w, h;
    // this calculates the rectangle used by this String
    tft.getTextBounds(msg, x, y, &x1, &y1, &w, &h);
    tft.fillRect(x1, y1, w, h, WHITE );
}


void buildTimeStrings() {
    // build String for SEC
    
    if( secs < 10 ) S = "0"; else S = "";
    S += String( secs );

    // build String for MIN
    if( min < 10 ) M = "0"; else M ="";
    M += String(min);
    
    // build String for hour, display only when changed or 1stTime
    if( hour < 10 ) H = "0"; else H = "";
    H+= String(hour);
}

void printDetail(uint8_t type, int value){
  Serial.print( "DFPlayer :" );
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}
