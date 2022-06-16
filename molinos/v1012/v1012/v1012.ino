/* VERSION 1012
* ULTIMA UPDATE: 25/04/22
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

// #### INICIO VARIABLES PARA EL CALCULO DE MEDIDA #### //

const float desc_alras = 40; // medida al pelo del agua
const float alto_tanque = 100; // medidas del tanque
const float diam_tanque = 755;

struct medida{
  float value;
  int validas;
};

float vol_cm3;
float vol_lit;
float auxiliar;
float llenado;

const double pi = 3.14159;

double pmin = 25.00; 

// #### FIN VARIABLES PARA EL CALCULO DE MEDIDA #### //

const int PinTrig = 6;
const int PinEcho = 7;
const int idSensor = 70;

NewPing sonar (PinTrig, PinEcho);

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

  medida rep = tomarMedida();

  Serial.println("########## RESULTADOS: ##########");
  Serial.print("Promedio de medidas: ");
  Serial.println(rep.value);
  Serial.print("Medidas validas: ");
  Serial.println(rep.validas);
  Serial.println();
  delay(1000);
  
  gsm_sendhttp(idSensor, rep.value, rep.validas);
  gsm_recall(operadora);

  delay(30*minutes);
}

// Tenemos que tomar un total de 12 medidas: 4 por minuto 
// De estas 12 medidas, descartamos las invalidas, y nos quedamos con las validas y las sumamos a un contador
// De estas validas, calculamos un promedio y lo devolvemos

medida tomarMedida(){
  int tiempo = 0;
  float datoNuevo;
  float totalValidas = 0; float totalInvalidas = 0;
  float promedio;
  int contadorValidas = 0; 
  
  while (tiempo < 12){
    Serial.print("TIEMPO ");
    Serial.println(tiempo);
    microSeconds = sonar.ping_median(10);
    distancia = sonar.convert_cm(microSeconds);
    datoNuevo = distancia;
    Serial.print("Tomando Medida: ");
    Serial.println(datoNuevo);
    if (((datoNuevo > desc_alras) & (datoNuevo < (alto_tanque + desc_alras)))){
       contadorValidas++;
       totalValidas = totalValidas + datoNuevo;
       Serial.print("Medidas validas encontrada: ");
       Serial.println(contadorValidas);
    }
    else {
      totalInvalidas = totalInvalidas + datoNuevo;
      Serial.println("Medida invalidas encontrada ");
    }
    tiempo++;
    delay(15*seconds);
  }
  if (contadorValidas != 0)
    promedio = totalValidas / contadorValidas;
  else
    promedio = totalInvalidas / 12;
    
  medida m = {promedio, contadorValidas};

  return m;
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

void gsm_sendhttp(int value1, float value2, int value3){
  
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
  
  String PostData = "data0="+apiKey+"&data1="+String(value1)+"&data2="+String(value2,3)+"&data3="+String(value3);
  
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
