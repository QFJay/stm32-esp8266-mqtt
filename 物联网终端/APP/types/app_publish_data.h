#ifndef __APP_PUBLISH_DATA_H
#define __APP_PUBLISH_DATA_H

typedef struct
{
	char payload[200];
	char topic[64];
} MQTTPublishData;

#endif
