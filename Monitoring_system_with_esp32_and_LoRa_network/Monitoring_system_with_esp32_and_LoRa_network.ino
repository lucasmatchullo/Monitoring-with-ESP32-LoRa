#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <SSD1306.h>

//Comente ou remova para compilar o Gatway
#define MASTER

#define RST 14  // GPIO14 RESET
#define DI00 26 // GPIO26 IRQ(Interrupt Request)

#define BAND 915E6 //Frequência do radio - exemplo : 433E6, 868E6, 915E6

//Constante para informar ao Slave que queremos os dados
const String GETDATA = "get";
//Constante que o Slave retorna junto com os dados para o Master
const String SETDATA = "set";

//Estrutura com os dados do sensor
typedef struct {
  float humidityGround;
  //float humidityGroundDig;
  float humidityAir;
  float temperatureAir;
}Data;

//Variável para controlar o display
SSD1306 display(0x3c, SDA, SCL);

void setupDisplay(){
  //O estado do GPIO16 é utilizado para controlar o display OLED
  pinMode(16, OUTPUT);
  //Reseta as configurações do display OLED
  digitalWrite(16, LOW);
  //Para o OLED permanecer ligado, o GPIO16 deve permanecer HIGH
  //Deve estar em HIGH antes de chamar o display.init() e fazer as demais configurações,
  //não inverta a ordem
  digitalWrite(16, HIGH);

  //Configurações do display
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

//Configurações iniciais do LoRa
void setupLoRa(){ 
  //Inicializa a comunicação
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI00);

  //Inicializa o LoRa
  if (!LoRa.begin(BAND)){
    //Se não conseguiu inicializar, mostra uma mensagem no display
    display.clear();
    display.drawString(0, 0, "Erro ao inicializar o LoRa!");
    display.display();
    while (1);
  }

  //Ativa o crc
  LoRa.enableCrc();
  //Ativa o recebimento de pacotes
  LoRa.receive();
}
