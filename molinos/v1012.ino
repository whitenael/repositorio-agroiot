/* VERSION 1012
* ULTIMA UPDATE: 17/12/21
* AUTOR: LUIS MANUEL VERA
/*
// DHT CONFIG
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 6
#define DHTTYPE 22
#define vent1 4

DHT dht(DHTPIN, DHTTYPE);

la implementacion de los DHT es experimental
*/
// CONVERSOR DE TIEMPO
#include <SoftwareSerial.h>
SoftwareSerial mySerial(13,12); // RX, TX

unsigned long seconds = 1000L; //unsigned = solo almacena numeros positivos
unsigned long minutes = seconds * 60;
unsigned long hours = minutes * 60;

unsigned long tiempo1 = 0;
unsigned long tiempo2 = 0;
unsigned long tiempoSegundos = 0;

// APN ORIGINAL y CONECTIVIDAD
String apn   = "igprs.claro.com.ar";
String apn_u = "clarogprs";
String apn_p = "clarogprs999";

String apiKey   = "2NE9RSVZXWL2X2GB";    

String url = "34.69.212.37/agroiot/post.php";

// CONSTANTES TANQUE (especificadas por el cliente) / expresadas en CM

const float desc_alras = 0.00;
const float alto_tanque = 100;
const float diam_tanque = 200;

const double pi = 3.14159;

double pmin = 25.00;

const int PinTrig = 6;
const int PinEcho = 7;

// Constante velocidad sonido en cm/s
const float VelSon = 34000.0;

float distancia;

void setup()
{
  // COM-SERIAL
  Serial.begin(9600);
  mySerial.begin(9600);
  mySerial.listen();
  gsm_init();

  // HC-SR04 CONFIG
  pinMode(PinTrig, OUTPUT);
  pinMode(PinEcho, INPUT);

  // DHT22 CONFIG
  // pinMode(DHTPIN, INPUT_PULLUP);
  // dht.begin();
}
void loop()
{
  iniciarTrigger();
 
  unsigned long tiempo = pulseIn(PinEcho, HIGH); 
  
  distancia = tiempo * 0.000001 * VelSon / 2.0;

  // CALCULO DE VOLUMEN

  float vol_cm3 = pi * (diam_tanque * diam_tanque) / 4 * alto_tanque;
  float vol_lit  = vol_cm3 / 1000;
  float auxiliar = vol_lit; //aux = 3141,6 Litros
  float llenado = 100.00;

  // calcular el llenado real

  vol_cm3 = pi * (diam_tanque * diam_tanque) / 4 * (alto_tanque - (distancia - desc_alras));
  vol_lit  = vol_cm3 / 1000;         
  llenado = vol_lit * 100.00 / auxiliar; 

  float pt1 = llenado;

  //float temp1 = dht.readTemperature();
 // float hum1 = dht.readHumidity();  
 
  Serial.println("########## RESULTADOS: ##########");
  Serial.print(llenado);
  Serial.print("%");
  Serial.println();
  delay(1000);

  gsm_sendhttp(12, llenado);
  gsm_recall();
  
  delay(30*minutes);
  gsm_recall();
  // chequeamos estado de la conexion antes de volver a iniciar el ciclo

  delay(5*seconds);

}

// Método que inicia la secuencia del Trigger para comenzar a medir
void iniciarTrigger()
{
  // Ponemos el Triiger en estado bajo y esperamos 2 ms
  digitalWrite(PinTrig, LOW);
  delayMicroseconds(2);
  
  // Ponemos el pin Trigger a estado alto y esperamos 10 ms
  digitalWrite(PinTrig, HIGH);
  delayMicroseconds(10);
  
  // Comenzamos poniendo el pin Trigger en estado bajo
  digitalWrite(PinTrig, LOW);
}

// COMUNICACION CON EL SERVIDOR

void print_gsm_status(){
  while(mySerial.available()){
    Serial.write(mySerial.read());
  }
}

void gsm_test(){

  mySerial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  print_gsm_status();
  delay(2000);

  mySerial.println("AT+CSTT=\"3gprs\",\"3gprs\",\"3gprs\"");
  print_gsm_status();
  delay(2000);

  mySerial.println("AT+SAPBR=1,1");
  print_gsm_status();
  delay(2000);

  mySerial.println("AT+SAPBR=2,1");
  print_gsm_status();
  delay(2000);

  mySerial.println("AT+CSQ");
  print_gsm_status();
  delay(2000);

  mySerial.println("AT+HTTPINIT");
  print_gsm_status();
  delay(2000);

  mySerial.println("AT+HTTPSSL=1");
  print_gsm_status();
  delay(2000);

  mySerial.println("AT+HTTPPARA=\"CID\",1");
  print_gsm_status();
  delay(2000);

  mySerial.println("AT+HTTPPARA=\"URL\",\"https://miliohm.com/miliohmSIM800L.php\"");
  print_gsm_status();
  delay(2000);

  mySerial.println("AT+HTTPACTION=0");
  print_gsm_status();
  delay(2000);

  mySerial.println("AT+HTTPREAD");
  print_gsm_status();
  delay(2000);

  mySerial.println("AT+HTTPTERM");
  print_gsm_status();
  delay(2000);  
  }
  
