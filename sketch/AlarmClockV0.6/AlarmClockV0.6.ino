/* 
 *  
 * Teil 1 : 
 * Laden deer Bibliotheken für die Bauteile
 * Initialisierung von Variablen
 *  
  */

/*
 * Arduino Bibliotheken
 */
#include "Arduino.h"
#define LED_PIN  13                            //Built-in Arduino green LED pin
#define SERIAL_SPEED 115200                    //Set the baud rate for Serial I/O
#define SPRINTF_BUFFER_SIZE 32                 // needed in readcommand

/*
 * DS3231 Real Time Clock 
 */
#include <DS3231M.h>            // Include the DS3231M RTC library
DS3231M_Class DS3231M;          // Create an instance of the DS3231M class

/*
 * DFRobot Player Mini Modul
 * MP3 Player
 * Asteueruing des Player erfolgt über SoftwareSerial mit 9600 Baud
 */
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;

/*
 * TFT Bildschirm ILY9486
 *  Tutorial für tft
 *  https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives
 */
#include <Adafruit_GFX.h>       // Core graphics library
#include <MCUFRIEND_kbv.h>      // Hardware-specific library
MCUFRIEND_kbv tft;              // tft ist unsere Variable für die Bildschirmansteuerung
#define BLACK   0x0000
#define RED     0xF800
#define GREEN   0x07E0
#define WHITE   0xFFFF
#define GREY    0x8410

/*
 * Fonts laden
 */
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>
#include <Fonts/FreeSerif24pt7b.h>
#include <FreeDefaultFonts.h>

/*
 * Variablen für Alarm
 */
static uint8_t secs=61, min=61, hour=61, t=61;
static uint8_t alarmMin = 255, alarmHour = 255;
static String H="", M="", S="", T = "", D="";

boolean h_skip, m_skip;
boolean alarmSet = false;
boolean alarmON = false;
boolean musicON = false;

/*
 * Touchscreen support
 */
#include <stdint.h>
#include "TouchScreen.h"
#define MINPRESSURE 10
#define MAXPRESSURE 100

// defintions for TouchScreen
// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
#define RESISTANCE 300
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin
#define TS_LEFT 907
#define TS_RT 136
#define TS_TOP 942
#define TS_BOT 139

TouchScreen ts = TouchScreen(XP, YP, XM, YM, RESISTANCE );
Adafruit_GFX_Button on_btn;
Adafruit_GFX_Button set_btn;

/*
 * unsere globale Funktion um eien TocuScreen Push zu erkennen
 * das Ergebnis wo gedrückt wurde witd in pixel_x und pixel_y gespeichert
 * x und y Koodinaten müssen auf TFT display grösse "map'ed" werden
 */
static int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void) {
    TSPoint p = ts.getPoint();
 
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
 
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        Serial.print( "Touch Event:" );
        Serial.print("X = "); Serial.print(p.x);
        Serial.print("\tY = "); Serial.print(p.y);
        Serial.print("\tPressure = "); Serial.println(p.z);
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width());
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}

/*
 * unsere setup Funktion
 * diese Funktion wird nur einmal durchlaufen beim Start des Arduinos
 * Hier machen wir Initialisierungen der Module
 */
