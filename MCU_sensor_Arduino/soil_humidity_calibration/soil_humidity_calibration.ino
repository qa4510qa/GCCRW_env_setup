int ledRed = 14;  //D5
int ledGreen = 13;  //D7
int ledBlue = 15; //D8
byte humidity_sensor_pin = A0;
byte humidity_sensor_vcc = 4; //D2

void setup() {
  // put your setup code here, to run once:
  pinMode(ledRed, OUTPUT);
  pinMode(ledBlue, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(humidity_sensor_vcc, OUTPUT);
  digitalWrite(humidity_sensor_vcc, LOW);
  Serial.begin(115200);
}


int soil_humidity_calibration() {
  digitalWrite(humidity_sensor_vcc, HIGH);
  delay(500);
  int value = analogRead(humidity_sensor_pin);
  digitalWrite(humidity_sensor_vcc, LOW);
  return value;
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("read value in totally dry soil: ");
  delay(5000);
  int dry_value = soil_humidity_calibration();
  Serial.print(dry_value);
  delay(10000);
  Serial.println("read value in water: ");
  delay(5000);
  int water_value = soil_humidity_calibration();
  Serial.print(water_value);
}
