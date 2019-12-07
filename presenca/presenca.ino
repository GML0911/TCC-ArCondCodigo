const int pinoPIR = 2; //PINO DIGITAL UTILIZADO PELO SENSOR DE PRESENÇA

 
void setup(){
  Serial.begin(9600);
  pinMode(pinoPIR, INPUT); //DEFINE O PINO COMO ENTRADA
}


const int tem_gente_contagem = 120;
int tem_gente = tem_gente_contagem;

bool temGente(){
  bool atual = false;

  if(digitalRead(pinoPIR) == HIGH){ //SE A LEITURA DO PINO FOR IGUAL A HIGH, FAZ
      Serial.println("SIM");
      atual = true;
   }else{ //SENÃO, FAZ
      Serial.println("NAO");
   }


  
  if (atual && tem_gente < tem_gente_contagem){
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


void loop(){
 
  Serial.println(tem_gente); 
  if (temGente()){
    Serial.println("TEM gente"); 
  } else {
    Serial.println("Vazio"); 
  }
  
  delay(1000);
}