void gsm_init(){

  Serial.println("Inicializando modem ...");

  delay(3*seconds);
  
  mySerial.println("AT+CFUN=1,1");
  print_gsm_status();
  delay(2000);

  //INTERRUMPIMOS MIENTRAS RESPONDE
  mySerial.println("AT");
  print_gsm_status();
  delay(4000);
  
  mySerial.println("AT+CREG?"); 
  print_gsm_status();
  delay(1000);
  
  Serial.println("Inicilizando conexión GPRS ...");
  mySerial.println("AT+CGATT=1"); 
  print_gsm_status();
  delay(1000);

  Serial.println("Inicilizando APN ...");

  //Setea conexion GPRS
  mySerial.println("AT+SAPBR=3,1,Contype,GPRS");
  print_gsm_status();
  delay(1000);
  mySerial.println("AT+SAPBR=3,1,APN," + apn);
  print_gsm_status();
  delay(1000);
  mySerial.println("AT+SAPBR=3,1,USER," + apn_u); 
  print_gsm_status();
  delay(1000);
  mySerial.println("AT+SAPBR=3,1,PWD," + apn_p); 
  print_gsm_status();
  delay(1000);
  mySerial.println("AT+SAPBR=1,1");
  print_gsm_status();
  delay(5000);
  mySerial.println("AT+SAPBR=2,1");
  print_gsm_status();
  delay(5000);
  // INTERRUMPIMOS MIENTRAS RESPONDE
  mySerial.println("AT");
  delay(10*seconds);
}

void gsm_sendhttp(int value1, float value2){
  
  mySerial.listen();  
 
  Serial.println("Iniciando HTTP ... (" + String(value1) + ")");
  
  mySerial.println("AT+HTTPINIT");
  print_gsm_status();
  delay(1000);
  mySerial.println("AT+HTTPPARA=CID,1");
  print_gsm_status();
  delay(1000);
  
  mySerial.println("AT+HTTPPARA=URL," + url);
  print_gsm_status();
  delay(100);
  mySerial.println("AT+HTTPPARA=CONTENT,application/x-www-form-urlencoded");
  print_gsm_status();
  delay(1000);

  String PostData = "data0="+apiKey+"&data1="+String(value1)+"&data2="+String(value2,3);
  
  mySerial.println("AT+HTTPDATA=" + String(PostData.length()) + ",10000");
  print_gsm_status();
  delay(1000);
  
  mySerial.println(PostData);
  print_gsm_status();
  delay(10000);
  mySerial.println("AT+HTTPACTION=1");
  print_gsm_status();
  delay(5000);
  mySerial.println("AT+HTTPREAD");
  print_gsm_status();
  delay(1000);
  mySerial.println("AT+HTTPTERM");
  print_gsm_status(); 
  delay(1000);

  Serial.println(" ");
  Serial.println("Finalizando HTTP ... (" + String(value1) + ")");
  delay(1000);
}

void gsm_out(){

  Serial.println("Finalizando conexión GPRS ...");
  mySerial.println("AT+CGATT=0"); 
  print_gsm_status();
  delay(1000);

}

void gsm_recall(){
  Serial.println("########## RECALLING... ##########");

  Serial.print("Señal del Modulo: ");
  mySerial.println("AT+CSQ");
  print_gsm_status();
  delay(1*seconds);
  
  Serial.print("IP del servidor: ");
  mySerial.println("AT+SAPBR=2,1");
  print_gsm_status();
  delay(1*seconds);

  mySerial.println("AT+CGATT=1"); 
  print_gsm_status();
  delay(1000);

  mySerial.println("AT+SAPBR=3,1,Contype,GPRS");
  print_gsm_status();
  delay(1000);
  
  mySerial.println("AT+SAPBR=3,1,APN," + apn);
  print_gsm_status();
  delay(1000);
  
  mySerial.println("AT+SAPBR=3,1,USER," + apn_u); 
  print_gsm_status();
  delay(1000);
  
  mySerial.println("AT+SAPBR=3,1,PWD," + apn_p); 
  print_gsm_status();
  delay(1000);
  
  mySerial.println("AT+SAPBR=1,1");
  print_gsm_status();
  delay(1000);
  
  delay(10*seconds);
}
