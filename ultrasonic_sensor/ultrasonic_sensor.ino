#include <PubSubClient.h>
#include <WiFi.h>


const char* ssid = "ssid";
const char* password = "password";
const char* mqtt_server = ""; // Provide localhost name of the broker devices
const char* mqtt_user = ""; // Provide the username of the broker devices
const char* mqtt_pass = ""; // Provide the password of the broker devices
const int mqtt_port = 1883; // Provide the port number of the broker devices (1883, 8883)

const int trigPin = 33;
const int echoPin = 4;

//define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

long duration;
float distanceCm;
float distanceInch;

long lastMsg = 0;

String activation_alarm = "Off";


WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  setup_wifi();

  Serial.begin(115200); // Starts the serial communication
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  //startCameraServer();
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

void setup_wifi() 
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* message, unsigned int length) 
{
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }

  if (String(topic) == "alarm/activation"){
    if(messageTemp == "On") {
      activation_alarm = "On";
    }
    else {
      activation_alarm = "Off";
    }
  }
}

void reconnect() 
{
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Proximity alarm")){
      Serial.println("connected");
      // Subscribe
      client.subscribe("alarm/activation");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
  }
  
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  distanceCm = duration * SOUND_SPEED/2;
  
  // Convert to inches
  distanceInch = distanceCm * CM_TO_INCH;
  
  // Prints the distance in the Serial Monitor each 5 seconds
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);

  if(distanceCm<5.0 && activation_alarm == "On") {
    Serial.println("Alarm!!");
    client.publish("alarm/detection", "ALARM");
    delay(6000);
  }
  
  delay(1000);
}
