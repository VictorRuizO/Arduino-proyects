#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#define PIN 11
#define PIN_2 2
#define NUM_LEDS 84
#define NUM_LEDS_2 18
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(NUM_LEDS_2, PIN_2, NEO_GRB + NEO_KHZ800);
static int brillo = 30;
byte r = 0xff;
byte g = 0xff;
byte b = 0xff;
byte ret = 5;
byte nLedV = 21;
byte nVent = 4;
byte animation = 3;

unsigned long tiempo;


void setup() {
  strip.begin();
  strip2.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(4800);
  animation = EEPROM.read (0);
  ret=EEPROM.read (1);
  r = EEPROM.read (2);
  g = EEPROM.read (3);
  b = EEPROM.read (4);
  brillo = EEPROM.read (5);
  setBrillo();

}


void showStrip() {
  strip.show();
}
void showStrip2() {
  strip2.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
}

void setPixel2(int Pixel, byte red, byte green, byte blue) {
  strip2.setPixelColor(Pixel, strip2.Color(red, green, blue));
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
  showStrip();
}

void setAll2(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS_2; i++ ) {
    setPixel2(i, red, green, blue);
  }
  showStrip2();
}

String getValue(String data, char separator, int index){
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void setBrillo() {
  strip.setBrightness(brillo);
  strip2.setBrightness(brillo);
}

bool esperar(int ms) {
  bool r;
  tiempo = millis();
  while (millis() < tiempo + ms) {
    r = leerSerial();
    if (r) return true;
  }
  return false;
}





/*********************************************************************************/
/***SERIAL***********************************************************************/
bool leerSerial() {
  bool bol=false;
  String cmd;
  if (Serial.available() > 0) {
    String str = Serial.readStringUntil('\n');
    cmd=getValue(str, '.', 0);
    
    if(cmd.equals("a")){
      animation = getValue(str, '.', 1).toInt();
      EEPROM.write (0, animation);
      setAll(0,0,0);
      bol=true;
    }else if(cmd.equals("r")){
      ret = getValue(str, '.', 1).toInt();
      EEPROM.write (1, ret);
    }else if(cmd.equals("c")){
      r = getValue(str, '.', 1).toInt();
      g = getValue(str, '.', 2).toInt();
      b = getValue(str, '.', 3).toInt();
      EEPROM.write (2, r);
      EEPROM.write (3, g);
      EEPROM.write (4, b);
    }else if(cmd.equals("b")){
      brillo = getValue(str, '.', 1).toInt();
      EEPROM.write (5, brillo);
      setBrillo();
    }
    //Serial.println(str);
  }
  return bol;
}


/*********************************************************************************/
/***LOOP***********************************************************************/

void loop() {

  switch (animation) {
    case 1:
      rainbowCycle();
      break;
    case 2:
      FadeInOut();
      break;
    case 3:
      CylonBounce( 6);
      break;
    case 4:
      TwinkleRandom(60, true);
      break;
    case 5:
      Sparkle();
      break;
    case 6:
      SnowSparkle( random(100, 1000));
      break;
    case 7:
      RunningLights();
      break;
    case 8:
      colorWipe();
      break;
    case 9:
      colorWipeInv();
      break;
    case 10:
      meteorRain( 10, 64, true);
      break;
    case 11:
      circ();
      break;
    case 12:
      randomColorFill(true);
      break;
    case 13:
      round2();
      break;
    case 14:
      colorWipeUni();
      break;
    case 15:
      colorWipeUniInv();
      break;
    case 16:
      randomColorFillUni(true);
      break;
    case 17:
      solid();
      break;
  }
  leerSerial();

  
}
/*********************************************************************************/
/***SOLID***********************************************************************/
void solid(){
  setAll(r,g,b);
  setAll2(r,g,b);
}

/*********************************************************************************/
/***RANDOMCOLORFILLUNI***********************************************************************/

void randomColorFillUni(bool aleat) {
  setAll(0, 0, 0);
  byte re, gr,bl;

  for (uint16_t i = 0; i < nLedV; i++) { // iterate over every LED of the strip
    if(aleat){
      re = random(0, 255); // generate a random color
      gr = random(0, 255);
      bl = random(0, 255);
    }else{
      re = r; // generate a random color
      gr = g;
      bl = b;
    }

    for (uint16_t j = 0; j < nLedV - i; j++) { // iterate over every LED of the strip, that hasn't lit up yet
      for(uint16_t k=0;k<4;k++){
        setPixel(nLedV*k+j - 1, 0, 0, 0); // turn previous LED off
        setPixel(nLedV*k+j, re, gr, bl); // turn current LED on

        setPixel2(nLedV*k+j - 1, 0, 0, 0); // turn previous LED off
        setPixel2(nLedV*k+j, re, gr, bl); // turn current LED on
      }
      for(uint16_t k=0;k<2&&j<=16;k++){
        setPixel(16*k+88+j -1, 0, 0, 0); // turn previous LED off
        setPixel(16*k+88+j, re, gr, bl); // turn current LED on

        setPixel2(16*k+88+j -1, 0, 0, 0); // turn previous LED off
        setPixel2(16*k+88+j, re, gr, bl); // turn current LED on
      }
      
      showStrip();
      showStrip2();
      if (esperar(ret)) return;
    }
  }
  if (esperar(ret)) return;
}

/*********************************************************************************/
/***COLORWIPEUNIINV***********************************************************************/
void colorWipeUniInv(){
  for (int i = 0; i < nLedV; i++) {
    for(int j = 0; j < 4; j++){
      setPixel(j*nLedV+i, r, g, b);
      setPixel2(j*nLedV+i, r, g, b);
    }

    showStrip();
    showStrip2();
    if (esperar(ret * 10)) return;
  }
  for (int i = 0; i < nLedV; i++) {
    for(int j = 0; j < 4; j++){
      setPixel(j*nLedV+i, 0, 0, 0);
      setPixel2(j*nLedV+i, 0, 0, 0);
    }    

    showStrip();
    showStrip2();
    if (esperar(ret * 10)) return;
  }
}

/*********************************************************************************/
/***COLORWIPEUNI***********************************************************************/
void colorWipeUni(){
  for (int i = 0; i < nLedV; i++) {
    for(int j = 0; j < 4; j++){
      setPixel(j*nLedV+i, r, g, b);
      setPixel2(j*nLedV+i, r, g, b);
    }

    showStrip();
    showStrip2();
    if (esperar(ret * 10)) return;
  }
  for (int i = nLedV - 1; i >= 0; i--) {
    for(int j = 0; j < 4; j++){
      setPixel(j*nLedV+i, 0, 0, 0);
      setPixel2(j*nLedV+i, 0, 0, 0);
    }    

    showStrip();
    showStrip2();
    if (esperar(ret * 10)) return;
  }
}
/*********************************************************************************/
/***ROUND2***********************************************************************/

void round2() {
  int k;
  double x;

  for (int i = 0; i < nLedV; i++) {
    //setAll(0,0,0);
    //setPixel(i, red/10, green/10, blue/10);
    for (int j = 0; j < nLedV; j++) {
      k = ((j + i) >= nLedV) ? (i + j) - nLedV : (j + i);
      //x=(log(j+1)/1.36;
      x = (exp((j) / 10.0) - 2) < 0 ? 0 : (exp((j) / 10.0) - 2) / 6.16;
      //Serial.println(k);
      for (int w = 0; w < nVent; w++) {
        setPixel(w * nLedV + k, r * x, g * x, b * x);
        setPixel2(w * nLedV + k, r * x, g * x, b * x);
      }

    }
    //setPixel(i+EyeSize+1, red/10, green/10, blue/10);
    showStrip();
    showStrip2();
    if (esperar(ret * 15)) return;
  }

}

/*********************************************************************************/
/***RANDOMCOLORFILL***********************************************************************/

void randomColorFill( bool aleat) {
  setAll(0, 0, 0);
  setAll2(0, 0, 0);
  byte re, gr,bl;
  for (uint16_t i = 0; i < NUM_LEDS; i++) { // iterate over every LED of the strip
    if(aleat){
      re = random(0, 255); // generate a random color
      gr = random(0, 255);
      bl = random(0, 255);
    }else{
      re = r; // generate a random color
      gr = g;
      bl = b;
    }

    for (uint16_t j = 0; j < NUM_LEDS - i; j++) { // iterate over every LED of the strip, that hasn't lit up yet
      setPixel(j - 1, 0, 0, 0); // turn previous LED off
      setPixel(j, r, g, b); // turn current LED on
      showStrip();
      if (esperar(ret)) return;
    }
  }
}


/*********************************************************************************/
/***ROUND***********************************************************************/

void circ() {
  for (int i = 0; i <= nLedV; i++) {
    for (int j = 0; j < nVent; j++) {
      setPixel(nLedV * j + i, r, g, b);
      setPixel2(nLedV * j + i, r, g, b);
    }
    showStrip();
    showStrip2();
    if (esperar(ret * 5)) return;
    for (int j = 0; j < nVent; j++) {
      setPixel(nLedV * j + i, 0, 0, 0);
      setPixel2(nLedV * j + i, 0, 0, 0);
    }
    showStrip();
    showStrip2();
  }
}


/*********************************************************************************/
/***METEORRAIN***********************************************************************/

void meteorRain( byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay) {
  setAll(0, 0, 0);
  setAll2(0, 0, 0);
  for (int i = 0; i < NUM_LEDS + NUM_LEDS; i++) {

    for (int j = 0; j < NUM_LEDS; j++) {
      if ( (!meteorRandomDecay) || (random(10) > 5) ) {
        fadeToBlack(j, meteorTrailDecay );
      }
    }

    // draw meteor
    for (int j = 0; j < meteorSize; j++) {
      if ( ( i - j < NUM_LEDS) && (i - j >= 0) ) {
        setPixel(i - j, r, g, b);
      }
    }

    showStrip();
    if (esperar(ret)) return;
  }
}

void fadeToBlack(int ledNo, byte fadeValue) {

  uint32_t oldColor;
  uint8_t r, g, b;
  int value;

  oldColor = strip.getPixelColor(ledNo);
  r = (oldColor & 0x00ff0000UL) >> 16;
  g = (oldColor & 0x0000ff00UL) >> 8;
  b = (oldColor & 0x000000ffUL);

  r = (r <= 10) ? 0 : (int) r - (r * fadeValue / 256);
  g = (g <= 10) ? 0 : (int) g - (g * fadeValue / 256);
  b = (b <= 10) ? 0 : (int) b - (b * fadeValue / 256);

  strip.setPixelColor(ledNo, r, g, b);

}

/*********************************************************************************/
/***COLORWIPEINV***********************************************************************/

void colorWipeInv() {
  setAll2(0, 0, 0);
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    setPixel(i, r, g, b);
    showStrip();
    if (esperar(ret * 10)) return;
  }
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    setPixel(i, 0, 0, 0);
    showStrip();
    if (esperar(ret * 10)) return;
  }
}

