// Klient
#include "heltec.h"

#define BAND    868E6 

String outgoing;              // outgoing message

byte localAddress = 0xAF;     // localAddr
byte destination = 0xFD;      // DestAddr

byte msgCount = 0;            // count of outgoing messages
unsigned long lastSendTime = 0;        // last send time
unsigned long interval = 12000;          // interval between sends

//main data

String shopNum = "R123";
//String gateNum = "1";
String alarmStatus = "0";
String alarmStatusOld = "0";
String powerStatus = "1";

int bufN[10];

struct LoRaP{
  byte dst;
  byte local;
  byte msgID;
  String msg;
};

unsigned int response; // lora response
unsigned int retryNum=0;

const unsigned int output36 = 23;
const unsigned int output37 = 17;

String relayStatus1="";
String relayStatus2="";

const unsigned int input38 = 38;
const unsigned int input39 = 39;

const unsigned long statusInterval=20000;

// Current time
unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const unsigned long timeoutTime = 3000; //def 2000

unsigned int sensorVal=0;

struct LoRaP l0,l1,l2,l3,l4,l5; // lora protocol

unsigned long nn=0;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  mazafaka();
  pinMode(output36, OUTPUT);pinMode(output37, OUTPUT);pinMode(input38, INPUT);pinMode(input39, INPUT);
  //digitalWrite(output36, LOW);//digitalWrite(output37, LOW);
  digitalWrite(output36, HIGH);digitalWrite(output37, HIGH);
  relayStatus1="0";relayStatus2="0";

}

void loop(){

  //azoka pins read
  sensorVal = analogRead(input38);
  if ( (nn % 1000) ==0) {Serial.println("input38 = "+ String(sensorVal) ); nn++;}
  //if (sensorVal == LOW) {alarmStatus = "NO"; } else {alarmStatus = "YES"; }
  if (sensorVal <=2048) {alarmStatus = "0"; } else {alarmStatus = "1"; }
  //Serial.println("alarmStatus: "+ alarmStatus);
  
  
  if (alarmStatusOld != alarmStatus ) { alarmStatusOld=alarmStatus; sendLoraStatus(1);} else { 
      sendLoraStatus(2);
      //Serial.println("No action - no Lora packet send....with time "); 
      }
      
  onReceive(LoRa.parsePacket());
  
}

void sendLoraStatus(unsigned int a){
  long interval1;
  if (a==2) {interval1= interval+5000; }
  if (a==1) {interval1= interval; }
  //Serial.println("sendLoraStatus interval : a= " +String(a));
  //Serial.println("sendLoraStatus: interval1= " +String(interval1));
  //Serial.println("sendLoraStatus: interval= " +String(interval));
  
  if ( ( (millis() - lastSendTime > interval1) &&  (a==1) )  || (a==2) && ( millis()- lastSendTime > statusInterval) ) 
  {
    //check status

    //Serial.println("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    //Serial.println("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    //Serial.println("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    //Serial.println("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
    
    String message = "Input38 Alarm status: " + alarmStatus +" "+ relayStatus1 +" "+ relayStatus2;   //msg
    
    //azoka display
    Heltec.display->clear(); Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT); 
    Heltec.display->setFont(ArialMT_Plain_10);
    Heltec.display->drawString(0, 10, "Laddr: 0x"+ String(localAddress, HEX) );
    Heltec.display->drawString(0, 20, "Daddr: 0x"+ String(destination, HEX) );  
    Heltec.display->display();
    //end azoka display
    
    sendMessage(message);
    Serial.println(String(lastSendTime)+ ": Sending to 0x" +String(destination, HEX) +" " + message);
    lastSendTime = millis();           
    interval = random(2000) + 2000;
  }
  
}

void mazafaka(){
  //WIFI Kit series V1 not support Vext control
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Enable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  //Serial.println("Hello");
}

void sendMessage(String outgoing){
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID

  bool a = savebuf();

  displaywarning();

  savebuf();
  
}

bool savebuf(){
  
  //if (LoRa1 > LoRa) { Serial.println("LoRa1=LoRa!!");}
  
  //Serial.println(LoRa1);
   
  //LoRa.onTxDone(onTxDone);
  //LoRa.onTxDone(onTxDone);
  
  //LoRaClass LoRa1; work ver

  //LoRa1 = LoRa; work ver

  if (retryNum==0){
    l0.dst = destination;
    l0.local = localAddress;
    l0.msgID= msgCount;
    l0.msg = outgoing;
    bufN[0] = 1;
    retryNum++;
  }
  return 1;
}

