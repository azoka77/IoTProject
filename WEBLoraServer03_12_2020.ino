//Server 
#include <WiFi.h>
#include "heltec.h"
#include <WiFiUdp.h>
#include <EEPROM.h> // for saving
#include <Base64.h> // for data encription

const PROGMEM uint16_t charSet[] = { 65000, 32796, 16843, 10, 11234};

#define BAND    868E6 

String outgoing;              // outgoing message

byte localAddress = 0xFD;     // localAddr
byte destination = 0xAC;      // DestAddr


byte msgCount = 0;            // count of msg
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends

//main data
const int maxdev= 10;

String shopNum = "XXXX";
//String gateNum = "1";
//String alarmStatus = "NO";
//String powerStatus = "YES";
char ac[maxdev];
const unsigned long webdatatime =60000;


const char* ssid = "xxx"; //wifi ssid
const char* password = "xxx"; //wifi password
const char * udpAddress = "x.x.x.x"; //ups address
const int udpPort = 3333;


//for time
long day = 86400000; // 86400000 milliseconds in a day
long hour = 3600000; // 3600000 milliseconds in an hour
long minute = 60000; // 60000 milliseconds in a minute
long second =  1000; // 1000 milliseconds in a second

boolean connected = false;
WiFiUDP udp;

WiFiServer server(80);

String header;


String output36State = "off";
String output37State = "off";
String output46State = "off";
String output47State = "off";
String output56State = "off";
String output57State = "off";
String output66State = "off";
String output67State = "off";
String output76State = "off";
String output77State = "off";


String reset36State = "off";

String lastoutput36State = "off"; //hz
String lastoutput37State = "off"; //hz
String lastoutput46State = "off"; //hz
String lastoutput47State = "off"; //hz
String lastoutput56State = "off"; //hz
String lastoutput57State = "off"; //hz
String lastoutput66State = "off"; //hz
String lastoutput67State = "off"; //hz
String lastoutput76State = "off"; //hz
String lastoutput77State = "off"; //hz

// time
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

class Gate{
public:
    Gate(int num){
    gateNum=String(num);
    alarmStatus="no data";
    powerStatus="no data";
    lastactive="no data";
    signalst="no data";
    relay1="no data";
    relay2="no data";
    }
    void cl(){
      alarmStatus="no data";
      powerStatus="no data";
      lastactive="no data";
      signalst="no data";
      relay1="no data";
      relay2="no data";
      
    }
    //void dot();
    //void dash();
  //protected:
    //int _pin;
    String alarmStatus;
    String powerStatus;
    String shopNum;
    String gateNum;
    String lastactive;
    String signalst;
    String relay1;
    String relay2;
};

Gate gate1(1);
Gate gate2(2);
Gate gate3(3);
Gate gate4(4);
Gate gate5(5);


void displaysettings(){
  //WIFI Kit series V1 not support Vext control
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Enable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT); 
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(0, 10, "Smart Gate Server v 1.0" );
  Heltec.display->drawString(0, 40, "!!!WARNING!!! " );
  Heltec.display->drawString(0, 50, "!HIGH VOLTAGE!" );
  //Heltec.display->drawString(0, 20, "Daddr: 0x"+ String(destination, HEX) );  
  Heltec.display->display();
}

void setup() {

  displaysettings();
  Serial.begin(115200);
  Serial.println("Smart Gate Server v 1.0 ");

  

  // wifi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // start web server
  Serial.println("Starting web server with IP..."); Serial.println("WiFi connected."); Serial.println("IP address: "); Serial.println(WiFi.localIP());
  server.begin();

  Serial.println(WiFi.localIP());
  
}

