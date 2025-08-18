#include "socket.h"

SocketState socketState;

void handleSocket() {
    // Change the light color without affecting the state
    if(!socketState.colorSet){ 
        setLeds({8, 64, 64 ,0}); 
        socketState.colorSet = true;
    }

    // If it's past the timeout and there's no client, exit the socket mode
    if(millis() > socketState.timeout && !socketState.socketClient){
        socketState.sockMode = false;
        socketState.socketClient.stop();
        resetStatic();
    // If there's no client look if there's one waiting
    } else if(!socketState.socketClient){
        socketState.socketClient = socketServer.accept();
        socketState.socketClient.Client::setTimeout(100); // the max wait time to get all the bytes for a single color
    }

    // If there's a client, get the color
    if(socketState.socketClient.connected()){
        socketState.socketClient.println(socketState.socketClient.getTimeout());
        Color color;
        // read the buffer and put the bytes in the color Struct
        uint8_t *rx = (uint8_t*) &color;
        int buff_len = socketState.socketClient.readBytes(rx, sizeof(Color));
        
        // if a full color was correctly read, set the leds to that color
        if(buff_len == sizeof(Color)){
            socketState.socketClient.write(rx, sizeof(Color));
            setLeds(color, false);
        }
    }
}

bool inSocketMode() { return socketState.sockMode; }

void getSocketConnection() {
    unsigned long timeout = server.arg("timeout").toInt();
    if(timeout == 0) { timeout = 1000; }

    socketState.sockMode = true;
    socketState.colorSet = false;
    socketState.timeout = millis() + timeout;

    String res = String("{\"port\": 1221, \"timeout\":") + timeout + "}";
    server.send(200, "application/json", res);
}

void setDefaultSocketState() {
    socketState.sockMode = false;
    socketState.colorSet = false;
    socketState.socketClient.stop();
}