#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h> // BMP180
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

// LCD
LiquidCrystal_I2C lcd(0x27,16,2);

// BMP180
Adafruit_BMP085 bmp;

// WiFi & ThingSpeak
const char* ssid = "Galaxy A13 AC61";
const char* password = "abhiwish";
unsigned long channelID = 3151791;
const char* writeAPIKey = "CZYXC74CPVBWYI8U";
WiFiClient client;

void setup() {
  Serial.begin(115200);

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Starting...");

  // WiFi
  WiFi.begin(ssid, password);
  lcd.setCursor(0,1);
  lcd.print("Connecting WiFi");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected!");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("WiFi Connected");

  ThingSpeak.begin(client);

  // BMP180
  if(!bmp.begin()){
    Serial.println("BMP180 not found!");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("BMP180 ERROR!");
    while(1); // stop
  }
}

// Read battery voltage
float getBatteryVoltage(){
  int raw = analogRead(A0);
  float voltage = raw * (4.2 / 1023.0) * 2.0; // voltage divider 100k/100k
  return voltage;
}

// Convert voltage to Li-ion %
int batteryPercent(float v){
  if(v>=4.2) return 100;
  else if(v>=4.1) return 95;
  else if(v>=4.0) return 90;
  else if(v>=3.95) return 85;
  else if(v>=3.9) return 80;
  else if(v>=3.85) return 75;
  else if(v>=3.8) return 70;
  else if(v>=3.75) return 60;
  else if(v>=3.7) return 50;
  else if(v>=3.65) return 40;
  else if(v>=3.6) return 30;
  else if(v>=3.5) return 20;
  else if(v>=3.4) return 10;
  else return 5;
}

void loop(){
  float temp = bmp.readTemperature();
  float pressure = bmp.readPressure()/100.0; // hPa
  float voltage = getBatteryVoltage();
  int percent = batteryPercent(voltage);

  // Display on LCD
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp:");
  lcd.print(temp,1);
  lcd.print("C ");

  lcd.setCursor(0,1);
  lcd.print("BAT:");
  lcd.print(percent);
  lcd.print("% ");
  lcd.print(voltage,2);
  lcd.print("V");

  // Upload to ThingSpeak
  ThingSpeak.setField(1,temp);
  ThingSpeak.setField(2,voltage);
  ThingSpeak.setField(3,percent);
  ThingSpeak.writeFields(channelID, writeAPIKey);

  Serial.print("Temp: "); Serial.print(temp); Serial.print("C ");
  Serial.print("BAT: "); Serial.print(percent); Serial.print("% ");
  Serial.print(voltage); Serial.println("V");

  delay(15000); // ThingSpeak minimum delay
}