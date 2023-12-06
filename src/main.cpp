#include <Arduino.h>
#include <CAN.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <math.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

#define DATA 4
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
int temperatureSave = 0;
unsigned long previousMillis = 0; // Almacena el tiempo del último evento
const long interval = 500;

OneWire ourWire(DATA);
DallasTemperature sensors(&ourWire);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void sendFloatAsBytes(int value);
void recibeSignalToEsp();

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;

  sensors.begin();
  Serial.println("Obteniendo la dirección del sensor...");
  if (!CAN.begin(500E3))
  {
    Serial.println("Starting CAN failed!");
    while (1)
      ;
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.display();
  delay(2000);

  display.clearDisplay();
  display.display();
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    sensors.requestTemperatures();
    float temperature = sensors.getTempCByIndex(0);
    int temperatureRounded = round(temperature);
    Serial.print(temperature);
    Serial.println();

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(3);
    display.setCursor(25, 5);
    display.print(temperature, 1);
    display.setTextSize(1);
    display.print("C");
    display.display();
    previousMillis = currentMillis;

    if (temperatureSave != temperatureRounded)
    {
      temperatureSave = temperatureRounded;
      Serial.print("Sending extended packet ... ");
      sendFloatAsBytes(temperatureRounded);
      Serial.println("done");
      Serial.print((char)temperatureRounded);
    }
  }
}

void sendFloatAsBytes(int value)
{
  const uint8_t valueBytes = static_cast<uint8_t>(value);
  CAN.beginPacket(0x12);
  CAN.write(valueBytes);
  CAN.endPacket();
  Serial.print("done");
  Serial.println();
  Serial.print(valueBytes);
}