/*********************************************************************************/
/***COLORWIPE***********************************************************************/

void colorWipe() {
  setAll2(0, 0, 0);
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    setPixel(i, r, g, b);
    showStrip();
    if (esperar(ret * 10)) return;
  }
  for (uint16_t i = NUM_LEDS + 1; i > 0; i--) {
    setPixel(i - 1, 0, 0, 0);
    showStrip();
    if (esperar(ret * 10)) return;
  }
}

/*********************************************************************************/
/***RAINBOW***********************************************************************/

void rainbowCycle() {
  byte *c;
  uint16_t i, j;

  for (j = 0; j < 256; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < NUM_LEDS; i++) {
      c = Wheel(((i * 256 / NUM_LEDS) + j) & 255);
      setPixel(i, *c, *(c + 1), *(c + 2));
      setPixel2(i, *c, *(c + 1), *(c + 2));
    }
    showStrip();
    showStrip2();
    if (esperar(ret)) return;
  }
}

byte * Wheel(byte WheelPos) {
  static byte c[3];

  if (WheelPos < 85) {
    c[0] = WheelPos * 3;
    c[1] = 255 - WheelPos * 3;
    c[2] = 0;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    c[0] = 255 - WheelPos * 3;
    c[1] = 0;
    c[2] = WheelPos * 3;
  } else {
    WheelPos -= 170;
    c[0] = 0;
    c[1] = WheelPos * 3;
    c[2] = 255 - WheelPos * 3;
  }

  return c;
}
/*********************************************************************************/
/***FADE***********************************************************************/
void FadeInOut() {
  setAll(0, 0, 0);
  setAll2(0, 0, 0);
  if (esperar(ret * 13)) return;

  for (double k = 0.1; k <= 1; k = k + 0.1) {
    setAll(k * r, k * g, k * b);
    setAll2(k * r, k * g, k * b);
    if (esperar(ret * 10)) return;
  }
  if (esperar(ret * 13)) return;
  for (double k = 1.1; k >= 0.1; k = k - 0.1) {
    setAll((k - 0.1)*r, (k - 0.1)*g, (k - 0.1)*b);
    setAll2((k - 0.1)*r, (k - 0.1)*g, (k - 0.1)*b);
    if (esperar(ret * 10)) return;
  }

}

