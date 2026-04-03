#include "esp8266/bsp_esp8266.h"
#include "usart/bsp_usart.h"
#include "cmsis_os2.h"
#include "FreeRTOS.h"

#define ESP8266_RST_Pin				GPIO_PIN_13
#define ESP8266_RST_GPIO_Port	GPIOB

#define UART_RX_BUFF_LEN			512

extern osSemaphoreId_t ESP8266ReceiveSemaphoreHandle;

uint8_t ESP8266_RxBuff[UART_RX_BUFF_LEN];
uint16_t ESP8266_RxBuff_Size;

volatile bool ESP_Rx_Complete = false;
volatile bool ESP_Tx_Complete = false;
volatile bool ESP8266_PrintResponse = false;

void ESP8266_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ESP8266_RST_GPIO_Port, ESP8266_RST_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : ESP8266_RST_Pin */
  GPIO_InitStruct.Pin = ESP8266_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ESP8266_RST_GPIO_Port, &GPIO_InitStruct);
}

void ESP8266_Reset(void)
{
	HAL_GPIO_WritePin(ESP8266_RST_GPIO_Port, ESP8266_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(100);

	HAL_GPIO_WritePin(ESP8266_RST_GPIO_Port, ESP8266_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(500);
}

void UART_StartReceive(void)
{
	HAL_UARTEx_ReceiveToIdle_DMA(&huart3, ESP8266_RxBuff, UART_RX_BUFF_LEN);
}

void ESP8266_Init(void)
{
	ESP8266_Reset();
	ESP8266_RX_DMA_Init();
	ESP8266_UART_Init();
	ESP8266_GPIO_Init();
	UART_StartReceive();
}

void ESP8266_SendString(char *str)
{
	HAL_UART_Transmit_IT(&huart3, (uint8_t *)str, strlen(str));
}

bool ESP8266_Cmd(const char *cmd)
{
	char buf[256];
	uint32_t tick_start;

	ESP_Rx_Complete = false;
	ESP_Tx_Complete = false;

	memset(ESP8266_RxBuff, 0, UART_RX_BUFF_LEN);
	snprintf(buf, sizeof(buf), "%s\r\n", cmd);
	ESP8266_SendString(buf);

	tick_start = HAL_GetTick();
	while (ESP_Tx_Complete == false)
	{
		if (HAL_GetTick() - tick_start > 100)
		{
			return false;
		}
	}

	return true;
}

bool ESP8266_Test(void)
{
	uint32_t tick_start;

	if (ESP8266_Cmd("AT") == false)
	{
		return false;
	}

	tick_start = HAL_GetTick();
	while (HAL_GetTick() - tick_start < 1000)
	{
		if (ESP_Rx_Complete == true)
		{
			/* Ensure that the ESP8266_RxBuff ends with '\0' to prevent string operation overflow */
			ESP8266_RxBuff[UART_RX_BUFF_LEN - 1] = '\0';

			if (strstr((char *)ESP8266_RxBuff, "OK") != NULL)
			{
				ESP_Rx_Complete = false;	// Clear the flag
				return true;
			}

			/* Clear the flag and continue waiting */
			ESP_Rx_Complete = false;
		}
	}

	return false;
}

bool ESP8266_SoftReset_WaitReady(void)
{
	uint32_t tick_start;

	if (ESP8266_Cmd("AT+RST") == false)
	{
		return false;
	}

	tick_start = HAL_GetTick();
	while (HAL_GetTick() - tick_start < 5000)
	{
		if (ESP_Rx_Complete == true)
		{
			/* Ensure that the ESP8266_RxBuff ends with '\0' to prevent string operation overflow */
			ESP8266_RxBuff[UART_RX_BUFF_LEN - 1] = '\0';

			if (strstr((char *)ESP8266_RxBuff, "ready") != NULL)
			{
				ESP_Rx_Complete = false;	// Clear the flag
				return true;
			}

			/* Clear the flag and continue waiting */
			ESP_Rx_Complete = false;
		}
	}

	return false;
}

bool ESP8266_SetNetMode(NetModeTypeDef mode)
{
	char *cmd;
	uint32_t tick_start;

	switch (mode)
	{
		case STA:
			cmd = "AT+CWMODE=1";
			break;
		case AP:
			cmd = "AT+CWMODE=2";
			break;
		case STA_AP:
			cmd = "AT+CWMODE=3";
			break;
		default:
			return false;
	}

	if (ESP8266_Cmd(cmd) == false)
	{
		return false;
	}

	tick_start = HAL_GetTick();
	while (ESP_Rx_Complete == false)
	{
		if (HAL_GetTick() - tick_start > 1000)
		{
			return false;
		}
	}

	/* Ensure that the ESP8266_RxBuff ends with '\0' to prevent string operation overflow */
	ESP8266_RxBuff[UART_RX_BUFF_LEN - 1] = '\0';

	if (strstr((char *)ESP8266_RxBuff, "OK") == NULL)
	{
		return false;
	}

	return true;
}

bool ESP8266_SetConnMode(ConnModeTypeDef mode)
{
	char *cmd;
	uint32_t tick_start;

	cmd = (mode == SINGLE_CONN) ? "AT+CIPMUX=0" : "AT+CIPMUX=1";

	if (ESP8266_Cmd(cmd) == false)
	{
		return false;
	}

	tick_start = HAL_GetTick();
	while (ESP_Rx_Complete == false)
	{
		if (HAL_GetTick() - tick_start > 1000)
		{
			return false;
		}
	}

	/* Ensure that the ESP8266_RxBuff ends with '\0' to prevent string operation overflow */
	ESP8266_RxBuff[UART_RX_BUFF_LEN - 1] = '\0';

	if (strstr((char *)ESP8266_RxBuff, "OK") == NULL)
	{
		return false;
	}

	return true;
}

ConnWifiStateTypeDef ESP8266_ConnectWifi(const char *ssid, const char *password)
{
	char cmd[128];
	uint32_t tick_start;

	snprintf(cmd, sizeof(cmd), "AT+CWJAP=\"%s\",\"%s\"", ssid, password);

	if (ESP8266_Cmd(cmd) == false)
	{
		return CONNECT_FAIL;
	}

	tick_start = HAL_GetTick();
	while (HAL_GetTick() - tick_start < 15000)
	{
		if (ESP_Rx_Complete == true)
		{
			/* Ensure that the ESP8266_RxBuff ends with '\0' to prevent string operation overflow */
			ESP8266_RxBuff[UART_RX_BUFF_LEN - 1] = '\0';

			if (strstr((char *)ESP8266_RxBuff, "OK") != NULL)
			{
				return CONNECT_OK;
			}
			else if (strstr((char *)ESP8266_RxBuff, "CWJAP:1") != NULL)
			{
				return CONNECT_TIMEOUT;
			}
			else if (strstr((char *)ESP8266_RxBuff, "CWJAP:2") != NULL)
			{
				return PASSWORD_ERROR;
			}
			else if (strstr((char *)ESP8266_RxBuff, "CWJAP:3") != NULL)
			{
				return TARGET_AP_NOT_FOUND;
			}
			else if (strstr((char *)ESP8266_RxBuff, "CWJAP:4") != NULL || strstr((char *)ESP8266_RxBuff, "FAIL") != NULL)
			{
				return CONNECT_FAIL;
			}

			/* Clear the flag and continue waiting */
			ESP_Rx_Complete = false;
		}
	}

	return CONNECT_TIMEOUT;
}

bool ESP8266_ConfigMQTTUser(uint8_t conn_id, bool enable_ssl_tls, const char *device_name, const char *product_id, const char *token)
{
	char cmd[256];
	uint32_t tick_start;

	/* conn_id range: 0 ~ 4 */
	if (conn_id > 4)
	{
		return false;
	}

	snprintf(cmd, sizeof(cmd), "AT+MQTTUSERCFG=%d,%d,\"%s\",\"%s\",\"%s\",0,0,\"\"", conn_id, enable_ssl_tls, device_name, product_id, token);

	if (ESP8266_Cmd(cmd) == false)
	{
		return false;
	}

	tick_start = HAL_GetTick();
	while (HAL_GetTick() - tick_start < 1000)
	{
		if (ESP_Rx_Complete == true)
		{
			/* Ensure that the ESP8266_RxBuff ends with '\0' to prevent string operation overflow */
			ESP8266_RxBuff[UART_RX_BUFF_LEN - 1] = '\0';

			if (strstr((char *)ESP8266_RxBuff, "OK") != NULL)
			{
				return true;
			}

			/* Clear the flag and continue waiting */
			ESP_Rx_Complete = false;
		}
	}

	return false;
}

bool ESP8266_ConnectMQTTServer(const char *ip, uint16_t port, uint8_t conn_id, bool enable_auto_reconnect)
{
	char cmd[128];
	uint32_t tick_start;

	/* conn_id range: 0 ~ 4 */
	if (conn_id > 4)
	{
		return false;
	}

	snprintf(cmd, sizeof(cmd), "AT+MQTTCONN=%d,\"%s\",%d,%d", conn_id, ip, port, enable_auto_reconnect);

	if (ESP8266_Cmd(cmd) == false)
	{
		return false;
	}

	tick_start = HAL_GetTick();
	while (HAL_GetTick() - tick_start < 10000)
	{
		if (ESP_Rx_Complete == true)
		{
			/* Ensure that the ESP8266_RxBuff ends with '\0' to prevent string operation overflow */
			ESP8266_RxBuff[UART_RX_BUFF_LEN - 1] = '\0';

			if (strstr((char *)ESP8266_RxBuff, "OK") != NULL)
			{
				return true;
			}

			/* Clear the flag and continue waiting */
			ESP_Rx_Complete = false;
		}
	}

	return false;
}

bool ESP8266_MQTTSubscribe(uint8_t conn_id, const char *topic, uint8_t qos)
{
	char cmd[128];

	/* conn_id range: 0 ~ 4 */
	if (conn_id > 4)
	{
		return false;
	}

	/* qos range: 0 ~ 2 */
	if (qos > 2)
	{
		return false;
	}

	snprintf(cmd, sizeof(cmd), "AT+MQTTSUB=%d,\"%s\",%d", conn_id, topic, qos);

	if (ESP8266_Cmd(cmd) == false)
	{
		return false;
	}

	HAL_Delay(1000);

	return true;
}

bool ESP8266_MQTTPublish(uint8_t conn_id, const char *topic, const char *payload, uint8_t qos, bool enable_retain)
{
	char cmd[256];

	/* conn_id range: 0 ~ 4 */
	if (conn_id > 4)
	{
		return false;
	}

	/* qos range: 0 ~ 2 */
	if (qos > 2)
	{
		return false;
	}

	/* AT command length <= 256 */
	if (strlen(topic) + strlen(payload) > 230)
	{
		return false;
	}

	snprintf(cmd, sizeof(cmd), "AT+MQTTPUB=%d,\"%s\",\"%s\",%d,%d", conn_id, topic, payload, qos, enable_retain);

	if (ESP8266_Cmd(cmd) == false)
	{
		return false;
	}

	return true;
}

void ESP8266_EnablePrintResponse(void)
{
	ESP8266_PrintResponse = true;
}

/**
  * @brief  Reception Event Callback (Rx event notification called after use of advanced reception service).
  * @param  huart UART handle
  * @param  Size  Number of data available in application reception buffer (indicates a position in
  *               reception buffer until which, data are available)
  * @retval None
  */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART3)
	{
		ESP_Rx_Complete = true;

		if (ESP8266_PrintResponse == true)
		{
			HAL_UART_Transmit_IT(&huart1, ESP8266_RxBuff, Size);

			ESP8266_RxBuff_Size = Size;
			osSemaphoreRelease(ESP8266ReceiveSemaphoreHandle);
		}

		HAL_UARTEx_ReceiveToIdle_DMA(&huart3, ESP8266_RxBuff, UART_RX_BUFF_LEN);
	}
}

/**
  * @brief  Tx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART3)
	{
		ESP_Tx_Complete = true;
	}
}

/**
  * @brief  UART error callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART3)
	{
		/* Clear frame error flag */
		__HAL_UART_CLEAR_FEFLAG(huart);

		HAL_UARTEx_ReceiveToIdle_DMA(&huart3, ESP8266_RxBuff, UART_RX_BUFF_LEN);
	}
}
