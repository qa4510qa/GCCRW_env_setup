#include "SimpleDHT.h"
#include "ESP8266WiFi.h"  // 提供Wi-Fi功能的程式庫
#include "PubSubClient.h"                             // MQTT Library
#include "DHT.h"                                      // DHT Sensor

const char* ssid="";        // WiFi ssid
const char* password="";    // WiFi password
const char* mqttServer = "";       // MQTT Server IP Address
const char* mqttUserName = "";       // 使用者名稱，隨意設定。 
const char* mqttPwd = "";     // MQTT密碼 
const char* clientID = "";     // The client id identifies the NodeMCU device.
const char* topicT = "";  // Topic temperature
const char* topicH = "";     // Topic humidity
const char* topicT_high = "";  // Topic temperature_high
const char* topicH_high = "";     // Topic humidity_high
const char* topicSH = "";     // Topic soil_humidity

unsigned long prevMillis = 0;    // 暫存經過時間（毫秒） 
const long interval = 20000;    // 上傳資料的間隔時間，20秒。 
String msgStr = "";         // 暫存MQTT訊息字串 

int ledRed = 14;  //D5
int ledGreen = 13;  //D7
int ledBlue = 15; //D8
int pinDHT11 = 0;  //D3 接在GPIO0
int pinDHT11_high = 4;  //D2 接在GPIO4

SimpleDHT11 dht11;
byte humidity_sensor_pin = A0;
byte humidity_sensor_vcc = 5; //D1 接在GPIO5

// Setup sensor value
byte temperature;
byte humidity;
byte temperature_high;
byte humidity_high;
byte errCode = 255;
int soil_humidity;
int soil_humidity_dry = 1024;  //1024
int soil_humidity_water = 300;  //300

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived, topic:");
  Serial.println(topic);
  Serial.print("payload:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else if ((char)payload[0] == '0') {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
  
}

// set LED state while D for default,R for ready, S for sensing,and U for uploading
void setLED(char LEDstate){
  switch(LEDstate){
    case 'R':
      digitalWrite(ledGreen,HIGH);
      digitalWrite(ledRed,LOW);
      digitalWrite(ledBlue,LOW);
      Serial.println("status: ready");
      break;
    case 'S':
      digitalWrite(ledGreen,LOW);
      digitalWrite(ledRed,HIGH);
      digitalWrite(ledBlue,LOW);
      Serial.println("status: reading_data");
      break;
    case 'U':
      digitalWrite(ledGreen,LOW);
      digitalWrite(ledRed,LOW);
      digitalWrite(ledBlue,HIGH);
      Serial.println("status: uploading_data");
      break;
    default:
      digitalWrite(ledGreen,LOW);
      digitalWrite(ledRed,LOW);
      digitalWrite(ledBlue,LOW);
      Serial.println("status: default");
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
  setup_wifi();
  client.setServer(mqttServer, 1883);
  client.setCallback(callback);
  reconnect();
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
    temperature = errCode;
    humidity = errCode;
    delay(1000);
  }
  if ((err = dht11.read(pinDHT11_high, &temperature_high, &humidity_high, NULL)) != SimpleDHTErrSuccess) {
    setLED('default');
    Serial.print("Read DHT11_high failed, err="); 
    Serial.println(err);
    temperature_high = errCode;
    humidity_high = errCode;
    delay(1000);
  }
  delay(2000);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("MCU_01/test", "reconnect successfully!");
      // ... and resubscribe
//      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
  client.subscribe("test");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // 等待20秒
  if (millis() - prevMillis > interval) {
    prevMillis = millis();
    
    read_soil_humidity(soil_humidity, soil_humidity_water, soil_humidity_dry);
    read_DHT11(temperature, humidity, temperature_high, humidity_high);
    setLED('U');
    char temp[16];
    dtostrf(temperature, 2, 2, temp);   // To convert float into char
    client.publish(topicT, temp); 
    delay(1000);  
//    publish_data(topicT, "MCU_01 temperature", temp);

    char humi[16];
    dtostrf(humidity, 3, 2, humi);      // To convert float into char
    client.publish(topicH, humi); 
    delay(1000);  
//    publish_data(topicH, "MCU_01 humidity", humi);
  
    char s_humi[16];
    dtostrf(soil_humidity, 4, 2, s_humi);      // To convert float into char
    client.publish(topicSH, s_humi); 
    delay(1000);  
//    publish_data(topicSH, "MCU_01 soil_humidity",s_humi); 

    char temp_high[16];
    dtostrf(temperature_high, 2, 2, temp_high);   // To convert float into char
    client.publish(topicT_high, temp_high); 
    delay(1000);  
//    publish_data(topicT_high, "MCU_01 temperature_high", temp);

    char humi_high[16];
    dtostrf(humidity_high, 3, 2, humi_high);      // To convert float into char
    client.publish(topicH_high, humi_high); 
    delay(1000);  
//    publish_data(topicH_high, "MCU_01 humidity_high", humi);   
    Serial.println("temperature:"+String(temp));
    Serial.println("humidity:"+String(humi));
    Serial.println("temperature_high:"+String(temp_high));
    Serial.println("humidity_high:"+String(humi_high));
    Serial.println("soil humidity:"+String(s_humi));  
    Serial.println("");
    setLED('R');
  }
}
