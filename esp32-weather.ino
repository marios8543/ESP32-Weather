#include "DHT.h"
#include "thingProperties.h"

#define DHTPIN 2
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define RAINPIN 26
#define SOILPIN 25
#define WATERPIN 25

unsigned long previousMillis = 0;
const long interval = 10000;

long watering_time = 0;
int watering_duration = 0;
bool should_water_if_raining = 0;
int soil_humidity_watering_threshold = 0;

/*void onWateringSettingsChange() {
  JsonDocument doc;
  deserializeJson(doc, watering_settings);

  watering_time = doc["watering_time"];
  watering_duration = doc["watering_duration"];
  should_water_if_raining = doc["should_water_if_raining"];
  soil_humidity_watering_threshold = doc["soil_humidity_watering_threshold"];
  water_now = doc["water_now"];
}*/

void onWaterNowChange()  {
  digitalWrite(WATERPIN, water_now);
}

void setup() {
  Serial.begin(9600);
  delay(1500);

  dht.begin();
  pinMode(RAINPIN, INPUT_PULLUP);
  pinMode(WATERPIN, OUTPUT);

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);
  setDebugMessageLevel(4);
  ArduinoCloud.printDebugInfo();
}

void loop() {
  ArduinoCloud.update();

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    soil_humidity = (100 - (analogRead(A0) / 4095.00) * 100);
    is_raining = !digitalRead(RAINPIN);
    humidity = dht.readHumidity();
    temperature = dht.readTemperature();

    if (isnan(humidity) || isnan(temperature)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }

    Serial.print(F("Humidity: "));
    Serial.print(humidity);
    Serial.print(F("%  Temperature: "));
    Serial.print(temperature);
    Serial.print(F("°C Soil humidity: "));
    Serial.print(soil_humidity);
    Serial.print(is_raining ? F(" - It's raining!") : F(""));
    Serial.print(water_now ? F(" - Watering...") : F(""));
    Serial.println();
  }
}