void loop(){

  checkdev();

  //String a = time();
  //Serial.println(a);

  //sendLora();
//AA
  if (( lastoutput36State== "off" )&& (output36State=="on" )){
    sendtogate(0xAA,"361");
    lastoutput36State=output36State;
  }

  if (( lastoutput36State== "on" )&& (output36State=="off" )){
    sendtogate(0xAA,"360");
    lastoutput36State=output36State;
  }
  
   if (( lastoutput37State== "off" )&& (output37State=="on" )){
    sendtogate(0xAA,"371");
    lastoutput37State=output37State;
  }

  if (( lastoutput37State== "on" )&& (output37State=="off" )){
    sendtogate(0xAA,"370");
    lastoutput37State=output37State;
  }
  


//AB
  if (( lastoutput46State== "off" )&& (output46State=="on" )){
    sendtogate(0xAB,"461");
    lastoutput46State=output46State;
  }

  if (( lastoutput46State== "on" )&& (output46State=="off" )){
    sendtogate(0xAB,"460");
    lastoutput46State=output46State;
  }
  
   if (( lastoutput47State== "off" )&& (output47State=="on" )){
    sendtogate(0xAB,"471");
    lastoutput47State=output47State;
  }

  if (( lastoutput47State== "on" )&& (output47State=="off" )){
    sendtogate(0xAB,"470");
    lastoutput47State=output47State;
  }

  
//AC
  if (( lastoutput56State== "off" )&& (output56State=="on" )){
    sendtogate(0xAC,"561");
    lastoutput56State=output56State;
  }

  if (( lastoutput56State== "on" )&& (output56State=="off" )){
    sendtogate(0xAC,"560");
    lastoutput56State=output56State;
  }
  
   if (( lastoutput57State== "off" )&& (output57State=="on" )){
    sendtogate(0xAC,"571");
    lastoutput57State=output57State;
  }

  if (( lastoutput57State== "on" )&& (output57State=="off" )){
    sendtogate(0xAC,"570");
    lastoutput57State=output57State;
  }

  
//AD
  if (( lastoutput66State== "off" )&& (output66State=="on" )){
    sendtogate(0xAD,"661");
    lastoutput66State=output66State;
  }

  if (( lastoutput66State== "on" )&& (output66State=="off" )){
    sendtogate(0xAD,"660");
    lastoutput66State=output66State;
  }
  
   if (( lastoutput67State== "off" )&& (output67State=="on" )){
    sendtogate(0xAD,"671");
    lastoutput67State=output67State;
  }

  if (( lastoutput67State== "on" )&& (output67State=="off" )){
    sendtogate(0xAD,"670");
    lastoutput67State=output67State;
  }

 
//AE
  if (( lastoutput76State== "off" )&& (output76State=="on" )){
    sendtogate(0xAE,"761");
    lastoutput76State=output76State;
  }

  if (( lastoutput76State== "on" )&& (output76State=="off" )){
    sendtogate(0xAE,"760");
    lastoutput76State=output76State;
  }
  
   if (( lastoutput77State== "off" )&& (output77State=="on" )){
    sendtogate(0xAE,"771");
    lastoutput77State=output77State;
  }

  if (( lastoutput77State== "on" )&& (output77State=="off" )){
    sendtogate(0xAE,"770");
    lastoutput77State=output77State;
  }


  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());

  webinterface();
  
  
  
}

void checkdev(){

  long temp;
  temp = gate1.lastactive.toInt();
  //Serial.println("Web data time: " + String (millis() - temp*1000));
  //Serial.println("gate1.lastactivity: " + String (temp) );
  
  if ( ( millis() - temp*1000 ) > webdatatime ) { gate1.cl(); }
  temp = gate2.lastactive.toInt();
  if ( ( millis() - temp*1000 ) > webdatatime ) { gate2.cl(); }
  temp = gate3.lastactive.toInt();
  if ( ( millis() - temp*1000 ) > webdatatime ) { gate3.cl(); }
  temp = gate4.lastactive.toInt();
  if ( ( millis() - temp*1000 ) > webdatatime ) { gate4.cl(); }
  temp = gate5.lastactive.toInt();
  if ( ( millis() - temp*1000 ) > webdatatime ) { gate5.cl(); }
  
}

void sendtogate(byte address, String msgtogate){
  //String message = "Msg to gate: "+ String(address);

  destination = address, HEX;
  Serial.println("Sending to gate: " + msgtogate);
  sendMessage(msgtogate);

}

