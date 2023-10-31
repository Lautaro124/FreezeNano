#include <Arduino.h>
#include <CAN.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <math.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define DATA 6
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

OneWire ourWire(DATA);
DallasTemperature sensors(&ourWire);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void sendFloatAsBytes(int value);

void setup()
{
  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("CAN Sender");
  sensors.begin();
  // start the CAN bus at 500 kbps
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
      ; // Don't proceed, loop forever
  }
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  display.display();
}

void loop()
{
  sensors.requestTemperatures();
  int temperature = static_cast<int>(round(sensors.getTempCByIndex(0)));
  Serial.print(temperature);
  Serial.println();

  // send extended packet: id is 29 bits, packet can contain up to 8 bytes of data
  Serial.print("Sending extended packet ... ");

  sendFloatAsBytes(temperature);
  Serial.println("done");
  delay(1000);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(3);
  display.setCursor(25, 5);
  display.print(sensors.getTempCByIndex(0), 1);
  display.setTextSize(1);
  display.print("C");
  display.display();

  delay(1000);
}

void sendFloatAsBytes(int value)
{
  CAN.beginPacket(0x12, 1);
  CAN.write(value);
  CAN.endPacket();
}