void setup(void) {
    // init serial bus for MP3 player
    mySoftwareSerial.begin(9600);

    // unser Serial Monitor zur Ausgabe von Debug Infos ---------------
    // und zur Eingabe von Befehlen
    Serial.begin(SERIAL_SPEED);         // Serial Monitor Setup
    Serial.println( "starting with setup for Carl's MP3 Alarm Clock" );
      
    //-- start init RTC -----------------------------------------------
    Serial.println( "start init DS3231M RTC module" );
    while ( !DS3231M.begin() ) {
        Serial.println( "DS3231MM notok" );
        delay(3000);
    } // of loop until device is located

    DS3231M.pinSquareWave();  // Make INT/SQW pin toggle at 1Hz
    //DS3231M.adjust();         // Set to library compile Date/Time
    Serial.println( "DS3231M now ok :-)  init done");

    // Init des TFT's -------------------------------------------------
    Serial.println( "start init TFT" );
    uint16_t ID = tft.readID();
    Serial.print("found TFT.ID=0x");
    Serial.println(ID, HEX);
    
    if (ID == 0xD3D3) ID = 0x9481; //force ID if write-only display
    tft.begin(ID);
    tft.setRotation(3); 
    
    tft.setTextColor(WHITE);  
    tft.fillScreen(BLACK); // löscht den Screen komplett
    Serial.println( "TFT now availiable (WHITE painted on BLACK)" );
        
    // if all the init's are okay we set LED to GREEN - not blinking
    pinMode(LED_PIN,OUTPUT);    // Make the LED light an output pin

    // Init des MP3 Players ---------------------------------------------
    Serial.println( "Initializing DFPlayer ... (May take some millis)");
    if (!myDFPlayer.begin(mySoftwareSerial)) {  
      //Use softwareSerial to communicate with mp3.
      Serial.println( "Unable to begin:");
      Serial.println( "1.Please recheck the connection!");
      Serial.println( "2.Please insert the SD card!");
      while(true);
    }
    Serial.println( "DFPlayer Mini init done :-)" );
    myDFPlayer.volume(20);   //Set volume value. From 0 to 30
    myDFPlayer.volumeUp();   //Volume Up
    myDFPlayer.volumeDown(); //Volume Down

    //----Set different EQ----
    myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
    //  myDFPlayer.EQ(DFPLAYER_EQ_POP);
    //  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
    //  myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);
    //  myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
    //  myDFPlayer.EQ(DFPLAYER_EQ_BASS);

    // defintion of buttons 
    set_btn.initButton( &tft, 150, 295, 80, 40, BLACK, GREY, BLACK, "", 2);
    on_btn.initButton( &tft,  60, 295, 80, 40, BLACK, GREY, BLACK, "", 2);

    showAlarm();    
    //delay( 1000 ); // lets wait a second for proper async inits
}

void loop(void)  {  

  // handle commands from SerialMonitor at first
  readCommand();

  DateTime now = DS3231M.now(); // get the current time from device

  // Clock Display if seconds have change
  if ( secs != now.second() ) {
    secs = now.second(); // Set the counter variable

    m_skip = (min != now.minute());
    if( m_skip ) {
        min = now.minute();
        showTemperature();
        alarmON = false;
    }

    h_skip = (hour != now.hour());
    if( h_skip ) {
        hour = now.hour();
        showDate();
    }        
    showTime();
  }

  // TouchPoint abholen
  bool down = Touch_getXY();

  // wenn ein TouchPoint vorliegt 
  if( down ) {    
    Serial.println( "down touch found" );
    // prüfen ob der ON/OFF Button gedrück wurde
    if( on_btn.contains(pixel_x, pixel_y) > 0 ) {
        Serial.println( "ButtonTouch Found" );
        alarmSet = !alarmSet;
        
        // special case: powerON & button ON; set DEfault Time for Alarm
        Serial.println( alarmSet );
        Serial.println( alarmMin );
        if( alarmSet && alarmMin == 255 ) {
          alarmMin = 0;
          alarmHour = 12;
          alarmSet = true;
        }
        showAlarm();
        //delay( 1000 );
     }
     // prüfen of auf dem Alarm gedrück wurde
     else if( set_btn.contains(pixel_x, pixel_y) > 0 ) {
       Serial.println( "set button pressed" );
       alarmSet = true; 
       setAlarm();  // start Alarm-SET Dialog
       showAlarm();
     }
     // Single press; not pressing any button
     else if( alarmON || musicON ) {
        Serial.println( "Alarm Mode: touch point received to set alarm or playing music off" );
        musicON = false;
        myDFPlayer.pause();
        delay( 500 );
     }
     else {
        Serial.println( "Player Mode: touch point received to start music; we play randomAll" );
        myDFPlayer.randomAll();
        musicON = true;
        delay(1000);
     }
  }
  
  if(alarmSet) {
    if( now.hour() == alarmHour && now.minute() == alarmMin && alarmON == false ) {
      alarmON = true;
      Serial.println( "alarm ON !!  let the music play" );
      myDFPlayer.randomAll();
      delay(2000); // lets wait 2sec before we can switch off
      
    }
  }     
}


void showDate() {
    DateTime now = DS3231M.now(); // get the current time from device
    tft.setFont(&FreeSerif12pt7b);
    deleteMsg( 250,110,1, D );
    D = String( String( now.day() ) + "." + String( now.month() ) + "." + now.year());
    showmsgXY( 250,110, 1, D );
}

