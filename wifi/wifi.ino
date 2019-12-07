#include "WiFiEsp.h" //INCLUSÃO DA BIBLIOTEC
#include "SoftwareSerial.h"//INCLUSÃO DA BIBLIOTECA
#include <IRremote.h>

IRsend irsend;
const int khz = 38;
 
SoftwareSerial Serial1(6, 7); //PINOS QUE EMULAM A SERIAL, ONDE O PINO 6 É O RX E O PINO 7 É O TX
 
char ssid[] = "GabrielM"; //VARIÁVEL QUE ARMAZENA O NOME DA REDE SEM FIO
char pass[] = "12345678";//VARIÁVEL QUE ARMAZENA A SENHA DA REDE SEM FIO



//Arescondicionados instalados nesse arduino
const String arconds[2] = {"42","33"};
const int totalArconds = 2;

 
int status = WL_IDLE_STATUS; //STATUS TEMPORÁRIO ATRIBUÍDO QUANDO O WIFI É INICIALIZADO E PERMANECE ATIVO
//ATÉ QUE O NÚMERO DE TENTATIVAS EXPIRE (RESULTANDO EM WL_NO_SHIELD) OU QUE UMA CONEXÃO SEJA ESTABELECIDA
//(RESULTANDO EM WL_CONNECTED)
 
WiFiEspServer server(80); //CONEXÃO REALIZADA NA PORTA 80
 
RingBuffer buf(16); //BUFFER PARA AUMENTAR A VELOCIDADE E REDUZIR A ALOCAÇÃO DE MEMÓRIA
 
int statusLed = LOW; //VARIÁVEL QUE ARMAZENA O ESTADO ATUAL DO LED (LIGADO / DESLIGADO)
 
void setup(){
  pinMode(LED_BUILTIN, OUTPUT); //DEFINE O PINO COMO SAÍDA (LED_BUILTIN = PINO 13)
  digitalWrite(LED_BUILTIN, LOW); //PINO 13 INICIA DESLIGADO
  Serial.begin(9600); //INICIALIZA A SERIAL
  Serial1.begin(9600);    // initialize serial for ESP module
  WiFi.init(&Serial1); //INICIALIZA A COMUNICAÇÃO SERIAL COM O SHIELD WIFI ESP8266
  WiFi.config(IPAddress(192,168,43,70)); //COLOQUE UMA FAIXA DE IP DISPONÍVEL DO SEU ROTEADOR
 
  //INÍCIO - VERIFICA SE O SHIELD ESTÁ ACOPLADO AO ARDUINO, CONECTA A REDE SEM FIO E INICIA O WEBSERVER
  if(WiFi.status() == WL_NO_SHIELD){
    while (true);
  }
  while(status != WL_CONNECTED){
    status = WiFi.begin(ssid, pass);
  }
  server.begin();
  //FIM - VERIFICA SE O SHIELD ESTÁ ACOPLADO AO ARDUINO, CONECTA A REDE SEM FIO E INICIA O WEBSERVER
}


 
void loop(){
  WiFiEspClient client = server.available(); //ATENDE AS SOLICITAÇÕES DO CLIENTE

  String req = "";
  int reqSize = 0;
  bool cmdRealizado = false;
  
  if (client) { //SE CLIENTE TENTAR SE CONECTAR, FAZ
    buf.init(); //INICIALIZA O BUFFER
    while (client.connected()){ //ENQUANTO O CLIENTE ESTIVER CONECTADO, FAZ

      if(client.available()){ //SE EXISTIR REQUISIÇÃO DO CLIENTE, FAZ
        char c = client.read(); //LÊ A REQUISIÇÃO DO CLIENTE
        buf.push(c); //BUFFER ARMAZENA A REQUISIÇÃO
        reqSize++;
        
        req += c;
        /*Serial.print(c);
        if (c == '\r'){
          Serial.println("");
        }*/

        //IDENTIFICA O FIM DA REQUISIÇÃO HTTP E ENVIA UMA RESPOSTA
        if(buf.endsWith("HTTP") || cmdRealizado) {
          sendHttpResponse(client);
          break;
        }


        if (reqSize < 11 ){
            continue;
        }

        Serial.println(req);

        //Serial.println("-----------------------");
      
        for (int i = 0; i < totalArconds; i++){
        
          //testa o comando ligar para cada arcondicionado nesse arduino
          String buscaLigar = "GET /ligar/" + arconds[i];
          String buscaDesligar = "GET /desligar/" + arconds[i];

          //Serial.println(buscaLigar+"<<");


          //converte para char
          char __buscaLigar[buscaLigar.length()+1];
          buscaLigar.toCharArray(__buscaLigar, buscaLigar.length()+1);
          //testa o comando desligar
          char __buscaDesligar[buscaDesligar.length()+1];
          buscaDesligar.toCharArray(__buscaDesligar, buscaDesligar.length()+1);

          //Serial.println((char*)__buscaLigar);

          if(buf.endsWith( __buscaLigar )){ //SE O PARÂMETRO DA REQUISIÇÃO VINDO POR GET FOR IGUAL A "H", FAZ 
            ligarArcond(arconds[i].toInt());
            cmdRealizado = true;
            break;
          } else
          if (buf.endsWith( __buscaDesligar )){ //SE O PARÂMETRO DA REQUISIÇÃO VINDO POR GET FOR IGUAL A "L", FAZ
            desligarArcond(arconds[i].toInt());
            cmdRealizado = true;
            break;
          } else
          if (buf.endsWith( "GET /favicon" )){
            cmdRealizado = true;
            break;
          }

        }
      }
    }

    client.stop(); //FINALIZA A REQUISIÇÃO HTTP E DESCONECTA O CLIENTE
  }
}




