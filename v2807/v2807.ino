#include <SoftwareSerial.h>
SoftwareSerial mySerial(12,13); // RX, TX

unsigned long seconds = 1000L; //unsigned = solo almacena numeros positivos
unsigned long minutes = seconds * 60;
unsigned long hours = minutes * 60;

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

const int idSensor = 71;

const int ldrExcPin = A2;
const int ldrRegPin = A1;
const int ldrLowPin = A0;

int ldrExc = 0; 
int ldrReg = 0; 
int ldrLow = 0;

int factorPromedio = 10;
int factorDesviacion = 50;

// TIPO DE DATO PARA MEDIDAS 

struct medida{
  int ldrHigh;
  int ldrReg;
  int ldrLow;
};

void setup() {
  
  Serial.begin(9600);
  //mySerial.begin(9600);
  //mySerial.listen();
  //gsm_init(operadora);
  pinMode(ldrExc, INPUT);
  pinMode(ldrReg, INPUT);
  pinMode(ldrLow, INPUT);
}

void loop() {
  chequearEstado();
  
  delay(30*minutes);
}

void chequearEstado(){
  medida ms = tomarPromMedidas();
  Serial.println("Calculando valores de estado...");
  delay(100);
  
  if(ms.ldrHigh > factorDesviacion) {
    //reportarEstado("Excelente");
    Serial.println("Excelente");
  }

  else if ((ms.ldrHigh < factorDesviacion)&(ms.ldrReg > factorDesviacion)){
    //reportarEstado("Regular"); 
    Serial.println("Regular");
  }

  else if ((ms.ldrHigh < factorDesviacion)&(ms.ldrReg < factorDesviacion)&(ms.ldrLow > factorDesviacion)){
    //reportarEstado("Bajo");
    Serial.println("Bajo");
  }

  else{
    //reportarEstado("Desconectado");
    Serial.println("Desconectado");
  }
}

medida tomarPromMedidas(){  //retorna promedio de 10 medidas
  int ldrExcTotal = 0;   int ldrRegTotal = 0;   int ldrLowTotal = 0;
  int promedioExc, promedioReg, promedioLow;
  int contadorMedida = 0;
  
  while (contadorMedida < factorPromedio){
    ldrExc = analogRead(ldrExcPin);
    ldrReg = analogRead(ldrRegPin);
    ldrLow = analogRead(ldrLowPin);

    ldrExcTotal = ldrExcTotal + ldrExc;
    ldrRegTotal = ldrRegTotal + ldrReg;
    ldrLowTotal = ldrLowTotal + ldrLow;

    contadorMedida++;

    Serial.print("Contando medida...");
    Serial.println(contadorMedida);
    Serial.print("Medida excelente: ");
    Serial.println(ldrExc);
    Serial.print("Medida regular: ");
    Serial.println(ldrReg);
    Serial.print("Medida baja: ");
    Serial.println(ldrLow);
    Serial.println();

    delay(1000);
  }

  promedioExc = ldrExcTotal / factorPromedio;
  promedioReg = ldrRegTotal / factorPromedio;
  promedioLow = ldrLowTotal / factorPromedio;

  mostrarMedidas(promedioExc, promedioReg, promedioLow);
  
  medida m = {promedioExc, promedioReg, promedioLow};
  return m;
}

void reportarEstado(String estado){
  
  gsm_sendhttp(idSensor, estado);
  gsm_recall(operadora);
  
  delay(10*seconds);
  gsm_recall(operadora);
}

void mostrarMedidas(int value1, int value2, int value3){
  Serial.print("Lectura Excelente: ");
  Serial.println(value1);
  Serial.println();
  Serial.print("Lectura regular: ");
  Serial.println(value2);
  Serial.println();
  Serial.print("Lectura baja: ");
  Serial.println(value3);
  Serial.println(" ------------ ");

  delay(500);
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

void gsm_sendhttp(int value1, String value2){
  
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
  
  String PostData = "data0="+apiKey+"&data1="+String(value1)+"&data2="+value2;
  
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
