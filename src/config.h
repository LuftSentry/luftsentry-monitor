#include <pgmspace.h>
#define U_PART U_SPIFFS

/*Put your SSID & Password*/
const char* ssid = "Darth Vader";  // Enter SSID here
const char* password = "stormtrooper";  //Enter Password here

IPAddress ip(192, 168, 1, 200);
IPAddress gateway(192, 168, 1, 255);
IPAddress subnet(255, 255, 255, 0);

const char* MQTT_BROKER_ADRESS = "a34yrxgh1yvd6y-ats.iot.us-east-1.amazonaws.com";
const uint16_t MQTT_PORT = 8883;

const char* REGISTER_TOPIC = "Luft_Sentry/STATUS/monitor";
const char* DATA_TOPIC = "Luft_Sentry/devices/esp32abc1234/aqi";

const char* FIRMWARE_URL = "https://luftsentry.s3.amazonaws.com/versions/latest.json";
const char* FIRMWARE_VERSION = "0.1.0";
const char* OTA_TOPIC = "Lust_Sentry/OTA/esp32-m";


String host = "luftsentry.s3.amazonaws.com";
int port = 80;
String bin = "/binaries/monitor.bin";


const char* filenames[] = {"/data/AmazonRootCA1.pem", "/data/api_server.cert.pem", "/data/api_server.private.key"};
const char* urls[] = { "https://luftsentry.s3.amazonaws.com/certificates/AmazonRootCA1.pem", "https://luftsentry.s3.amazonaws.com/certificates/api_server.cert.pem", "https://luftsentry.s3.amazonaws.com/certificates/api_server.private.key" };
int num_files = sizeof(filenames) / sizeof(filenames[0]);

char* pRead_rootca;
char* pRead_cert;
char* pRead_privatekey;

const char* getMQTTClientName() {
    uint32_t chipId = 0;
    for(int i=0; i<17; i=i+8) {
        chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
    }
  
    // Crear una cadena de caracteres para almacenar el nombre generado
    static char clientName[20]; // Ajusta el tamaño según tus necesidades
    sprintf(clientName, "esp32_%u", chipId);

    return clientName;
}

const char* MQTT_CLIENT_NAME = getMQTTClientName();
