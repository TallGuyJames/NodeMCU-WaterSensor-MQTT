#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Arduino.h>

#define WIFI_SSID        "BeOurGuest"
#define MQTT_SERVER       "10.76.127.20"
#define MQTT_PORT         1883
#define MQTT_USER         "esp"
#define MQTT_PASSWORD     "esp"

#define topic        "sensor/water/sunroom"

int sensorPin = A0;    // input 
int enable2 = 13;      // enable
int sensorValue2 = 0;  // state

char message_buff[100];
bool debug = true;

WiFiClient espClient;
PubSubClient client(espClient);


//Connecfion WiFi
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED) {
    delay(4000);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("=> IP address: ");
  Serial.print(WiFi.localIP());
  Serial.println("");
}


//Reconnexion
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed, Error: ");
      Serial.print(client.state());
      Serial.println(" Retrying MQTT connection in 5 seconds...");
      delay(5000);  // Wait 5 seconds before retrying
    }
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
  int i = 0;
  if ( debug ) {
    Serial.println("Message recu =>  topic: " + String(topic));
    Serial.print(" | longueur: " + String(length, DEC));
  }
  // create character buffer with ending null terminator (string)
  for (i = 0; i < length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';

  String msgString = String(message_buff);
  if ( debug ) {
    Serial.println("Payload: " + msgString);
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(enable2, OUTPUT);
  setup_wifi();                           
  client.setServer(MQTT_SERVER, MQTT_PORT); 
  client.setCallback(callback); 
}

  
void loop() {
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  
  delay(500);
  sensorValue2 = analogRead(sensorPin);
  if (sensorValue2<= 1000)
  {
    Serial.print("FLOOD ");
    client.publish(topic, "flood", true);
    
    digitalWrite(enable2, HIGH);
    }
    else
    
  {
    Serial.print("DRY ");
    client.publish(topic, "dry", true);
    digitalWrite(enable2, LOW); 
  }
  Serial.print(sensorValue2);
  Serial.println();
  delay(10000);
}
