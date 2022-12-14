#include<LiquidCrystal.h>
#include <Process.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

unsigned long timer1;
unsigned long timer2;
unsigned long TimeU;
unsigned long lineCheck;

float Time;

int flag1 = 0;
int flag2 = 0;

//distance between sensors in ft 
//.218 for months, switched on 7/29/21
float distance = .23012171;

float speed;

int ir_s1 = A0;
int ir_s2 = A1;

int buzzer = 13;

void setup() {
  Bridge.begin();
  Serial.begin(9600);
  pinMode(ir_s1, INPUT);
  pinMode(ir_s2, INPUT);
  pinMode(buzzer, OUTPUT);
  
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Turning On ");
  lcd.setCursor(0,1);
  lcd.print(" ... ");
  delay(500);
  lcd.clear();
}

void loop() {
  if(digitalRead (ir_s2) == LOW && flag2 == 0) {
    timer2 = millis();
    flag2 = 1;
    Serial.println("sensor2");
    lineCheck = timer2;
  }
  
  if(digitalRead (ir_s1) == LOW && flag1 == 0) {
    timer1 = millis(); 
    flag1 = 1;
    Serial.println("sensor1");
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
    Serial.println("both true");
    
    lineCheck = millis();
    
    if(timer1 > timer2) {
      TimeU = timer1 - timer2;
      Time = TimeU;
      Time = Time / 60000;
      speed = distance/Time; //v=d/t
    

    Process p;
    //set address http://192.168.1.253/jobpoint?key=newpoint&speed=
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
    lcd.print("ft/min  ");

    
    delay(2000);
    } 
  
    
    lcd.setCursor(0, 1); 
    speed = 0;
    flag1 = 0;
    flag2 = 0;
    lcd.setCursor(0, 1); 
  }
}