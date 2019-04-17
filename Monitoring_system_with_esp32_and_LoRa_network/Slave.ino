//Compila apenas se MASTER não estiver definido no arquivo principal
#ifndef MASTER

#include <DHT.h>

#define MQ_analog 12 //pino analógico do sensor solo
#define MQ_ 13    //pino digital do sensor solo

//Objeto que realiza a leitura da temperatura e da umidade no pin 23
 DHT dht(23, DHT22);
 
 //Variável para guardarmos o valor da temperatura do ar (dh22)
float temperatureAir = 0;
 
//Variável para guardarmos o valor da umidade do ar (dh22)
float humidityAir = 0;

//Variável para guardarmos o valor da humidade do solo
float humidityGround = 0;

//Variável para guardarmos o valor da humidade do solo valor digital input
//float humidityGroundDig = 0;

int count = 0;
 
void setup(){
  Serial.begin(115200);
  //Chama a configuração inicial do display
  
  //configura pino analog. do sensor como entrada
  pinMode(MQ_analog, INPUT);
  //configura pino digital do sensor como entrada
  pinMode(MQ_dig, INPUT);
  
  setupDisplay();
  //Chama a configuração inicial do LoRa
  setupLoRa();

  //Inicilaizando dht
  dht.begin();

  //0x76 se pino SDO do sensor estiver no GND
  //0x77 se pino SDO do sensor estiver no 3.3v 
//  if (!dht.begin(0x76))
//  {
//    Serial.println("Sensor não encontrado");
//    display.clear();
//    display.drawString(0, 0, "Sensor não encontrado");
//    display.display();
//    while(1);
//  }

  display.clear();
  display.drawString(0, 0, "EndPoint start");
  display.display();
}

void loop(){
  //Tenta ler o pacote
  int packetSize = LoRa.parsePacket();

  //Verifica se o pacote possui a quantidade de caracteres que esperamos
  if (packetSize == GETDATA.length())
  {
    String received = "";

    //Armazena os dados do pacote em uma string
    while(LoRa.available())
    {
      received += (char) LoRa.read();
    }

    if(received.equals(GETDATA))
    {
      //Faz a leitura dos dados
      Data data = readData();
      Serial.println("Criando pacote para envio");
      //Cria o pacote para envio
      LoRa.beginPacket();
      LoRa.print(SETDATA);
      LoRa.write((uint8_t*)&data, sizeof(data));
      //Finaliza e envia o pacote
      LoRa.endPacket();
      showSentData(data);
    }
  }
}

//Função onde se faz a leitura dos dados 
Data readData()
{
  Data data;
  // váriavél que recebe valor vindo do sensor de solo
  float umidadePorcentual;
  umidadePorcentual = analogRead(MQ_analog);
  //convertendo valor recebido em porcentagem
  data.humidityGround = 100 * ((4095 - umidadePorcentual) / 4095);
  //data.humidityGroundDig = digitalRead(MQ_dig);
  
  // lendo DHT 
  //Faz a leitura da umidade
  float value;
  value = dht.readTemperature();
  Serial.println(String(value) + " T");
  //Se o valor for válido
  if(!isnan(value)){
    //Armazena o novo valor da umidade
    data.humidityAir = value;
  }
  
  //Faz a leitura da temperatura
  value = dht.readHumidity();
  Serial.println(String(value) + " H");
  //Se o valor lido é válido
  if(!isnan(value)){
    //Armazena o novo valor da temperatura
    data.temperatureAir = value;
  }
  
  return data;
}

void showSentData(Data data)
{ 
  Serial.println("Status sensores: ");
  Serial.println(String(data.humidityGround) + "%");
  //Serial.println(String(data.humidityGroundDig) + "%");
  Serial.println(String(data.humidityAir) + "%");
  Serial.println(String(data.temperatureAir) + " C");

  //Mostra no display
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(0, 0, "Status sensores");
  display.drawString(0, 15,  String(data.humidityGround) + "%");
  //display.drawString(0, 15, String(data.humidityGroundDig) + "%");
  display.drawString(0, 30, String(data.humidityAir) + "%");
  display.drawString(0, 45, String(data.temperatureAir) + " C");
  display.display();
}

#endif
