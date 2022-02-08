// Code for ESP32

#define RXp2 16
#define TXp2 17
String msg="";

#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
String machineId = "esp32dev-test";

const char* ssid = "<WIFI_SSID>";
const char* password = "<WIFI_PASS>";
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;
String url = "https://ses-webserver.herokuapp.com/api/dumps/create";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXp2, TXp2);

  lcd.begin();
  lcd.backlight();
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi"); 
  lcd.print("Connecting WiFi");
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected at " + WiFi.localIP().toString());
  lcd.setCursor(0,1);
  lcd.print("Connected");
}
void loop() {
  msg = Serial2.readString();
  lcd.clear();
  if(msg!=""){
    lcd.setCursor(0,0);
    lcd.print("Sending request");
    lcd.setCursor(0,1);
    Serial.println("Message Received: ");
    Serial.println(msg);
    if(WiFi.status()==WL_CONNECTED){
      // WiFiClient client;
      HTTPClient http;
      // url = urlencode(url);
      Serial.println(url);
      http.begin(url);
      http.addHeader("Content-Type", "application/json");
      // http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String payload = "{\"machineId\":\""+ machineId +"\",\"content\":\""+msg+"\"}";
      // payload = urlencode(payload);
      Serial.println(payload);
      int httpResponseCode = http.POST(payload);
      String response = http.getString();
      Serial.print("Response code: ");
      Serial.println(httpResponseCode);
      Serial.print("Response: ");
      Serial.println(response);
      if(httpResponseCode >= 200 && httpResponseCode < 300){
        lcd.print("Success");
      }else{
        lcd.print("Failed");
      }
      http.end();
    }
    
  }
}

