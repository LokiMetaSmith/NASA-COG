#include <BigTreeTechMini12864.h>

#include <RotaryEncoder.h>
#include <U8g2lib.h>
#include <Adafruit_NeoPixel.h>
#include <Ethernet.h>
#include <cog_hal.h>
#include <machine.h>

#include <SPI.h>



Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIX_DIN, NEO_RGB + NEO_KHZ400);

// OLED Display
U8G2_ST7567_JLX12864_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ DISPLAY_CS, /* dc=*/ DISPLAY_DC, /* reset=*/ DISPLAY_RESET); //Rotation 180
   int my_offsetX = 0;
    int my_offsetY = 41; //Beneath center
// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(PIN_IN1, PIN_IN2, RotaryEncoder::LatchMode::TWO03);


void Mini12864::Mini12864Setup(void) {
	

        // pinMode(DISPLAY_CS, OUTPUT);      // set the display pin CS pin mode 
		// pinMode(DISPLAY_DC, OUTPUT);
		// pinMode(DISPLAY_RESET, OUTPUT);
		// digitalWrite(DISPLAY_CS, HIGH);   // turn the CS on (HIGH is the logic level and is normally held high) 
		// digitalWrite(DISPLAY_DC, HIGH);   // turn the CS on (HIGH is the logic level and is normally held high)
		// digitalWrite(DISPLAY_RESET, HIGH);
		 
		delay(5);
  pinMode(ETHERNET_CS, OUTPUT);    // make sure that the default chip select pin is set to output, even if you don't use it:
  digitalWrite(ETHERNET_CS, HIGH);
  pinMode(4, OUTPUT);      // On the Ethernet Shield, CS is pin 4
  pinMode(DISPLAY_CS, OUTPUT);    // make sure that the default chip select pin is set to output, even if you don't use it:
  pinMode(DISPLAY_DC, OUTPUT);
  pinMode(DISPLAY_RESET, OUTPUT);
  digitalWrite(DISPLAY_CS, HIGH);   // turn the CS on (HIGH is the logic level and is normally held high)
  digitalWrite(DISPLAY_DC, HIGH);   // turn the CS on (HIGH is the logic level and is normally held high)
  digitalWrite(DISPLAY_RESET, HIGH);
 // pinMode(SHUT_DOWN, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);


  
  setupBacklights(); //Setup the neopixels
  
  
  //digitalWrite(DISPLAY_CS, HIGH);   // turn the CS on (HIGH is the logic level and is normally held high)
  //digitalWrite(DISPLAY_DC, HIGH);   // turn the CS on (HIGH is the logic level and is normally held high)
  disableDisplay();
  setupu8g2(); //Setup the graphics display
  disableDisplay();
  //digitalWrite(DISPLAY_CS, HIGH);   // turn the CS on (HIGH is the logic level and is normally held high)
  //digitalWrite(DISPLAY_DC, HIGH);   // turn the CS on (HIGH is the logic level and is normally held high)

  //  pinMode(ENC_SW, INPUT_PULLUP);  //Causes Link status : Unknown.
  //  setupBacklights(); //Setup the neopixels
  //  setupu8g2(); //Setup the graphics display
}

void Mini12864::u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void Mini12864::u8g2_box_frame(uint8_t a) {
  //  u8g2.drawStr( 0, 0, "drawBox");
  u8g2.drawStr( 5, 0, "drawBox");
  u8g2.drawBox(5, 10, 20, 10);
  u8g2.drawBox(10 + a, 15, 30, 7);
  u8g2.drawStr( 0, 30, "drawFrame");
  u8g2.drawFrame(5, 10 + 30, 20, 10);
  u8g2.drawFrame(10 + a, 15 + 30, 30, 7);
}

void Mini12864::u8g2_disc_circle(uint8_t a) {
  u8g2.drawStr( 0, 0, "drawDisc");
  u8g2.drawDisc(10, 18, 9);
  u8g2.drawDisc(24 + a, 16, 7);
  u8g2.drawStr( 0, 30, "drawCircle");
  u8g2.drawCircle(10, 18 + 30, 9);
  u8g2.drawCircle(24 + a, 16 + 30, 7);
}

void Mini12864::u8g2_r_frame(uint8_t a) {
  u8g2.drawStr( 0, 0, "drawRFrame/Box");
  u8g2.drawRFrame(5, 10, 40, 30, a + 1);
  u8g2.drawRBox(50, 10, 25, 40, a + 1);
}