void displaywarning(){
  if (localAddress!= 0xFD){
  Heltec.display->drawString(20, 30, "!!!WARNING!!!");
  Heltec.display->drawString(20, 40, "!HIGH VOLTAGE!" );    
  Heltec.display->drawString(0, 50, "SendMsg : "+ String(msgCount) );  
  Heltec.display->display();
  }
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available()) { incoming += (char)LoRa.read(); }

  if (incomingLength != incoming.length())
  {   // check length for error
    Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message have other addressAZOKA.");
    Serial.println("Recipient= "+ recipient);
    Serial.println("Localaddress= " + localAddress);
    
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


  //if ( ((sender, HEX) == destination) && ( (recipient, HEX) == localAddress) ){
  //   Serial.println("Received from : "+ destination);
  //   Serial.println("Message: " + incoming);
  //}
  if ( String(sender, HEX)="0xfd") {
  Serial.println("JOOOOOOOO");
  Serial.println("Recipient:" + String(recipient, HEX) );
  Serial.println("Recipient:" + String(recipient) );
    
  Serial.println(String(incoming));

//AA
    if ( ( String(incoming)== "360") && ( recipient==170 ) ){ digitalWrite(output36, HIGH); relayStatus1="0"; }
    if ( ( String(incoming)== "361") && ( recipient==170 ) ){ digitalWrite(output36, LOW); relayStatus1="1"; }
    if ( ( String(incoming)== "370") && ( recipient==170 ) ){ digitalWrite(output37, HIGH); relayStatus2="0"; }
    if ( ( String(incoming)== "371") && ( recipient==170 ) ){ digitalWrite(output37, LOW); relayStatus2="1"; }

//AB
    if ( ( String(incoming)== "460") && ( recipient==171 ) ){ digitalWrite(output36, HIGH); relayStatus1="0"; }
    if ( ( String(incoming)== "461") && ( recipient==171 ) ){ digitalWrite(output36, LOW); relayStatus1="1"; }
    if ( ( String(incoming)== "470") && ( recipient==171 ) ){ digitalWrite(output37, HIGH); relayStatus2="0"; }
    if ( ( String(incoming)== "471") && ( recipient==171 ) ){ digitalWrite(output37, LOW); relayStatus2="1"; }

//AC
    if ( ( String(incoming)== "560") && ( recipient==172 ) ){ digitalWrite(output36, HIGH); relayStatus1="0"; }
    if ( ( String(incoming)== "561") && ( recipient==172 ) ){ digitalWrite(output36, LOW); relayStatus1="1"; }
    if ( ( String(incoming)== "570") && ( recipient==172 ) ){ digitalWrite(output37, HIGH); relayStatus2="0"; }
    if ( ( String(incoming)== "571") && ( recipient==172 ) ){ digitalWrite(output37, LOW); relayStatus2="1"; }

//AD
    if ( ( String(incoming)== "660") && ( recipient==173 ) ){ digitalWrite(output36, HIGH); relayStatus1="0"; }
    if ( ( String(incoming)== "661") && ( recipient==173 ) ){ digitalWrite(output36, LOW); relayStatus1="1"; }
    if ( ( String(incoming)== "670") && ( recipient==173 ) ){ digitalWrite(output37, HIGH); relayStatus2="0"; }
    if ( ( String(incoming)== "671") && ( recipient==173 ) ){ digitalWrite(output37, LOW); relayStatus2="1"; }

//AE
    if ( ( String(incoming)== "760") && ( recipient==174 ) ){ digitalWrite(output36, HIGH); relayStatus1="0"; }
    if ( ( String(incoming)== "761") && ( recipient==174 ) ){ digitalWrite(output36, LOW); relayStatus1="1"; }
    if ( ( String(incoming)== "770") && ( recipient==174 ) ){ digitalWrite(output37, HIGH); relayStatus2="0"; }
    if ( ( String(incoming)== "771") && ( recipient==174 ) ){ digitalWrite(output37, LOW); relayStatus2="1"; }
    
    //Serial.println(incoming[3]+incoming[2]+incoming[1]);
  }

  //azoka
  
  Heltec.display->drawString(0, 30, "MsgFrom 0x: "+ String(sender, HEX) );
  Heltec.display->drawString(0, 40, "MsgID : "+ String(incomingMsgId) );
   
}
