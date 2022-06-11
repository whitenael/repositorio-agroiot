/* VERSION 1012
* ULTIMA UPDATE: 23/05/22
* AUTOR: LUIS MANUEL VERA
/*
*/
// CONVERSOR DE TIEMPO
#include <SoftwareSerial.h>
#include <NewPing.h>
SoftwareSerial mySerial(12,13); // RX, TX

unsigned long seconds = 1000L; //unsigned = solo almacena numeros positivos
unsigned long minutes = seconds * 60;
unsigned long hours = minutes * 60;

unsigned long tiempo1 = 0;
unsigned long tiempo2 = 0;
unsigned long tiempoSegundos = 0;

String operadora = "personal";

//APN ORIGINAL y CONECTIVIDAD CLARO
String apn_claro   = "igprs.claro.com.ar";
String apn_u_claro = "clarogprs";
String apn_p_claro = "clarogprs999";

// APN ORIGINAL y CONECTIVIDAD PERSONAL
String apn_personal   = "internet.personal.com";
String apn_u_personal = "internet";
String apn_p_personal = "internet";

// APN 2 PERSONAL

String apn_personal2 = "datos.personal.com";
String apn_u_personal2 = "datos";
String apn_p_personal2 = "datos";

String apiKey   = "2NE9RSVZXWL2X2GB";    

String url = "www.agroiot.com.ar/servicios/sensores/cargar/muestra";

// CONSTANTES TANQUE (especificadas por el cliente) / expresadas en CM

const float desc_alras = 40; // variable a confirmar
const float alto_tanque = 100;
const float diam_tanque = 755;

const double pi = 3.14159;

double pmin = 25.00;

const int PinTrig = 6;
const int PinEcho = 7;

NewPing sonar (PinTrig, PinEcho);

float vol_cm3;
float vol_lit;
float auxiliar;
float llenado;

float distancia;
int microSeconds;

void setup()
{
  // COM-SERIAL
  Serial.begin(9600);
  mySerial.begin(9600);
  mySerial.listen();
  gsm_init(operadora);

  // SRT-04 CONFIG
  pinMode(PinTrig, OUTPUT);
  pinMode(PinEcho, INPUT_PULLUP);

  // DHT22 CONFIG
  // pinMode(DHTPIN, INPUT_PULLUP);
  // dht.begin();
}
void loop()
{
 // CALCULO DE DISTANCIA

  float distanciaChequeada;

  microSeconds = sonar.ping_median(10);
  distancia = sonar.convert_cm(microSeconds);
  distanciaChequeada = chequearMedida(distancia);

  if (distanciaChequeada == -1){llenado=-1;}
  
  else if (distanciaChequeada == -2){llenado == -2};

  else{llenado = distanciaChequeada;}
 
  Serial.println("########## RESULTADOS: ##########");
  Serial.print(llenado);
  Serial.print("%");
  Serial.println();
  delay(1000);
  
  gsm_sendhttp(69, llenado);
  gsm_recall(operadora);
  
  delay(10*seconds);
  gsm_recall(operadora);

  delay(30*minutes);
}

// Chequeamos que la medida este dentro de un rango valido

// Caso contrario, hacer una nueva medida

float chequearMedida(float medida){

  Serial.print("Medida actual: ");
  Serial.println(medida);
  
  int counter = 0;
  int datoNuevo = medida;
  
  if (medida == 0){return -2;}

  else if (datoNuevo < desc_alras | datoNuevo > (alto_tanque + desc_alras))
  {  
    while ((counter < 50) & (datoNuevo < desc_alras | datoNuevo > (alto_tanque + desc_alras)))
    {
      Serial.print("Retomando muestra intento #");
      Serial.print(counter);
      Serial.println();
      microSeconds = sonar.ping_median(10);
      datoNuevo = sonar.convert_cm(microSeconds);
      Serial.println(datoNuevo);
      
      counter++;
      delay(200);
    }

    if (counter == 50){return -1;}
    else{
      Serial.print("Dato recolectado: ");
      Serial.println(datoNuevo);
      return datoNuevo;}
  }

  else {
    return medida;
  } 
  
}

float calcularLlenado(float distanciaChequeada){
  // CALCULO DE VOLUMEN

  vol_cm3 = pi * (diam_tanque * diam_tanque) / 4 * alto_tanque;
  vol_lit  = vol_cm3 / 1000;
  auxiliar = vol_lit; //aux = 3141,6 Litros
  llenado = 100.00;

  // calcular el llenado real

  vol_cm3 = pi * (diam_tanque * diam_tanque) / 4 * (alto_tanque - (distanciaChequeada - desc_alras));
  vol_lit  = vol_cm3 / 1000;         
  llenado = vol_lit * 100.00 / auxiliar; 

  return llenado;
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
  
void gsm_init(String operadora){

  String apn;
  String apn_u;
  String apn_p;

  if (operadora == "claro"){
    apn = apn_claro;
    apn_u = apn_u_claro;
    apn_p = apn_p_claro;
  }

  else if(operadora = "personal"){
    apn = apn_personal;
    apn_u = apn_u_personal;
    apn_p = apn_p_personal;
  }

  else if(operadora = "personal2"){
    apn = apn_personal2;
    apn_u = apn_u_personal2;
    apn_p = apn_p_personal2;
  }

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
  delay(1000);
  
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

void gsm_recall(String operadora){

  String apn;
  String apn_u;
  String apn_p;

  if (operadora == "claro"){
    apn = apn_claro;
    apn_u = apn_u_claro;
    apn_p = apn_p_claro;
  }

  else if(operadora = "personal"){
    apn = apn_personal;
    apn_u = apn_u_personal;
    apn_p = apn_p_personal;
  }

  else if(operadora = "personal2"){
    apn = apn_personal2;
    apn_u = apn_u_personal2;
    apn_p = apn_p_personal2;
  }
  
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
