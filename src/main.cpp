#include <Arduino.h>
#include <LittleFS.h>
#include <WiFiManager.h>
#include <WebSocketsServer.h>
#include "WindSpeed.h"
#include "WindDirection.h"
#include "Climate.h"

WindSpeed anemometer(4, 60);        // pin GPIO4, 60 mm radius rotating scoops 
WindDirection compass;
Climate climate(5);                 // pin GPIO5

WiFiManager wifiMgr;
WebSocketsServer webSocket = WebSocketsServer(81);
unsigned long lastUpdate = 0;

// Callback to bind LittleFS files to URLs
void bindServerCallback() {
    // Serve the HTML file
    wifiMgr.server->on("/weather", []() {
        if (!LittleFS.exists("/weather.html")) {
            wifiMgr.server->send(404, "text/plain", "File not found");
            return;
        }
        File file = LittleFS.open("/weather.html", "r");
        wifiMgr.server->streamFile(file, "text/html");
        file.close();
    });

    // Serve the CSS file
    wifiMgr.server->on("/style.css", []() {
        File file = LittleFS.open("/style.css", "r");
        wifiMgr.server->streamFile(file, "text/css");
        file.close();
    });
}

void initClimate() {
    Serial.print("Initializing Climate (T/H/P) Sensor... ");
    if (climate.begin()) {
        Serial.println("Ready");
    } else {
        Serial.println("Error: I2C error");
    }
}

void initAnemometer() {
    anemometer.begin();
}

void initCompass() {
    Serial.print("Initializing Wind Direction Sensor... ");
    if (compass.begin()) {
        Serial.println("Ready");
    } else {
        Serial.println("Error: Magnet not detected or I2C error");
    }
}

void setup() {
    Serial.begin(115200);

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("LittleFS Mount Failed");
        return;
    }

    // Sensors Setup
    initCompass();
    initAnemometer();
    initClimate();

    // WiFiManager Setup
    wifiMgr.setWebServerCallback(bindServerCallback);
    wifiMgr.setConfigPortalBlocking(false);              // Critical for background tasks
    wifiMgr.autoConnect("baoit-weather-AP");
    webSocket.begin();
    //wifiMgr.setMenu({"wifi", "custom", "exit"});
    //wifiMgr.setCustomMenuHTML("<form action='/weather'><button>View Live Weather</button></form><br/>");
}

void loop() {
    wifiMgr.process();           // Handle WiFiManager tasks
    webSocket.loop();       // Handle WebSocket tasks
    anemometer.update();    // Handle anemometer tasks

    // Broadcast sensor data periodically (5 seconds)
    if (millis() - lastUpdate > 5000) {
        // Collect data from sensors
        float temp = -999.9;
        float hum  = -999.9;
        float press  = -999.9;
        WeatherData data = climate.getData();
        if (data.valid) {
            temp = data.temp;
            hum = data.hum;
            press = data.press;
        }
        float speed = anemometer.getSpeedMS();
        int dir = compass.getDegrees();

        // Create JSON payload
        String json = "{";
        json += "\"temp\":" + String(temp) + ",";
        json += "\"hum\":" + String(hum) + ",";
        json += "\"press\":" + String(press) + ",";
        json += "\"speed\":" + String(speed) + ",";
        json += "\"dir\":\"" + String(dir) + "\"";
        json += "}";

        // Send the readings to all connected clients
        webSocket.broadcastTXT(json);

        lastUpdate = millis();
    }
}