void ligarArcond(int arcond){
  Serial.print("Ligando arcond");
  Serial.println(arcond);

  digitalWrite(LED_BUILTIN, HIGH); //ACENDE O LED
  statusLed = 1; //VARIÁVEL RECEBE VALOR 1(SIGNIFICA QUE O LED ESTÁ ACESO)


  //unsigned int irSignal[] = {4384, 4448, 512, 1700, 508, 588, 488, 1708, 488, 1700, 488, 592, 504, 592, 508, 1700, 488, 592, 508, 592, 512, 1704, 488, 588, 512, 588, 508, 1700, 492, 1700, 488, 588, 512, 1704, 488, 588, 512, 1700, 492, 1696, 488, 1704, 488, 1700, 488, 588, 512, 1700, 492, 1700, 492, 1704, 488, 588, 512, 584, 516, 588, 508, 588, 512, 1700, 488, 588, 508, 592, 508, 1708, 488, 1700, 492, 1700, 488, 588, 508, 592, 508, 588, 512, 588, 508, 588, 516, 588, 508, 588, 512, 592, 508, 1704, 488, 232, 196, 1272, 492, 1696, 516, 1676, 488, 1700, 492, 5300, 4360, 4456, 584, 1628, 560, 540, 560, 1632, 560, 1604, 584, 540, 556, 540, 560, 1604, 584, 544, 556, 544, 556, 1612, 580, 540, 556, 544, 552, 1612, 576, 1612, 572, 548, 552, 1620, 572, 552, 524, 1640, 548, 1640, 544, 1644, 544, 1644, 544, 580, 520, 1648, 540, 1648, 544, 1652, 540, 580, 520, 580, 520, 580, 516, 580, 520, 1644, 544, 580, 516, 584, 516, 1656, 536, 1652, 536, 1652, 536, 584, 516, 584, 512, 584, 516, 580, 520, 584, 512, 588, 512, 584, 516, 584, 512, 1656, 540, 1648, 540, 1652, 536, 1652, 536, 1652, 500};
  //irsend.sendRaw(irSignal, sizeof(irSignal) / sizeof(irSignal[0]), khz);
}



