#include <Arduino.h>
#include <FreeRTOS.h>
#include <task.h>

#define LED1 PA6
#define LED2 PA4

TaskHandle_t task1_handle;
TaskHandle_t task2_handle;

TickType_t task1_delay = pdMS_TO_TICKS(1000);
TickType_t task2_delay = pdMS_TO_TICKS(500);

void task1(void* pvParameters)
{
  while(1)
  {
    digitalWrite(LED1,HIGH);
    vTaskDelay(task1_delay);
    digitalWrite(LED1,LOW);
    vTaskDelay(task1_delay);
  }
}

void task2(void* pvParameters)
{
  while(1)
  {
    digitalWrite(LED2,HIGH);
    vTaskDelay(task2_delay);
    digitalWrite(LED2,LOW);
    vTaskDelay(task2_delay);
  }
}


void setup() {
  // put your setup code here, to run once:

  pinMode(LED1,OUTPUT);
  pinMode(LED2, OUTPUT);

  xTaskCreate(task1,"Task1",128,NULL,1,&task1_handle);
  xTaskCreate(task2,"Task2",128,NULL,1,&task2_handle);

  vTaskStartScheduler();
}

void loop() {
  // put your main code here, to run repeatedly:

}

