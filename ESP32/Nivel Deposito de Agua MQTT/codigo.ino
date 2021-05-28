#include <ESP8266WiFi.h> //Libreria para el wifi
#include <PubSubClient.h> //Libreria para MQTT by Nick O'Leary

//Datos del wifi
const char* ssid = "SSID";
const char* password =  "PASSWORD";

//Datos del broker MQTT
const char* mqtt_server = "test.mosquitto.org";

//Variables para MQTT
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];

//Variables para convertir long a string
char buffer[10]=" ";
char* formato="%i";


//Pines para el HC-SR04
#define echoPin D7 // Echo Pin
#define trigPin D6 // Trigger Pin
 
//Variables del HC-SR04
long tiempo, distancia;
 
void setup() {
  Serial.begin (9600);
  conectarWifi();

  //Pines del HC-SR04
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED,HIGH);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 2000) {
    distancia = medirDistancia();
    lastMsg = now;
    sprintf(buffer, formato, distancia);
    client.publish("Gasificador/DepositoAgua/Nivel2", buffer);
  }
}

long medirDistancia(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  tiempo = pulseIn(echoPin, HIGH);
  return tiempo / 58.2; //Distancia en cm
}

void conectarWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a:\t");
  Serial.println(ssid); 

  // Esperar a que nos conectemos
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(200);
    Serial.print('.');
  }
  // Mostrar mensaje de exito y dirección IP asignada
  Serial.println();
  Serial.print("Conectado a:\t");
  Serial.println(WiFi.SSID()); 
  Serial.print("Dirección IP:\t");
  Serial.println(WiFi.localIP());
}

//Función que se ejecuta cuando se recibe un dato de MQTT
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje Recibido: ");
  Serial.print(topic);
  Serial.print(" ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  //Verificamos si el topic recibido es alguno de los que estamos suscritos
  if(strcmp(topic,"Gasificador/DepositoAgua/Nivel") == 0){
    //Leemos el payload
    if ((char)payload[0] == '1') {
      digitalWrite(BUILTIN_LED, LOW); 
    } else {
      digitalWrite(BUILTIN_LED, HIGH);  
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Intentando conectar al broker MQTT...");
    // Intentar conectar
    if (client.connect("WemosD1MiniGasificadorDepositoAgua")) {
      Serial.println("Conectado al broker");
      //Una vez estamos conectados nos suscribimos a los topics
      client.subscribe("Gasificador/DepositoAgua/Nivel");
    } else {
      Serial.print("Error");
      Serial.print(client.state());
      Serial.println("Reintentar en 5 segundos");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