void Mini12864::u8g2_string(uint8_t a) {
  u8g2.setFontDirection(0);
  u8g2.drawStr(30 + a, 31, " 0");
  u8g2.setFontDirection(1);
  u8g2.drawStr(30, 31 + a, " 90");
  u8g2.setFontDirection(2);
  u8g2.drawStr(30 - a, 31, " 180");
  u8g2.setFontDirection(3);
  u8g2.drawStr(30, 31 - a, " 270");
}

void Mini12864::u8g2_line(uint8_t a) {
  u8g2.drawStr( 0, 0, "drawLine");
  u8g2.drawLine(7 + a, 10, 40, 55);
  u8g2.drawLine(7 + a * 2, 10, 60, 55);
  u8g2.drawLine(7 + a * 3, 10, 80, 55);
  u8g2.drawLine(7 + a * 4, 10, 100, 55);
}

void Mini12864::u8g2_triangle(uint8_t a) {
  uint16_t offset = a;
  u8g2.drawStr( 0, 0, "drawTriangle");
  u8g2.drawTriangle(14, 7, 45, 30, 10, 40);
  u8g2.drawTriangle(14 + offset, 7 - offset, 45 + offset, 30 - offset, 57 + offset, 10 - offset);
  u8g2.drawTriangle(57 + offset * 2, 10, 45 + offset * 2, 30, 86 + offset * 2, 53);
  u8g2.drawTriangle(10 + offset, 40 + offset, 45 + offset, 30 + offset, 86 + offset, 53 + offset);
}

void Mini12864::u8g2_ascii_1() {
  char s[2] = " ";
  uint8_t x, y;
  u8g2.drawStr( 0, 0, "ASCII page 1");
  for ( y = 0; y < 6; y++ ) {
    for ( x = 0; x < 16; x++ ) {
      s[0] = y * 16 + x + 32;
      u8g2.drawStr(x * 7, y * 10 + 10, s);
    }
  }
}

void Mini12864::u8g2_ascii_2() {
  char s[2] = " ";
  uint8_t x, y;
  u8g2.drawStr( 0, 0, "ASCII page 2");
  for ( y = 0; y < 6; y++ ) {
    for ( x = 0; x < 16; x++ ) {
      s[0] = y * 16 + x + 160;
      u8g2.drawStr(x * 7, y * 10 + 10, s);
    }
  }
}

void Mini12864::u8g2_extra_page(uint8_t a)
{
  u8g2.drawStr( 0, 0, "Unicode");
  u8g2.setFont(u8g2_font_unifont_t_symbols);
  u8g2.setFontPosTop();
  u8g2.drawUTF8(0, 24, "☀ ☁");
  switch (a) {
    case 0:
    case 1:
    case 2:
    case 3:
      u8g2.drawUTF8(a * 3, 36, "☂");
      break;
    case 4:
    case 5:
    case 6:
    case 7:
      u8g2.drawUTF8(a * 3, 36, "☔");
      break;
  }
}

#define cross_width 24
#define cross_height 24
static const unsigned char cross_bits[] U8X8_PROGMEM  = {
  0x00, 0x18, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x00, 0x42, 0x00,
  0x00, 0x42, 0x00, 0x00, 0x42, 0x00, 0x00, 0x81, 0x00, 0x00, 0x81, 0x00,
  0xC0, 0x00, 0x03, 0x38, 0x3C, 0x1C, 0x06, 0x42, 0x60, 0x01, 0x42, 0x80,
  0x01, 0x42, 0x80, 0x06, 0x42, 0x60, 0x38, 0x3C, 0x1C, 0xC0, 0x00, 0x03,
  0x00, 0x81, 0x00, 0x00, 0x81, 0x00, 0x00, 0x42, 0x00, 0x00, 0x42, 0x00,
  0x00, 0x42, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x00, 0x18, 0x00,
};

