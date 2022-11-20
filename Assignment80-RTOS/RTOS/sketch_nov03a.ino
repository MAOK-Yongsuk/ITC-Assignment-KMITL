#include <Arduino_FreeRTOS.h>
#include "queue.h"

#define SW1 2
#define SW2 3
#define SW3 4

#define RED     6
#define YELLOW  7
#define GREEN    8

QueueHandle_t Blinking;

void setup(){
  Serial.begin(9600);
    //สร้าง Queue
    Blinking =  xQueueCreate(5, sizeof(int32_t));
    //สร้าง Task
    xTaskCreate(vSenderTask, "Sender Task 1", 100, SW1, 1, NULL);
    xTaskCreate(vSenderTask, "Sender Task 2", 100, SW2, 1, NULL);
    xTaskCreate(vSenderTask, "Sender Task 3", 100, SW3, 1, NULL);
    xTaskCreate(vReceiverTask, "Receiver Task", 100, NULL, 1, NULL);

}

void vSenderTask(void *pvParameters)
{
  /*
  เช็คว่ามีการกดสวิตซ์หรือไม่ ถ้ามีการกดจะให้ค่าของตัวแปร valueToSend มีค่าเท่ากับ Pin ที่มีการกด 
  ไม่งั้นก็จะมีค่าเป็น 0 จากนั้นจะมีการตรวจสอบว่า หาก valueToSend มีค่าไม่เท่ากับ 0 (คือมีการกด) ก็ให้ส่งค่าเข้าไปในคิว
  */
  BaseType_t qStatus;
  int32_t valueToSend = 0;
  int SW = (int32_t)pvParameters;
  pinMode(SW, INPUT);
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
  while (1)
  {
    if (digitalRead(SW) == HIGH)
      valueToSend = SW;
    else
      valueToSend = 0;
    if (valueToSend != 0) {
      qStatus = xQueueSend(Blinking, &valueToSend, xTicksToWait);
      if (qStatus !=  pdPASS)
      {
        Serial.println("Could not send to the queue");
      }
      vTaskDelay(100);
    }
  }
}

void vReceiverTask(void *pvParameters)
{
  int32_t valueReceived;
  BaseType_t qStatus;
  const TickType_t xTicksToWait = pdMS_TO_TICKS(100);
  bool yellow_blink = false;
  bool green_blink = false;
  int greencount = 0;
  pinMode(RED, OUTPUT); 
  pinMode(YELLOW, OUTPUT); 
  pinMode(GREEN, OUTPUT); 
  while (1)
  {
    qStatus = xQueueReceive(Blinking, &valueReceived, xTicksToWait);
    if (qStatus  != pdPASS)
    {
      Serial.println("Could not receive from queue ");
    }
    else
    {
      Serial.print("Received value  : ");
      Serial.println(valueReceived);
      if (valueReceived == 2) {
          digitalWrite(RED,HIGH);
          vTaskDelay(300);
          digitalWrite(RED,LOW);
      } else if (valueReceived == 3) {
          yellow_blink = true;
          if(yellow_blink){
              digitalWrite(YELLOW,!digitalRead(YELLOW));
              vTaskDelay(50);
            }
          else{
            digitalWrite(YELLOW,LOW);
            }        
      } else if (valueReceived == 4){
          //green_blink = !green_blink;
          if(greencount == 3){greencount = 0;}
          while(greencount != 3){           
            digitalWrite(GREEN,HIGH);
            delay(700);
            digitalWrite(GREEN,LOW);
            greencount += 1;
            delay(700);           
          }     
        }
      else {
        vTaskDelay(1);
      }
    }
  }
}
void loop() {}
