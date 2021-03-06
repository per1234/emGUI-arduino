/***************************************************
  Arduino TFT graphics library targetted at ESP8266
  based boards such as the NodeMCU.

  This library has been derived from the Adafruit_GFX
  library and the associated driver library. See text
  at the end of this file.

  This is a standalone library that contains the
  hardware driver, the graphics funtions and the
  proportional fonts.

  The larger fonts are Run Length Encoded to reduce their
  size.

 ****************************************************/

 #include "TFT_ILI9341_ESP.h"
 
 #include <pgmspace.h>
 
 #include <limits.h>
 #include "pins_arduino.h"
 #include "wiring_private.h"
 #include <SPI.h>
 
 // If the SPI library has transaction support, these functions
 // establish settings and protect from interference from other
 // libraries.  Otherwise, they simply do nothing.
 
 inline void TFT_ILI9341_ESP::spi_begin(void){
 #ifdef SPI_HAS_TRANSACTION
   #ifdef SUPPORT_TRANSACTIONS
   _SPI->beginTransaction(SPISettings(SPI_FREQUENCY, MSBFIRST, SPI_MODE0));
   #endif
 #endif
 }
 
 inline void TFT_ILI9341_ESP::spi_end(void){
 #ifdef SPI_HAS_TRANSACTION
   #ifdef SUPPORT_TRANSACTIONS
   _SPI->endTransaction();
   #endif
 #endif
 }
 
 /***************************************************************************************
 ** Function name:           TFT_ILI9341_ESP
 ** Description:             Constructor , we must use hardware SPI pins
 ***************************************************************************************/
 TFT_ILI9341_ESP::TFT_ILI9341_ESP(int16_t w, int16_t h)
 {
 
   _dc   = TFT_DC;
 
 #ifdef USE_SPI9
   _SPI = &SPI9; // addition JZ 27.12.2016
 #else
   _SPI = &SPI; // fixed JZ 27.12.2016
 #endif
 
   //if (TFT_RST > 0) {
     //digitalWrite(TFT_RST, LOW);
     //pinMode(TFT_RST, OUTPUT);
   //}
 
   digitalWrite(TFT_DC, HIGH);
   pinMode(TFT_DC, OUTPUT);
 
   _width    = w;
   _height   = h;
   rotation  = 0;
   cursor_y  = cursor_x    = 0;
   textfont  = 1;
   textsize  = 1;
   textcolor   = 0xFFFF;
   textbgcolor = 0x0000;
   padX = 0;
   textwrap  = true;
   textdatum = 0; // Left text alignment is default
   fontsloaded = 0;
 
   addr_row = 0xFFFF;
   addr_col = 0xFFFF;
   win_xe = 0xFFFF;
   win_ye = 0xFFFF;
 
 #ifdef LOAD_GLCD
   fontsloaded = 0x0002; // Bit 1 set
 #endif
 
 #ifdef LOAD_FONT2
   fontsloaded |= 0x0004; // Bit 2 set
 #endif
 
 #ifdef LOAD_FONT4
   fontsloaded |= 0x0010; // Bit 4 set
 #endif
 
 #ifdef LOAD_FONT6
   fontsloaded |= 0x0040; // Bit 6 set
 #endif
 
 #ifdef LOAD_FONT7
   fontsloaded |= 0x0080; // Bit 7 set
 #endif
 
 #ifdef LOAD_FONT8
   fontsloaded |= 0x0100; // Bit 8 set
 #endif
 
 }
 
 /***************************************************************************************
 ** Function name:           spiwrite
 ** Description:             Write 8 bits to SPI port
 ***************************************************************************************/
 void TFT_ILI9341_ESP::spiwrite(uint8_t c)
 {
   _SPI->transfer(c);
 }
 
 /***************************************************************************************
 ** Function name:           writecommand
 ** Description:             Send an 8 bit command to the TFT
 ***************************************************************************************/
 void TFT_ILI9341_ESP::writecommand(uint8_t c)
 {
   *dcport &= ~dcpinmask;
   spiwrite(c);
   *dcport |= dcpinmask;
 }
 
 /***************************************************************************************
 ** Function name:           writedata
 ** Description:             Send a 8 bit data value to the TFT
 ***************************************************************************************/
 void TFT_ILI9341_ESP::writedata(uint8_t c)
 {
   spiwrite(c);
 }
 
 /***************************************************************************************
 ** Function name:           begin
 ** Description:             Included for backwards compatibility
 ***************************************************************************************/
 void TFT_ILI9341_ESP::begin(void)
 {
  init();
 }
 
 /***************************************************************************************
 ** Function name:           init
 ** Description:             Reset, then initialise the TFT display registers
 ***************************************************************************************/
 void TFT_ILI9341_ESP::init(void)
 {
   dcport    = portOutputRegister(digitalPinToPort(_dc));
   dcpinmask = digitalPinToBitMask(_dc);
 
   _SPI->begin();
   _SPI->setHwCs(true);
 
 #ifndef SUPPORT_TRANSACTIONS
   _SPI->setBitOrder(MSBFIRST);
   _SPI->setDataMode(SPI_MODE0);
   _SPI->setFrequency(SPI_FREQUENCY);
 #endif
 
     spi_begin();
 
   //digitalWrite(TFT_RST, HIGH);
   //delay(1); //Delay 1ms
   //digitalWrite(TFT_RST, LOW);
   //delay(10); //Delay 10ms
   //digitalWrite(TFT_RST, HIGH);
   delay(120); //Delay 120ms
   //---------------------------------------------------------------------------------------------------//
   writecommand (ILI9341_SLPOUT);
   delay(120); //Delay 120ms 
 
   // DATASHEET SEQUENCE
   writecommand (ILI9341_MADCTL);
   writedata(0x00);

   writecommand (ILI9341_PIXFMT);
   writedata(0x05);

   writecommand (ILI9341_INVON);

   writecommand (ILI9341_CASET);
   writedata(0x00);
   writedata(0x00);
   writedata(0x00);
   writedata(0xef);

   writecommand (ILI9341_PASET);
   writedata(0x00);
   writedata(0x00);
   writedata(0x00);
   writedata(0xef);
   //--------------------------------ST7789V Frame ratesetting----------------------------------//
   writecommand (ILI9341_FRMCTR2);
   writedata(0x0c);
   writedata(0x0c);
   writedata(0x00);
   writedata(0x33);
   writedata(0x33);

   writecommand (0xb7);
   writedata(0x35); 
   writecommand (0xbb);
   writedata(0x1f);
   writecommand (0xc0);
   writedata(0x2c);
   writecommand (0xc2);
   writedata(0x01);
   writecommand (0xc3);
   writedata(0x12);
   writecommand (0xc4);
   writedata(0x20);
   writecommand (0xc6);
   writedata(0x0f);
   writecommand (0xd0);
   writedata(0xa4);
   writedata(0xa1);
   
   //--------------------------------ST7789V gammasetting--------------------------------------//
   writecommand (ILI9341_GMCTRP1);
   writedata(0xd0);
   writedata(0x08);
   writedata(0x11);
   writedata(0x08);
   writedata(0x0c);
   writedata(0x15);
   writedata(0x39);
   writedata(0x33);
   writedata(0x50);
   writedata(0x36);
   writedata(0x13);
   writedata(0x14);
   writedata(0x29);
   writedata(0x2d);
   writecommand (ILI9341_GMCTRN1);
   writedata(0xd0);
   writedata(0x08);
   writedata(0x10);
   writedata(0x08);
   writedata(0x06);
   writedata(0x06);
   writedata(0x39);
   writedata(0x44);
   writedata(0x51);
   writedata(0x0b);
   writedata(0x16);
   writedata(0x14);
   writedata(0x2f);
   writedata(0x31);
   writecommand (ILI9341_DISPON);
 }
 
 
 /***************************************************************************************
 ** Function name:           drawCircle
 ** Description:             Draw a circle outline
 ***************************************************************************************/
 void TFT_ILI9341_ESP::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
 {
   int16_t f = 1 - r;
   int16_t ddF_x = 1;
   int16_t ddF_y = - r - r;
   int16_t x = 0;
 
   //fastSetup();
 
   drawPixel(x0 + r, y0  , color);
   drawPixel(x0 - r, y0  , color);
   drawPixel(x0  , y0 - r, color);
   drawPixel(x0  , y0 + r, color);
 
   while (x < r) {
     if (f >= 0) {
       r--;
       ddF_y += 2;
       f += ddF_y;
     }
     x++;
     ddF_x += 2;
     f += ddF_x;
 
     drawPixel(x0 + x, y0 + r, color);
     drawPixel(x0 - x, y0 + r, color);
     drawPixel(x0 - x, y0 - r, color);
     drawPixel(x0 + x, y0 - r, color);
 
     drawPixel(x0 + r, y0 + x, color);
     drawPixel(x0 - r, y0 + x, color);
     drawPixel(x0 - r, y0 - x, color);
     drawPixel(x0 + r, y0 - x, color);
   }
 }
 
 /***************************************************************************************
 ** Function name:           drawCircleHelper
 ** Description:             Support function for circle drawing
 ***************************************************************************************/
 void TFT_ILI9341_ESP::drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
 {
   int16_t f     = 1 - r;
   int16_t ddF_x = 1;
   int16_t ddF_y = -2 * r;
   int16_t x     = 0;
 
   while (x < r) {
     if (f >= 0) {
       r--;
       ddF_y += 2;
       f     += ddF_y;
     }
     x++;
     ddF_x += 2;
     f     += ddF_x;
     if (cornername & 0x4) {
       drawPixel(x0 + x, y0 + r, color);
       drawPixel(x0 + r, y0 + x, color);
     }
     if (cornername & 0x2) {
       drawPixel(x0 + x, y0 - r, color);
       drawPixel(x0 + r, y0 - x, color);
     }
     if (cornername & 0x8) {
       drawPixel(x0 - r, y0 + x, color);
       drawPixel(x0 - x, y0 + r, color);
     }
     if (cornername & 0x1) {
       drawPixel(x0 - r, y0 - x, color);
       drawPixel(x0 - x, y0 - r, color);
     }
   }
 }
 
 /***************************************************************************************
 ** Function name:           fillCircle
 ** Description:             draw a filled circle
 ***************************************************************************************/
 void TFT_ILI9341_ESP::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
 {
   drawFastVLine(x0, y0 - r, r + r + 1, color);
   fillCircleHelper(x0, y0, r, 3, 0, color);
 }
 
 /***************************************************************************************
 ** Function name:           fillCircleHelper
 ** Description:             Support function for filled circle drawing
 ***************************************************************************************/
 // Used to do circles and roundrects
 void TFT_ILI9341_ESP::fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
 {
   int16_t f     = 1 - r;
   int16_t ddF_x = 1;
   int16_t ddF_y = -r - r;
   int16_t x     = 0;
 
   delta++;
   while (x < r) {
     if (f >= 0) {
       r--;
       ddF_y += 2;
       f     += ddF_y;
     }
     x++;
     ddF_x += 2;
     f     += ddF_x;
 
     if (cornername & 0x1) {
       drawFastVLine(x0 + x, y0 - r, r + r + delta, color);
       drawFastVLine(x0 + r, y0 - x, x + x + delta, color);
     }
     if (cornername & 0x2) {
       drawFastVLine(x0 - x, y0 - r, r + r + delta, color);
       drawFastVLine(x0 - r, y0 - x, x + x + delta, color);
     }
   }
 }
 
 /***************************************************************************************
 ** Function name:           drawEllipse
 ** Description:             Draw a ellipse outline
 ***************************************************************************************/
 void TFT_ILI9341_ESP::drawEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color)
 {
   if (rx<2) return;
   if (ry<2) return;
   int16_t x, y;
   int32_t rx2 = rx * rx;
   int32_t ry2 = ry * ry;
   int32_t fx2 = 4 * rx2;
   int32_t fy2 = 4 * ry2;
   int32_t s;
 
   //fastSetup();
 
   for (x = 0, y = ry, s = 2*ry2+rx2*(1-2*ry); ry2*x <= rx2*y; x++)
   {
     drawPixel(x0 + x, y0 + y, color);
     drawPixel(x0 - x, y0 + y, color);
     drawPixel(x0 - x, y0 - y, color);
     drawPixel(x0 + x, y0 - y, color);
     if (s >= 0)
     {
       s += fx2 * (1 - y);
       y--;
     }
     s += ry2 * ((4 * x) + 6);
   }
 
   for (x = rx, y = 0, s = 2*rx2+ry2*(1-2*rx); rx2*y <= ry2*x; y++)
   {
     drawPixel(x0 + x, y0 + y, color);
     drawPixel(x0 - x, y0 + y, color);
     drawPixel(x0 - x, y0 - y, color);
     drawPixel(x0 + x, y0 - y, color);
     if (s >= 0)
     {
       s += fy2 * (1 - x);
       x--;
     }
     s += rx2 * ((4 * y) + 6);
   }
 }
 
 /***************************************************************************************
 ** Function name:           fillEllipse
 ** Description:             draw a filled ellipse
 ***************************************************************************************/
 void TFT_ILI9341_ESP::fillEllipse(int16_t x0, int16_t y0, int16_t rx, int16_t ry, uint16_t color)
 {
   if (rx<2) return;
   if (ry<2) return;
   int16_t x, y;
   int32_t rx2 = rx * rx;
   int32_t ry2 = ry * ry;
   int32_t fx2 = 4 * rx2;
   int32_t fy2 = 4 * ry2;
   int32_t s;
 
   for (x = 0, y = ry, s = 2*ry2+rx2*(1-2*ry); ry2*x <= rx2*y; x++)
   {
     drawFastHLine(x0 - x, y0 - y, x + x + 1, color);
     drawFastHLine(x0 - x, y0 + y, x + x + 1, color);
 
     if (s >= 0)
     {
       s += fx2 * (1 - y);
       y--;
     }
     s += ry2 * ((4 * x) + 6);
   }
 
   for (x = rx, y = 0, s = 2*rx2+ry2*(1-2*rx); rx2*y <= ry2*x; y++)
   {
     drawFastHLine(x0 - x, y0 - y, x + x + 1, color);
     drawFastHLine(x0 - x, y0 + y, x + x + 1, color);
 
     if (s >= 0)
     {
       s += fy2 * (1 - x);
       x--;
     }
     s += rx2 * ((4 * y) + 6);
   }
 
 }
 
 /***************************************************************************************
 ** Function name:           fillScreen
 ** Description:             Clear the screen to defined colour
 ***************************************************************************************/
 void TFT_ILI9341_ESP::fillScreen(uint16_t color)
 {
   fillRect(0, 0, _width, _height, color);
 }
 
 /***************************************************************************************
 ** Function name:           drawRect
 ** Description:             Draw a rectangle outline
 ***************************************************************************************/
 // Draw a rectangle
 void TFT_ILI9341_ESP::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
 {
   drawFastHLine(x, y, w, color);
   drawFastHLine(x, y + h - 1, w, color);
   drawFastVLine(x, y, h, color);
   drawFastVLine(x + w - 1, y, h, color);
 }
 
 /***************************************************************************************
 ** Function name:           drawRoundRect
 ** Description:             Draw a rounded corner rectangle outline
 ***************************************************************************************/
 // Draw a rounded rectangle
 void TFT_ILI9341_ESP::drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
 {
   // smarter version
   drawFastHLine(x + r  , y    , w - r - r, color); // Top
   drawFastHLine(x + r  , y + h - 1, w - r - r, color); // Bottom
   drawFastVLine(x    , y + r  , h - r - r, color); // Left
   drawFastVLine(x + w - 1, y + r  , h - r - r, color); // Right
   // draw four corners
   drawCircleHelper(x + r    , y + r    , r, 1, color);
   drawCircleHelper(x + w - r - 1, y + r    , r, 2, color);
   drawCircleHelper(x + w - r - 1, y + h - r - 1, r, 4, color);
   drawCircleHelper(x + r    , y + h - r - 1, r, 8, color);
 }
 
 /***************************************************************************************
 ** Function name:           fillRoundRect
 ** Description:             Draw a rounded corner filled rectangle
 ***************************************************************************************/
 // Fill a rounded rectangle
 void TFT_ILI9341_ESP::fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t color)
 {
   // smarter version
   fillRect(x + r, y, w - r - r, h, color);
 
   // draw four corners
   fillCircleHelper(x + w - r - 1, y + r, r, 1, h - r - r - 1, color);
   fillCircleHelper(x + r    , y + r, r, 2, h - r - r - 1, color);
 }
 
 /***************************************************************************************
 ** Function name:           drawTriangle
 ** Description:             Draw a triangle outline using 3 arbitrary points
 ***************************************************************************************/
 // Draw a triangle
 void TFT_ILI9341_ESP::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
 {
   drawLine(x0, y0, x1, y1, color);
   drawLine(x1, y1, x2, y2, color);
   drawLine(x2, y2, x0, y0, color);
 }
 
 /***************************************************************************************
 ** Function name:           fillTriangle 
 ** Description:             Draw a filled triangle using 3 arbitrary points
 ***************************************************************************************/
 // Fill a triangle - original Adafruit function works well and code footprint is small
 void TFT_ILI9341_ESP::fillTriangle ( int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
 {
   int16_t a, b, y, last;
 
   // Sort coordinates by Y order (y2 >= y1 >= y0)
   if (y0 > y1) {
     swap(y0, y1); swap(x0, x1);
   }
   if (y1 > y2) {
     swap(y2, y1); swap(x2, x1);
   }
   if (y0 > y1) {
     swap(y0, y1); swap(x0, x1);
   }
 
   if (y0 == y2) { // Handle awkward all-on-same-line case as its own thing
     a = b = x0;
     if (x1 < a)      a = x1;
     else if (x1 > b) b = x1;
     if (x2 < a)      a = x2;
     else if (x2 > b) b = x2;
     drawFastHLine(a, y0, b - a + 1, color);
     return;
   }
 
   int16_t
   dx01 = x1 - x0,
   dy01 = y1 - y0,
   dx02 = x2 - x0,
   dy02 = y2 - y0,
   dx12 = x2 - x1,
   dy12 = y2 - y1,
   sa   = 0,
   sb   = 0;
 
   // For upper part of triangle, find scanline crossings for segments
   // 0-1 and 0-2.  If y1=y2 (flat-bottomed triangle), the scanline y1
   // is included here (and second loop will be skipped, avoiding a /0
   // error there), otherwise scanline y1 is skipped here and handled
   // in the second loop...which also avoids a /0 error here if y0=y1
   // (flat-topped triangle).
   if (y1 == y2) last = y1;  // Include y1 scanline
   else         last = y1 - 1; // Skip it
 
   for (y = y0; y <= last; y++) {
     a   = x0 + sa / dy01;
     b   = x0 + sb / dy02;
     sa += dx01;
     sb += dx02;
 
     if (a > b) swap(a, b);
     drawFastHLine(a, y, b - a + 1, color);
   }
 
   // For lower part of triangle, find scanline crossings for segments
   // 0-2 and 1-2.  This loop is skipped if y1=y2.
   sa = dx12 * (y - y1);
   sb = dx02 * (y - y0);
   for (; y <= y2; y++) {
     a   = x1 + sa / dy12;
     b   = x0 + sb / dy02;
     sa += dx12;
     sb += dx02;
 
     if (a > b) swap(a, b);
     drawFastHLine(a, y, b - a + 1, color);
   }
 }
 
 /***************************************************************************************
 ** Function name:           drawBitmap
 ** Description:             Draw an image stored in an array on the TFT
 ***************************************************************************************/
 void TFT_ILI9341_ESP::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {
 
   int16_t i, j, byteWidth = (w + 7) / 8;
   //fastSetup();
   for (j = 0; j < h; j++) {
     for (i = 0; i < w; i++ ) {
       if (pgm_read_byte(bitmap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
         drawPixel(x + i, y + j, color);
       }
     }
   }
 }
 
 /***************************************************************************************
 ** Function name:           setCursor
 ** Description:             Set the text cursor x,y position
 ***************************************************************************************/
 void TFT_ILI9341_ESP::setCursor(int16_t x, int16_t y)
 {
   cursor_x = x;
   cursor_y = y;
 }
 
 /***************************************************************************************
 ** Function name:           setCursor
 ** Description:             Set the text cursor x,y position and font
 ***************************************************************************************/
 void TFT_ILI9341_ESP::setCursor(int16_t x, int16_t y, uint8_t font)
 {
   textfont = font;
   cursor_x = x;
   cursor_y = y;
 }
 
 /***************************************************************************************
 ** Function name:           setTextSize
 ** Description:             Set the text size multiplier
 ***************************************************************************************/
 void TFT_ILI9341_ESP::setTextSize(uint8_t s)
 {
   if (s>7) s = 7; // Limit the maximum size multiplier so byte variables can be used for rendering
   textsize = (s > 0) ? s : 1; // Don't allow font size 0
 }
 
 /***************************************************************************************
 ** Function name:           setTextFont
 ** Description:             Set the font for the print stream
 ***************************************************************************************/
 void TFT_ILI9341_ESP::setTextFont(uint8_t f)
 {
   textfont = (f > 0) ? f : 1; // Don't allow font 0
 }
 
 /***************************************************************************************
 ** Function name:           setTextColor
 ** Description:             Set the font foreground colour (background is transparent)
 ***************************************************************************************/
 void TFT_ILI9341_ESP::setTextColor(uint16_t c)
 {
   // For 'transparent' background, we'll set the bg
   // to the same as fg instead of using a flag
   textcolor = textbgcolor = c;
 }
 
 /***************************************************************************************
 ** Function name:           setTextColor
 ** Description:             Set the font foreground and background colour
 ***************************************************************************************/
 void TFT_ILI9341_ESP::setTextColor(uint16_t c, uint16_t b)
 {
   textcolor   = c;
   textbgcolor = b;
 }
 
 /***************************************************************************************
 ** Function name:           setTextWrap
 ** Description:             Define if text should wrap at end of line
 ***************************************************************************************/
 void TFT_ILI9341_ESP::setTextWrap(boolean w)
 {
   textwrap = w;
 }
 
 /***************************************************************************************
 ** Function name:           setTextDatum
 ** Description:             Set the text position reference datum
 ***************************************************************************************/
 void TFT_ILI9341_ESP::setTextDatum(uint8_t d)
 {
   textdatum = d;
 }
 
 /***************************************************************************************
 ** Function name:           setTextPadding
 ** Description:             Define padding width (aids erasing old text and numbers)
 ***************************************************************************************/
 void TFT_ILI9341_ESP::setTextPadding(uint16_t x_width)
 {
   padX = x_width;
 }
 
 /***************************************************************************************
 ** Function name:           getRotation
 ** Description:             Return the rotation value (as used by setRotation())
 ***************************************************************************************/
 uint8_t TFT_ILI9341_ESP::getRotation(void)
 {
   return rotation;
 }
 
 /***************************************************************************************
 ** Function name:           width
 ** Description:             Return the pixel width of display (per current rotation)
 ***************************************************************************************/
 // Return the size of the display (per current rotation)
 int16_t TFT_ILI9341_ESP::width(void)
 {
   return _width;
 }
 
 /***************************************************************************************
 ** Function name:           height
 ** Description:             Return the pixel height of display (per current rotation)
 ***************************************************************************************/
 int16_t TFT_ILI9341_ESP::height(void)
 {
   return _height;
 }
 
 /***************************************************************************************
 ** Function name:           textWidth
 ** Description:             Return the width in pixels of a string in a given font
 ***************************************************************************************/
 int16_t TFT_ILI9341_ESP::textWidth(const char *string, int font)
 {
   unsigned int str_width  = 0;
   char uniCode;
   char *widthtable;
 
   if (font>0 && font<9)
   {
     widthtable = (char *)pgm_read_dword( &(fontdata[font].widthtbl ) ) - 32; //subtract the 32 outside the loop
 
     while (*string)
     {
       uniCode = *(string++);
 #ifdef LOAD_GLCD
       if (font == 1) str_width += 6;
       else
 #endif
       str_width += pgm_read_byte( widthtable + uniCode); // Normally we need to subract 32 from uniCode
     }
   }
   return str_width * textsize;
 }
 
 /***************************************************************************************
 ** Function name:           fontsLoaded
 ** Description:             return an encoded 16 bit value showing the fonts loaded
 ***************************************************************************************/
 // Returns a value showing which fonts are loaded (bit N set =  Font N loaded)
 
 uint16_t TFT_ILI9341_ESP::fontsLoaded(void)
 {
   return fontsloaded;
 }
 
 /***************************************************************************************
 ** Function name:           fontHeight
 ** Description:             return the height of a font
 ***************************************************************************************/
 int16_t TFT_ILI9341_ESP::fontHeight(int font)
 {
   return pgm_read_byte( &fontdata[font].height ) * textsize;
 }
 
 /***************************************************************************************
 ** Function name:           drawChar
 ** Description:             draw a single character in the Adafruit GLCD font
 ***************************************************************************************/
 void TFT_ILI9341_ESP::drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
 {
 #ifdef LOAD_GLCD
   if ((x >= (int16_t)_width)            || // Clip right
       (y >= (int16_t)_height)           || // Clip bottom
       ((x + 6 * size - 1) < 0) || // Clip left
       ((y + 8 * size - 1) < 0))   // Clip top
     return;
   boolean fillbg = (bg != color);
 
 spi_begin();
 
   if ((size==1) && fillbg)
   {
     byte column[6];
     byte mask = 0x1;
     setAddrWindow(x, y, x+5, y+8);
     for (int8_t i = 0; i < 5; i++ ) column[i] = pgm_read_byte(font + (c * 5) + i);
     column[5] = 0;
 
     for (int8_t j = 0; j < 8; j++) {
       for (int8_t k = 0; k < 5; k++ ) {
         if (column[k] & mask) {
           spiwrite(color >> 8);
           spiwrite(color);
         }
         else {
           spiwrite(bg >> 8);
           spiwrite(bg);
         }
       }
 
       mask <<= 1;
       spiwrite(bg >> 8);
       spiwrite(bg);
     }
   }
   else
   {
     for (int8_t i = 0; i < 6; i++ ) {
       uint8_t line;
       if (i == 5)
         line = 0x0;
       else
         line = pgm_read_byte(font + (c * 5) + i);
 
       if (size == 1) // default size
       {
         for (int8_t j = 0; j < 8; j++) {
           if (line & 0x1) drawPixel(x + i, y + j, color);
           line >>= 1;
         }
       }
       else {  // big size
         for (int8_t j = 0; j < 8; j++) {
           if (line & 0x1) fillRect(x + (i * size), y + (j * size), size, size, color);
           else if (fillbg) fillRect(x + i * size, y + j * size, size, size, bg);
           line >>= 1;
         }
       }
     }
   }
 spi_end();
 
 #endif // LOAD_GLCD
 }
 
 /***************************************************************************************
 ** Function name:           setAddrWindow
 ** Description:             define an area to rexeive a stream of pixels
 ***************************************************************************************/
 // Chip select is high at the end of this function
 
 void TFT_ILI9341_ESP::setWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
 {
   spi_begin();
   setAddrWindow(x0, y0, x1, y1);
   spi_end();
 }
 
 /***************************************************************************************
 ** Function name:           setAddrWindow
 ** Description:             define an area to receive a stream of pixels
 ***************************************************************************************/
 // Chip select stays low, use setWindow() from sketches
 
 void TFT_ILI9341_ESP::setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
 {
   uint8_t buffC[] = { (uint8_t) (x0 >> 8), (uint8_t) x0, (uint8_t) (x1 >> 8), (uint8_t) x1 };
   uint8_t buffP[] = { (uint8_t) (y0 >> 8), (uint8_t) y0, (uint8_t) (y1 >> 8), (uint8_t) y1 };
 
   spi_begin();
 
   // Column addr set
   *dcport &= ~dcpinmask;
 
   spiwrite(ILI9341_CASET);
 
   *dcport |= dcpinmask;
 
   _SPI->writePattern(&buffC[0], 4, 1);
 
 /*
   spiwrite(x0 >> 8);
   addr_col = 0xFFFF;
   spiwrite(x0);
   if(x1!=win_xe) {
     spiwrite(x1 >> 8);
     win_xe=x1;
     spiwrite(x1);
   }
 */
 
   // Row addr set
   *dcport &= ~dcpinmask;
 
   spiwrite(ILI9341_PASET);
 
   *dcport |= dcpinmask;
 
 _SPI->writePattern(&buffP[0], 4, 1);
 
 /*
   spiwrite(y0 >> 8);
   addr_row = 0xFFFF;
   spiwrite(y0);
   if(y1!=win_ye) {
     spiwrite(y1 >> 8);
     win_ye=y1;
     spiwrite(y1);
   }
 */
 
   // write to RAM
   *dcport &= ~dcpinmask;
   spiwrite(ILI9341_RAMWR);
 
   //CS, HIGH;
   //*csport |= cspinmask;
   *dcport |= dcpinmask;
 
   spi_end();
 }
 
 /*
 void TFT_ILI9341_ESP::setAddrWindow(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
 {
   spi_begin();
 
   // Column addr set
   *dcport &= ~dcpinmask;
   *csport &= ~cspinmask;
   spiwrite(ILI9341_CASET);
  
 
   *dcport |= dcpinmask;
   spiwrite(x0 >> 8);
   spiwrite(x0);
   spiwrite(x1 >> 8);
   spiwrite(x1);
 
   // Row addr set
   *dcport &= ~dcpinmask;
   spiwrite(ILI9341_PASET);
 
   *dcport |= dcpinmask;
   spiwrite(y0 >> 8);
   spiwrite(y0);
   spiwrite(y1 >> 8);
   spiwrite(y1);
 
   // write to RAM
   *dcport &= ~dcpinmask;
   spiwrite(ILI9341_RAMWR);
 
   //CS, HIGH;
   //*csport |= cspinmask;
   *dcport |= dcpinmask;
 
   spi_end();
 }
 */
 /***************************************************************************************
 ** Function name:           drawPixel
 ** Description:             push a single pixel at an arbitrary position
 ***************************************************************************************/
 void TFT_ILI9341_ESP::drawPixel(uint16_t x, uint16_t y, uint16_t color)
 {
   // Faster range checking, possible because x and y are unsigned
   if ((x >= _width) || (y >= _height)) return;
   spi_begin();

 if (addr_col != x) {
   uint8_t buffC[] = { (uint8_t) (x >> 8), (uint8_t) x, (uint8_t) (x >> 8), (uint8_t) x };
 
   *dcport &= ~dcpinmask;
   spiwrite(ILI9341_CASET);
  
   addr_col = x;
   *dcport |= dcpinmask;
 
   _SPI->writePattern(&buffC[0], 4, 1);
 
   //spiwrite(x >> 8);
   //spiwrite(x);
 
   //spiwrite(x >> 8);
   //spiwrite(x);
 }
 
 if (addr_row != y) {
   uint8_t buffP[] = { (uint8_t) (y >> 8), (uint8_t) y, (uint8_t) (y >> 8), (uint8_t) y };
 
   *dcport &= ~dcpinmask;
   spiwrite(ILI9341_PASET);
  
   addr_row = y;
   *dcport |= dcpinmask;
 
   _SPI->writePattern(&buffP[0], 4, 1);
 
   //spiwrite(y >> 8);
   //spiwrite(y);
 
   //spiwrite(y >> 8);
   //spiwrite(y);
 }
 
   *dcport &= ~dcpinmask;
 
   spiwrite(ILI9341_RAMWR);
 
   *dcport |= dcpinmask;
 
   spiwrite(color >> 8);
   win_xe=x;
   spiwrite(color);
   win_ye=y;
  
   spi_end();
 }
 
 /*
 void TFT_ILI9341_ESP::fastPixel(uint16_t x, uint16_t y, uint16_t color)
 {
   // Faster range checking, possible because x and y are unsigned
   if ((x >= _width) || (y >= _height)) return;
   spi_begin();
 
   *csport &= ~cspinmask;
 
 if (addr_col != x) {
   *dcport &= ~dcpinmask;
   spiwrite(ILI9341_CASET);
  
   addr_col = x;
   *dcport |= dcpinmask;
 
   spiwrite(x >> 8);
   spiwrite(x);
 }
 
 if (addr_row != y) {
   *dcport &= ~dcpinmask;
   spiwrite(ILI9341_PASET);
  
   addr_row = y;
   *dcport |= dcpinmask;
 
   spiwrite(y >> 8);
   spiwrite(y);
 }
 
   *dcport &= ~dcpinmask;
 
   spiwrite(ILI9341_RAMWR);
 
   *dcport |= dcpinmask;
 
   spiwrite(color >> 8);
   spiwrite(color);
 
   *csport |= cspinmask;
 
   spi_end();
 }
 */
 
 /*
 void TFT_ILI9341_ESP::fastSetup(void)
 {
   spi_begin();
 
   *dcport &= ~dcpinmask;
   *csport &= ~cspinmask;
 
   spiwrite(ILI9341_CASET);
  
   *dcport |= dcpinmask;
   spiwrite(0);
   addr_col = 0;
   spiwrite(0);
   win_xe=_width-1;
   spiwrite(win_xe >> 8);
   spiwrite(win_xe);
 
   *dcport &= ~dcpinmask;
 
   spiwrite(ILI9341_PASET);
  
   *dcport |= dcpinmask;
   spiwrite(0);
   addr_row = 0;
   spiwrite(0);
   win_ye=_height-1;
   spiwrite(win_ye >> 8);
   spiwrite(win_ye);
 
   *csport |= cspinmask;
 
   spi_end();
 }
 */
 
 /*
 void TFT_ILI9341_ESP::drawPixel(uint16_t x, uint16_t y, uint16_t color)
 {
   // Faster range checking, possible because x and y are unsigned
   if ((x >= _width) || (y >= _height)) return;
   spi_begin();
 
   // Column addr set
   *dcport &= ~dcpinmask;
   *csport &= ~cspinmask;
   spiwrite(ILI9341_CASET);
  
 
   *dcport |= dcpinmask;
   spiwrite(x >> 8);
   spiwrite(x; x++); 
   spiwrite(x >> 8);
   spiwrite(x);
 
   // Row addr set
   *dcport &= ~dcpinmask;
   //*csport &= ~cspinmask;
   spiwrite(ILI9341_PASET);
 
   *dcport |= dcpinmask;
   spiwrite(y >> 8);
   spiwrite(y; y++); 
   spiwrite(y >> 8);
   spiwrite(y);
 
   // write to RAM
   *dcport &= ~dcpinmask;
   spiwrite(ILI9341_RAMWR);
 
   *dcport |= dcpinmask;
 
   spiwrite(color >> 8);
   spiwrite(color);
 
   //CS, HIGH;
   *csport |= cspinmask;
   //*dcport |= dcpinmask;
 
   spi_end();
 }
 */
 
 /***************************************************************************************
 ** Function name:           pushColor
 ** Description:             push a single pixel
 ***************************************************************************************/
 void TFT_ILI9341_ESP::pushColor(uint16_t color)
 {
   spi_begin();
  
   spiwrite(color>>8);
   spiwrite(color);
  
   spi_end();
 }
 
 /***************************************************************************************
 ** Function name:           pushColor
 ** Description:             push a single colour to "len" pixels
 ***************************************************************************************/
 void TFT_ILI9341_ESP::pushColor(uint16_t color, uint16_t len)
 {
   spi_begin();
  
   uint8_t colorBin[] = { (uint8_t) (color >> 8), (uint8_t) color };
   while(len>32) { _SPI->writePattern(&colorBin[0], 2, 32); len-=32;}
   _SPI->writePattern(&colorBin[0], 2, len);
  
   spi_end();
 }
 
 /***************************************************************************************
 ** Function name:           pushColors
 ** Description:             push an aray of pixels for BMP image drawing
 ***************************************************************************************/
 // Sends an array of 16-bit color values to the TFT; used
 // externally by BMP examples.  Assumes that setWindow() has
 // previously been called to define the bounds.  Max 255 pixels at
 // a time (BMP examples read in small chunks due to limited RAM).
 
 void TFT_ILI9341_ESP::pushColors(uint16_t *data, uint8_t len)
 {
   uint16_t color;
   spi_begin();
  
   while (len--) {
     color = *(data++);
     spiwrite(color >> 8);
     spiwrite(color);
   }
  
   spi_end();
 }
 
 /***************************************************************************************
 ** Function name:           pushColors
 ** Description:             push an aray of pixels for 16 bit raw image drawing
 ***************************************************************************************/
 // Assumed that setWindow() has previously been called
 
 void TFT_ILI9341_ESP::pushColors(uint8_t *data, uint16_t len)
 {
   spi_begin();
   len = len<<1;
  
   while (len--) spiwrite(*(data++));
  
   spi_end();
 }
 
 /***************************************************************************************
 ** Function name:           drawLine
 ** Description:             draw a line between 2 arbitrary points
 ***************************************************************************************/
 
 // Bresenham's algorithm - thx wikipedia - speed enhanced by Bodmer to use
 // an eficient FastH/V Line draw routine for line segments of 2 pixels or more
 
 void TFT_ILI9341_ESP::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
 {
   boolean steep = abs(y1 - y0) > abs(x1 - x0);
   if (steep) {
     swap(x0, y0);
     swap(x1, y1);
   }
 
   if (x0 > x1) {
     swap(x0, x1);
     swap(y0, y1);
   }
 
   int16_t dx = x1 - x0, dy = abs(y1 - y0);;
 
 
   int16_t err = dx >> 1, ystep = -1, xs = x0, dlen = 0;
   if (y0 < y1) ystep = 1;
 
   // Split into steep and not steep for FastH/V separation
   if (steep) {
     for (; x0 <= x1; x0++) {
       dlen++;
       err -= dy;
       if (err < 0) {
         err += dx;
         if (dlen == 1) drawPixel(y0, xs, color);
         else drawFastVLine(y0, xs, dlen, color);
         dlen = 0; y0 += ystep; xs = x0 + 1;
       }
     }
     if (dlen) drawFastVLine(y0, xs, dlen, color);
   }
   else
   {
     for (; x0 <= x1; x0++) {
       dlen++;
       err -= dy;
       if (err < 0) {
         err += dx;
         if (dlen == 1) drawPixel(xs, y0, color);
         else drawFastHLine(xs, y0, dlen, color);
         dlen = 0; y0 += ystep; xs = x0 + 1;
       }
     }
     if (dlen) drawFastHLine(xs, y0, dlen, color);
   }
 }
 
 /***************************************************************************************
 ** Function name:           drawFastVLine
 ** Description:             draw a vertical line
 ***************************************************************************************/
 void TFT_ILI9341_ESP::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
 {
   // Rudimentary clipping
   if ((x >= _width) || (y >= _height)) return;
   if ((y + h - 1) >= _height) h = _height - y;
 
   spi_begin();
 
   setAddrWindow(x, y, x, y + h - 1);
 
   uint8_t colorBin[] = { (uint8_t) (color >> 8), (uint8_t) color };
   //while(h>32) { _SPI->writePattern(&colorBin[0], 2, 32); h-=32;}
   _SPI->writePattern(&colorBin[0], 2, h);
  
   spi_end();
 }
 
 /***************************************************************************************
 ** Function name:           drawFastHLine
 ** Description:             draw a horizontal line
 ***************************************************************************************/
 void TFT_ILI9341_ESP::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
 {
   // Rudimentary clipping
   if ((x >= _width) || (y >= _height)) return;
   if ((x + w - 1) >= _width)  w = _width - x;
 
   spi_begin();
 
   setAddrWindow(x, y, x + w - 1, y);
 
   uint8_t colorBin[] = { (uint8_t) (color >> 8), (uint8_t) color };
   //while(w>32) { _SPI->writePattern(&colorBin[0], 2, 32); w-=32;}
   _SPI->writePattern(&colorBin[0], 2, w);
  
   spi_end();
 }
 
 /***************************************************************************************
 ** Function name:           fillRect
 ** Description:             draw a filled rectangle
 ***************************************************************************************/
 void TFT_ILI9341_ESP::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
 {
   // rudimentary clipping (drawChar w/big text requires this)
   if ((x > _width) || (y > _height) || (w==0) || (h==0)) return;
   if ((x + w - 1) > _width)  w = _width  - x;
   if ((y + h - 1) > _height) h = _height - y;
 
   spi_begin();
   setAddrWindow(x, y, x + w - 1, y + h - 1);
 
   uint8_t colorBin[] = { (uint8_t) (color >> 8), (uint8_t) color };
   uint32_t n = (uint32_t)w * (uint32_t)h;
   //while(n>32) { _SPI->writePattern(&colorBin[0], 2, 32); n-=32;}
   _SPI->writePattern(&colorBin[0], 2, n);
  
   spi_end();
 }
 
 /***************************************************************************************
 ** Function name:           color565
 ** Description:             convert three 8 bit RGB levels to a 16 bit colour value
 ***************************************************************************************/
 uint16_t TFT_ILI9341_ESP::color565(uint8_t r, uint8_t g, uint8_t b)
 {
   return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
 }
 
 /***************************************************************************************
 ** Function name:           setRotation
 ** Description:             rotate the screen orientation m = 0-3 or 4-7 for BMP drawing
 ***************************************************************************************/
 void TFT_ILI9341_ESP::setRotation(uint8_t m)
 {
   rotation = m % 8;
   spi_begin();
   writecommand(ILI9341_MADCTL);
   switch (rotation) {
     case 0:
       writedata(ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR);
       _width  = ILI9341_TFTWIDTH;
       _height = ILI9341_TFTHEIGHT;
       break;
     case 1:
       writedata(ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
       _width  = ILI9341_TFTHEIGHT;
       _height = ILI9341_TFTWIDTH;
       break;
     case 2:
       writedata(ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
       _width  = ILI9341_TFTWIDTH;
       _height = ILI9341_TFTHEIGHT;
       break;
     case 3:
       writedata(ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
       _width  = ILI9341_TFTHEIGHT;
       _height = ILI9341_TFTWIDTH;
       break;
   // These next rotations are for bottum up BMP drawing
     case 4:
       writedata(ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
       _width  = ILI9341_TFTWIDTH;
       _height = ILI9341_TFTHEIGHT;
       break;
     case 5:
       writedata(ILI9341_MADCTL_MV | ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR);
       _width  = ILI9341_TFTHEIGHT;
       _height = ILI9341_TFTWIDTH;
       break;
     case 6:
       writedata(ILI9341_MADCTL_BGR);
       _width  = ILI9341_TFTWIDTH;
       _height = ILI9341_TFTHEIGHT;
       break;
     case 7:
       writedata(ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
       _width  = ILI9341_TFTHEIGHT;
       _height = ILI9341_TFTWIDTH;
       break;
 
   }
   spi_end();
   //fastSetup(); // Just incase setRotation is called inside a fast pixel loop
   addr_row = 0xFFFF;
   addr_col = 0xFFFF;
   win_xe = 0xFFFF;
   win_ye = 0xFFFF;
 
 }
 
 /***************************************************************************************
 ** Function name:           invertDisplay
 ** Description:             invert the display colours i = 1 invert, i = 0 normal
 ***************************************************************************************/
 void TFT_ILI9341_ESP::invertDisplay(boolean i)
 {
   spi_begin();
   // Send the command twice as otherwise it does not always work!
   writecommand(i ? ILI9341_INVON : ILI9341_INVOFF);
   writecommand(i ? ILI9341_INVON : ILI9341_INVOFF);
   spi_end();
 }
 
 /***************************************************************************************
 ** Function name:           write
 ** Description:             draw characters piped through serial stream
 ***************************************************************************************/
 size_t TFT_ILI9341_ESP::write(uint8_t uniCode)
 {
   if (uniCode == '\r') return 1;
   unsigned int width = 0;
   unsigned int height = 0;
   //Serial.print((char) uniCode); // Debug line sends all printed TFT text to serial port
 
 #ifdef LOAD_FONT2
   if (textfont == 2)
   {
       // This is 20us faster than using the fontdata structure (0.443ms per character instead of 0.465ms)
       width = pgm_read_byte(widtbl_f16 + uniCode-32);
       height = chr_hgt_f16;
       // Font 2 is rendered in whole byte widths so we must allow for this
       width = (width + 6) / 8;  // Width in whole bytes for font 2, should be + 7 but must allow for font width change
       width = width * 8;        // Width converted back to pixles
   }
   #ifdef LOAD_RLE
   else
   #endif
 #endif
 
 #ifdef LOAD_RLE
   {
     if ((textfont>2) && (textfont<9))
     {
       // Uses the fontinfo struct array to avoid lots of 'if' or 'switch' statements
       // A tad slower than above but this is not significant and is more convenient for the RLE fonts
       // Yes, this code can be needlessly executed when textfont == 1...
       width = pgm_read_byte( pgm_read_dword( &(fontdata[textfont].widthtbl ) ) + uniCode-32 );
       height= pgm_read_byte( &fontdata[textfont].height );
     }
   }
 #endif
 
 #ifdef LOAD_GLCD
   if (textfont==1)
   {
       width =  6;
       height = 8;
   }
 #else
   if (textfont==1) return 0;
 #endif
 
   height = height * textsize;
 
   if (uniCode == '\n') {
     cursor_y += height;
     cursor_x  = 0;
   }
   else
   {
     if (textwrap && (cursor_x + width * textsize >= _width))
     {
       cursor_y += height;
       cursor_x = 0;
     }
     cursor_x += drawChar(uniCode, cursor_x, cursor_y, textfont);
   }
   return 1;
 }
 
 /***************************************************************************************
 ** Function name:           drawChar
 ** Description:             draw a unicode onto the screen
 ***************************************************************************************/
 int16_t TFT_ILI9341_ESP::drawChar(unsigned int uniCode, int x, int y, int font)
 {
 
   if (font==1)
   {
 #ifdef LOAD_GLCD
       drawChar(x, y, uniCode, textcolor, textbgcolor, textsize);
       return 6 * textsize;
 #else
       return 0;
 #endif
   }
 
   int width  = 0;
   int height = 0;
   uint32_t flash_address = 0; // 16 bit address OK for Arduino if font files <60K
   uniCode -= 32;
 
 #ifdef LOAD_FONT2
   if (font == 2)
   {
       // This is 20us faster than using the fontdata structure (0.413ms per character instead of 0.433ms)
       flash_address = pgm_read_dword(&chrtbl_f16[uniCode]);
       width = pgm_read_byte(widtbl_f16 + uniCode);
       height = chr_hgt_f16;
   }
   #ifdef LOAD_RLE
   else
   #endif
 #endif
 
 #ifdef LOAD_RLE
   {
     if ((font>2) && (font<9))
     {
       // This is slower than above but is more convenient for the RLE fonts
       flash_address = pgm_read_dword( pgm_read_dword( &(fontdata[font].chartbl ) ) + uniCode*sizeof(void *) );
       width = pgm_read_byte( pgm_read_dword( &(fontdata[font].widthtbl ) ) + uniCode );
       height= pgm_read_byte( &fontdata[font].height );
     }
   }
 #endif
 
   int w = width;
   int pX      = 0;
   int pY      = y;
   byte line = 0;
 
   byte tl = textcolor;
   byte th = textcolor >> 8;
   byte bl = textbgcolor;
   byte bh = textbgcolor >> 8;
 
 #ifdef LOAD_FONT2 // chop out 962 bytes of code if we do not need it
   if (font == 2) {
     w = w + 6; // Should be + 7 but we need to compensate for width increment
     w = w / 8;
     if (x + width * textsize >= (int16_t)_width) return width * textsize ;
 
     if (textcolor == textbgcolor || textsize != 1) {
 
       for (int i = 0; i < height; i++)
       {
         if (textcolor != textbgcolor) fillRect(x, pY, width * textsize, textsize, textbgcolor);
 
         for (int k = 0; k < w; k++)
         {
           line = pgm_read_byte(flash_address + w * i + k);
           if (line) {
             if (textsize == 1) {
               pX = x + k * 8;
               if (line & 0x80) drawPixel(pX, pY, textcolor);
               if (line & 0x40) drawPixel(pX + 1, pY, textcolor);
               if (line & 0x20) drawPixel(pX + 2, pY, textcolor);
               if (line & 0x10) drawPixel(pX + 3, pY, textcolor);
               if (line & 0x08) drawPixel(pX + 4, pY, textcolor);
               if (line & 0x04) drawPixel(pX + 5, pY, textcolor);
               if (line & 0x02) drawPixel(pX + 6, pY, textcolor);
               if (line & 0x01) drawPixel(pX + 7, pY, textcolor);
             }
             else {
               pX = x + k * 8 * textsize;
               if (line & 0x80) fillRect(pX, pY, textsize, textsize, textcolor);
               if (line & 0x40) fillRect(pX + textsize, pY, textsize, textsize, textcolor);
               if (line & 0x20) fillRect(pX + 2 * textsize, pY, textsize, textsize, textcolor);
               if (line & 0x10) fillRect(pX + 3 * textsize, pY, textsize, textsize, textcolor);
               if (line & 0x08) fillRect(pX + 4 * textsize, pY, textsize, textsize, textcolor);
               if (line & 0x04) fillRect(pX + 5 * textsize, pY, textsize, textsize, textcolor);
               if (line & 0x02) fillRect(pX + 6 * textsize, pY, textsize, textsize, textcolor);
               if (line & 0x01) fillRect(pX + 7 * textsize, pY, textsize, textsize, textcolor);
             }
           }
         }
         pY += textsize;
       }
     }
     else
       // Faster drawing of characters and background using block write
     {
       spi_begin();
       setAddrWindow(x, y, (x + w * 8) - 1, y + height - 1);
 
       byte mask;
       for (int i = 0; i < height; i++)
       {
         for (int k = 0; k < w; k++)
         {
           line = pgm_read_byte(flash_address + w * i + k);
           pX = x + k * 8;
           mask = 0x80;
           while (mask) {
             if (line & mask) {
               spiwrite(th);
               spiwrite(tl);
             }
             else {
               spiwrite(bh);
               spiwrite(bl);
             }
             mask = mask >> 1;
           }
         }
         pY += textsize;
       }
 
        spi_end();
     }
   }
 
   #ifdef LOAD_RLE
   else
   #endif
 #endif  //FONT2
 
 #ifdef LOAD_RLE  //674 bytes of code
   // Font is not 2 and hence is RLE encoded
   {
     spi_begin();
 
     w *= height; // Now w is total number of pixels in the character
     if ((textsize != 1) || (textcolor == textbgcolor)) {
       if (textcolor != textbgcolor) fillRect(x, pY, width * textsize, textsize * height, textbgcolor);
       int px = 0, py = pY; // To hold character block start and end column and row values
       int pc = 0; // Pixel count
       byte np = textsize * textsize; // Number of pixels in a drawn pixel
 
       byte tnp = 0; // Temporary copy of np for while loop
       byte ts = textsize - 1; // Temporary copy of textsize
       // 16 bit pixel count so maximum font size is equivalent to 180x180 pixels in area
       // w is total number of pixels to plot to fill character block
       while (pc < w)
       {
         line = pgm_read_byte(flash_address);
         flash_address++; // 20 bytes smaller by incrementing here
         if (line & 0x80) {
           line &= 0x7F;
           line++;
           if (ts) {
             px = x + textsize * (pc % width); // Keep these px and py calculations outside the loop as they are slow
             py = y + textsize * (pc / width);
           }
           else {
             px = x + pc % width; // Keep these px and py calculations outside the loop as they are slow
             py = y + pc / width;
           }
           while (line--) { // In this case the while(line--) is faster
             pc++; // This is faster than putting pc+=line before while()?
             setAddrWindow(px, py, px + ts, py + ts);
 
             if (ts) {
               tnp = np;
               while (tnp--) {
                 spiwrite(th);
                 spiwrite(tl);
               }
             }
             else {
               spiwrite(th);
               spiwrite(tl);
             }
             px += textsize;
 
             if (px >= (x + width * textsize))
             {
               px = x;
               py += textsize;
             }
           }
         }
         else {
           line++;
           pc += line;
         }
       }
 
       spi_end();
     }
     else // Text colour != background && textsize = 1
          // so use faster drawing of characters and background using block write
     {
       spi_begin();
       setAddrWindow(x, y, x + width - 1, y + height - 1);
 
       uint8_t textcolorBin[] = { (uint8_t) (textcolor >> 8), (uint8_t) textcolor };
       uint8_t textbgcolorBin[] = { (uint8_t) (textbgcolor >> 8), (uint8_t) textbgcolor };
 
       // Maximum font size is equivalent to 180x180 pixels in area
       while (w > 0)
       {
         line = pgm_read_byte(flash_address++); // 8 bytes smaller when incrementing here
         if (line & 0x80) {
           line &= 0x7F;
           line++; w -= line;
           while(line>32) { _SPI->writePattern(&textcolorBin[0], 2, 32); line-=32;}
           _SPI->writePattern(&textcolorBin[0], 2, line);
         }
         else {
           line++; w -= line;
           while(line>32) { _SPI->writePattern(&textbgcolorBin[0], 2, 32); line-=32;}
           _SPI->writePattern(&textbgcolorBin[0], 2, line);
         }
       }
       spi_end();
     }
   }
   // End of RLE font rendering
 #endif
   return width * textsize;    // x +
 }
 
 /***************************************************************************************
 ** Function name:           drawString
 ** Description :            draw string with padding if it is defined
 ***************************************************************************************/
 int16_t TFT_ILI9341_ESP::drawString(const char *string, int poX, int poY, int font)
 {
   int16_t sumX = 0;
   uint8_t padding = 1;
   unsigned int cheight = 0;
 
   if (textdatum || padX)
   {
     // Find the pixel width of the string in the font
     unsigned int cwidth  = textWidth(string, font);
 
     // Get the pixel height of the font
     cheight = pgm_read_byte( &fontdata[font].height ) * textsize;
 
     switch(textdatum) {
       case TC_DATUM:
         poX -= cwidth/2;
         padding = 2;
         break;
       case TR_DATUM:
         poX -= cwidth;
         padding = 3;
         break;
       case ML_DATUM:
         poY -= cheight/2;
         padding = 1;
         break;
       case MC_DATUM:
         poX -= cwidth/2;
         poY -= cheight/2;
         padding = 2;
         break;
       case MR_DATUM:
         poX -= cwidth;
         poY -= cheight/2;
         padding = 3;
         break;
       case BL_DATUM:
         poY -= cheight;
         padding = 1;
         break;
       case BC_DATUM:
         poX -= cwidth/2;
         poY -= cheight;
         padding = 2;
         break;
       case BR_DATUM:
         poX -= cwidth;
         poY -= cheight;
         padding = 3;
         break;
     }
     // Check coordinates are OK, adjust if not
     if (poX < 0) poX = 0;
     if (poX+cwidth>_width)   poX = _width - cwidth;
     if (poY < 0) poY = 0;
     if (poY+cheight>_height) poY = _height - cheight;
   }
 
   while (*string) sumX += drawChar(*(string++), poX+sumX, poY, font);
 
 //#define PADDING_DEBUG
 
 #ifndef PADDING_DEBUG
   if((padX>sumX) && (textcolor!=textbgcolor))
   {
     int padXc = poX+sumX; // Maximum left side padding
     switch(padding) {
       case 1:
         fillRect(padXc,poY,padX-sumX,cheight, textbgcolor);
         break;
       case 2:
         fillRect(padXc,poY,(padX-sumX)>>1,cheight, textbgcolor);
         padXc = (padX-sumX)>>1;
         if (padXc>poX) padXc = poX;
         fillRect(poX - padXc,poY,(padX-sumX)>>1,cheight, textbgcolor);
         break;
       case 3:
         if (padXc>padX) padXc = padX;
         fillRect(poX + sumX - padXc,poY,padXc-sumX,cheight, textbgcolor);
         break;
     }
   }
 #else
 
   // This is debug code to show text (green box) and blanked (white box) areas
   // to show that the padding areas are being correctly sized and positioned
   if((padX>sumX) && (textcolor!=textbgcolor))
   {
     int padXc = poX+sumX; // Maximum left side padding
     drawRect(poX,poY,sumX,cheight, TFT_GREEN);
     switch(padding) {
       case 1:
         drawRect(padXc,poY,padX-sumX,cheight, TFT_WHITE);
         break;
       case 2:
         drawRect(padXc,poY,(padX-sumX)>>1, cheight, TFT_WHITE);
         padXc = (padX-sumX)>>1;
         if (padXc>poX) padXc = poX;
         drawRect(poX - padXc,poY,(padX-sumX)>>1,cheight, TFT_WHITE);
         break;
       case 3:
         if (padXc>padX) padXc = padX;
         drawRect(poX + sumX - padXc,poY,padXc-sumX,cheight, TFT_WHITE);
         break;
     }
   }
 #endif
 
 return sumX;
 }
 
 /***************************************************************************************
 ** Function name:           drawCentreString
 ** Descriptions:            draw string centred on dX
 ***************************************************************************************/
 int16_t TFT_ILI9341_ESP::drawCentreString(const char *string, int dX, int poY, int font)
 {
   byte tempdatum = textdatum;
   int sumX = 0;
   textdatum = TC_DATUM;
   sumX = drawString(string, dX, poY, font);
   textdatum = tempdatum;
   return sumX;
 }
 
 /***************************************************************************************
 ** Function name:           drawRightString
 ** Descriptions:            draw string right justified to dX
 ***************************************************************************************/
 int16_t TFT_ILI9341_ESP::drawRightString(const char *string, int dX, int poY, int font)
 {
   byte tempdatum = textdatum;
   int sumX = 0;
   textdatum = TR_DATUM;
   sumX = drawString(string, dX, poY, font);
   textdatum = tempdatum;
   return sumX;
 }
 
 /***************************************************************************************
 ** Function name:           drawNumber
 ** Description:             draw a long integer
 ***************************************************************************************/
 int16_t TFT_ILI9341_ESP::drawNumber(long long_num, int poX, int poY, int font)
 {
   char str[12];
   ltoa(long_num, str, 10);
   return drawString(str, poX, poY, font);
 }
 
 /***************************************************************************************
 ** Function name:           drawFloat
 ** Descriptions:            drawFloat, prints 7 non zero digits maximum
 ***************************************************************************************/
 // Adapted to assemble and print a string, this permits alignment relative to a datum
 // looks complicated but much more compact and actually faster than using print class
 int16_t TFT_ILI9341_ESP::drawFloat(float floatNumber, int dp, int poX, int poY, int font)
 {
   char str[14];               // Array to contain decimal string
   uint8_t ptr = 0;            // Initialise pointer for array
   int8_t  digits = 1;         // Count the digits to avoid array overflow
   float rounding = 0.5;       // Round up down delta
 
   if (dp > 7) dp = 7; // Limit the size of decimal portion
 
   // Adjust the rounding value
   for (uint8_t i = 0; i < dp; ++i) rounding /= 10.0;
 
   if (floatNumber < -rounding)    // add sign, avoid adding - sign to 0.0!
   {
     str[ptr++] = '-'; // Negative number
     str[ptr] = 0; // Put a null in the array as a precaution
     digits = 0;   // Set digits to 0 to compensate so pointer value can be used later
     floatNumber = -floatNumber; // Make positive
   }
 
   floatNumber += rounding; // Round up or down
 
   // For error put ... in string and return (all TFT_ILI9341_ESP library fonts contain . character)
   if (floatNumber >= 2147483647) {
     strcpy(str, "...");
     return drawString(str, poX, poY, font);
   }
   // No chance of overflow from here on
 
   // Get integer part
   unsigned long temp = (unsigned long)floatNumber;
 
   // Put integer part into array
   ltoa(temp, str + ptr, 10);
 
   // Find out where the null is to get the digit count loaded
   while ((uint8_t)str[ptr] != 0) ptr++; // Move the pointer along
   digits += ptr;                  // Count the digits
 
   str[ptr++] = '.'; // Add decimal point
   str[ptr] = '0';   // Add a dummy zero
   str[ptr + 1] = 0; // Add a null but don't increment pointer so it can be overwritten
 
   // Get the decimal portion
   floatNumber = floatNumber - temp;
 
   // Get decimal digits one by one and put in array
   // Limit digit count so we don't get a false sense of resolution
   uint8_t i = 0;
   while ((i < dp) && (digits < 9)) // while (i < dp) for no limit but array size must be increased
   {
     i++;
     floatNumber *= 10;       // for the next decimal
     temp = floatNumber;      // get the decimal
     ltoa(temp, str + ptr, 10);
     ptr++; digits++;         // Increment pointer and digits count
     floatNumber -= temp;     // Remove that digit
   }
   
   // Finally we can plot the string and return pixel length
   return drawString(str, poX, poY, font);
 }
 
 
 /***************************************************
 
   ORIGINAL LIBRARY HEADER
 
   This is our library for the Adafruit  ILI9341 Breakout and Shield
   ----> http://www.adafruit.com/products/1651
 
   Check out the links above for our tutorials and wiring diagrams
   These displays use SPI to communicate, 4 or 5 pins are required to
   interface (RST is optional)
   Adafruit invests time and resources providing this open source code,
   please support Adafruit and open-source hardware by purchasing
   products from Adafruit!
 
   Written by Limor Fried/Ladyada for Adafruit Industries.
   MIT license, all text above must be included in any redistribution
 
  ****************************************************/
 