void sendLora(){
   if (millis() - lastSendTime > interval)
  {
    String message = "Hello,new msg sended.. !";   // send a message
    //azoka

    Heltec.display->clear(); Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT); 
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 10, "Laddr: 0x"+ String(localAddress, HEX) );
    Heltec.display->drawString(0, 20, "Daddr: 0x"+ String(destination, HEX) );  
    Heltec.display->display();
    
    //when send a msg to client?
    int qw = 0;

    if (qw ==1) { 
    sendMessage(message);
    Serial.println("Sending " + message);
    }
    
    lastSendTime = millis();         
    interval = random(2000) + 1000;   
  }
}

void webinterface(){
  
  WiFiClient client = server.available();   // Listen for clients

  if (client) {                             
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New web Client.");         
    String currentLine = "";                
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
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /36/on") >= 0) {
              Serial.println("GPIO 36 on");
              output36State = "on";
              
            } else if (header.indexOf("GET /36/off") >= 0) {
              Serial.println("GPIO 36 off");
              output36State = "off";
              
            } else if (header.indexOf("GET /37/on") >= 0) {
              Serial.println("GPIO 37 on");
              output37State = "on";
              
            } else if (header.indexOf("GET /37/off") >= 0) {
              Serial.println("GPIO 37 off");
              output37State = "off";
              
            }

            //azoka

            if (header.indexOf("GET /46/on") >= 0) {
              Serial.println("GPIO 46 on");
              output46State = "on";
              
            } else if (header.indexOf("GET /46/off") >= 0) {
              Serial.println("GPIO 46 off");
              output46State = "off";
              
            } else if (header.indexOf("GET /47/on") >= 0) {
              Serial.println("GPIO 47 on");
              output47State = "on";
              
            } else if (header.indexOf("GET /47/off") >= 0) {
              Serial.println("GPIO 47 off");
              output47State = "off";
              
            }


               if (header.indexOf("GET /56/on") >= 0) {
              Serial.println("GPIO 56 on");
              output56State = "on";
              
            } else if (header.indexOf("GET /56/off") >= 0) {
              Serial.println("GPIO 56 off");
              output56State = "off";
              
            } else if (header.indexOf("GET /57/on") >= 0) {
              Serial.println("GPIO 57 on");
              output57State = "on";
              
            } else if (header.indexOf("GET /57/off") >= 0) {
              Serial.println("GPIO 57 off");
              output57State = "off";
              
            }

                  if (header.indexOf("GET /66/on") >= 0) {
              Serial.println("GPIO 66 on");
              output66State = "on";
              
            } else if (header.indexOf("GET /66/off") >= 0) {
              Serial.println("GPIO 66 off");
              output66State = "off";
              
            } else if (header.indexOf("GET /67/on") >= 0) {
              Serial.println("GPIO 67 on");
              output67State = "on";
              
            } else if (header.indexOf("GET /67/off") >= 0) {
              Serial.println("GPIO 67 off");
              output67State = "off";
              
            }


            
            //azoka end

            
            
            // html
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center; background-color:powderblue;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 4px 10px; border-radius: 6px;");
            
            
            //client.println("<body style="background-color:powderblue;">");
            
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");

            //table
            client.println(".table th, td { border: 1px solid black; border-collapse: collapse; }");

            
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h2>SMART GATE V 1.0 Server </h2>");

            //String zx="asd1";
            //String zx1="asd2";
            //String time2= gate1.lastactive;

            client.println("<h4>Server Uptime:" + String(millis()/3600000) +"h " + String(millis()/60000)+"m "+ String(millis()/1000) +" sec " + "</h4>");

            client.println("<body><h7>Active devices founded:</h7>");



            if (reset36State=="off") { // If the output36State is off, it displays the ON button 
              client.println("<td><a href=\"/r36/on\"><button class=\"button\">Refresh</button></a></td>");
            } else {client.println("<td><a href=\"/r36/off\"><button class=\"button button2\">Refresh</button></a></td>"); } 

            
            //client.println("<body><h4>Gate1 time : " + gate1.lastactive);client.println("</h4>");

            
            //client.println("<body><h4>Gate2 time : " + gate2.lastactive);client.println("</h4>");

            
            //client.println("<body><h4>Gate3 time : " + gate3.lastactive);client.println("</h4>");

            
            //client.println("<body><h4>Gate4 time : " + gate4.lastactive);client.println("</h4>");


            client.println("<table style='width:100%'>");
            client.println("<tr>");
            client.println("<th>Device</th>");
            client.println("<th>Pwr</th>");
            client.println("<th>Alarm</th>");
            client.println("<th>Relay1</th>");
            client.println("<th>Relay2</th>");
            client.println("<th>Activity</th>");
            client.println("<th>Signal</th>");
            
            client.println("</tr>");
            
            client.println("<tr>");
            client.println("<td>" + gate1.gateNum + "</td>");
            client.println("<td>" + gate1.powerStatus + "</td>");
            client.println("<td>" + gate1.alarmStatus + "</td>");
            client.println("<td>" + gate1.relay1 + "</td>");
            client.println("<td>" + gate1.relay2 + "</td>");
            client.println("<td>" + gate1.lastactive + "</td>");
            client.println("<td>" + gate1.signalst + "</td>");
            
            client.println("</tr>");

            
            client.println("<tr>");
            client.println("<td>" + gate2.gateNum + "</td>");
            client.println("<td>" + gate2.powerStatus + "</td>");
            client.println("<td>" + gate2.alarmStatus + "</td>");
            client.println("<td>" + gate2.relay1 + "</td>");
            client.println("<td>" + gate2.relay2 + "</td>");
            client.println("<td>" + gate2.lastactive + "</td>");
            client.println("<td>" + gate2.signalst + "</td>");
            
            
            client.println("</tr>");

            client.println("<tr>");
            client.println("<td>" + gate3.gateNum + "</td>");
            client.println("<td>" + gate3.powerStatus + "</td>");
            client.println("<td>" + gate3.alarmStatus + "</td>");
            client.println("<td>" + gate3.relay1 + "</td>");
            client.println("<td>" + gate3.relay2 + "</td>");
            client.println("<td>" + gate3.lastactive + "</td>");
            client.println("<td>" + gate3.signalst + "</td>");
            
            
            client.println("</tr>");

            client.println("<tr>");
            client.println("<td>" + gate4.gateNum + "</td>");
            client.println("<td>" + gate4.powerStatus + "</td>");
            client.println("<td>" + gate4.alarmStatus + "</td>");
            client.println("<td>" + gate4.relay1 + "</td>");
            client.println("<td>" + gate4.relay2 + "</td>");
            client.println("<td>" + gate4.lastactive + "</td>");
            client.println("<td>" + gate4.signalst + "</td>");
            
            
            client.println("</tr>");

            client.println("<tr>");
            client.println("<td>" + gate5.gateNum + "</td>");
            client.println("<td>" + gate5.powerStatus + "</td>");
            client.println("<td>" + gate5.alarmStatus + "</td>");
            client.println("<td>" + gate5.relay1 + "</td>");
            client.println("<td>" + gate5.relay2 + "</td>");
            client.println("<td>" + gate5.lastactive + "</td>");
            client.println("<td>" + gate5.signalst + "</td>");
            
            
            client.println("</tr>");

            
            
            
            
            client.println("</table>");
            

            client.println("<h3>Relay settings:</h3>");
            if (reset36State=="off") { // If the output36State is off, it displays the ON button 
              client.println("<td><a href=\"/r36/on\"><button class=\"button\">Refresh</button></a></td>");
            } else {client.println("<td><a href=\"/r36/off\"><button class=\"button button2\">Refresh</button></a></td>"); }


            

            client.println("<table style='width:100%'>");
            
            client.println("<tr>");
            client.println("<th>Device</th>");
            client.println("<th>Relay1</th>");
            client.println("<th>Relay2</th>");
            client.println("<th>Change relay1</th>");
            client.println("<th>Change relay2</th>");
            client.println("</tr>");

            client.println("<tr>");
            client.println("<td>" + gate1.gateNum + "</td>");
            client.println("<td>" + gate1.relay1 + "</td>");
            client.println("<td>" + gate1.relay2 + "</td>");
            if (output36State=="off") { // If the output36State is off, it displays the ON button 
              client.println("<td><a href=\"/36/on\"><button class=\"button\">ON</button></a></td>");
            } else {client.println("<td><a href=\"/36/off\"><button class=\"button button2\">OFF</button></a></td>"); } 

            if (output37State=="off") { // If the output37State is off, it displays the ON button 
              client.println("<td><a href=\"/37/on\"><button class=\"button\">ON</button></a></td>");
            } else {client.println("<td><a href=\"/37/off\"><button class=\"button button2\">OFF</button></a></td>"); }

            client.println("</tr>");


            client.println("<tr>");
            client.println("<td>" + gate2.gateNum + "</td>");
            client.println("<td>" + gate2.relay1 + "</td>");
            client.println("<td>" + gate2.relay2 + "</td>");

            if (output46State=="off") { // If the output46State is off, it displays the ON button 
              client.println("<td><a href=\"/46/on\"><button class=\"button\">ON</button></a></td>");
            } else {client.println("<td><a href=\"/46/off\"><button class=\"button button2\">OFF</button></a></td>"); } 
            

            if (output47State=="off") { // If the output47State is off, it displays the ON button 
              client.println("<td><a href=\"/47/on\"><button class=\"button\">ON</button></a></td>");
            } else {client.println("<td><a href=\"/47/off\"><button class=\"button button2\">OFF</button></a></td>"); }

            client.println("</tr>");


            client.println("<tr>");
            client.println("<td>" + gate3.gateNum + "</td>");
            client.println("<td>" + gate3.relay1 + "</td>");
            client.println("<td>" + gate3.relay2 + "</td>");

            if (output56State=="off") { // If the output56State is off, it displays the ON button 
              client.println("<td><a href=\"/56/on\"><button class=\"button\">ON</button></a></td>");
            } else {client.println("<td><a href=\"/56/off\"><button class=\"button button2\">OFF</button></a></td>"); } 
            

            if (output57State=="off") { // If the output57State is off, it displays the ON button 
              client.println("<td><a href=\"/57/on\"><button class=\"button\">ON</button></a></td>");
            } else {client.println("<td><a href=\"/57/off\"><button class=\"button button2\">OFF</button></a></td>"); }


            client.println("</tr>");




             client.println("<tr>");
            client.println("<td>" + gate4.gateNum + "</td>");
            client.println("<td>" + gate4.relay1 + "</td>");
            client.println("<td>" + gate4.relay2 + "</td>");

            if (output66State=="off") { // If the output66State is off, it displays the ON button 
              client.println("<td><a href=\"/66/on\"><button class=\"button\">ON</button></a></td>");
            } else {client.println("<td><a href=\"/66/off\"><button class=\"button button2\">OFF</button></a></td>"); } 
            

            if (output67State=="off") { // If the output67State is off, it displays the ON button 
              client.println("<td><a href=\"/67/on\"><button class=\"button\">ON</button></a></td>");
            } else {client.println("<td><a href=\"/67/off\"><button class=\"button button2\">OFF</button></a></td>"); }


            client.println("</tr>");


             client.println("<tr>");
            client.println("<td>" + gate5.gateNum + "</td>");
            client.println("<td>" + gate5.relay1 + "</td>");
            client.println("<td>" + gate5.relay2 + "</td>");

            if (output76State=="off") { // If the output66State is off, it displays the ON button 
              client.println("<td><a href=\"/76/on\"><button class=\"button\">ON</button></a></td>");
            } else {client.println("<td><a href=\"/76/off\"><button class=\"button button2\">OFF</button></a></td>"); } 
            

            if (output77State=="off") { // If the output67State is off, it displays the ON button 
              client.println("<td><a href=\"/77/on\"><button class=\"button\">ON</button></a></td>");
            } else {client.println("<td><a href=\"/77/off\"><button class=\"button button2\">OFF</button></a></td>"); }


            client.println("</tr>");



            client.println("</table>");



            client.println("<h3>SMART GATE V 1.0 2020</h3>");
             
            //client.println("<p>Gate 1 sound " + output36State + "</p>"); // Display current state, and ON/OFF buttons for GPIO 36 
                  
            //if (output36State=="off") { // If the output36State is off, it displays the ON button 
            //  client.println("<p><a href=\"/36/on\"><button class=\"button\">ON</button></a></p>");
            //} else {
            //  client.println("<p><a href=\"/36/off\"><button class=\"button button2\">OFF</button></a></p>");
            //} 
  
              
            //client.println("<p>Gate 1 power " + output37State + "</p>"); // Display current state, and ON/OFF buttons for GPIO 37
                  
            //if (output37State=="off") { // If the output37State is off, it displays the ON button 
            //  client.println("<p><a href=\"/37/on\"><button class=\"button\">ON</button></a></p>");
            //} else {
            //  client.println("<p><a href=\"/37/off\"><button class=\"button button2\">OFF</button></a></p>");
            //}





            //azoka
            //client.println("<p>Gate 1 Power Status " + powerStatus + "</p>");
            //client.println("<p>Gate 1 Alarm Status " + alarmStatus + "</p>");
            //client.println("<p>Gate 4 sound - ON" + output37State + "</p>");
            //client.println("<p>Gate 4 sound - ON" + output37State + "</p>");


            
            //client.println("<p>Alarm status: OFF" +  "</p>");
            //client.println("<p>UDP status: ON"  + "</p>");
            
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void sendMessage(String outgoing)
{
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
  
  Serial.println("Lora sendMessage: " + String(msgCount) );
  Serial.println("Lora outgoing: " + outgoing );
  

  if (localAddress!= 0xFD){
  Heltec.display->drawString(0, 50, "SendMsg : "+ String(msgCount) );  
  Heltec.display->display();
  }
  
}

void onReceive(int packetSize)
{
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available())
  {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length())
  {   // check length for error
    Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }

  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();


  //Serial.println("INC msg:");
  //Serial.println("azoka: " + incoming);
  //Serial.println("char23: " +String(incoming[22])+ " " + String(incoming[23]));

  switch (sender){
    case 170:

    gate1.lastactive = millis()/1000;
    if ( String(incoming[22])=="1" ) {gate1.alarmStatus = "1"; Serial.println("gate1.Alarm status: YES, Sending UDP function"); UDPsend(1); }
    if ( String(incoming[22])=="0" ) {gate1.alarmStatus = "0"; Serial.println("gate1.Alarm status: NO");}
    gate1.signalst = String(LoRa.packetRssi());
    if ( String(incoming[24])=="1" ) {gate1.relay1 = "1"; Serial.println("gate1.relay1 status: YES, Sending UDP function"); UDPsend(1); }
    if ( String(incoming[24])=="0" ) {gate1.relay1 = "0"; Serial.println("gate1.relay1 status: NO");}

    if ( String(incoming[26])=="1" ) {gate1.relay2 = "1"; Serial.println("gate1.relay2 status: YES, Sending UDP function"); UDPsend(1); }
    if ( String(incoming[26])=="0" ) {gate1.relay2 = "0"; Serial.println("gate1.relay2 status: NO");}

    if ( String(incoming[27])=="1" ) {gate1.powerStatus = "1"; Serial.println("gate1.powerStatus: YES, Sending UDP function"); UDPsend(1); }
    if ( String(incoming[27])=="0" ) {gate1.powerStatus = "0"; Serial.println("gate1.powerStatus: NO");}
    
    
    break;

    case 171:
    
    gate2.lastactive = millis()/1000;
    if ( String(incoming[22])=="1" ) {gate2.alarmStatus = "1"; Serial.println("gate2.Alarm status: YES");UDPsend(2);}
    if ( String(incoming[22])=="0" ) {gate2.alarmStatus = "0"; Serial.println("gate2.Alarm status: NO");}
    gate2.signalst = String(LoRa.packetRssi());
    if ( String(incoming[24])=="1" ) {gate2.relay1 = "1"; Serial.println("gate2.relay1 status: YES");UDPsend(2);}
    if ( String(incoming[24])=="0" ) {gate2.relay1 = "0"; Serial.println("gate2.relay1 status: NO");}

    if ( String(incoming[26])=="1" ) {gate2.relay2 = "1"; Serial.println("gate2.relay2 status: YES");UDPsend(2);}
    if ( String(incoming[26])=="0" ) {gate2.relay2 = "0"; Serial.println("gate2.relay2 status: NO");}

    if ( String(incoming[27])=="1" ) {gate2.powerStatus = "1"; Serial.println("gate2.powerStatus: YES, Sending UDP function"); UDPsend(2); }
    if ( String(incoming[27])=="0" ) {gate2.powerStatus = "0"; Serial.println("gate2.powerStatus: NO");}
    
    break;

    case 172:

    gate3.lastactive = millis()/1000;
    if ( String(incoming[22])=="1" ) {gate3.alarmStatus = "1"; Serial.println("gate3.Alarm status: YES"); UDPsend(3); }
    if ( String(incoming[22])=="0" ) {gate3.alarmStatus = "0"; Serial.println("gate3.Alarm status: NO");}
    gate3.signalst = String(LoRa.packetRssi());
    if ( String(incoming[24])=="1" ) {gate3.relay1 = "1"; Serial.println("gate3.relay1 status: YES");UDPsend(3);}
    if ( String(incoming[24])=="0" ) {gate3.relay1 = "0"; Serial.println("gate3.relay1 status: NO");}

    if ( String(incoming[26])=="1" ) {gate3.relay2 = "1"; Serial.println("gate3.relay2 status: YES");UDPsend(3);}
    if ( String(incoming[26])=="0" ) {gate3.relay2 = "0"; Serial.println("gate3.relay2 status: NO");}

    if ( String(incoming[27])=="1" ) {gate3.powerStatus = "1"; Serial.println("gate3.powerStatus: YES, Sending UDP function"); UDPsend(3); }
    if ( String(incoming[27])=="0" ) {gate3.powerStatus = "0"; Serial.println("gate3.powerStatus: NO");}
    
    break;


    case 173:

    gate4.lastactive = millis()/1000;
    if ( String(incoming[22])=="1" ) {gate4.alarmStatus = "1"; Serial.println("gate4.Alarm status: YES"); UDPsend(4);}
    if ( String(incoming[22])=="0" ) {gate4.alarmStatus = "0"; Serial.println("gate4.Alarm status: NO");}
    gate4.signalst = String(LoRa.packetRssi());
    if ( String(incoming[24])=="1" ) {gate4.relay1 = "1"; Serial.println("gate4.relay1 status: YES"); UDPsend(4);}
    if ( String(incoming[24])=="0" ) {gate4.relay1 = "0"; Serial.println("gate4.relay1 status: NO");}

    if ( String(incoming[26])=="1" ) {gate4.relay2 = "1"; Serial.println("gate4.relay2 status: YES"); UDPsend(4);}
    if ( String(incoming[26])=="0" ) {gate4.relay2 = "0"; Serial.println("gate4.relay2 status: NO");}

    if ( String(incoming[27])=="1" ) {gate4.powerStatus = "1"; Serial.println("gate4.powerStatus: YES, Sending UDP function"); UDPsend(4); }
    if ( String(incoming[27])=="0" ) {gate4.powerStatus = "0"; Serial.println("gate4.powerStatus: NO");}
    
    break;

    case 174:

    gate5.lastactive = millis()/1000;
    if ( String(incoming[22])=="1" ) {gate5.alarmStatus = "1"; Serial.println("gate5.Alarm status: YES"); UDPsend(5); }
    if ( String(incoming[22])=="0" ) {gate5.alarmStatus = "0"; Serial.println("gate5.Alarm status: NO");}
    gate5.signalst = String(LoRa.packetRssi());
    if ( String(incoming[24])=="1" ) {gate5.relay1 = "1"; Serial.println("gate5.relay1 status: YES"); UDPsend(5); }
    if ( String(incoming[24])=="0" ) {gate5.relay1 = "0"; Serial.println("gate5.relay1 status: NO");}

    if ( String(incoming[26])=="1" ) {gate5.relay2 = "1"; Serial.println("gate5.relay2 status: YES"); UDPsend(5); }
    if ( String(incoming[26])=="0" ) {gate5.relay2 = "0"; Serial.println("gate5.relay2 status: NO");}

    if ( String(incoming[27])=="1" ) {gate5.powerStatus = "1"; Serial.println("gate5.powerStatus: YES, Sending UDP function"); UDPsend(5); }
    if ( String(incoming[27])=="0" ) {gate5.powerStatus = "0"; Serial.println("gate5.powerStatus: NO");}
    
    break;
    
    
    default:
    
    break;
  }
  
  //if ( String(incoming[22])=="1" ) {alarmStatus = "YES"; Serial.println("Alarm status: YES");}
  //if ( String(incoming[22])=="0" ) {alarmStatus = "NO"; Serial.println("Alarm status: NO");}
  //Serial.println("azoka: " + String(incoming[22]));

  //azoka
  Heltec.display->drawString(0, 30, "MsgFrom 0x: "+ String(sender, HEX) );
  Heltec.display->drawString(0, 40, "MsgID : "+ String(incomingMsgId) ); 
  
}

