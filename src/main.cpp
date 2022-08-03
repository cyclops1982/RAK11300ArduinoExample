#include <Arduino.h>
#include <SPI.h>
#include <stdio.h>
#include <Wire.h>
#include <vl53l0x_class.h>
#include "batteryhelper.h"
#include "lorahelper.h"
#include "ledhelper.h"

#ifdef RAK11310
#include "mbed.h"
#include "rtos.h"
#endif

#define MAX_SAVE

VL53L0X sensor_vl53l0x(&Wire, WB_IO2);

void setup()
{
  delay(3000); // For whatever reason, some pins/things are not available at startup right away. So we wait 3 seconds for stuff to warm up or something

  LedHelper::init();

  // Initialize serial for output.
#ifndef MAX_SAVE
  time_t timeout = millis();
  Serial.begin(115200);
  // check if serial has become available and if not, just wait for it.
  while (!Serial)
  {
    if ((millis() - timeout) < 5000)
    {
      delay(100);
    }
    else
    {
      break;
    }
  }

  delay(3000); // just wait 3 sec so we can plugin the connection or setup the serial session.
#endif

  pinMode(WB_IO2, OUTPUT);
  digitalWrite(WB_IO2, HIGH);

  // Initialize I2C bus.
  Wire.begin();

  // Configure VL53L0X component.
  sensor_vl53l0x.begin();

  // Switch off VL53L0X component.
  sensor_vl53l0x.VL53L0X_Off();

  // Initialize VL53L0X component.
  if (sensor_vl53l0x.InitSensor(0x52) != VL53L0X_ERROR_NONE)
  {
#ifndef MAX_SAVE
    Serial.println("Init sensor_vl53l0x failed...");
#endif
    LedHelper::BlinkHalt();
  }

  LoraHelper::InitAndJoin();
}

uint16_t msgcount = 0;
void loop()
{
  uint32_t distance;
  int status;

  status = sensor_vl53l0x.GetDistance(&distance);

  if (status == VL53L0X_ERROR_NONE)
  {
    if (distance < 50)
    {
      digitalWrite(LED_GREEN, HIGH);
    }
    else
    {
      digitalWrite(LED_GREEN, LOW);
    }
  }
  else
  {
    distance = 0;
  }

  uint16_t vbat_mv = BatteryHelper::readVBAT();
  uint8_t vbat_per = BatteryHelper::mvToPercent(vbat_mv);

  memset(m_lora_app_data.buffer, 0, LORAWAN_APP_DATA_BUFF_SIZE);
  int size = 0;
  m_lora_app_data.port = 2;
  m_lora_app_data.buffer[size++] = 0x02; // device
  m_lora_app_data.buffer[size++] = 0x02; // msg version

  // bat voltage
  m_lora_app_data.buffer[size++] = vbat_mv >> 8;
  m_lora_app_data.buffer[size++] = vbat_mv;
  // distance
  uint16_t dist2 = distance;
  m_lora_app_data.buffer[size++] = dist2 >> 8;
  m_lora_app_data.buffer[size++] = dist2;

  m_lora_app_data.buffer[size++] = msgcount >> 8;
  m_lora_app_data.buffer[size++] = msgcount;
  m_lora_app_data.buffsize = size;

  lmh_error_status error = lmh_send(&m_lora_app_data, LMH_CONFIRMED_MSG);
#ifndef MAX_SAVE
  if (error == LMH_SUCCESS)
  {
    Serial.println("lmh_send ok");
  }
  else
  {
    Serial.printf("lmh_send failed: %d\n", error);
  }
#endif
  msgcount++;

  delay(300000);
}
