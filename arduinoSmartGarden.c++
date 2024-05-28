//Incluir bibliotecas
#include <DHTesp.h>
#include <EspMQTTClient.h>

#define AOUT_PIN 17
#define THRESHOLD 430
#define RELAY_PIN 0


//Para saber mais sobre esta biblioteca, acessar https://github.com/plapointe6/EspMQTTClient

//Definicoes e constantes
char SSIDName[] = "caues24"; //nome da rede WiFi
char SSIDPass[] = "rojao1213"; //senha da rede WiFI

const int DHT_PIN = 4; //terminal do sensor de temperatura e umidade

char BrokerURL[] = "broker.hivemq.com"; //URL do broker MQTT
char BrokerUserName[] = ""; //nome do usuario para autenticar no broker MQTT
char BrokerPassword[] = ""; //senha para autenticar no broker MQTT
char MQTTClientName[] = "caue-longhi"; //nome do cliente MQTT
int BrokerPort = 1883; //porta do broker MQTT

String TopicoPrefixo = "longhiTeste"; //prefixo do topico
String Topico_01 = TopicoPrefixo+"/Temperatura"; //nome do topico 01
String Topico_02 = TopicoPrefixo+"/Umidade"; //nome do topico 02
String Topico_03 = TopicoPrefixo+"/Soil"; //nome do topico 02
String Topico_04 = TopicoPrefixo+"/Pump";

//Variaveis globais e objetos
DHTesp dhtSensor; //instancia o objeto dhtSensor a partir da classa DHTesp

EspMQTTClient clienteMQTT(SSIDName, SSIDPass, BrokerURL, BrokerUserName, BrokerPassword, MQTTClientName, BrokerPort); //inicializa o cliente MQTT

//Este prototipo de funcao deve estar sempre presente
void onConnectionEstablished() {
}

//Setup
void setup() {
  Serial.begin(9600);

  dhtSensor.setup(DHT_PIN, DHTesp::DHT11); //inicializa o sensor de temperatura e umidade
  pinMode(RELAY_PIN, OUTPUT);
  clienteMQTT.enableDebuggingMessages(); //habilita mensagens de debug no monitor serial
  //clienteMQTT.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridded with enableHTTPWebUpdater("user", "password").
  //clienteMQTT.enableOTA(); // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword. Port is the default OTA port. Can be overridden with enableOTA("password", port).
  //clienteMQTT.enableLastWillMessage("TestClient/lastwill", "Vou ficar offline");
}

void pump(){
  Serial.println("bomba");
  digitalWrite(RELAY_PIN, HIGH); // turn on pump 4 seconds
  delay(2000);
  digitalWrite(RELAY_PIN, LOW);  // turn off pump 4 seconds
}

void enviarDados() {
  TempAndHumidity temp_umid = dhtSensor.getTempAndHumidity(); //instancia o objeto temp_umid a partir da classe TempAndHumidity
  int value = analogRead(AOUT_PIN);

  clienteMQTT.publish(Topico_01, String(temp_umid.temperature, 0)); 
  clienteMQTT.publish(Topico_02, String(temp_umid.humidity, 0));
  clienteMQTT.publish(Topico_03, String(value));
  

  if(value > 450){
    pump();
    clienteMQTT.publish(Topico_04, String(1));
  }else{
    clienteMQTT.publish(Topico_04, String(0));
    digitalWrite(RELAY_PIN, HIGH);
  }

  //Serial.println("Temperatura: " + String(temp_umid.temperature, 2) + "°C");
  //Serial.println("Umidade: " + String(temp_umid.humidity, 1) + "%");
  //Serial.println("---");

}

//Loop
void loop() {

  //clienteMQTT.enableMQTTPersistence(); //estabelece uma conexao persistente
  clienteMQTT.loop(); //funcao necessaria para manter a conexao com o broker MQTT ativa e coletar as mensagens recebidas
  enviarDados(); //funcao para publicar os dados no broker MQTT


  if (clienteMQTT.isWifiConnected() == 1) {
    Serial.println("Conectado ao WiFi!");
  } else {
    Serial.println("Nao conectado ao WiFi!");
  }

  if (clienteMQTT.isMqttConnected() == 1) {
    Serial.println("Conectado ao broker MQTT!");
  } else {
    Serial.println("Nao conectado ao broker MQTT!");
  }

  Serial.println("Nome do cliente: " + String(clienteMQTT.getMqttClientName())
    + " / Broker MQTT: " + String(clienteMQTT.getMqttServerIp())
    + " / Porta: " + String(clienteMQTT.getMqttServerPort())
  );

  delay(5000);
}