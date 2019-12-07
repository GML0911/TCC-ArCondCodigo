#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

IRsend irsend(D1);
const int khz = 38;
const char* ssid = "linksys";
const char* password = "12345678";

WiFiServer server(80);


/**
 * Variáveis para verificar se tem gente na sala
 */
const int tem_gente_contagem = 120;
int tem_gente = tem_gente_contagem;
bool tem_gente_atual = false;
const int pinoPIR = D2;

/**
 * Variáveis para verificar se o arcondicionado está ligado
 */
const int pinoFotoresistor = A0;

void setup(){
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pinoFotoresistor, INPUT);
  pinMode(pinoPIR, INPUT);  
  digitalWrite(LED_BUILTIN, LOW);

  irsend.begin();
  Serial.begin(9600);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL : ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}


 



void ligarArcond(){
  Serial.print("Ligando arcond");
  digitalWrite(LED_BUILTIN, HIGH); //ACENDE O LED
  uint16_t irSignal3[199] = {4408, 4408, 536, 1648, 544, 580, 516, 1652, 544, 1648, 540, 580, 520, 580, 520, 1644, 544, 580, 520, 584, 516, 1652, 544, 576, 524, 576, 520, 1644, 548, 1644, 544, 576, 524, 1648, 544, 580, 524, 576, 520, 580, 520, 1644, 544, 1644, 548, 1640, 548, 1644, 544, 1644, 548, 1648, 548, 1644, 544, 1644, 548, 572, 524, 576, 524, 576, 524, 572, 544, 556, 548, 556, 548, 1620, 572, 552, 548, 548, 548, 1616, 552, 572, 524, 576, 548, 552, 548, 1620, 572, 548, 528, 1644, 568, 1620, 568, 556, 524, 1644, 548, 1640, 548, 1640, 500, 5312, 4444, 4376, 540, 1648, 544, 576, 520, 1652, 540, 1648, 544, 580, 516, 580, 520, 1648, 540, 584, 516, 584, 516, 1652, 540, 584, 516, 580, 520, 1648, 536, 1652, 536, 584, 516, 1656, 540, 584, 516, 584, 512, 584, 516, 1652, 536, 1652, 540, 1652, 536, 1652, 536, 1656, 536, 1660, 532, 1656, 536, 1652, 536, 584, 516, 584, 512, 584, 516, 584, 516, 584, 516, 584, 516, 1660, 532, 588, 512, 584, 516, 1636, 552, 584, 512, 588, 512, 588, 512, 1660, 532, 584, 516, 1660, 532, 1656, 532, 588, 512, 1660, 532, 1656, 532, 1660, 480};  
  irsend.sendRaw(irSignal3, 199, khz); //Note the approach used to automatically calculate the size of the array.
  
  delay(500);
}



void desligarArcond(){
  Serial.print("Desligando arcond");
  digitalWrite(LED_BUILTIN, LOW);
  
  uint16_t irSignal[201] = {4384, 4448, 512, 1700, 508, 588, 488, 1708, 488, 1700, 488, 592, 504, 592, 508, 1700, 488, 592, 508, 592, 512, 1704, 488, 588, 512, 588, 508, 1700, 492, 1700, 488, 588, 512, 1704, 488, 588, 512, 1700, 492, 1696, 488, 1704, 488, 1700, 488, 588, 512, 1700, 492, 1700, 492, 1704, 488, 588, 512, 584, 516, 588, 508, 588, 512, 1700, 488, 588, 508, 592, 508, 1708, 488, 1700, 492, 1700, 488, 588, 508, 592, 508, 588, 512, 588, 508, 588, 516, 588, 508, 588, 512, 592, 508, 1704, 488, 232, 196, 1272, 492, 1696, 516, 1676, 488, 1700, 492, 5300, 4360, 4456, 584, 1628, 560, 540, 560, 1632, 560, 1604, 584, 540, 556, 540, 560, 1604, 584, 544, 556, 544, 556, 1612, 580, 540, 556, 544, 552, 1612, 576, 1612, 572, 548, 552, 1620, 572, 552, 524, 1640, 548, 1640, 544, 1644, 544, 1644, 544, 580, 520, 1648, 540, 1648, 544, 1652, 540, 580, 520, 580, 520, 580, 516, 580, 520, 1644, 544, 580, 516, 584, 516, 1656, 536, 1652, 536, 1652, 536, 584, 516, 584, 512, 584, 516, 580, 520, 584, 512, 588, 512, 584, 516, 584, 512, 1656, 540, 1648, 540, 1652, 536, 1652, 536, 1652, 500};
  irsend.sendRaw(irSignal, 201, khz);
  delay(500);
}


 
//MÉTODO DE RESPOSTA A REQUISIÇÃO HTTP DO CLIENTE
void sendHttpResponse(WiFiClient client){
  
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");

  //constroi o JSON
  client.println("");
  client.println("[");
  
  String tem_gente_str = boolToString(temGente());
  String esta_ligado = boolToString(estaLigado());
  String ip = WiFi.localIP().toString();
  
  client.println("{ \"num_sala\":42, \"marca\":\"midea\", \"ligado\":"+ esta_ligado + ", \"tem_gente_contagem\":"+tem_gente+", \"tem_pessoas\":" +tem_gente_str+ ", \"ip\":\""+ ip +"\"");

  String photor = String(analogRead(pinoFotoresistor), DEC);
  client.println(", \"photoresistor\": " + photor  );

  client.println("}]");

  delay(1);
}




void loop(){

  //verifica se tem pessoas na sala
  temGente();
  delay(100);
  
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }


  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();


  int value = LOW;
  if (request.indexOf("/desligar") != -1) {
    desligarArcond();
  } else
  if (request.indexOf("/ligar") != -1){
    ligarArcond();
  }

  sendHttpResponse(client);
}



/**
 * 
 * IDENTIFICA SE TEM PESSOAS NA SALA
 *
 */

bool temGente(){

  if(digitalRead(pinoPIR) == HIGH){ //SE A LEITURA DO PINO FOR IGUAL A HIGH, FAZ
      Serial.print("!");
      tem_gente_atual = true;
   }else{ //SENÃO, FAZ
      Serial.print(".");
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


bool estaLigado(){
  if (analogRead(pinoFotoresistor) > 100){
    return true;
  } else {
    return false;
  }
}


String boolToString(bool b){
  if (b){
    return "true";
  } else {
    return "false";  
  }
}
