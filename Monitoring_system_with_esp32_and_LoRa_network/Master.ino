//Compila apenas se MASTER estiver definido no arquivo principal
#ifdef MASTER

#include <PubSubClient.h>
#include <WiFi.h>

//Substitua pelo SSID da sua rede
#define SSID "Sol.net"

//Substitua pela senha da sua rede
#define PASSWORD "Alessandra."

//Server MQTT que iremos utlizar
#define MQTT_SERVER "quickstart.messaging.internetofthings.ibmcloud.com"

//Nome do tópico que devemos enviar os dados
//para que eles apareçam nos gráficos
#define TOPIC_NAME "iot-2/evt/status/fmt/json"

//ID que usaremos para conectar 
//QUICK_START deve permanecer como está
const String QUICK_START = "d:quickstart:arduino:";

//No DEVICE_ID você deve mudar para um id único
const String DEVICE_ID = "F8DA0C84DAAF";

//Concatemos o id do quickstart com o id do nosso
//dispositivo
const String CLIENT_ID =  QUICK_START + DEVICE_ID;

//Cliente WiFi que o MQTT irá utilizar para se conectar
WiFiClient wifiClient;

//Cliente MQTT, passamos a url do server, a porta
//e o cliente WiFi
PubSubClient client(MQTT_SERVER, 1883, wifiClient);

//Intervalo entre os envios
#define INTERVAL 10000

//Tempo do último envio
long lastSendTime = 0;

//Onde ficam os dados que chegam do outro dispositivo LoRa 
Data data;

void setup(){
  //Definindo velocidade comunicação serial
  Serial.begin(115200);
  //Chama a configuração inicial do display
  setupDisplay();
  //Chama a configuração inicial do LoRa
  setupLoRa();

  display.clear();
  display.drawString(23, 25, "Gateway ON");
  display.display();
  //Conectando à rede WiFi
  conectWiFi();
  //Cenetando ao Broker
  connectMQTTServer();
}

void loop(){

  //Verificando conexão com Servidor
  if (!client.connected()) {
   reconnectMQTT();
  }
  //Verificando conexão Wi-Fi
  recconectWiFi();

  client.loop();

  //Verificamos se há pacotes para recebermos
  receive();
}

//Função responsável por conectar ao server MQTT
void connectMQTTServer() {
  Serial.println("Conectando ao Broker via MQTT...");
  //Se conecta ao id que definimos
  if (client.connect(CLIENT_ID.c_str())) {
    //Se a conexão foi bem sucedida
    Serial.println("conectado");
  } else {
    //Se ocorreu algum erro
    Serial.print("error = ");
    Serial.println(client.state());
  }
}

//Função responsável por conectar à rede WiFi
void conectWiFi() {
  delay(10);
  Serial.println();
  Serial.println("Conectando em " + String(SSID));
  Serial.print(SSID);

  //Manda o esp se conectar à rede através
  //do ssid e senha
  WiFi.begin(SSID, PASSWORD);

  //Espera até que a conexão com a rede seja estabelecida
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Se chegou aqui é porque conectou
  Serial.println();
  Serial.print("Conectado em " + String(SSID) + " | Endereço IP: ");
  Serial.println(WiFi.localIP());
}

void reconnectMQTT() {
 while (!client.connected()) {
   Serial.println("Tentando se conectar ao Broker MQTT: " + String(MQTT_SERVER));
   if (client.connect(MQTT_SERVER)) {
     Serial.println("Conectado");
   } else {
     Serial.println("Falha ao Reconectar");
     Serial.println("Tentando se reconectar em 2 segundos");
     delay(2000);
   }
 }
}

void recconectWiFi() {
 while (WiFi.status() != WL_CONNECTED) {
   delay(100);
   Serial.print(".");
 }
}

void receive(){
  //Tentamos ler o pacote
  int packetSize = LoRa.parsePacket();
  
  //Verificamos se o pacote tem o tamanho mínimo de caracteres que esperamos
  if (packetSize > SETDATA.length()){
    String received = "";
    //Armazena os dados do pacote em uma string
    for(int i=0; i<SETDATA.length(); i++){
      received += (char) LoRa.read();
    }

    //Se o cabeçalho é o que esperamos
    if(received.equals(SETDATA)){
      //Fazemos a leitura dos dados
      LoRa.readBytes((uint8_t*)&data, sizeof(data));
      //Mostramos os dados no display
      showData();

      Serial.print("Publish message: ");
      //Criamos o json que enviaremos para o server mqtt
      String msg = createJsonString();
      Serial.println(msg);
      //Publicamos no tópico onde o servidor espera para receber 
      //e gerar o gráfico
      client.publish(TOPIC_NAME, msg.c_str());
    }
  }
}

void showData(){

  Serial.println("Status sensor: ");
  Serial.println(String(data.humidityGround) + "% de úmidade no solo");
 
  //Mostra no display dados recebidos
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(38, 0, "System ON");
  display.drawString(16, 13, "Informations received");
  display.drawString(33, 26, "Umidade solo:");
  display.setFont(ArialMT_Plain_16);
  display.drawString(40, 43,  String(data.humidityGround) + "%");
  display.display();
}

//Função responsável por criar
//um Json com os dados lidos
String createJsonString() {
  String json = "{";
    json+= "\"d\": {";
      json+="\"Umidade do solo\":";
      json+=String(data.humidityGround);
    json+="}";
  json+="}";
  return json;
}

#endif
