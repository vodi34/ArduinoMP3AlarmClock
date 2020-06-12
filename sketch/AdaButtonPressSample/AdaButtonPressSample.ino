#if 1

#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
#include <TouchScreen.h>
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
//const int XP = 6, XM = A2, YP = A1, YM = 7; //ID=0x9341
const int TS_LEFT = 907, TS_RT = 136, TS_TOP = 942, TS_BOT = 139;


#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM 8   // can be a digital pin
#define XP 9   // can be a digital pin
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);


Adafruit_GFX_Button on_btn, off_btn;

int pixel_x, pixel_y;     //Touch_getXY() updates global vars
bool Touch_getXY(void)
{
    TSPoint p = ts.getPoint();
   
    pinMode(YP, OUTPUT);      //restore shared pins
    pinMode(XM, OUTPUT);
    digitalWrite(YP, HIGH);   //because TFT control pins
    digitalWrite(XM, HIGH);
    bool pressed = (p.z > MINPRESSURE && p.z < MAXPRESSURE);
    if (pressed) {
        Serial.print( "Touch Event:" );
        Serial.print("X = "); Serial.print(p.x);
        Serial.print("\tY = "); Serial.print(p.y);
        Serial.print("\tPressure = "); Serial.println(p.z);
        pixel_x = map(p.x, TS_LEFT, TS_RT, 0, tft.width()); //.kbv makes sense to me
        pixel_y = map(p.y, TS_TOP, TS_BOT, 0, tft.height());
    }
    return pressed;
}

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0xFFEE
//#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

static int status = 0; // off

void setup(void)
{
    Serial.begin(9600);
    uint16_t ID = tft.readID();
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    Serial.println("Calibrate for your Touch Panel");
    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    tft.begin(ID);
    tft.setRotation(3);            
    tft.fillScreen(BLACK);
    on_btn.initButton(&tft,  60, 295, 80, 40, WHITE, CYAN, BLACK, "ON", 2);
    //off_btn.initButton(&tft, 180, 295, 80, 40, WHITE, CYAN, BLACK, "OFF", 2);
    on_btn.drawButton(false);
    //off_btn.drawButton(false);
    tft.fillRect(40, 80, 160, 80, RED);
}

/* two buttons are quite simple
 */
void loop(void)
{
    bool down = Touch_getXY();
    on_btn.press(down && on_btn.contains(pixel_x, pixel_y));
    //off_btn.press(down && off_btn.contains(pixel_x, pixel_y));
    if (on_btn.justReleased()) {

        if( status == 1 ) {
           on_btn.initButton(&tft,  60, 295, 80, 40, WHITE, CYAN, BLACK, "OFF", 2);
           status = 0;
        }
        else {
           on_btn.initButton(&tft,  60, 295, 80, 40, WHITE, CYAN, BLACK, "ON", 2);
           status = 1;
        }
        
        on_btn.drawButton();
        delay( 1000 );
    }
    
   
}
#endif
