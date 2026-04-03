#ifndef __APP_SENSOR_DATA_H
#define __APP_SENSOR_DATA_H

typedef enum
{
	DHT11_DATA_ERROR = 0,
	DHT11_DATA_OK,
	DHT11_CHECK_RESULT_COUNT
} DHT11_CheckResult;

typedef struct
{
	DHT11_CheckResult dht11_check_result;
	float humi_value;
	float temp_value;
	float light_percentage_value;
} SensorData;

typedef struct
{
	DHT11_CheckResult dht11_check_result;
	float humi_value;
	float humi_max;
	float humi_min;
	float temp_value;
	float temp_max;
	float temp_min;
	float light_percentage_value;
	float light_percentage_max;
	float light_percentage_min;
} DisplayData;

#endif
