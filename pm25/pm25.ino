#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
// PM25 使用
#include <SoftwareSerial.h>
SoftwareSerial Serial1(2, 3); // RX, TX

// 設定 LCD I2C 位址
// Set the pins on the I2C chip used for LCD connections:
// I2C位址 addr, en, rw, rs, d4, d5, d6, d7, bl, blpol
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// LED
int green = 5;
int yellow = 6;
int red = 9;                                                                                                                                                                                                      

// PMS5003T
long pmValue=0;
long pmcf10=0;
long pmcf25=0;
long pmcf100=0;
long pmat10=0;
long pmat25=0;
long pmat100=0;
unsigned int temperature = 0;
unsigned int humandity = 0;

char buf[50];

// 處理PM5003T數值
void retrievepm25(){
  int count = 0;
  int type = 0;
  unsigned char c;
  unsigned char high;
  while (Serial1.available()) {
    c = Serial1.read();
    if((count==0 && c!=0x42) || (count==1 && c!=0x4d)){
      Serial.println("check failed");
      break;
    }
    if(count > 27){
      Serial.println("complete");
      break;
    }
    else if(count == 4 || count == 6 || count == 8 || count == 10 || count == 12 || count == 14 || count == 24 || count == 26) high = c;
    else if(count == 5){
      pmcf10 = 256*high + c;
      pmValue = pmcf10;
      Serial.print("CF=1, PM1.0=");
      Serial.print(pmValue);
      Serial.println(" ug/m3");
    }
    else if(count == 7){
      pmcf25 = 256*high + c;
      pmValue = pmcf25;
      Serial.print("CF=1, PM2.5=");
      Serial.print(pmValue);
      Serial.println(" ug/m3");
    }
    else if(count == 9){
      pmcf100 = 256*high + c;
      pmValue = pmcf100;
      Serial.print("CF=1, PM10=");
      Serial.print(pmValue);
      Serial.println(" ug/m3");
    }
    else if(count == 11){
      pmat10 = 256*high + c;
      pmValue = pmat10;
      type = 1;
      Serial.print("atmosphere, PM1.0=");
      Serial.print(pmValue);
      Serial.println(" ug/m3");
    }
    else if(count == 13){
      pmat25 = 256*high + c;
      pmValue = pmat25;
      type = 2;
      Serial.print("atmosphere, PM2.5=");
      Serial.print(pmValue);
      Serial.println(" ug/m3");
    }
    else if(count == 15){
      pmat100 = 256*high + c;
      pmValue = pmat100;
      type = 3;
      Serial.print("atmosphere, PM10=");
      Serial.print(pmValue);
      Serial.println(" ug/m3");
    }
    else if(count == 25){        
      temperature = (256*high + c)/10;
      Serial.print("Temp=");
      Serial.print(temperature);
      Serial.println(" (C)");
    }
    else if(count == 27){            
      humandity = (256*high + c)/10;
      Serial.print("Humidity=");
      Serial.print(humandity);
      Serial.println(" (%)");
    }   
    count++;
    
    if(type > 0){
      observe(type, pmValue);
    }
  }
  while(Serial1.available()) Serial1.read();
  Serial.println();
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  
  // put your setup code here, to run once:
  // 初始化 LCD，一行 16 的字元，共 2 行，預設開啟背光
  lcd.begin(16, 2);
 
  // 測試閃爍三次
  for(int i = 0; i < 3; i++) {
    lcd.backlight(); // 開啟背光
    delay(250);
    lcd.noBacklight(); // 關閉背光
    delay(250);
  }
  
  lcd.backlight();
   
  // 輸出初始化文字
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("MAKER2FUN DIY");
  delay(1000);
  lcd.setCursor(0, 1); // 設定游標位置在第二行行首
  lcd.print("OPENING");
  delay(3000);
  lcd.clear(); //顯示清除

  // LED
  pinMode(green, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(red, OUTPUT);
}

// 依照PM2.5顯示LED，可依自己判斷方式修改
void observe(int type, long pmValue) {
  switch (type) {
    case 1: // atmosphere pm1.0
      break;
    case 2: // atmosphere pm2.5
      if(pmValue <= 15.4){
        turnLed(1, 0, 0);
      }else if(pmValue > 15.4 && pmValue <= 35.4){
        turnLed(0, 1, 0);
      }else{
        turnLed(0, 0, 1);
      }
      // statements
      break;
     case 3: // atmosphere pm10.0
     // statements
      break;
    default:
      break;
  }
}

// LED控制
void turnLed(int g, int y, int r){
  // 綠燈
  if(g == 1){
    digitalWrite(green, HIGH);
  }else{
    digitalWrite(green, LOW);
  }

  // 黃燈
  if(y == 1){
    digitalWrite(yellow, HIGH);
  }else{
    digitalWrite(yellow, LOW);
  }

  // 紅燈
  if(r == 1){
    digitalWrite(red, HIGH);
  }else{
    digitalWrite(red, LOW);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

  retrievepm25();

  delay(3000);
  lcd.clear();
  
   // 顯示PM1.0
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("PM1.0: " + String(pmat10) + " ug");

   // 顯示PM2.5
  lcd.setCursor(0, 1); // 設定游標位置在第二行行首
  lcd.print("PM2.5: " + String(pmat25) + " ug");

  delay(3000);
  lcd.clear();
  
  // 顯示PM10.0
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("PM10.0 : " + String(pmat100) + " ug");

  delay(3000);
  lcd.clear();
  
  // 顯示溫度
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("Temp: " + String(temperature) + " C");
  
  // 顯示濕度
  lcd.setCursor(0, 1); // 設定游標位置在第二行行首
  lcd.print("Huma: " + String(humandity) + " %");

}
