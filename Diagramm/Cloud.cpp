#include "Cloud.h"

constexpr char* ssid = "KronstadtBrasov";
constexpr char* password = "AlexAndreeaDanRalphVierling";
constexpr unsigned long channelID = 1363075;
constexpr char* writeKey = "4HF52H0KRO52LUML";
constexpr char* readKey = "UJDW2P7XH3SP2RV2";

WiFiClient client;

void init_wifi() {
    Serial.println("Verbinde mit WLan");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(WiFi.status());
        Serial.print(".");
        delay(500);
    };
}

void init_cloud() {
    ThingSpeak.begin(client);
}

namespace cl {
    void send(Record values) {
        ThingSpeak.setField(1, values.temp);
        ThingSpeak.setField(2, values.press);
        ThingSpeak.setField(3, values.humid);

        ThingSpeak.writeFields(channelID, writeKey);
    }
}
