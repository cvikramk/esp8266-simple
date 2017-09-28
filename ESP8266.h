#include <inttypes.h>
#include "IPAddress.h"
#include "Arduino.h"

class ESP8266{
    public:
        String response;
        bool recvMode = false;              
        unsigned long timeout = 100;
        int channelID = 0;
        bool setupInit = false;
        bool ssidFound = false;
        
        ESP8266();
        bool begin(uint8_t*, IPAddress);
        bool test();
        bool connectToWiFi();
        bool setStaticIP(byte, byte, byte, byte);
        bool setMACAddr(uint8_t*);
        String readResponse(String);
};

class ESP8266Client: public ESP8266{
    public:
    unsigned long start = 0;
    String data;
    bool dataNotRead = true;
    int numberOfClients = 0;
    String clientMessage;
    
    char read();
    String clientRecv();
    bool clientCheckForConnect();
    bool checkForMessage();
};

class ESP8266Server: public ESP8266{
    public:
    bool begin();
    bool setupTelnetServer();
    void sendData(String,int);    
};
    
