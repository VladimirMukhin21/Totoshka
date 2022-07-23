//#include <Wire.h>
#include <Adafruit_TCS34725softi2c.h>

/* Example code for the Adafruit TCS34725 breakout library */

/* Connect SCL    to digital 8
   Connect SDA    to digital 9
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */

// You can use any digital pin for emulate SDA / SCL
#define SDApinl 30
#define SCLpinl 31

#define SDApinr 20
#define SCLpinr 21

/* Initialise with specific int time and gain values */
Adafruit_TCS34725softi2c tcsl = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X, SDApinl, SCLpinl);
Adafruit_TCS34725softi2c tcsr = Adafruit_TCS34725softi2c(TCS34725_INTEGRATIONTIME_2_4MS, TCS34725_GAIN_1X, SDApinr, SCLpinr);

void setup(void) {
  Serial.begin(9600);
  tcsl.begin();
  tcsr.begin();
  
  /*if (tcs1.begin()) {
    Serial.println("Found sensor1");
    } else {
    Serial.println("No TCS34725 found 1 ... check your connections");
    while (1);
    }

    if (tcs2.begin()) {
    Serial.println("Found sensor2");
    } else {
    Serial.println("No TCS34725 found 2 ... check your connections");
    while (1);
    }*/
  //Serial.println("End of init");
}

void loop(void) {
  uint16_t rl, gl, bl, cl, colorTempl, luxl;
  uint16_t rr, gr, br, cr, colorTempr, luxr;

  tcsl.getRawData(&rl, &gl, &bl, &cl);
  tcsr.getRawData(&rr, &gr, &br, &cr);
  //Serial.println("1");
  //colorTemp = tcs.calculateColorTemperature(r, g, b);
  //Serial.println("2");
  //lux = tcs.calculateLux(r, g, b);

  //Serial.print("Color Temp: "); Serial.print(colorTempl, DEC); Serial.print(" K - ");
  //Serial.print("Lux: "); Serial.print(luxl, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(rl, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(gl, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(bl, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(cl, DEC); Serial.print("        ");

  //Serial.print("Color Temp: "); Serial.print(colorTempr, DEC); Serial.print(" K - ");
  //Serial.print("Lux: "); Serial.print(luxr, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(rr, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(gr, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(br, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(cr, DEC); Serial.print(" ");
  Serial.println(" ");
}
