#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#define FULL_PIN A0
#define THREEFOURTH_PIN A1
#define HALF_PIN A2
#define QUARTER_PIN A3
#define INFLOW_PIN A4
#define SUMPEMPTY_PIN A5
#define MOTOR_PIN 6
#define FAULT_PIN 7

//display Pins
#define FULL_DISP_PIN 8
#define THREEFOURTH_DISP_PIN 7
#define HALF_DISP_PIN 5
#define QUARTER_DISP_PIN 3

/*
* Using SPI.
* keep away from the following pins
* MOSI 4
* MISO 12 
* SCK 13
* SS 10
* https://www.arduino.cc/en/Reference/SPI
*/


//tank level
String levelMessage = "";

//interrupt pin for full sense and switch off pump
int buttonInt = 0; //AKA digital pin 2

byte mac[] = { 0xDE, 0xAD, 0x09, 0xEF, 0xCE, 0xBF };
byte server[] = { 192, 168, 0, 90 }; //local Mosquitto broker
EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

boolean DEBUG_FLAG = false;


void callback(char* topic, byte* payload, unsigned int length) {
    payload[length] = '\0';
    String payloadStr = String((char*) payload);
    if(payloadStr=="ON")
    {
       debugln("Turn ON Pump");
       digitalWrite(MOTOR_PIN, HIGH);
    }
    if(payloadStr=="OFF")
    {
       debugln("Turn OFF Pump");
       digitalWrite(MOTOR_PIN, LOW);
     }
}


void setup()
{
  if(debug)
    Serial.begin(9600);
  
  pinMode(INFLOW_PIN,INPUT);
  pinMode(SUMPEMPTY_PIN,INPUT);

  pinMode(MOTOR_PIN,OUTPUT);
  pinMode(FAULT_PIN, OUTPUT);
  digitalWrite(FAULT_PIN, LOW);

  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);
  
  debugln("Initiating Ethernet Connection");
  if(Ethernet.begin(mac))
  {
    debugln("Ethernet connection successful");
    debugln("IP Address assigned via DHCP Server : ");
    String localIPsTR = Ethernet.localIP() + ".";
    debugln(localIPsTR);
  }
  else
  {
    debugln("Ethernet connection failed. Going into a endless loop...");
    for(;;){}
  }
 
  if(ethClient.connected())
    debugln("Ehternet Client is connected.");
  else
    debugln("Ethernet Client is disconnected.");
    
  connectToBroker();
  
  if(ethClient.connected())
    debugln("Ehternet Client is connected.");
  else
    debugln("Ethernet Client is disconnected.");
    
    
 attachInterrupt(buttonInt, interruptFalling, FALLING);
  
}//end setup
  
  
void loop()
{
  client.loop();
  checkLevel();
  delay(9000);
}//loop




  
 void connectToBroker()
 {
  debugln("Initiating connection to Broker.");
  if (client.connect("motorController")) {
    debugln("Connected to MQTT Broker");
    client.publish("/com/jmahendr/h1/utility/pump/controller","CONNECTED");
    client.subscribe("/com/jmahendr/h1/utility/pump/P1");
  }
  else
  {
    debugln("Could not connect to MBroker");
    digitalWrite(FAULT_PIN, HIGH);
  }
}//connect to broker






void checkLevel()
{
  String levelMessage = "0";
  int qtr, half, threeFourth, full = 0;
  
  qtr = analogRead(QUARTER_PIN);
  half = analogRead(HALF_PIN);
  threeFourth = analogRead(THREEFOURTH_PIN);
  full = analogRead(FULL_PIN);
  
  digitalWrite(QUARTER_DISP_PIN, LOW);
  digitalWrite(HALF_DISP_PIN, LOW);
  digitalWrite(THREEFOURTH_DISP_PIN, LOW);
  digitalWrite(FULL_DISP_PIN, LOW);
  
  if (digitalRead(QUARTER_PIN))
  {
    levelMessage = "25";
    debugln("Quarter");
    digitalWrite(QUARTER_DISP_PIN, HIGH);
  }
  
  if (digitalRead(HALF_PIN))
  {
    levelMessage = "50";
    debugln("Half");
    digitalWrite(HALF_DISP_PIN, HIGH);
  }
    
  if (digitalRead(THREEFOURTH_PIN))
  {
    levelMessage = "75";
    debugln("Three Fourth");
    digitalWrite(THREEFOURTH_DISP_PIN, HIGH);
  }
    
  if (digitalRead(FULL_PIN))
  {
    levelMessage = "100";
    debugln("Full");
    digitalWrite(FULL_DISP_PIN, HIGH);
  }
    
    
  debug("CheckLevel: ");
  debugln(levelMessage);
  publishMessage("/com/jmahendr/h1/utility/OHT", levelMessage);
  
  //publishMessage("/com/jmahendr/h1/utility/pump/controller", counterStr);

}//checkLevel




void interruptFalling()
{
  debugln("InterruptFalling: ");
  digitalWrite(MOTOR_PIN, LOW);
  publishMessage("/com/jmahendr/h1/utility/pump/P1", "OFF");
}//interrupt







void publishMessage(String topic, String message)
{
  char messageArray[10];
  message.toCharArray(messageArray, sizeof(messageArray));
  
  char topicArray[50];
  topic.toCharArray(topicArray, sizeof(topicArray));
  
  client.publish(topicArray, messageArray);
}//publishMessage


void debug(String msg)
{
  if(DEBUG_FLAG)
    Serial.print(msg);
}

void debugln(String msg)
{
  if(DEBUG_FLAG)
    Serial.println(msg);
}