void showTemperature() {
    tft.setFont(&FreeSerif12pt7b);
    deleteMsg( 270,220,1, T );
    T = String(DS3231M.temperature()/100.0);
    showmsgXY( 270,220,1, T + " °C" );
}

void showAlarm() {
    String alarmString = "";
    String btnText = "";

    if( alarmSet ) {
      if( alarmHour < 10 ) alarmString += "0";
      alarmString += String(alarmHour);
      alarmString += ":";
      if( alarmMin < 10 ) alarmString += "0"; 
      alarmString += String(alarmMin);
      alarmString += "     ";
      btnText = "OFF";
    }
    else {
      btnText = "ON";
    }

    // Button is GREY and Font Color Black
    tft.setFont(&FreeSerif12pt7b);
    tft.setTextColor(BLACK);  

    // wenn Alarm ON ist, dann schreiben wir 
    // Uhrzeit auf den Button
    // Wenn Alarm OFF ist, dann zeigen wir diesen Button nicht an
    deleteMsg( 120, 300, 1, alarmString );
    if( alarmSet ) {
      set_btn.drawButton( false );
      showmsgXY( 120, 300, 1, alarmString );
    }
    else {
      // damit überschreiben wir den GREY Button mit einer 
      // schwarzen Box
      tft.fillRect(60, 255, 130, 80, BLACK );      
    }

    // das ist der ON / OFF Button für Alarm AN/AUS
    // dieser Button wird immer angezeigt
    // der Text auf dem Button ändert sich abhängig ob Alarm AN/AUS ist
    on_btn.drawButton(false);
    showmsgXY( 38, 300, 1, btnText );    
    tft.setTextColor(WHITE);  

    Serial.print( "showAlarm: " + alarmString );  
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

    tft.setFont(&FreeSerif12pt7b);
    showmsgXY( 210, 160, 1, ":" );
    showmsgXY( 290, 160, 1, ":" );
    
}

void showmsgXY(int x, int y, int sz, String msg) {
    tft.setCursor(x, y);
    tft.setTextSize(sz);
    tft.println(msg);
}

