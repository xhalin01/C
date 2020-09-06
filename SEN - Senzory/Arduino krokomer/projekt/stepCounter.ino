#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <LSM303.h>

LSM303 compass;
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Set the LCD I2C address

int calibratedX;
int calibratedY;
int calibratedZ;

int stepCount=0;

//display and compass inicialization
void setup() {
  lcd.begin(20, 4);
  lcd.setCursor(0, 0);
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();
  Serial.println("Calibrating...");
  lcd.print("Calibrating...");
  calibrate();
  lcd.setCursor(0, 1);
  Serial.println("Done!");
  lcd.print("Done!");  
}


//main cycle for step counting
void loop() {
  
  compass.read();
  
  long deltaX,deltaZ,deltaY;
  deltaX=calibratedX-(compass.a.x>>4);
  deltaZ=calibratedZ-(compass.a.z>>4);
  deltaY=calibratedY-(compass.a.y>>4);
  
  long temp=(deltaX*deltaX)+(deltaY*deltaY)+(deltaZ*deltaZ);
  temp=sqrt(temp);

  //if vector treshold is bigger than 0,21 g count it as step
  if(temp>210)
  {
        stepCount++;
        Serial.print("Steps: ");
        Serial.println(stepCount);
        lcd.clear();
        lcd.print("steps: ");
        lcd.print(stepCount);
        calibrate();
        delay(500);
  }
}

//senzor calibration
void calibrate()
{
    long tempX=0,tempY=0,tempZ=0;
    for(int i=0;i<10;i++){
      compass.read();
      tempX+=compass.a.x>>4;
      tempY+=compass.a.y>>4;
      tempZ+=compass.a.z>>4;
    }
    calibratedX=tempX/10;
    calibratedY=tempY/10;
    calibratedZ=tempZ/10;
}
