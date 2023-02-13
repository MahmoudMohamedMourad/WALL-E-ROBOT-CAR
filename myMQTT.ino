/////////////////////////////////
// Generated with a lot of love//
// with TUNIOT FOR ESP32     //
// Website: Easycoding.tn      //
/////////////////////////////////
include <WiFi.h>
include <elapsedMillis.h> //load the library
elapsedMillis timeElapsed;//Create an Instance
include <PubSubClient.h>git commit -m "first commit"
git branch -M main
git remote add origin https://github.com/MahmoudMohamedMourad/WALL-E-ROBOT-CAR.git
git push -u origin main

#define TRIG_PIN 23 // ESP32 pin GIOP23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 22 // ESP32 pin GIOP22 connected to Ultrasonic Sensor's ECHO pin
float duration_us, distance_cm;

boolean offline = false;

//----------------------------------------------
const int motorA1  = 32;  // IN3 Input of L298N
const int motorA2  = 33;  // IN1 Input of L298N
const int motorB1  = 14;  // IN2 Input of L298N
const int motorB2  = 12;  // IN4 Input of L298N
//----------------------------------------------


String  MYData;
WiFiClient espClient;
PubSubClient client(espClient);

void reconnectmqttserver() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("flutter_client");//generalTopic2
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

char msgmqtt[50];
void callback(char* topic, byte* payload, unsigned int length) {
  String MQTT_DATA = "";
  for (int i = 0; i < length; i++) {
    MQTT_DATA += (char)payload[i];
  }
  //Serial.println("New data Recived");
  Serial.println(MQTT_DATA);
  //Serial.println("From Topic");
  //Serial.println(topic);

  if (MQTT_DATA == "0")
  {
    offline = false;
    Stop();
  }
  else if (MQTT_DATA == "1")
  {
    offline = false;
    Stop();
    Forward(127);

  }
  else if (MQTT_DATA == "2")
  {
    offline = false;
    Stop();
    backword(127);
  }
  else if (MQTT_DATA == "4")
  {
    offline = false;
    Stop();
    right(100);
  }
  else if (MQTT_DATA == "3")
  {
    offline = false;
    Stop();
    left(100); 
  }

  else if (MQTT_DATA == "5")
  {
    offline = true;

  }
}

void setup()
{
  pinMode(TRIG_PIN, OUTPUT); // set arduino pin to output mode
  pinMode(ECHO_PIN, INPUT);  // set arduino pin to input mode

  MYData = "";

  Serial.begin(9600);

  WiFi.disconnect();
  Serial.println("Start");
  WiFi.begin("TE_F2EC42", "73199499");
  while ((!(WiFi.status() == WL_CONNECTED))) {
    delay(300);
  }
  Serial.println("Yor local IP is ");
  Serial.print((WiFi.localIP()));
  client.setServer("broker.emqx.io", 1883);
  client.setCallback(callback);

  //------------------------------------------------------
  // Let's set our pins
  pinMode(motorA1, OUTPUT); //right
  pinMode(motorA2, OUTPUT); //left
  pinMode(motorB1, OUTPUT); //right
  pinMode(motorB2, OUTPUT); //left


  ledcSetup(0 , 5000 , 8); //RIGHT BACK   IN3 Input of L298N
  ledcAttachPin(32 , 0);

  ledcSetup(1 , 5000 , 8); //LEFT FORWORD   IN1 Input of L298N
  ledcAttachPin(33 , 1);


  ledcSetup(2 , 5000 , 8); //LEFT BACK    IN2 Input of L298N
  ledcAttachPin(14 , 2);


  ledcSetup(3 , 5000 , 8); //RIGHT FORWARD   IN4 Input of L298N
  ledcAttachPin(12 , 3);
  //------------------------------------------------------
  pinMode(16, INPUT); //s1
  pinMode(17, INPUT); //s2
  pinMode(18, INPUT); //s3
}


void loop()
{

  if (!client.connected()) {
    reconnectmqttserver();
  }
  client.loop();

  



  if (offline == true)
  {
    if (timeElapsed > 10000)
  {
    Stop();
    delay(10);
    for (int i=0;i<10;i++)
    {
      MYData = PIR();
      snprintf (msgmqtt, 50, "%s", MYData);
      client.publish("flutter", msgmqtt);//generalTopic
      Serial.println(MYData);
      delay(1000);
    }
    timeElapsed = 0;              // reset the counter to 0 so the counting starts over...
  }
  
    int distance = getdistance();
    if ( distance < 30)
      distance = 10;

    if ( distance > 50 )
      distance = 50;

    int Speed = map (distance, 5, 50, 0, 127);

    Forward(Speed);//Forward(Speed);

    if (distance <= 10)
    {
      check();

    } 

    delay (10);
  }

}

void Forward(int SPEED)
{
  ledcWrite(1, SPEED);
  ledcWrite(3, SPEED);
}

void right(int SPEED)
{
  ledcWrite(3, SPEED);
  ledcWrite(2, SPEED);
}

void Stop()
{
  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
}

void backword(int SPEED)
{
  ledcWrite(0, SPEED);
  ledcWrite(2, SPEED);
}

void left(int SPEED)
{
  ledcWrite(1, SPEED);
  ledcWrite(0, SPEED);
}

int PIR()
{
  int result = 0;
  int s1 = digitalRead(16);
  int s2 = digitalRead(17);
  int s3 = digitalRead(18);
  if (s1 || s2 || s3)
  {
    result = 1;
  }
  else result = 0;
  return result;
}

//------------------------------------------------------------
void check()
{
  do
  {
    backword(255);
  } while (getdistance() < 50);


  Stop();
  delay(10);

  right(255);
  delay(200);
  do
  {
    right(255);
  } while (getdistance() < 50);

  Stop();
  delay(10);
}

int getdistance( )
{
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration_us = pulseIn(ECHO_PIN, HIGH);
  distance_cm = 0.017 * duration_us;
  return distance_cm ;
}
