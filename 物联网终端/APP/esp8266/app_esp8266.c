#include "esp8266/app_esp8266.h"
#include "esp8266/bsp_esp8266.h"
#include "types/app_sensor_data.h"
#include "types/app_publish_data.h"
#include "oled/bsp_i2c_oled.h"
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

extern osSemaphoreId_t ESP8266ReceiveSemaphoreHandle;
extern osMessageQueueId_t ESP8266PackQueueHandle;
extern osMessageQueueId_t ESP8266RespondQueueHandle;
extern osMessageQueueId_t ESP8266SendQueueHandle;

extern uint8_t ESP8266_RxBuff[];
extern uint16_t ESP8266_RxBuff_Size;

bool ESP8266_Preparation(void)
{
	printf(">>> ESP8266 is powered on for initialization...\r\n");

	ESP8266_Init();
	printf(">>> ESP8266 has been initialized and is now ready to receive...\r\n");
	OLED_ClearArea(0, 0, 128, 48);
	OLED_ShowString(0, 0, "[1/9] OK", OLED_8X16);
	OLED_ShowString(0, 16, "ESP8266 Init", OLED_8X16);
	OLED_Update();
	HAL_Delay(300);

	printf(">>> ESP8266 is undergoing software reset...\r\n");
	OLED_ClearArea(0, 0, 128, 48);
	OLED_ShowString(0, 0, "[2/9]", OLED_8X16);
	OLED_ShowString(0, 16, "ESP8266 Reset", OLED_8X16);
	OLED_Update();
	if (ESP8266_SoftReset_WaitReady() == true)
	{
		printf(">>> ESP8266 has been reset.\r\n");
		OLED_ShowString(48, 0, "OK", OLED_8X16);
		OLED_Update();
		HAL_Delay(300);
	}
	else
	{
		printf(">>> ESP8266 reset timeout!\r\n");
		return false;
	}

	OLED_ClearArea(0, 0, 128, 48);
	OLED_ShowString(0, 0, "[3/9]", OLED_8X16);
	OLED_ShowString(0, 16, "ESP8266 Test", OLED_8X16);
	OLED_Update();
	if (ESP8266_Test() == true)
	{
		printf(">>> ESP8266 has passed the test.\r\n");
		OLED_ShowString(48, 0, "OK", OLED_8X16);
		OLED_Update();
		HAL_Delay(300);
	}
	else
	{
		printf(">>> ESP8266 is unavailable!\r\n");
		return false;
	}

	OLED_ClearArea(0, 0, 128, 48);
	OLED_ShowString(0, 0, "[4/9]", OLED_8X16);
	OLED_ShowString(0, 16, "ESP8266 Set To", OLED_8X16);
	OLED_ShowString(0, 32, "STA Mode", OLED_8X16);
	OLED_Update();
	if (ESP8266_SetNetMode(STA) == true)
	{
		printf(">>> ESP8266 has been set to STA mode.\r\n");
		OLED_ShowString(48, 0, "OK", OLED_8X16);
		OLED_Update();
		HAL_Delay(300);
	}
	else
	{
		printf(">>> ESP8266 failed to set the working mode!\r\n");
		return false;
	}

	OLED_ClearArea(0, 0, 128, 48);
	OLED_ShowString(0, 0, "[5/9]", OLED_8X16);
	OLED_ShowString(0, 16, "ESP8266 Connect", OLED_8X16);
	OLED_ShowString(0, 32, "Wi-Fi", OLED_8X16);
	OLED_Update();
	if (ESP8266_ConnectWifi(ESP8266_AP_SSID, ESP8266_AP_PWD) == CONNECT_OK)
	{
		printf(">>> Wi-Fi connected.\r\n");
		OLED_ShowString(48, 0, "OK", OLED_8X16);
		OLED_Update();
		HAL_Delay(300);
	}
	else
	{
		printf(">>> Wi-Fi failed to connect!\r\n");
		return false;
	}

	OLED_ClearArea(0, 0, 128, 48);
	OLED_ShowString(0, 0, "[6/9]", OLED_8X16);
	OLED_ShowString(0, 16, "Config MQTT User", OLED_8X16);
	OLED_ShowString(0, 32, "Information", OLED_8X16);
	OLED_Update();
	if (ESP8266_ConfigMQTTUser(CONNECT_ID, true, DEVICE_NAME, PRODUCT_ID, TOKEN) == true)
	{
		printf(">>> MQTT user information configured successfully.\r\n");
		OLED_ShowString(48, 0, "OK", OLED_8X16);
		OLED_Update();
		HAL_Delay(300);
	}
	else
	{
		printf(">>> Failed to configure MQTT user information!\r\n");
		return false;
	}

	OLED_ClearArea(0, 0, 128, 48);
	OLED_ShowString(0, 0, "[7/9]", OLED_8X16);
	OLED_ShowString(0, 16, "ESP8266 Connect", OLED_8X16);
	OLED_ShowString(0, 32, "MQTT Server", OLED_8X16);
	OLED_Update();
	if (ESP8266_ConnectMQTTServer(MQTT_SERVER_IP, MQTT_SERVER_PORT, CONNECT_ID, true) == true)
	{
		printf(">>> Connected to the MQTT server successfully.\r\n");
		OLED_ShowString(48, 0, "OK", OLED_8X16);
		OLED_Update();
		HAL_Delay(300);
	}
	else
	{
		printf(">>> Failed to connect to the MQTT server!\r\n");
		return false;
	}

	OLED_ClearArea(0, 0, 128, 48);
	OLED_ShowString(0, 0, "[8/9]", OLED_8X16);
	OLED_ShowString(0, 16, "Subscribe Topic", OLED_8X16);
	OLED_ShowString(0, 32, "\"post/reply\"", OLED_8X16);
	OLED_Update();
	if (ESP8266_MQTTSubscribe(CONNECT_ID, MQTT_TOPIC_POST_REPLY, MQTT_QOS_0) == true)
	{
		printf(">>> Subscribed to MQTT topic \"post/reply\" successfully.\r\n");
		OLED_ShowString(48, 0, "OK", OLED_8X16);
		OLED_Update();
		HAL_Delay(300);
	}
	else
	{
		printf(">>> Failed to subscribe to MQTT topic \"post/reply\"!\r\n");
		return false;
	}

	OLED_ClearArea(0, 0, 128, 48);
	OLED_ShowString(0, 0, "[9/9]", OLED_8X16);
	OLED_ShowString(0, 16, "Subscribe Topic", OLED_8X16);
	OLED_ShowString(0, 32, "\"set\"", OLED_8X16);
	OLED_Update();
	if (ESP8266_MQTTSubscribe(CONNECT_ID, MQTT_TOPIC_SET, MQTT_QOS_0) == true)
	{
		printf(">>> Subscribed to MQTT topic \"set\" successfully.\r\n\r\n");
		OLED_ShowString(48, 0, "OK", OLED_8X16);
		OLED_Update();
		HAL_Delay(300);
	}
	else
	{
		printf(">>> Failed to subscribe to MQTT topic \"set\"!\r\n");
		return false;
	}

	ESP8266_EnablePrintResponse();

	return true;
}

