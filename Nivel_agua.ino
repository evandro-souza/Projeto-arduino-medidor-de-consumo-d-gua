/*************************************************************************
  Tópicos Especiais - IoT -
  Medidor de Agua
  Indica o nível de volume de agua no recepiente
  Circuito - Potenciomero ligado ao pino A0.
  Adaptado por  - 02/07/18
**************************************************************************/
#define espAgua Serial2
#define pinResetEsp 4 
#define velEsp 115200
 
#define DEBUG true

const int PinoPot = A0;

int ValorLido = 0;              // Variável que armazena o valor da leitura do Potenciometro
int ValorPont = 0;              // Variável que armazena o valor a ser exibido
 
void setup(){
  espAgua.begin (velEsp); 
  Serial.begin(9600);
  resetEsp();
  IniciaWIFI(); 
}

void loop(){
  ValorLido = analogRead(PinoPot);                // Lê o valor do potenciometro(resistor variavel)
  ValorPont = map(ValorLido, 0, 1023, 0, 100);   // muda a escala (valor entre 0 e 1000)
  if (espAgua.available()){
    if (espAgua.find("+IPD,")){
      delay(300);
      int idConex = espAgua.read() - 48;
      String WEB = "<head><meta http-equiv=""refresh"" content=""3"">";
      WEB += "</head><h1>Medidor de Caixa d'Agua</h1><h2>";
      WEB += "<h2>Nivel: ";
      WEB += ValorPont;
      WEB += " porcento";
      WEB += "</h2>";
      
      String cipSend = "AT+CIPSEND=";
      cipSend += idConex;
      cipSend += ",";
      cipSend += WEB.length();
      cipSend += "\r\n";
 
      enviarDados(cipSend, 1000, DEBUG);
      enviarDados(WEB, 1000, DEBUG);
 
      String fecharComando = "AT+CIPCLOSE=";
      fecharComando += idConex; // append connection id
      fecharComando += "\r\n";
 
      enviarDados(fecharComando, 3000, DEBUG);
    }
  }
}
/*************************************************/
void IniciaWIFI(){
  enviarDados("AT+RST\r\n", 2000, DEBUG); // reset
  enviarDados("AT+CWJAP=\"Minha Rede\",\"Minha senha\"\r\n", 2000, DEBUG); //Connect network
  delay(3000);
  enviarDados("AT+CWMODE=1\r\n", 1000, DEBUG);
  enviarDados("AT+CIFSR\r\n", 1000, DEBUG); // Show IP Adress
  enviarDados("AT+CIPMUX=1\r\n", 1000, DEBUG); // Multiple conexions
  enviarDados("AT+CIPSERVER=1,80\r\n", 1000, DEBUG); // start comm port 80
}
/*************************************************/ 
String enviarDados(String comando, const int timeout, boolean debug){
  String responde = "";
  espAgua.print(comando);
  long int time = millis();
  while ( (time + timeout) > millis()){
    while (espAgua.available()){
      char c = espAgua.read();
      responde += c;
    }
  }
  if (debug){
    Serial.print(responde);
  }
  return responde;
}
/*************************************************/
void resetEsp (){
  pinMode(pinResetEsp, OUTPUT);
  digitalWrite(pinResetEsp, LOW);
  delay(300);
  digitalWrite(pinResetEsp, HIGH);
}