#define cross_fill_width 24
#define cross_fill_height 24
static const unsigned char cross_fill_bits[] U8X8_PROGMEM  = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x18, 0x64, 0x00, 0x26,
  0x84, 0x00, 0x21, 0x08, 0x81, 0x10, 0x08, 0x42, 0x10, 0x10, 0x3C, 0x08,
  0x20, 0x00, 0x04, 0x40, 0x00, 0x02, 0x80, 0x00, 0x01, 0x80, 0x18, 0x01,
  0x80, 0x18, 0x01, 0x80, 0x00, 0x01, 0x40, 0x00, 0x02, 0x20, 0x00, 0x04,
  0x10, 0x3C, 0x08, 0x08, 0x42, 0x10, 0x08, 0x81, 0x10, 0x84, 0x00, 0x21,
  0x64, 0x00, 0x26, 0x18, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

#define cross_block_width 14
#define cross_block_height 14
static const unsigned char cross_block_bits[] U8X8_PROGMEM  = {
  0xFF, 0x3F, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20,
  0xC1, 0x20, 0xC1, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20,
  0x01, 0x20, 0xFF, 0x3F,
};

void Mini12864::u8g2_bitmap_overlay(uint8_t a) {
  uint8_t frame_size = 28;

  u8g2.drawStr(0, 0, "Bitmap overlay");

  u8g2.drawStr(0, frame_size + 12, "Solid / transparent");
  u8g2.setBitmapMode(false /* solid */);
  u8g2.drawFrame(0, 10, frame_size, frame_size);
  u8g2.drawXBMP(2, 12, cross_width, cross_height, cross_bits);
  if (a & 4)
    u8g2.drawXBMP(7, 17, cross_block_width, cross_block_height, cross_block_bits);

  u8g2.setBitmapMode(true /* transparent*/);
  u8g2.drawFrame(frame_size + 5, 10, frame_size, frame_size);
  u8g2.drawXBMP(frame_size + 7, 12, cross_width, cross_height, cross_bits);
  if (a & 4)
    u8g2.drawXBMP(frame_size + 12, 17, cross_block_width, cross_block_height, cross_block_bits);
}

void Mini12864::u8g2_bitmap_modes(uint8_t transparent) {
  const uint8_t frame_size = 24;

  u8g2.drawBox(0, frame_size * 0.5, frame_size * 5, frame_size);
  u8g2.drawStr(frame_size * 0.5, 50, "Black");
  u8g2.drawStr(frame_size * 2, 50, "White");
  u8g2.drawStr(frame_size * 3.5, 50, "XOR");

  if (!transparent) {
    u8g2.setBitmapMode(false /* solid */);
    u8g2.drawStr(0, 0, "Solid bitmap");
  } else {
    u8g2.setBitmapMode(true /* transparent*/);
    u8g2.drawStr(0, 0, "Transparent bitmap");
  }
  u8g2.setDrawColor(0);// Black
  u8g2.drawXBMP(frame_size * 0.5, 24, cross_width, cross_height, cross_bits);
  u8g2.setDrawColor(1); // White
  u8g2.drawXBMP(frame_size * 2, 24, cross_width, cross_height, cross_bits);
  u8g2.setDrawColor(2); // XOR
  u8g2.drawXBMP(frame_size * 3.5, 24, cross_width, cross_height, cross_bits);
}

uint8_t draw_state = 0;

void Mini12864::draw(void) {
  u8g2_prepare();
  switch (draw_state >> 3) {
    case 0: u8g2_box_frame(draw_state & 7); break;
    case 1: u8g2_disc_circle(draw_state & 7); break;
    case 2: u8g2_r_frame(draw_state & 7); break;
    case 3: u8g2_string(draw_state & 7); break;
    case 4: u8g2_line(draw_state & 7); break;
    case 5: u8g2_triangle(draw_state & 7); break;
    case 6: u8g2_ascii_1(); break;
    case 7: u8g2_ascii_2(); break;
    case 8: u8g2_extra_page(draw_state & 7); break;
    case 9: u8g2_bitmap_modes(0); break;
    case 10: u8g2_bitmap_modes(1); break;
    case 11: u8g2_bitmap_overlay(draw_state & 7); break;
  }
}

void Mini12864::setupu8g2(void) {
  //Display setup
  u8g2.begin();
  u8g2.setContrast(128);
  u8g2.setFont(u8g2_font_helvB12_tr); //FLE
  u8g2.setFontMode(1);
  u8g2.setCursor(0, 16);
 // u8g2.print(COMPANY_NAME);
  //  u8g2.print("Iot Demo");
  u8g2.setFont(u8g2_font_6x10_mf); //Small, Not transparent font
  u8g2.setFontMode(0);
  u8g2.setCursor(0, 32);
 // u8g2.print(PROG_NAME);
  u8g2.sendBuffer();
}
void Mini12864::setupBacklights(void) {
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  pixels.setPixelColor(0, pixels.Color(150, 50, 50)); //encoder back light
  pixels.setPixelColor(1, pixels.Color(150, 50, 50)); //encoder back light
  pixels.setPixelColor(2, pixels.Color(250, 250, 250)); //Back light display
  pixels.show();
}

