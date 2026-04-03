#include "esp8266/app_esp8266.h"
#include "esp8266/bsp_esp8266.h"
#include "types/app_sensor_data.h"
#include "led/bsp_led.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"

#define ESP8266_AP_SSID					"Play6T"
#define ESP8266_AP_PWD					"qazwsxedc"

#define CONNECT_ID							0

#define MQTT_SERVER_IP					"mqtts.heclouds.com"
#define MQTT_SERVER_PORT				1883

#define DEVICE_NAME							"device1"
#define PRODUCT_ID							"Roh7b244ZQ"
#define TOKEN										"version=2018-10-31&res=products%2FRoh7b244ZQ%2Fdevices%2Fdevice1&et=1806304980&method=md5&sign=qn35a42%2BXjkg2dKa70b8aQ%3D%3D"

#define MQTT_TOPIC_POST					"$sys/Roh7b244ZQ/device1/thing/property/post"
#define MQTT_TOPIC_POST_REPLY		"$sys/Roh7b244ZQ/device1/thing/property/post/reply"
#define MQTT_TOPIC_SET					"$sys/Roh7b244ZQ/device1/thing/property/set"
#define MQTT_TOPIC_SET_REPLY		"$sys/Roh7b244ZQ/device1/thing/property/set_reply"
#define MQTT_QOS								0

extern osSemaphoreId_t ESP8266ReceiveSemaphoreHandle;
extern osMessageQueueId_t ESP8266SendQueueHandle;
extern osMessageQueueId_t ESP8266ReceiveQueueHandle;

extern uint8_t ESP8266_RxBuff[];
extern uint16_t ESP8266_RxBuff_Size;

bool ESP8266_Preparation(void)
{
	printf(">>> ESP8266 is powered on for initialization...\r\n");

	ESP8266_Init();
	printf(">>> ESP8266 has been initialized and is now ready to receive...\r\n");

	printf(">>> ESP8266 is undergoing software reset...\r\n");
	if (ESP8266_SoftReset_WaitReady() == true)
	{
		printf(">>> ESP8266 has been reset.\r\n");
	}
	else
	{
		printf(">>> ESP8266 reset timeout!\r\n");
		return false;
	}

	if (ESP8266_Test() == true)
	{
		printf(">>> ESP8266 has passed the test.\r\n");
	}
	else
	{
		printf(">>> ESP8266 is unavailable!\r\n");
		return false;
	}

	if (ESP8266_SetNetMode(STA) == true)
	{
		printf(">>> ESP8266 has been set to STA mode.\r\n");
	}
	else
	{
		printf(">>> ESP8266 failed to set the working mode!\r\n");
		return false;
	}

	if (ESP8266_ConnectWifi(ESP8266_AP_SSID, ESP8266_AP_PWD) == CONNECT_OK)
	{
		printf(">>> Wi-Fi connected.\r\n");
	}
	else
	{
		printf(">>> Wi-Fi failed to connect!\r\n");
		return false;
	}

	if (ESP8266_ConfigMQTTUser(CONNECT_ID, true, DEVICE_NAME, PRODUCT_ID, TOKEN) == true)
	{
		printf(">>> MQTT user information configured successfully.\r\n");
	}
	else
	{
		printf(">>> Failed to configure MQTT user information!\r\n");
		return false;
	}

	if (ESP8266_ConnectMQTTServer(MQTT_SERVER_IP, MQTT_SERVER_PORT, CONNECT_ID, true) == true)
	{
		printf(">>> Connected to the MQTT server successfully.\r\n");
	}
	else
	{
		printf(">>> Failed to connect to the MQTT server!\r\n");
		return false;
	}

	if (ESP8266_MQTTSubscribe(CONNECT_ID, MQTT_TOPIC_POST_REPLY, MQTT_QOS) == true)
	{
		printf(">>> Subscribed to MQTT topic \"post/reply\" successfully.\r\n");
	}
	else
	{
		printf(">>> Failed to subscribe to MQTT topic \"post/reply\"!\r\n");
		return false;
	}

	if (ESP8266_MQTTSubscribe(CONNECT_ID, MQTT_TOPIC_SET, MQTT_QOS) == true)
	{
		printf(">>> Subscribed to MQTT topic \"set\" successfully.\r\n\r\n");
	}
	else
	{
		printf(">>> Failed to subscribe to MQTT topic \"set\"!\r\n");
		return false;
	}

	ESP8266_EnablePrintResponse();

	return true;
}

void StartESP8266SendTask(void *argument)
{
	uint32_t payload_id = 1;

	while (1)
	{
		char payload[200];
		SensorData *sensor_data;

		osMessageQueueGet(ESP8266SendQueueHandle, &sensor_data, 0, osWaitForever);

		if (sensor_data->dht11_check_result == DHT11_DATA_OK)
		{
			snprintf(payload, sizeof(payload),
				"{\\\"id\\\":\\\"%d\\\"\\,\\\"version\\\":\\\"1.0\\\"\\,\\\"params\\\":{\\\"humidity\\\":{\\\"value\\\":%.1f}\\,\\\"temperature\\\":{\\\"value\\\":%.1f}\\,\\\"light\\\":{\\\"value\\\":%.2f}}}",
				payload_id++, sensor_data->humi_value, sensor_data->temp_value, sensor_data->light_percentage_value);

			ESP8266_MQTTPublish(CONNECT_ID, MQTT_TOPIC_POST, payload, MQTT_QOS, false);
		}

		vPortFree(sensor_data);
		sensor_data = NULL;
	}
}

void StartESP8266ReceiveTask(void *argument)
{
	while (1)
	{
		osSemaphoreAcquire(ESP8266ReceiveSemaphoreHandle, osWaitForever);

		char *message = pvPortMalloc(sizeof(char) * (ESP8266_RxBuff_Size + 1));

		if (message != NULL)
		{
			memcpy(message, ESP8266_RxBuff, ESP8266_RxBuff_Size);
			message[ESP8266_RxBuff_Size] = '\0';
			osMessageQueuePut(ESP8266ReceiveQueueHandle, &message, 0, osWaitForever);
		}
	}
}

void StartESP8266RespondTask(void *argument)
{
//	uint32_t payload_id;
	char head[15];

	snprintf(head, sizeof(head), "+MQTTSUBRECV:%d", CONNECT_ID);

	while (1)
	{
		char *message;

		osMessageQueueGet(ESP8266ReceiveQueueHandle, &message, 0, osWaitForever);

		if (strstr(message, head) != NULL)
		{
			if (strstr(message, "\"LED\":true") != NULL)
			{
				LED_On(LED_ONBOARD);
			}
			else if (strstr(message, "\"LED\":false") != NULL)
			{
				LED_Off(LED_ONBOARD);
			}
		}

		vPortFree(message);
		message = NULL;
	}
}
