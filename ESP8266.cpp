#include "ESP8266.h"
#include "string.h"
#include "EEPROM.h"

ESP8266::ESP8266(){
    Serial3.begin(115200);
}

bool ESP8266::begin(uint8_t *mac_addr, IPAddress local_ip){
    if(test())
        Serial.println("ESP8266 present");
    setStaticIP(local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
    setMACAddr(mac_addr);
    if(connectToWiFi()) 
        Serial.println("Connected to wifi"); 
}

bool ESP8266::test(){           //basic test message; use to see if ESP8266 is present and working properly
    Serial3.write("AT\r\n");
    response = readResponse("OK");
    if(response.indexOf("OK")!=-1)
        return true;
    else
        return false;
}


bool ESP8266::connectToWiFi(){
    String ssid,pass;
    char output[10];
    char output1[10];
    char c;
    int timesTried = 0 ;
    int messageLength = 0;
    Serial3.write("AT+CWMODE=1\r\n");
    response = readResponse("OK");
    while(timesTried < 3){
        Serial3.write("AT+CWJAP?\r\n");
        response = readResponse("OK");
        
        delay(100);
          messageLength = EEPROM.read(250);
          Serial.println(messageLength);
          for(int i =0;i<messageLength;i++){
               c = EEPROM.read(300+i);
              ssid += c;
                
            }
        ssid.remove(messageLength - 4);     //comment this if there is no trailing whitespace after string
        messageLength = EEPROM.read(251);
        Serial.println(messageLength);
          for(int i =0;i<messageLength;i++){
               c = EEPROM.read(400+i);
              pass += c;
                
            }
        pass.remove(messageLength - 4);     //comment this if there is no trailing whitespace after string

        if(response.indexOf(ssid)==-1){                                                   //check
            Serial3.write("AT+CWJAP=\"");
            Serial3.print(ssid);
            
            Serial3.write("\",\"");
            Serial3.print(pass);
           
            Serial3.write("\"\r\n");
            timesTried++;
            delay(5000);
            response = readResponse("WIFI CONNECTED");
            Serial.println(response);
            if(response.indexOf("WIFI CONNECTED")!=-1){
                return true;
            }
        }
        else
        {
           return true;
        }
    }
    return false;
    
}

bool ESP8266::setStaticIP(byte IP1, byte IP2, byte IP3, byte IP4){  //To change static IP
    Serial3.write("AT+CIPSTA=\"");
    Serial3.print(IP1);
    Serial3.write(".");
    Serial3.print(IP2);
    Serial3.write(".");
    Serial3.print(IP3);
    Serial3.write(".");
    Serial3.print(IP4);
    Serial3.write("\"\r\n");
    response = readResponse("OK");
    if(response.indexOf("OK")!=-1){
        Serial.println("IP address set to");
        Serial.print(IP1);
        Serial.print(".");
        Serial.print(IP2);
        Serial.print(".");
        Serial.print(IP3);
        Serial.print(".");
        Serial.println(IP4);
        return true;
    }    
    else
    {
        Serial.println("IP change failure");
        return false;
    }
}

bool ESP8266::setMACAddr(uint8_t *mac){     //to set MAC address
    Serial3.write("AT+CIPSTAMAC=\"");
    Serial3.print(mac[0]);
    Serial3.write(":");
    Serial3.print(mac[1]);
    Serial3.write(":");
    Serial3.print(mac[2]);
    Serial3.write(":");
    Serial3.print(mac[3]);
    Serial3.write(":");
    Serial3.print(mac[4]);
    Serial3.write(":");
    Serial3.print(mac[5]);
    Serial3.write("\"\r\n");
    response = readResponse("OK");
    if(response.indexOf("OK")!=-1){
        return true;
    }    
    else
    {
        return false;
    }
}


String ESP8266::readResponse(String target)         //Universal function to read any command response from ESP8266. 
{
    String data;
    char a;
    unsigned long start = millis();
    while (millis() - start < timeout) {
        while(Serial3.available() > 0) {
            a = Serial3.read();
            if(a == '\r') continue;
            data += a; 
        }
         if (data.indexOf(target) != -1) 
            break;
           
    }
    return data;

}

ESP8266 esp;




//
//Server Class Functions
//


bool ESP8266Server::begin(){
    
    setupTelnetServer();
    
}

bool ESP8266Server::setupTelnetServer(){            //Setting up Telnet server on Piston on port 23
    Serial.println("Setting up server...");
    Serial3.write("AT+CIPMUX=1\r\n");
    response = readResponse("OK");
    if(response.indexOf("OK")!= -1){
    Serial3.write("AT+CIPSERVER=1,23");
    Serial3.write("\r\n");
    response = readResponse("OK");
    if(response.indexOf("OK")==-1){
        Serial.println("Unable to create server");
        return false;
    }
    else{
        Serial.println("Server created at port 23");
        return true;
    }
    
    }
    else
        return false;
}



void ESP8266Server::sendData(String data,int channel){
    
      Serial3.write("AT+CIPSEND=");                                                     //Writing data to client
      Serial3.print(channel);
      Serial3.print(",");
      Serial3.print(data.length());                                                              
      Serial3.write("\r\n");
      delay(500);
      Serial3.print(data);
      response = readResponse("\r\n");
      Serial.println(response);
}



    
//
//Client Class functions
//




char ESP8266Client::read(){             //Not used; only for abstraction
    return Serial3.read();
}



bool ESP8266Client::clientCheckForConnect(){
    
    
    if( this->recvMode == false){
        
        if(Serial3.available()){
            response = readResponse("CONNECT");
            if(response.indexOf("CONNECT")!=-1){            //only on initial connect
                numberOfClients++;
                int pos = response.indexOf(',');
                response.remove(pos);
                this->channelID = response.toInt();    
                this->recvMode = true;
                Serial.println("Channel ID:");
                Serial.println(this->channelID);
                response = readResponse("z"); //read initial junk/startup data sent by client; replace argument with last byte of incoming data.
                Serial.println("Client Connected");
                return true;
            }
            return false;
        }
    }
    else
        return true;
    
}


bool ESP8266Client::checkForMessage(){          //Checks what type of message is incoming
    char a;
    
    
    if(this->recvMode == true && Serial3.available() && dataNotRead){     
        dataNotRead = false;
        this->data = "";
        this->start = millis();          //Starts timer to keep track of incoming message when main loop is running through other functions
        
    }
    
    
    if(this->recvMode == true && (millis()-this->start<200) && dataNotRead == false){
  
        //Reading data as long as Serial3 is available or until 200ms of total time has elapsed since the first byte
    
        while(Serial3.available()){
            
            
            a = Serial3.read();
            this->data +=a;     
        }
        
        return false;
    

    }
    
    if(this->recvMode == true){
        this->start = 0;
        dataNotRead = true;
        if(data.indexOf("CONNECT")!=-1){            //Checks for Secondary connection if any
            numberOfClients++;
            int pos = data.indexOf(',');                
            data.remove(pos);
            this->channelID = data.toInt();    
            this->recvMode = true;
            Serial.println("Channel ID:");
            Serial.println(this->channelID);
            response = readResponse("z"); //read initial junk/startup data sent by client; replace argument with last byte of incoming data.
            Serial.println("Client Connected");
            this->data = "";
            return checkForMessage();
        }
        
            else if(data.indexOf("CLOSED")!=-1){            //Checks for disconnection message
                numberOfClients--;
                if(numberOfClients==0)
                    this->recvMode = false;         
                Serial.println("Client disconnected");
                this->data = "";
                return false;
            }
        else if(this->data.indexOf("+IPD")!=-1){
                Serial.println(this->data);
                this->clientMessage = this->data;
                data.remove(0,7);
                //Serial.println(data);
                int pos = data.indexOf(',');
                data.remove(pos);
                this->channelID=data.toInt();           //Checks for machine message
                this->recvMode = true;
                Serial.println("Channel ID:");
                Serial.println(this->channelID);        //If client ID changes at any given point 
                this->data = "";
                return true;
        }
        
        else
            return false;
    }
   
}
String ESP8266Client::clientRecv() {            // Removes "+IPD" header from incoming packet
    int pos = this->clientMessage.indexOf(':');
    this->clientMessage.remove(0,pos+1);                                        
    return this->clientMessage;
}
