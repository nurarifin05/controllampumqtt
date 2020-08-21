#include <PubSubClient.h>

#include <WiFiClient.h>
#include <WiFi.h>




const char* ssid = "LAB-FIK";
const char* password = "UNN@RJ@Y@";

const char* mqtt_server = "202.154.58.125";
const char* mqtt_user = "labiot2019";
const char* mqtt_password = "labiot2019";

float temperature = 0;


WiFiClient espClient;
PubSubClient client(espClient);

const char ledhijau = 12;
const char ledmerah = 13;

#define TEMP_TOPIC    "floor1/temp"
#define LED_TOPIC     "mesin/hijau"
#define LED_TOPIC2    "mesin/merah"
#define LED_TOPIC3    "mesin/off"

long lastMsg = 0;
char msg[20];

void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);
 // Serial.print("]");
  String payloadtemp;

  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }/*for (int u =0; u < length; u--){
    Serial.print((char)payload[u]);
  }*/
  Serial.println();

  if((char)payload[0] =='h'){
    digitalWrite(ledhijau, HIGH);
    digitalWrite(ledmerah, LOW);
  }if((char)payload[0] == 'm'){
    digitalWrite(ledhijau, LOW);
    digitalWrite(ledmerah, HIGH);
  }

 /* if (payloadtemp == "i") {
    digitalWrite(led, HIGH); 
  } else /*if (payloadtemp == "off"){

    digitalWrite(led, LOW);}*/
  
}


void mqttconnect() {

  while (!client.connected()) {
    Serial.print("MQTT connecting ...");

    String clientId = "ESP32Client";

    if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");

      client.subscribe(LED_TOPIC);
      client.subscribe(LED_TOPIC2);
      client.subscribe(LED_TOPIC3);
    } /*else if (client.connect(clientId.c_str(), mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(LED_TOPIC2);
      }*/else{
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");

      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }


  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  client.setServer(mqtt_server, 1883);

  client.setCallback(receivedCallback);
}
void loop() {

  if (!client.connected()) {
    mqttconnect();
  }

  client.loop();

  long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;
    temperature = random(0, 40);
    if (!isnan(temperature)) {
      snprintf (msg, 20, "%lf", temperature);
   
      client.publish(TEMP_TOPIC, msg);
    }
  }
}
