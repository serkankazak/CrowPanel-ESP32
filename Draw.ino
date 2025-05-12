#include <Wire.h>
#include <SPI.h>
#include "gfx_conf.h"
#include <WiFi.h>

const char* ssid     = "wifi_name";
const char* password = "wifi_password";

WiFiServer server(80);
String header;

const char* wl_status_to_string(wl_status_t status) {

    switch (status) {
    
        case WL_NO_SHIELD: return "WL_NO_SHIELD";
        case WL_IDLE_STATUS: return "WL_IDLE_STATUS";
        case WL_NO_SSID_AVAIL: return "WL_NO_SSID_AVAIL";
        case WL_SCAN_COMPLETED: return "WL_SCAN_COMPLETED";
        case WL_CONNECTED: return "WL_CONNECTED";
        case WL_CONNECT_FAILED: return "WL_CONNECT_FAILED";
        case WL_CONNECTION_LOST: return "WL_CONNECTION_LOST";
        case WL_DISCONNECTED: return "WL_DISCONNECTED";
    
    }

}

String wifi_str = "";
int max_wifi_i = 0;

void connect() {
    
    int connecting_i = 0;
    String connecting_s = "";
    
    while (WiFi.status() != WL_CONNECTED) {

        if (max_wifi_i++ > 50) {
            max_wifi_i = 0;
            WiFi.reconnect();
        }

        delay(500);
        connecting_i += 1;
        connecting_s = "";
        for (int i = 0; i < connecting_i; i++) {
            connecting_s += ".";
        }
        if (connecting_i > 10) {
            connecting_i = 0;
        }
        connecting_s = "Connecting to " + (String)ssid + " (" + wl_status_to_string(WiFi.status()) + ") " + connecting_s;

        if (wifi_str != connecting_s) {
            
            wifi_str = connecting_s;

            tft.fillRect(0, 0, 800, 15, TFT_BLACK);
            tft.setTextSize(2);
            tft.setCursor(0, 0);
            tft.print(wifi_str);
        
        }

    }

    connecting_s = "Connected to " + (String)ssid + " (Ip address: " + WiFi.localIP().toString() + ")";

    if (wifi_str != connecting_s) {
            
        wifi_str = connecting_s;

        tft.fillRect(0, 0, 800, 15, TFT_BLACK);
        tft.setTextSize(2);
        tft.setCursor(0, 0);
        tft.print(wifi_str);

    }

}

void setup() {

    tft.begin();
    tft.fillScreen(TFT_BLACK);

    WiFi.begin(ssid, password);
    connect();
    server.begin();

}

String strp(String header) {

    String s = header.substring(8, header.length());

    char buf[s.length() + 1];
    s.toCharArray(buf, (unsigned int)s.length());
    char *ptr;
    ptr = strchr(buf, ' ');
    if (ptr != NULL) {
        *ptr = '\0';
    }
    s = String(buf);

    s.replace("%20", " ");
    s.replace("%0A", "\n");
    s.replace("%0D", "\r");
    s.replace("%09", "\t");

    s.replace("%21", "!");
    s.replace("%22", "\"");
    s.replace("%23", "#");
    s.replace("%24", "$");
    s.replace("%25", "%");
    s.replace("%26", "&");
    s.replace("%27", "'");
    s.replace("%28", "(");
    s.replace("%29", ")");
    s.replace("%2A", "*");
    s.replace("%2B", "+");
    s.replace("%2C", ",");
    s.replace("%2D", "-");
    s.replace("%2E", ".");
    s.replace("%2F", "/");
    s.replace("%3A", ":");
    s.replace("%3B", ";");
    s.replace("%3C", "<");
    s.replace("%3D", "=");
    s.replace("%3E", ">");
    s.replace("%3F", "?");
    s.replace("%40", "@");
    s.replace("%5B", "[");
    s.replace("%5C", "\\");
    s.replace("%5D", "]");
    s.replace("%5E", "^");
    s.replace("%5F", "_");
    s.replace("%60", "`");
    s.replace("%7B", "{");
    s.replace("%7C", "|");
    s.replace("%7D", "}");
    s.replace("%7E", "~");
    s.replace("%82", "‚");
    s.replace("%84", "„");
    s.replace("%88", "ˆ");
    s.replace("%91", "‘");
    s.replace("%92", "’");
    s.replace("%93", "“");
    s.replace("%94", "”");
    s.replace("%95", "•");
    s.replace("%96", "–");
    s.replace("%97", "—");
    s.replace("%98", "˜");
    s.replace("%AF", "¯");
    s.replace("%B4", "´");
    s.replace("%B7", "·");
    s.replace("%B8", "¸");

    return s;

}

unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

int p1 = 0;
int p2 = 0;
String s1 = "";
String s2 = "";

void loop() {

    WiFiClient client = server.available();

    if (client) {  
        Serial.println("New Client");    
        String currentLine = "";    
        currentTime = millis();
        previousTime = currentTime;
        while (client.connected() && currentTime - previousTime <= timeoutTime) {    
            currentTime = millis();                 
            if (client.available()) {    
                char c = client.read();    
                Serial.write(c);    
                header += c;
                if (c == '\n') {    
                    if (currentLine.length() == 0) {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();
                        
                        bool update = false;

                        if (header.length() >= 3) {
                            if (header.substring(0, 7) == "GET /p1") {
                                int t = header.substring(8, 11).toInt();
                                if (t != p1) { p1 = t; update = true; }
                            } else if (header.substring(0, 7) == "GET /p2") {
                                int t = header.substring(8, 11).toInt();
                                if (t != p2) { p2 = t; update = true; }
                            } else if (header.substring(0, 7) == "GET /s1") {
                                String t = strp(header);
                                if (t != s1) { s1 = t; update = true; }
                            } else if (header.substring(0, 7) == "GET /s2") {
                                String t = strp(header);
                                if (t != s2) { s2 = t; update = true; }
                            }
                        }

                        if (update) {

                            tft.fillRect(0, 15, 800, 465, TFT_BLACK);

                            tft.fillRect(0, 440, 800, 20, TFT_WHITE);
                            tft.fillRect(0, 440, p1, 20, TFT_BLUE);

                            tft.fillRect(0, 460, 800, 20, TFT_WHITE);
                            tft.fillRect(0, 460, p2, 20, TFT_GREEN);

                            tft.setTextSize(5);
                            tft.setCursor(0, 25);
                            tft.print(s1);

                            tft.setTextSize(4);
                            tft.setCursor(0, 260);
                            tft.print(s2);

                        }
                        
                        client.println("<!DOCTYPE html><body>");
                        client.println("</body></html>");  
                        client.println();
                        break;
                    } else {    
                        currentLine = "";
                    }
                } else if (c != '\r') {    
                    currentLine += c;    
                }
            }
        }
        header = "";
        client.stop();
    }
    
    connect();

}
