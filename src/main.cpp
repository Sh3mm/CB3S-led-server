//
// Created by sh3mm on 8/4/25.
//
#include "main.h"

#include <WiFi.h>
#include <WebServer.h>

WebServer server(80);

void res(){
    int x = server.args();

    String b;
    for (int i = 0; i < x; i++) {
        String v = server.arg(i);
        int v2 = v.toInt();
        
        b += v;
        b += '|';
        b += server.argName(i);
        b += '|';
    }
    

    server.send(200, "text/plain", b);
}

void setup() {
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }

    server.on("/lights", HTTP_POST, res);

    server.on("/inline", [](){
        server.send(200, "text/plain", "this works as well");
    });

      server.onNotFound([](){
        server.send(404, "text/plain", "Error 404");
    });
    
    server.begin();
}


void loop() {
    server.handleClient();
}