//Display splash screen at setup
void Mini12864::setupDisplay(void) {
  u8g2.begin();
  u8g2.setContrast(128);
}//end setupDisplay()

//Display splash screen at setup
void Mini12864::splashScreen(void) {
//  u8g2.begin();
//  u8g2.setContrast(128);
  u8g2.setFont(u8g2_font_helvB12_tr); //Large
  u8g2.setFontMode(1);
  u8g2.setCursor(0, 16);
  u8g2.print("PubInv");
  u8g2.setCursor(0, 32);
  u8g2.print("OEDCS");
  u8g2.setFont(u8g2_font_6x10_mf); //Small, Not transparent font
  u8g2.setFontMode(0);
  u8g2.setCursor(0, 48);
  u8g2.print(F(__DATE__ " " __TIME__));
  u8g2.sendBuffer();
}

//Display link status
void Mini12864::reportLAN_DisplayUnknown(void) {
      u8g2.setFont(u8g2_font_6x10_mf); //Small, Not transparent font
      u8g2.setFontMode(0);
      u8g2.setCursor(0, 41);
      u8g2.print(F("Link: Unknown?"));
      u8g2.sendBuffer();
}//end unknown

void Mini12864::reportLAN_DisplayOn(void) {
      u8g2.setFont(u8g2_font_6x10_mf); //Small, Not transparent font
      u8g2.setFontMode(0);
      u8g2.setCursor(0, 41);
      u8g2.print(F("Link: On           "));
      u8g2.sendBuffer();
}//end On


void Mini12864::reportLAN_DisplayOff(void) {
      u8g2.setFont(u8g2_font_6x10_mf); //Small, Not transparent font
      u8g2.setFontMode(0);
      u8g2.setCursor(0, 41);
      u8g2.print(F("Link: Off         "));
      u8g2.sendBuffer();
}//end Off


//Check for encoder button pressed and return true
bool Mini12864::updateENC_BUT() {
  if (digitalRead(ENC_SW) == LOW) {
    Serial.println("ENC pressed");
    digitalWrite(BEEPER, !digitalRead(BEEPER));  //Make some sound
    return true; //Reset the position
  } else {

    return false;
  }
}//end update shutdown button

void Mini12864::enableDisplay()
{
	
        digitalWrite(DISPLAY_CS, LOW);       // select Display mode
}

void Mini12864::disableDisplay()
{
	
        digitalWrite(DISPLAY_CS, HIGH);       // deselect Display mode
        digitalWrite(DISPLAY_DC, HIGH);   // turn the CS on (HIGH is the logic level and is normally held high)
}

void Mini12864::Update_Display()
{
	
      // check to see if it's time to change the state of the LED
     // unsigned long currentMillis = millis();
     // if (currentMillis - previousMillis >= ReadPeriod)
     // {
       // previousMillis = currentMillis;  // Remember the time
       // voltage = //analogRead(ADCinPin) * 3.3 * (my_R1 + my_R2) / (1023 * my_R2); // RAW Read of the ADC
       // Serial.print(my_pinName);  //
       // Serial.print(": ");  //
       // Serial.println(voltage);  // RAW Read of the ADC
       enableDisplay();
        //Update OLED display
        u8g2.setFont(u8g2_font_6x10_mf); //Not transparent font
        u8g2.setFontMode(0);
        u8g2.setCursor(my_offsetX, my_offsetY);
        u8g2.print("               ");
        u8g2.sendBuffer();
       // switch (link_status) {
       //   case Unknown:
       //     reportLAN_DisplayUnknown();
       //     break;
       //   case LinkON:
            reportLAN_DisplayOn();
        //    break;
       //   case LinkOFF:
       //     reportLAN_DisplayOff();
       //     break;
      //  }
        u8g2.setCursor(my_offsetX, my_offsetY);
        //u8g2.print(my_pinName);
       // u8g2.print(99);
        u8g2.sendBuffer();
		disableDisplay();
}
   // }
