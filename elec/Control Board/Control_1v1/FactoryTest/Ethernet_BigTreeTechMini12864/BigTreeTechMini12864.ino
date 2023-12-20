#include <RotaryEncoder.h>
#include <U8g2lib.h>
#include <Adafruit_NeoPixel.h>

#define NEOPIX_DIN 43
#define NUMPIXELS 3
Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIX_DIN, NEO_RGB + NEO_KHZ400);


//Name the display pins on the Due GPIO
//#define DISPLAY_RESET 46            // Reset button on display front panel
//#define DISPLAY_DC 47               // Data on display
//#define DISPLAY_CS 48               // Chip select for display

// OLED Display
U8G2_ST7567_JLX12864_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ DISPLAY_CS, /* dc=*/ DISPLAY_DC, /* reset=*/ DISPLAY_RESET); //Rotation 180

//Rotary Encoder on BigTreeTech MINI 12864
#define PIN_IN1 40
#define PIN_IN2 41
#define ENC_SW 42   //A switch


void Mini12864Setup(void) {
  u8g2.begin();
  //  pinMode(ENC_SW, INPUT_PULLUP);  //Causes Link status : Unknown.
  //  setupBacklights(); //Setup the neopixels
  //  setupu8g2(); //Setup the graphics display
}


void setupBacklights(void) {
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setPixelColor(0, pixels.Color(150, 50, 50)); //encoder back light
  pixels.setPixelColor(1, pixels.Color(150, 50, 50)); //encoder back light
  pixels.setPixelColor(2, pixels.Color(250, 250, 250)); //Back light display
  pixels.show();
}

//Display splash screen at setup
void setupDisplay(void) {
  u8g2.begin();
  u8g2.setContrast(128);
}//end setupDisplay()

//Display splash screen at setup
void splashScreen(void) {
//  u8g2.begin();
//  u8g2.setContrast(128);
  u8g2.setFont(u8g2_font_helvB12_tr); //Large
  u8g2.setFontMode(1);
  u8g2.setCursor(0, 16);
  u8g2.print(COMPANY_NAME);
  u8g2.setCursor(0, 32);
  u8g2.print(PROG_NAME);
  u8g2.setFont(u8g2_font_6x10_mf); //Small, Not transparent font
  u8g2.setFontMode(0);
  u8g2.setCursor(0, 48);
  u8g2.print(F(__DATE__ " " __TIME__));
  u8g2.sendBuffer();
}

//Display link status
void reportLAN_DisplayUnknown(void) {
      u8g2.setFont(u8g2_font_6x10_mf); //Small, Not transparent font
      u8g2.setFontMode(0);
      u8g2.setCursor(0, 58);
      u8g2.print(F("Link: Unknown?"));
      u8g2.sendBuffer();
}//end unknown

void reportLAN_DisplayOn(void) {
      u8g2.setFont(u8g2_font_6x10_mf); //Small, Not transparent font
      u8g2.setFontMode(0);
      u8g2.setCursor(0, 58);
      u8g2.print(F("Link: On           "));
      u8g2.sendBuffer();
}//end On


void reportLAN_DisplayOff(void) {
      u8g2.setFont(u8g2_font_6x10_mf); //Small, Not transparent font
      u8g2.setFontMode(0);
      u8g2.setCursor(0, 58);
      u8g2.print(F("Link: Off         "));
      u8g2.sendBuffer();
}//end Off