void deleteMsg( int x, int y, int sz, String msg ) {
    int16_t x1, y1;
    uint16_t w, h;
    // this calculates the rectangle used by this String
    tft.getTextBounds(msg, x, y, &x1, &y1, &w, &h);
    tft.fillRect(x1, y1, w, h, BLACK );
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

void setAlarm() {
  tft.fillScreen(WHITE); // löscht den Screen komplett
  tft.setTextColor(BLACK);  

  // 4 Buttons 
  Adafruit_GFX_Button btn_HU;
  btn_HU.initButton( &tft,   80, 110, 40, 40, WHITE, GREY, BLACK, "", 2);  // Hour Up
  Adafruit_GFX_Button btn_HD;
  btn_HD.initButton( &tft,   80, 220, 40, 40, WHITE, GREY, BLACK, "", 2);  // Hour Down
  Adafruit_GFX_Button btn_MU;
  btn_MU.initButton( &tft,  350, 110, 40, 40, WHITE, GREY, BLACK, "", 2);  // Minute Up
  Adafruit_GFX_Button btn_MD;
  btn_MD.initButton( &tft,  350, 220, 40, 40, WHITE, GREY, BLACK, "", 2);  // Minute Down

  btn_HU.drawButton(false);
  btn_HD.drawButton(false);
  btn_MU.drawButton(false);
  btn_MD.drawButton(false);

  tft.setFont(&FreeSerif24pt7b);
  showmsgXY( 68, 120, 1, "+" );
  showmsgXY( 72, 228, 1, "-" );
  showmsgXY(338, 120, 1, "+" );
  showmsgXY(342, 228, 1, "-" );
 

  Adafruit_GFX_Button btn_Exit;
  btn_Exit.initButton( &tft, 135, 115, 165, 90, WHITE, GREY, BLACK, "-", 2);  // Minute Down

  bool change = false;
  while (true) {
    change = false;
    if( Touch_getXY() ) {
      if(btn_HU.contains(pixel_x, pixel_y) > 0 ) {
       alarmHour ++;
       if( alarmHour == 24 ) alarmHour = 0;
       change = true;
      }
      else if(btn_HD.contains(pixel_x, pixel_y) > 0) {
       alarmHour--;
       if( alarmHour == -1 ) { alarmHour = 23;  } 
       change = true;
      }
      else if(btn_MU.contains(pixel_x, pixel_y) > 0) {
       alarmMin ++;
       if( alarmMin == 60 ) { alarmMin = 0;  }        
       change = true;
      }
      else if(btn_MD.contains(pixel_x, pixel_y) > 0) {
       alarmMin --;
       if( alarmMin == -1 ) { alarmMin = 59; }
       change = true;
      }
      else if( btn_Exit.contains(pixel_x,pixel_y) > 0 ){
        Serial.println( "exit aetAlarm" );
        delay( 1000 );
        break;
      }
      else {
        Serial.println( "touch doesn hit a button" );
      }
    }

    tft.fillRect(135, 115, 165, 90, GREY );      

    if( alarmMin < 10 ) M = "0"; else M ="";
    M += String(alarmMin);
    if( alarmHour < 10 ) H = "0"; else H = "";
    H+= String(alarmHour);
    tft.setFont(&FreeSevenSegNumFont);
    showmsgXY(140, 180, 1, H );
    showmsgXY(220, 180, 1, M );
    tft.setFont(&FreeSerif12pt7b);
    showmsgXY( 210, 160, 1, ":" );
    delay(75);
    
    if( change ) {
      Serial.print( "setAlarm: " ); Serial.print( H ) ; Serial.print( ":" ) ; Serial.println( M );  
    }
  }

  tft.fillScreen(BLACK);
  tft.setTextColor(WHITE);

  // little Trick to repaint H:MM
  m_skip = true;  // little Trick to repaint H:MM
  h_skip = true;
  buildTimeStrings();  
  showTime();
}
void readCommand() 
{
  /*************************************************************************************************************//*!
  * @brief    Read incoming data from the Serial port
  * @details  This function checks the serial port to see if there has been any input. If there is data it is read 
  *           until a terminator is discovered and then the command is parsed and acted upon
  * @return   void
  *****************************************************************************************************************/
  static char    text_buffer[SPRINTF_BUFFER_SIZE]; ///< Buffer for sprintf()/sscanf()
  static uint8_t text_index = 0;                   ///< Variable for buffer position
  while (Serial.available()) {
    // Loop while there is incoming serial data
    text_buffer[text_index] = Serial.read();       // Get the next byte of data
    // keep on reading until a newline shows up or the buffer is full
    if (text_buffer[text_index] != '\n' && text_index < SPRINTF_BUFFER_SIZE) {
      text_index++;
    }
    else
    {
      text_buffer[text_index] = 0;              // Add the termination character
      for (uint8_t i = 0; i < text_index; i++) {
      // Convert the whole input buffer to uppercase
        text_buffer[i] = toupper(text_buffer[i]);
      } 
      Serial.print(F("\nCommand \""));
      Serial.write(text_buffer);
      Serial.print(F("\" received.\n"));
      /*************************************************************************************************************
      ** Parse the single-line command and perform the appropriate action. The current list of commands           **
      ** understood are as follows:                                                                               **
      **                                                                                                          **
      ** SETDATE      - Set the device time                                                                       **
      **                                                                                                          **
      *************************************************************************************************************/
      enum commands { SetDate, SetAlarm, DeleteAlarm, PauseAlarm, ActivateAlarm, MusicPlay, MusicStop, Unknown_Command }; // enumerate all commands
      commands command;                           // declare enumerated type
      char workBuffer[SPRINTF_BUFFER_SIZE];       // Buffer to hold string compare
      sscanf(text_buffer,"%s %*s",workBuffer);    // Parse the string for first word
      if (!strcmp(workBuffer, "SETDATE")) {
        command = SetDate; // Set command number when found
      }
      else if ( (!strcmp(workBuffer, "SETALARM") ) ){
        command = SetAlarm; // Set command number when found
      }
      else if ( (!strcmp(workBuffer, "DELETEALARM") ) ){
        command = DeleteAlarm; // Set command number when found
      }
      else if ( (!strcmp(workBuffer, "PAUSEALARM") ) ){
        command = PauseAlarm; // Set command number when found
      }
      else if ( (!strcmp(workBuffer, "ACTIVATEALARM") ) ){
        command = ActivateAlarm; // Set command number when found
      }
      else if ( (!strcmp(workBuffer, "MUSICPLAY") ) ){
        command = MusicPlay; // Set command number when found
      }
      else if ( (!strcmp(workBuffer, "MUSICSTOP") ) ){
        command = MusicStop; // Set command number when found
      }
      else {
        command = Unknown_Command; // Otherwise set to not found
      }
      
      unsigned int tokens,year,month,day,hour,minute,second; // Variables to hold parsed date/time
      switch (command)
      {
        /***********************************************************************************************************
        ** Set the device time and date                                                                           **
        ***********************************************************************************************************/
        case SetDate:
          // Use sscanf() to parse the date/time into component variables
          tokens = sscanf(text_buffer,"%*s %u-%u-%u %u:%u:%u;",&year,&month,&day,&hour,&minute,&second);
          if (tokens != 6 && tokens !=5 ) {
            // Check to see if it was parsed correctly
            Serial.println("Unable to parse date/time");
            Serial.println("SETDATE yyyy-mm-dd hh:mm:ss");
            Serial.println("or");
            Serial.println("SETDATE yyyy-mm-dd hh:mm");
          }
          else
          {
            if( tokens != 6 ) second = 0;
            DS3231M.adjust(DateTime(year,month,day,hour,minute,second)); // Adjust the RTC date/time
            Serial.print(F("Date has been set."));
          }
          break;
        /***********************************************************************************************************
        ** SetAlarm                                                                                        **
        ***********************************************************************************************************/
        case SetAlarm:
          // Use sscanf() to parse the date/time into component variables
          tokens = sscanf(text_buffer,"%*s %u:%u;", &hour, &minute );
          if (tokens != 2) {
            Serial.println( "Unable to parse date/time" );
            Serial.println( "SETALARM hh:mm" );
          }
          else {
            Serial.println( "Alarm has been set" );
            alarmMin = minute;
            alarmHour = hour;
            if( alarmMin <0 || alarmMin > 59 ) alarmMin = 0;
            if( alarmHour <0 || alarmHour > 23 ) alarmHour = 12;       
            alarmSet = true;
            alarmON = false;
            showAlarm();
          } 
          break;        
        /***********************************************************************************************************
        ** DeleteAlarm                                                                                        **
        ***********************************************************************************************************/
        case DeleteAlarm:
          Serial.println( "Alarm has been deleted" );
          if( alarmON )  myDFPlayer.pause();
          alarmMin = 255;
          alarmHour = 255;
          alarmSet = false;
          alarmON = false;
          showAlarm();
          break;        
        /***********************************************************************************************************
        ** PauseAlarm                                                                                        **
        ***********************************************************************************************************/
        case PauseAlarm:
          Serial.println( "Alarm has been paused" );
          if( alarmON )  myDFPlayer.pause();
          alarmSet = false;
          alarmON = false;
          showAlarm();
          break;        
        /***********************************************************************************************************
        ** ActivateAlarm                                                                                        **
        ***********************************************************************************************************/
        case ActivateAlarm:
          Serial.println( "Alarm has been activated" );
          alarmSet = true;
          alarmON = false;
          if( alarmMin <0 || alarmMin > 59 ) alarmMin = 0;
          if( alarmHour <0 || alarmHour > 23 ) alarmHour = 12;
          showAlarm();
          break;        
        /***********************************************************************************************************
        ** MusicPlay command                                                                                      **
        ***********************************************************************************************************/
        case MusicPlay:
          myDFPlayer.play();
          break;
        /***********************************************************************************************************
        ** MusicStop command                                                                                      **
        ***********************************************************************************************************/
        case MusicStop:
          myDFPlayer.pause();
          break;
        /***********************************************************************************************************
        ** Unknown command                                                                                        **
        ***********************************************************************************************************/
        case Unknown_Command: // Show options on bad command
        default:
          Serial.println("Unknown command. Valid commands are:");
          Serial.println("SETDATE yyyy-mm-dd hh:mm:ss");
          Serial.println("SETALARM hh:mm");   
          Serial.println("DELETEALARM" );
          Serial.println("PAUSEALARM   (keeps the alarm time but did not start alarm)" );
          Serial.println("ACTIVATEALARM" );
          Serial.println("MUSICPLAY" );
          Serial.println("MUSICSTOP" );

      }
      text_index = 0; // reset the counter
    } 
  } 
}

void printDetail(uint8_t type, int value) {
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
