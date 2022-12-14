#include <SharpIR.h>
#include <Wire.h>
#include <Process.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);   

#define IRPin A0
#define model 1080

#define IRPin1 A1
#define model1 1080

/* Model :
  GP2Y0A02YK0F --> 20150
  GP2Y0A21YK0F --> 1080
  GP2Y0A710K0F --> 100500
  GP2YA41SK0F --> 430
*/

unsigned long timer1;
unsigned long timer2;
unsigned long TimeU;
unsigned long lineCheck;
float Time;
int flag1 = 0;
int flag2 = 0;

SharpIR mySensor1 = SharpIR(IRPin, model);
SharpIR mySensor = SharpIR(IRPin1, model1);

/*distance between sensors in ft*/
/*distance will be calibrated once mounted*/

float distance = .24351;
float speed;

void setup() {
  Bridge.begin();
  Serial.begin(9600);
  lcd.begin();                    // initialize the lcd 
  lcd.backlight();
  lcd.setCursor(1,0);
  lcd.print(" Turning On ");
  lcd.setCursor(0,1);
  lcd.print(" ... ");
  delay(500);
  lcd.clear();
}

void loop() {
  if(mySensor.distance() > 10 && mySensor.distance() < 50 && flag2 == 0) {
    timer2 = millis();
    flag2 = 1;
    lineCheck = timer2;
  }
  
  if(mySensor.distance() > 10 && mySensor1.distance() < 50 && flag1 == 0) {
    timer1 = millis(); 
    flag1 = 1;
    lineCheck = timer1;
  }
  
  lineCheck = millis();
  
  if (lineCheck - timer2 >= 30000) {
    speed = 0;
    lcd.clear(); 
    lcd.setCursor(0, 0); 
    lcd.print("Speed:");
    lcd.print(speed,1);
    lcd.print("ft/min  ");
    delay(2000);
    flag1 = 1;
    flag2 = 1;
    timer2 = millis();
    timer1 = timer2;
  }
  
  if (flag1 == 1 && flag2 == 1) {   
    lineCheck = millis();
    if(timer1 > timer2) {
      TimeU = timer1 - timer2;
      Time = TimeU;
      Time = Time / 60000;
      speed = distance/Time; //v=d/t 

    
    Process p;
    //http://192.168.1.253/jobpoint?key=newpoint&speed=
    char cmd[] = "";
    char speedstr[4];
    dtostrf(speed, -3, 2, speedstr);
    char fullcmd[150];
    strcpy(fullcmd, strcat(cmd, speedstr));
    char endc[] = "";
    Serial.println(strcat(fullcmd, endc));
    p.begin("curl");     
    p.addParameter(strcat(fullcmd, endc));
    p.run();
    while (p.available()>0) {
      char c = p.read();
      Serial.print(c);
    }
    Serial.flush();  
    
      
      lcd.clear(); 
      lcd.setCursor(0, 0); 
      lcd.print("Speed:");
      lcd.print(speed,1);
      Serial.println(speed);
      lcd.print("ft/min  ");
      delay(2000);
    } 
    
    speed = 0;
    flag1 = 0;
    flag2 = 0;
  }
}