#include <SPI.h>
#include <Ethernet.h>

// MAC & IP Configuration (Update as per your network setup)
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 81);
EthernetServer server(80);

// Define pin mappings
const int outputPins[] = {2, 3, 4, 5, 6, 7, 8, 9}; // 8 digital output pins

void setup() {
    Ethernet.begin(mac, ip);
    server.begin();
    Serial.begin(115200);
    
    // Set output pins to OUTPUT mode
    for (int i = 0; i < 8; i++) {
        pinMode(outputPins[i], OUTPUT);
        digitalWrite(outputPins[i], LOW); // Default to LOW
    }
}

void loop() {
    EthernetClient client = server.available();
    if (client) {
        String request = "";
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                request += c;
                if (c == '\n') break;
            }
        }
        
        if (request.indexOf("GET /status") >= 0) {
            sendStatus(client);
        } else if (request.indexOf("GET /set/") >= 0) {
            handleSetCommand(client, request);            
        }
        
        client.stop();
    }
}

void sendStatus(EthernetClient client) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Access-Control-Allow-Origin: *"); // This line enables CORS
    client.println("Connection: close");
    client.println();
    
    client.print("{\"outputs\": [");
    for (int i = 0; i < 8; i++) {
        client.print(digitalRead(outputPins[i]));
        if (i < 7) client.print(", ");
    }
    client.println("]}");
}

void handleSetCommand(EthernetClient client, String request) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Access-Control-Allow-Origin: *"); // This line enables CORS
    client.println("Connection: close");
    client.println();
    
    int channel = request.substring(9, 10).toInt(); // Extract pin number from URL
    int state = request.substring(11, 12).toInt(); // Extract state from URL
    if(operate(channel, state) == -1){
      Serial.println("!!! FAILED TO OPERATE !!! ");
    }
}

int operate(int channel, int state) {
    if (channel >= 0 && channel < 8 && (state == 0 || state == 1)) {
        digitalWrite(outputPins[channel], state);   
        Serial.print("Changing channel ");
        Serial.print(channel);
        Serial.print(" (pin ");
        Serial.print(outputPins[channel]);
        Serial.print(") to ");
        Serial.println(state);
        return 0;
    }
    else {
        return -1;
    }

}
