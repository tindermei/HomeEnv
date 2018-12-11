
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <dht11.h>
#include <pms.h>

Pms5003 pms;
int pm25;
  const auto n = Pms5003::Reserved;
    Pms5003::pmsData data[n];
    
#define DHT_PIN 10
#define BLUE 3
#define BLUE_PIN 6
#define RED 1
#define RED_PIN 5
#define GREEN 2
#define GREEN_PIN 4
#define GAS_PIN 7
#define GAS_APIN A5

dht11 DHT11;

LiquidCrystal_I2C lcd(0x3F,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

void setup()
{
 // init LED PIN 
  pinMode(BLUE_PIN,OUTPUT); 
  pinMode(RED_PIN,OUTPUT);
  pinMode(GREEN_PIN,OUTPUT);
  setLED(GREEN); 

  //init GAS PIN
  pinMode(GAS_PIN,INPUT);
  
  //init serial port
  Serial.begin(9600);
  
  // initialize the lcd 
  lcd.begin();                      
  lcd.backlight();
  lcd.cursor();
  lcd.clear();
  //show welcome words
  lcd.print("Welcome!");
  lcd.setCursor(0,1);
  lcd.print("Jason MEI");
  delay(1000);
    bool blinking = true;
  lcd.clear();
//  lcd.blink();

   //init PMS5003
     pms.begin ();
    pms.waitForData (Pms5003::wakeupTime);
    pms.write (Pms5003::cmdModeActive);
 
} 

void loop()
{
  lcd.home();
  showPM();
  showGAS();
  showTemp();
  
 

  Serial.print("PM25: ");
  Serial.println(pm25);
 
    delay(1000);
 

  
}

void showPM() {
   pms5003();
  lcd.setCursor(0,0);
  lcd.print("PM25:");
  lcd.print(pm25);
  lcd.print(" ");
  if (pm25<=50) {
    setLED(GREEN);
  }else if (pm25>50 and pm25<=100) {
    setLED(BLUE);
  }else if (pm25>100 and pm25<=150) {
    setLED(RED);
  }else if (pm25>150) {
    int i = 0;
    while( i++<20) {
      setLED(GREEN);
      delay(100);
      setLED(RED);
      delay(100);
      setLED(BLUE);
      delay(100);
    }
  }
  delay(1000);
}


void showGAS() {
 int gas = analogRead(GAS_APIN);
  if (digitalRead(GAS_PIN)) {
    Serial.print("GAS is OK. ");
  }
  Serial.print("GAS data is:");
  Serial.println(gas);
  lcd.setCursor(8,0);
  lcd.print(" GAS:");
  lcd.print(gas);
}

void showTemp() {
   DHT11.read(DHT_PIN);
  int humidity = DHT11.humidity;
  float temp = (float)DHT11.temperature;
 
  lcd.setCursor(0,1);
  lcd.print("T:");
  lcd.print(temp);
  lcd.print("C,");
  lcd.print("H:");
  lcd.print(humidity);
  lcd.print("% ");
  Serial.print("Temp:");
  Serial.print(temp);
  Serial.print("C, Humidity:");
  Serial.print(humidity);
  Serial.println("% ");
  delay(1000);

}

void changecolor(int temp, int humidity) {
    if (temp <10) {
    
   // setLED(RED);
    delay(500);
  } else if(humidity >10) {
    for (int i=0; i<humidity;i++) {
     // setLED(temp+i*10,humidity+i*5,i*15);
      delay(500);
    }
  }
}
float getTemp(char type) {
    float temp = (float)DHT11.temperature;//get temp
  if(type =='F')
  {
    return temp * 1.8 + 32;// convert to fahrenheit
  }else if(type =='K')
  {
    return temp + 274.15;// convert to Kelvin
  }else{
   return temp; 
  }
}

void setLED(int color){
   switch(color) {
    case 1:
    analogWrite(RED_PIN, 255);//一种颜色全亮，其他颜色熄灭
    analogWrite(GREEN_PIN, 0);
    analogWrite(BLUE_PIN, 0);
    break;
    case 2:
    analogWrite(RED_PIN, 0);//一种颜色全亮，其他颜色熄灭
    analogWrite(GREEN_PIN, 255);
    analogWrite(BLUE_PIN, 0);
    break;
    case 3:
    analogWrite(RED_PIN, 0);//一种颜色全亮，其他颜色熄灭
  analogWrite(GREEN_PIN, 0);
  analogWrite(BLUE_PIN, 255);
  }
}

void setLED(int red,int green,int blue) { // 根据输入数值驱动LED现实颜色
  analogWrite(RED_PIN, red);
  analogWrite(GREEN_PIN, green);
  analogWrite(BLUE_PIN, blue);
}

void pms5003() {
    static auto lastRead = millis ();

    const auto n = Pms5003::Reserved;
    Pms5003::pmsData data[n];

    Pms5003::PmsStatus status = pms.read (data, n);

    switch (status) {
        case Pms5003::OK:
        {
            Serial.println ("_________________");
            auto newRead = millis ();
            Serial.print ("Wait time ");
            Serial.println (newRead - lastRead);
            lastRead = newRead;

            // For loop starts from 3
            // Skip the first three data (PM1dot0CF1, PM2dot5CF1, PM10CF1)
            for (size_t i = Pms5003::PM1dot0; i < n; ++i) {
                Serial.print (data[i]);
                Serial.print ("\t");
                Serial.print (Pms5003::dataNames[i]);
                Serial.print (" [");
                Serial.print (Pms5003::metrics[i]);
                Serial.print ("]");
                Serial.println ();
            }
            pm25 = data[4];
            break;
        }
        case Pms5003::noData:
            break;
        default:
            Serial.println ("_________________");
            Serial.println (Pms5003::errorMsg[status]);
    };
}