/*********************************************************************************/
/***CYCLON***********************************************************************/

void CylonBounce( int EyeSize) {
  setAll2(0, 0, 0);
  for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++) {
    setAll(0, 0, 0);
    setPixel(i, r / 10, g / 10, b / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, r, g, b);
    }
    setPixel(i + EyeSize + 1, r / 10, g / 10, b / 10);
    showStrip();
    if (esperar(ret*10)) return;
  }

  if (esperar(ret*10)) return;

  for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--) {
    setAll(0, 0, 0);
    setPixel(i, r / 10, g / 10, b / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, r, g, b);
    }
    setPixel(i + EyeSize + 1, r / 10, g / 10, b / 10);
    showStrip();
    if (esperar(ret*10)) return;
  }

  esperar(ret * 10);
}
/*********************************************************************************/
/***TWINKLE***********************************************************************/

void TwinkleRandom(int Count, boolean OnlyOne) {
  setAll(0, 0, 0);
  setAll2(0, 0, 0);
  for (int i = 0; i < Count; i++) {
    setPixel(random(NUM_LEDS), random(0, 255), random(0, 255), random(0, 255));
    setPixel2(random(NUM_LEDS_2), random(0, 255), random(0, 255), random(0, 255));
    showStrip();
    showStrip2();
    if (esperar(ret)) return;
    if (OnlyOne) {
      setAll(0, 0, 0);
      setAll2(0, 0, 0);
    }
  }

  if (esperar(ret)) return;
}
/*********************************************************************************/
/***SPARKLE***********************************************************************/

