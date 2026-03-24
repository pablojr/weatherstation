#include <Arduino.h>
#include <LittleFS.h>
#include <WiFiManager.h>
#include <WebSocketsServer.h>

WiFiManager wm;
WebSocketsServer webSocket = WebSocketsServer(81);
//const uint16_t UPDATE_INTERVAL = 5000;       // 5 seconds interval to send data updates
unsigned long lastUpdate = 0;

// Mock sensor function - replace with actual sensor code
String getSensorDataJSON() {
  float t = 22.5 + (random(-10, 10) / 10.0); // Dummy temp
  float h = 50.0 + (random(-20, 20) / 10.0); // Dummy humidity
  return "{\"temp\":" + String(t) + ",\"hum\":" + String(h) + "}";
}

// Callback to bind LittleFS files to URLs
void bindServerCallback() {
    // Serve the HTML file
    wm.server->on("/weather", []() {
        if (!LittleFS.exists("/weather.html")) {
            wm.server->send(404, "text/plain", "File not found");
            return;
        }
        File file = LittleFS.open("/weather.html", "r");
        wm.server->streamFile(file, "text/html");
        file.close();
    });

    // Serve the CSS file
    wm.server->on("/style.css", []() {
        File file = LittleFS.open("/style.css", "r");
        wm.server->streamFile(file, "text/css");
        file.close();
    });
}

void setup() {
    Serial.begin(115200);

    // Initialize LittleFS
    if (!LittleFS.begin()) {
        Serial.println("LittleFS Mount Failed");
        return;
    }

    // WiFiManager Setup
    wm.setWebServerCallback(bindServerCallback);
    wm.setConfigPortalBlocking(false);              // Critical for background tasks
    wm.autoConnect("baoit-weather-AP");
    webSocket.begin();
    //wm.setMenu({"wifi", "custom", "exit"});
    //wm.setCustomMenuHTML("<form action='/weather'><button>View Live Weather</button></form><br/>");
}

void loop() {
    wm.process();         // Handle WiFiManager tasks
    webSocket.loop();     // Handle WebSocket tasks

    // Broadcast sensor data every 2 seconds
    if (millis() - lastUpdate > 5000) {
        // Collect data from sensors
        float temp = 12.0;
        float hum  = 88.5;
        float speed = 2.3;
        int dir = 123;

        // Create JSON payload
        String json = "{";
        json += "\"temp\":" + String(temp) + ",";
        json += "\"hum\":" + String(hum) + ",";
        json += "\"speed\":" + String(speed) + ",";
        json += "\"dir\":\"" + String(dir) + "\"";
        json += "}";
        // Send the readings to all connected clients
        webSocket.broadcastTXT(json);

        lastUpdate = millis();
    }
}