void StartESP8266PackTask(void *argument)
{
	uint32_t payload_id = 1;

	while (1)
	{
		SensorData *sensor_data;

		osMessageQueueGet(ESP8266PackQueueHandle, &sensor_data, 0, osWaitForever);

		if (sensor_data->dht11_check_result == DHT11_DATA_OK)
		{
			MQTTPublishData *publish_data = pvPortMalloc(sizeof(MQTTPublishData));
			if (publish_data == NULL)
			{
				Error_Handler();
			}

			memcpy(publish_data->topic, MQTT_TOPIC_POST, sizeof(MQTT_TOPIC_POST));

			snprintf(publish_data->payload, sizeof(publish_data->payload),
				"{\\\"id\\\":\\\"%d\\\"\\,\\\"params\\\":{\\\"humidity\\\":{\\\"value\\\":%.1f}\\,\\\"temperature\\\":{\\\"value\\\":%.1f}\\,\\\"light\\\":{\\\"value\\\":%.2f}\\,\\\"LED\\\":{\\\"value\\\":%s}}}",
			payload_id++, sensor_data->humi_value, sensor_data->temp_value, sensor_data->light_percentage_value, LED_IsOn(LED_ONBOARD) ? "true" : "false");

			osMessageQueuePut(ESP8266SendQueueHandle, &publish_data, 0, osWaitForever);
		}

		vPortFree(sensor_data);
		sensor_data = NULL;
	}
}

void StartESP8266ReceiveTask(void *argument)
{
	char head[100];

	snprintf(head, sizeof(head), "+MQTTSUBRECV:%d,\"%s\"", CONNECT_ID, MQTT_TOPIC_SET);

	while (1)
	{
		osSemaphoreAcquire(ESP8266ReceiveSemaphoreHandle, osWaitForever);

		if (strstr((char *)ESP8266_RxBuff, head) != NULL)
		{
			char *message = pvPortMalloc(sizeof(char) * (ESP8266_RxBuff_Size + 1));
			if (message == NULL)
			{
				Error_Handler();
			}

			memcpy(message, ESP8266_RxBuff, ESP8266_RxBuff_Size);
			message[ESP8266_RxBuff_Size] = '\0';
			osMessageQueuePut(ESP8266RespondQueueHandle, &message, 0, osWaitForever);
		}
	}
}

void StartESP8266RespondTask(void *argument)
{
	while (1)
	{
		char *message;
		uint32_t payload_id;

		osMessageQueueGet(ESP8266RespondQueueHandle, &message, 0, osWaitForever);
		char *json_start = strstr(message, "{");

		if (json_start != NULL)
		{
			if (sscanf(json_start, "{\"id\":\"%d\"", &payload_id) == 1)
			{
				MQTTPublishData *publish_data = pvPortMalloc(sizeof(MQTTPublishData));
				if (publish_data == NULL)
				{
					Error_Handler();
				}

				memcpy(publish_data->topic, MQTT_TOPIC_SET_REPLY, sizeof(MQTT_TOPIC_SET_REPLY));
				snprintf(publish_data->payload, sizeof(publish_data->payload), "{\\\"id\\\":\\\"%d\\\"\\,\\\"code\\\":200\\,\\\"msg\\\":\\\"success\\\"}", payload_id);
				osMessageQueuePut(ESP8266SendQueueHandle, &publish_data, 0, osWaitForever);

				if (strstr(json_start, "\"LED\":true") != NULL)
				{
					LED_On(LED_ONBOARD);
				}
				else if (strstr(json_start, "\"LED\":false") != NULL)
				{
					LED_Off(LED_ONBOARD);
				}
			}
		}

		vPortFree(message);
		message = NULL;
	}
}

void StartESP8266SendTask(void *argument)
{
	while (1)
	{
		MQTTPublishData *publish_data;

		osMessageQueueGet(ESP8266SendQueueHandle, &publish_data, 0, osWaitForever);

		ESP8266_MQTTPublish(CONNECT_ID, publish_data->topic, publish_data->payload, MQTT_QOS_0, false);

		vPortFree(publish_data);
		publish_data = NULL;
	}
}
