#include <Wire.h>
#include <ExpressLink.h>

#include <cstdint>
#include <Adafruit_SHT31.h>

Adafruit_SHT31 sht31;
ExpressLink el;

void setup()
{
  // USB Host
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // ExpressLink
  Serial1.end();
  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.setFIFOSize(4096);
  Serial1.begin(ExpressLink::BAUDRATE);
  if (!el.begin(Serial1, 10, 11, 12))
  {
    Serial.println("Failed to initialize ExpressLink.");
    while (true) { ;; }
  }

  // I2C temperature sensor
  Wire.begin();
  if (!sht31.begin()) {
    Serial.println("SHT31 init error.");
    while (true) { ; ; }
  }

  Serial.println("Setup complete.");

  // I2C SDA/SCL: 8, 9
  // Buttons: 13, 14, 15
  // Neopixel: 7
  // Ambient Light: 26, analog A0
  // Main UART RX/TX: 0, 1
  // Debug UART RX/TX: 4, 5

  ExpressLink el;
  if (!el.begin(Serial1, 10, 11, 12))
  {
    Serial.println("Failed to initialize ExpressLink.");
    while (true) { ;; }
  }

  el.config.setTopic(1, "wind_turbine/telemetry");

  Serial.println("Setup complete.");
}

void loop()
{
  el.reset();
  delay(4000);
  el.connect();
  Serial.println("loop complete.");

  delay(10000);

  float temperature;
  float humidity;
  sht31.readBoth(&temperature, &humidity);

  String json = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
  el.send(1, json);

  delay(5000);
}
