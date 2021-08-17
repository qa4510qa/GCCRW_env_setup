#include "SimpleDHT.h"
#include "ESP8266WiFi.h"  // 提供Wi-Fi功能的程式庫
#include "PubSubClient.h"                             // MQTT Library
#include "DHT.h"                                      // DHT Sensor

int ledRed = 14;  //D5
int ledGreen = 13;  //D7
int ledBlue = 15; //D8
int pinDHT11 = 0;  //D3 接在GPIO0
int pinDHT11_high = 4;  //D2 接在GPIO4

SimpleDHT11 dht11;
byte humidity_sensor_pin = A0;
byte humidity_sensor_vcc = 5; //D1 接在GPIO5

// Setup sensor value
byte temperature = 0;
byte humidity = 0;
byte temperature_high = 0;
byte humidity_high = 0;
int soil_humidity = 0;
int soil_humidity_dry = 1024;  //1024
int soil_humidity_water = 300;  //300

// set LED state while D for default,R for ready, S for sensing,and U for uploading
void setLED(char LEDstate){
  switch(LEDstate){
    case 'R':
      digitalWrite(ledGreen,HIGH);
      digitalWrite(ledRed,LOW);
      digitalWrite(ledBlue,LOW);
      Serial.println("ready!");
      break;
    case 'S':
      digitalWrite(ledGreen,LOW);
      digitalWrite(ledRed,HIGH);
      digitalWrite(ledBlue,LOW);
      Serial.println("reading_data!");
      break;
    case 'U':
      digitalWrite(ledGreen,LOW);
      digitalWrite(ledRed,LOW);
      digitalWrite(ledBlue,HIGH);
      Serial.println("uploading_data!");
      break;
    default:
      digitalWrite(ledGreen,LOW);
      digitalWrite(ledRed,LOW);
      digitalWrite(ledBlue,LOW);
      Serial.println("default!");
      break;
    }
}

void setup() {
    // Setup LED
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(ledRed, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  setLED('default');
  // Init the humidity sensor board
  pinMode(humidity_sensor_vcc, OUTPUT);
  digitalWrite(humidity_sensor_vcc, LOW);
  // Setup Serial
  Serial.begin(115200);
  // 修正土壤濕度計，只有要修正時需要做
//  soil_humidity_calibration(soil_humidity_dry, soil_humidity_water);  
}

void soil_humidity_calibration(int &soil_humidity_dry, int &soil_humidity_water) {
  Serial.println("");
  Serial.println("read value in totally dry soil: ");
  delay(15000);
  for(int i=0; i<10; i++){
    Serial.print(".");
    digitalWrite(humidity_sensor_vcc, HIGH);
    delay(500);
    soil_humidity_dry = max(soil_humidity_dry, analogRead(humidity_sensor_pin));
    digitalWrite(humidity_sensor_vcc, LOW);
    delay(500);
  }
  Serial.println("");
  Serial.println("dry value= ");
  Serial.print(soil_humidity_dry);
  Serial.println("");
  Serial.println("read value in water: ");
  delay(15000);
  for(int i=0; i<10; i++){
    Serial.print(".");
    digitalWrite(humidity_sensor_vcc, HIGH);
    delay(500);
    soil_humidity_water = min(soil_humidity_water, analogRead(humidity_sensor_pin));  
    digitalWrite(humidity_sensor_vcc, LOW);
    delay(500);
  } 
  Serial.println("");
  Serial.println("water value= ");
  Serial.print(soil_humidity_water);
}

void read_soil_humidity(int &soil_humidity, int &soil_humidity_water, int &soil_humidity_dry) {
  setLED('S');
  digitalWrite(humidity_sensor_vcc, HIGH);
  delay(500);
  int value = analogRead(humidity_sensor_pin);
  digitalWrite(humidity_sensor_vcc, LOW);
  if (value <= soil_humidity_water) {
    soil_humidity = 100;
  }else if (value >= soil_humidity_dry) {
    soil_humidity = 0;
  }else{
    soil_humidity = map(value, soil_humidity_water, soil_humidity_dry, 100, 0);
  }
}

void read_DHT11(byte &temperature, byte &humidity, byte &temperature_high, byte &humidity_high) {
  setLED('S');
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(pinDHT11, &temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    setLED('default');
    Serial.print("Read DHT11 failed, err="); 
    Serial.println(err);
    delay(1000);
    return;
  }
  if ((err = dht11.read(pinDHT11_high, &temperature_high, &humidity_high, NULL)) != SimpleDHTErrSuccess) {
    setLED('default');
    Serial.print("Read DHT11_high failed, err="); 
    Serial.println(err);
    delay(1000);
    return;
  }
  delay(2000);
}

void loop() {
    read_DHT11(temperature, humidity, temperature_high, humidity_high);
    setLED('U');
    char temp[16];
    dtostrf(temperature, 2, 2, temp);   // To convert float into char
    delay(1000);  

    char humi[16];
    dtostrf(humidity, 3, 2, humi);      // To convert float into char
    delay(1000);  
    
    char temp_high[16];
    dtostrf(temperature_high, 2, 2, temp_high);   // To convert float into char
    delay(1000);  

    char humi_high[16];
    dtostrf(humidity_high, 3, 2, humi_high);      // To convert float into char
    delay(1000);  

    Serial.println("temperature:"+String(temp));
    Serial.println("humidity:"+String(humi));
    Serial.println("temperature_high:"+String(temp_high));
    Serial.println("humidity_high:"+String(humi_high));
    setLED('R');
}
