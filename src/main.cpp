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
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);
  int temperatureRounded = round(temperature);
  Serial.print(temperature);
  Serial.println();

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(3);
  display.setCursor(25, 5);
  display.print(sensors.getTempCByIndex(0), 1);
  display.setTextSize(1);
  display.print("C");
  display.display();

  if (temperatureSave != temperatureRounded)
  {
    temperatureSave = temperatureRounded;
    Serial.print("Sending extended packet ... ");
    sendFloatAsBytes(temperatureRounded);
    Serial.println("done");
    Serial.print((char)temperatureRounded);
  }
  recibeSignalToEsp();
  delay(1000);
}

void sendFloatAsBytes(int value)
{
  CAN.beginPacket(0x12, 1);
  CAN.write(value);
  CAN.endPacket();
}

void recibeSignalToEsp()
{
  while (CAN.available())
  {
    int packetSize = CAN.parsePacket();

    if (packetSize || CAN.packetId() != -1)
    {
      if (CAN.packetId() == 0x1)
      {
        Serial.print("Received ");
        Serial.println();
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1.2);
        display.setCursor(25, 5);
        display.print((char)CAN.read());
        display.display();

        delay(1000);
      }
    }
  }
}