void desligarArcond(int arcond){
  Serial.print("Desligando arcond");
  Serial.println(arcond);

  digitalWrite(LED_BUILTIN, LOW);
  statusLed = 0;


  //unsigned int irSignal3[] = {4408, 4408, 536, 1648, 544, 580, 516, 1652, 544, 1648, 540, 580, 520, 580, 520, 1644, 544, 580, 520, 584, 516, 1652, 544, 576, 524, 576, 520, 1644, 548, 1644, 544, 576, 524, 1648, 544, 580, 524, 576, 520, 580, 520, 1644, 544, 1644, 548, 1640, 548, 1644, 544, 1644, 548, 1648, 548, 1644, 544, 1644, 548, 572, 524, 576, 524, 576, 524, 572, 544, 556, 548, 556, 548, 1620, 572, 552, 548, 548, 548, 1616, 552, 572, 524, 576, 548, 552, 548, 1620, 572, 548, 528, 1644, 568, 1620, 568, 556, 524, 1644, 548, 1640, 548, 1640, 500, 5312, 4444, 4376, 540, 1648, 544, 576, 520, 1652, 540, 1648, 544, 580, 516, 580, 520, 1648, 540, 584, 516, 584, 516, 1652, 540, 584, 516, 580, 520, 1648, 536, 1652, 536, 584, 516, 1656, 540, 584, 516, 584, 512, 584, 516, 1652, 536, 1652, 540, 1652, 536, 1652, 536, 1656, 536, 1660, 532, 1656, 536, 1652, 536, 584, 516, 584, 512, 584, 516, 584, 516, 584, 516, 584, 516, 1660, 532, 588, 512, 584, 516, 1636, 552, 584, 512, 588, 512, 588, 512, 1660, 532, 584, 516, 1660, 532, 1656, 532, 588, 512, 1660, 532, 1656, 532, 1660, 480};  
  //irsend.sendRaw(irSignal3, sizeof(irSignal3) / sizeof(irSignal3[0]), khz); //Note the approach used to automatically calculate the size of the array.

}


 
//MÉTODO DE RESPOSTA A REQUISIÇÃO HTTP DO CLIENTE
void sendHttpResponse(WiFiEspClient client){

  String tem_gente = "";
  
  client.println("HTTP/1.1 200 OK"); //ESCREVE PARA O CLIENTE A VERSÃO DO HTTP
  client.println("Content-Type: text/html"); //ESCREVE PARA O CLIENTE O TIPO DE CONTEÚDO(texto/html)
  client.println("");
  client.println("[");

  //passa a porta onde o sensor esta conectado
  //isso permite que se tenha um arduino para duas salas
  if (temGente()){
    tem_gente = "true";
  } else {
    tem_gente = "false";
  }
  client.println("{ \"id\":42, \"ligado\":true, \"tem_pessoas\":" +tem_gente+ ", \"ip\":\"192.168.43.70\" }");

  //ignora isso por enquanto, tem que melhorar o codigo pra poder capturar duas salas com 1 arduino
  //aqui, passaria uma outra porta, com outro sensor de presenca
  /*if (temGente(3)){
    tem_gente = "true";
  } else {
    tem_gente = "false";
  }
  client.println("{ \"id\":43, \"ligado\":true, \"tem_pessoas\":" +tem_gente+ ", \"ip\":\"192.168.0.70\" }");*/
  client.println("]");


  
  //isso tem que sair
  if (statusLed == HIGH){ //SE VARIÁVEL FOR IGUAL A HIGH (1), FAZ
    client.println("<p style='line-height:0'><font color='green'>LIGADO</font></p>"); //ESCREVE "LIGADO" NA PÁGINA
    client.println("<a href='/ligar'>APAGAR</a>"); //COMANDO PARA APAGAR O LED (PASSA O PARÂMETRO /L)
  }else{ //SENÃO, FAZ
    if (statusLed == LOW){ //SE VARIÁVEL FOR IGUAL A LOW (0), FAZ
    client.println("<p style='line-height:0'><font color='red'>DESLIGADO</font></p>"); //ESCREVE "DESLIGADO" NA PÁGINA
    client.println("<a href='/desligar'>ACENDER</a>"); //COMANDO PARA ACENDER O LED (PASSA O PARÂMETRO /H)
    }
  }


  delay(1); //INTERVALO DE 1 MILISSEGUNDO
}




const int tem_gente_contagem = 120;
int tem_gente = tem_gente_contagem;
bool tem_gente_atual = false;
const int pinoPIR = 2;

bool temGente(){

  if(digitalRead(pinoPIR) == HIGH){ //SE A LEITURA DO PINO FOR IGUAL A HIGH, FAZ
      Serial.println("SIM");
      tem_gente_atual = true;
   }else{ //SENÃO, FAZ
      Serial.println("NAO");
   }

  if (tem_gente_atual && tem_gente < tem_gente_contagem){
    tem_gente = tem_gente_contagem;
  } else {
    tem_gente--;
  }

  if (tem_gente == 0){
    return false;  
  } else {
    return true;  
  }
}
