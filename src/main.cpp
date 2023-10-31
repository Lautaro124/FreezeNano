// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
#include <Arduino.h>
#include <CAN.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include <math.h>

#define DATA 6

OneWire ourWire(DATA);
DallasTemperature sensors(&ourWire);

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
}

void loop()
{
  sensors.requestTemperatures();
  int temperature = static_cast<int>(round(sensors.getTempCByIndex(0)));
  Serial.print(temperature);
  Serial.println();
  // send packet: id is 11 bits, packet can contain up to 8 bytes of data

  // send extended packet: id is 29 bits, packet can contain up to 8 bytes of data
  Serial.print("Sending extended packet ... ");

  sendFloatAsBytes(temperature);
  Serial.println("done");

  delay(1000);
}

void sendFloatAsBytes(int value)
{
  CAN.beginPacket(0x12, 1);
  CAN.write(value);
  CAN.endPacket();
}