void UDPsend(byte num){
   //UDP send

  Serial.println("Connection status : "+ String(connected));
  Serial.println("udpAddress: " + String(udpAddress) + "udpPort: "+ String(udpPort));
  //if(connected){
    //Send a packet
    udp.beginPacket(udpAddress,udpPort);
    String qw = gate1.alarmStatus;
    //udp.printf("GateN alarm status, num= ", String(num));
    Serial.println("UDP SEND PACKET!!! to addr");
    //udp.printf(qw);
    //shopNum= shopNum+"01234";
    
    //udp.printf("Seconds since boot: %lu", millis()/1000, shopNum,qw);
    //udp.printf(shopNum);
    
    //udp.write(12345678);
    String p="";

    
    p=shopNum[0]+shopNum[1]+shopNum[2]+shopNum[3];

    if (num==1) { p+="01"+gate1.alarmStatus+gate1.powerStatus+gate1.relay1+gate1.relay2; }
    
    if (num==2) { p+="02"+gate2.alarmStatus+gate2.powerStatus+gate2.relay1+gate2.relay2; }
    
    if (num==3) { p+="03"+gate3.alarmStatus+gate3.powerStatus+gate3.relay1+gate3.relay2; }
    
    if (num==4) { p+="04"+gate4.alarmStatus+gate4.powerStatus+gate4.relay1+gate4.relay2; }
    
    if (num==5) { p+="05"+gate5.alarmStatus+gate5.powerStatus+gate5.relay1+gate5.relay2; }
    
    
    //udp.printf(12345678);
    udp.print(p);
    //char asd[4] = p[0]+p[1]+p[2]+p[3];    
    //udp.printf(asd);
    
    //udp.printf("ShopNum: %lu" + shopNum );
    //udp.printf("GateNum:" + gateNum);
    //udp.printf("AlarmStatus:" + AlarmStatus);
    //udp.printf("PowerStatus:");
    
    udp.endPacket();
  //} //end if connected
  //Wait for 1 second
  //delay(1000);
 

  //end azoka
  
}

void time1(){
  
long timeNow = millis();
 
int days = timeNow / day ;                                
int hours = (timeNow % day) / hour;
int minutes = ((timeNow % day) % hour) / minute ;
int seconds = (((timeNow % day) % hour) % minute) / second;

Serial.println("Time now: ");
Serial.println(days,DEC);
Serial.print(hours);
Serial.print(minutes);
Serial.print(seconds);
}
