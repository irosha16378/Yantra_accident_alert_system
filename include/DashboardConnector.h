#ifndef DASHBOARD_CONNECTOR_H
#define DASHBOARD_CONNECTOR_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>

// ==========================================
// CLASS: DashboardConnector
// Purpose: Connects to Wi-Fi and sends accident 
// data (JSON) to the Python Web Dashboard.
// ==========================================
class DashboardConnector {
private:
    const char* ssid;
    const char* password;
    String serverUrl;

public:
    // Constructor: Store Wi-Fi credentials and the Python Server URL
    DashboardConnector(const char* wifiSsid, const char* wifiPass, String url) {
        ssid = "Dialog 4G 815";
        password = "bd7cAe60";
        serverUrl = url;
    }

    // Attempt to connect to the specified Wi-Fi network
    void initWiFi() {
        Serial.print("\nConnecting to Wi-Fi: ");
        Serial.println(ssid);
        
        WiFi.begin(ssid, password);
        
        // Wait until connection is successful
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
        
        Serial.println("\n✅ Connected to Wi-Fi Successfully!");
        Serial.print("ESP32 IP Address: ");
        Serial.println(WiFi.localIP());
        Serial.println("----------------------------------------\n");
    }

    // Send the latitude and longitude to the Web Dashboard via HTTP POST
    bool sendAlert(double lat, double lng) {
        if (WiFi.status() == WL_CONNECTED) {
            HTTPClient http;
            http.begin(serverUrl);
            
            // Specify that we are sending JSON data
            http.addHeader("Content-Type", "application/json");

            // Format the JSON payload matching the Python Server requirements
            String jsonPayload = "{\"latitude\":" + String(lat, 6) + ",\"longitude\":" + String(lng, 6) + "}";
            
            // Send the request
            int httpResponseCode = http.POST(jsonPayload);
            http.end();

            if (httpResponseCode > 0) {
                Serial.println("🚀 Alert successfully sent to Web Dashboard!");
                return true;
            }
        }
        Serial.println("❌ Failed to send data to server.");
        return false;
    }
};

#endif