void Sparkle() {
  int Pixel = random(NUM_LEDS);
  int Pixel2 = random(NUM_LEDS_2);
  setPixel(Pixel, r, g, b);
  setPixel2(Pixel2, r, g, b);
  showStrip();
  showStrip2();
  if (esperar(ret*10)) return;
  setPixel(Pixel, 0, 0, 0);
  setPixel2(Pixel2, 0, 0, 0);
}

/*********************************************************************************/
/***SNOWSPARKLE***********************************************************************/

void SnowSparkle( int SparkleDelay) {
  setAll(r, g, b);
  setAll2(r, g, b);
  int Pixel = random(NUM_LEDS);
  int Pixel2 = random(NUM_LEDS_2);
  setPixel(Pixel, 0xff, 0xff, 0xff);
  setPixel2(Pixel2, 0xff, 0xff, 0xff);
  showStrip();
  showStrip2();
  if (esperar(ret)) return;
  setPixel(Pixel, r, g, b);
  setPixel2(Pixel2, r, g, b);
  showStrip();
  showStrip2();
  if (esperar(ret)) return;
}

/*********************************************************************************/
/***RUNNINGLIGHTS***********************************************************************/

void RunningLights() {
  int Position = 0;
  setAll2(0, 0, 0);
  for (int j = 0; j < NUM_LEDS * 2; j++)
  {
    Position++; // = 0; //Position + Rate;
    for (int i = 0; i < NUM_LEDS; i++) {
      // sine wave, 3 offset waves make a rainbow!
      //float level = sin(i+Position) * 127 + 128;
      //setPixel(i,level,0,0);
      //float level = sin(i+Position) * 127 + 128;
      setPixel(i, ((sin(i + Position) * 127 + 128) / 255)*r,
               ((sin(i + Position) * 127 + 128) / 255)*g,
               ((sin(i + Position) * 127 + 128) / 255)*b);
    }

    showStrip();
    if (esperar(ret*10)) return;
  }
}
