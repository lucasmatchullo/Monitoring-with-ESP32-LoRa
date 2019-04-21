//Compila apenas se MASTER não estiver definido no arquivo principal
#ifndef MASTER

#include <DHT.h>

#define Sen_analog 12    //pino analógico do sensor solo
#define Dht_analog 13    //pino digital do sensor solo

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
  pinMode(Sen_analog, INPUT);
  //configura pino digital do sensor como entrada
  pinMode(Dht_analog, INPUT);
  
  setupDisplay();
  //Chama a configuração inicial do LoRa
  setupLoRa();

  display.clear();
  display.drawString(10, 20, "EndPoint start");
  display.display();
}

void loop(){
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
  delay(5000);
}

//Função onde se faz a leitura dos dados 
Data readData()
{
  Data data;
  // váriavél que recebe valor vindo do sensor de solo
  float umidadePorcentual;
  umidadePorcentual = analogRead(Sen_analog);
  Serial.print(umidadePorcentual);
  //convertendo valor recebido em porcentagem
  data.humidityGround = 100 * ((4095 - umidadePorcentual) / 4095);
  
  return data;
}

void showSentData(Data data)
{ 
  Serial.println("Status sensores: ");
  Serial.println(String(data.humidityGround) + "% umidade no solo");
  //Serial.println(String(data.humidityGroundDig) + "%");
//  Serial.println(String(data.humidityAir) + "%");
//  Serial.println(String(data.temperatureAir) + " C");

  //Mostra no display
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.drawString(38, 0, "Systen ON");  // 38
  display.drawString(20, 13, "Monitoring sensors");  // 20
  display.drawString(23, 26, "Umidade do solo:");    // 23
  display.setFont(ArialMT_Plain_16);
  display.drawString(40, 43,  String(data.humidityGround) + "%"); // 40
  display.display();
}

#endif
