#ifndef _CLOUD_H
#define _CLOUD_H

#include "Record.h"
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

extern WiFiClient client;
void init_wifi();

void init_cloud();

namespace cl { //Cloud
    void send(Record values);
}

#endif //_CLOUD_H
