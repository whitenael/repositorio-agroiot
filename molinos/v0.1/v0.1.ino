/* 
VERSION MOLINO V0.1 12/8/2021

*/


#include "SoftwareSerial.h"

SoftwareSerial mySerial(12,13);

unsigned long seconds = 1000L; 
unsigned long minutes = seconds * 60;
unsigned long hours = minutes * 60;

unsigned long tiempo1 = 0;
unsigned long tiempo2 = 0;
unsigned long tiempoSegundos = 0;

String apn   = "igprs.claro.com.ar";
String apn_u = "clarogprs";
String apn_p = "clarogprs999";

String apiKey   = "2NE9RSVZXWL2X2GB";                 

String url = "34.69.212.37/agroiot/post.php";

const float desc_alras = 0.00;      /* CONSTANTES DEL CLIENTE*/
const float alto_tanque = 100;      /* EXPRESARLAS EN CMS */
const float diam_tanque = 200;      /* FIN CONSTANTES DEL CLIENTE*/

const float pi = 3.14159;   
                                                    // Arduino  Sensor
                                                    // *********************
const int trigPin = 6;                              // D06      TRIGGER (HC-SR04)
const int echoPin = 7;                              // D07      ECHO (HC-SR04)
                                                    // +5V      VCC (HC-SR04)
                                                    // GND      GND (HC-SR04)     

float duracion, lectura;
                                                    // Arduino  Sensor
                                                    // *********************
const int a1 = 9;                                   // D09      AL AGUA CON GND
const int a2 = 10;                                  // D10      AL AGUA CON GND
const int a3 = 11;                                  // D11      AL AGUA CON GND
const int a4 = 12;                                  // D12      AL AGUA CON GND

float pminimo = 25.00;  // porcentaje_minimo a controlar


void setup() {

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);  

  Serial.begin(9600);
  tiempo1 = millis();

  mySerial.begin(9600);
  mySerial.listen();  
  gsm_init();
}

void loop() {

  tiempo2 = millis();

  /*1800000 milisegundos = 30 minutos
   * 300000 milisegundos = 5 minutos
   */

   if(tiempo2 > (tiempo1+10)){  
    
    tiempo1 = millis(); //Actualiza el tiempo actual
    
    tiempoSegundos = tiempo1/1000;
    
    delay(2*seconds);

    float pt1 = 0.00;  // porcentaje_tanque_1
  
    float pa1 = 0.00;  // porcentaje_aguada_1
    float pa2 = 0.00;  // porcentaje_aguada_2
    float pa3 = 0.00;  // porcentaje_aguada_3
    float pa4 = 0.00;  // porcentaje_aguada_4
    
//emite pulso
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);

    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);

    digitalWrite(trigPin, LOW);     

    duracion = pulseIn(echoPin, HIGH);
    lectura = (duracion*.0343)/2;

    // calcular el volumen de tanque lleno
  
    float vol_cm3 = pi * (diam_tanque * diam_tanque) / 4 * alto_tanque;
    float vol_lit  = vol_cm3 / 1000;
    float auxiliar = vol_lit; //aux = 3141,6 Litros
    float llenado = 100.00;

    // calcular el llenado real
  
    vol_cm3 = pi * (diam_tanque * diam_tanque) / 4 * (alto_tanque - (lectura - desc_alras));
    vol_lit  = vol_cm3 / 1000;         
    llenado = vol_lit * 100.00 / auxiliar; 

    pt1 = llenado;

    pinMode(a1, INPUT_PULLUP);  
    pinMode(a2, INPUT_PULLUP);  
    pinMode(a3, INPUT_PULLUP);  
    pinMode(a4, INPUT_PULLUP);  

    // HIGH =  no tiene agua la altura
    // LOW  =  tiene agua la altura
    
    if (digitalRead(a1) == LOW){
       pa1 = pminimo;
      }
    if (digitalRead(a2) == LOW){
      pa2 = pminimo;
      }
    if (digitalRead(a3) == LOW){
      pa3 = pminimo;
      }    
    if (digitalRead(a4) == LOW){
      pa4 = pminimo;
      }       
/*
    Serial.println((pt1));
    Serial.println(digitalRead(a1));
    Serial.println(digitalRead(a2));
    Serial.println(digitalRead(a3));
    Serial.println(digitalRead(a4));
    Serial.println((pa1));
    Serial.println((pa2));
    Serial.println((pa3));
    Serial.println((pa4));
*/        
    pinMode(a1, OUTPUT);  
    pinMode(a2, OUTPUT);  
    pinMode(a3, OUTPUT);  
    pinMode(a4, OUTPUT);  
  
    digitalWrite(a1,LOW);
    digitalWrite(a2,LOW);
    digitalWrite(a3,LOW);
    digitalWrite(a4,LOW);
   
    gsm_sendhttp(12,pt1); // tipo_sensor_5
    gsm_recall();
    delay(10*seconds);
    
    gsm_sendhttp(13,pa1); // tipo_sensor_6
    gsm_recall();
    delay(10*seconds);
    
    gsm_sendhttp(14,pa2); // tipo_sensor_6
    gsm_recall();
    delay(10*seconds);

    gsm_sendhttp(15,pa3); // tipo_sensor_6
    gsm_recall();
    delay(10*seconds);    

    gsm_sendhttp(16,pa4); // tipo_sensor_6
    gsm_recall();
    delay(10*seconds);
    
    gsm_out();

  }
}

//************************************************************ gsm_init
void gsm_init(){

  Serial.println("Inicializando modem ...");
  
  mySerial.println("AT+CFUN=1,1");
  print_gsm_status();
  delay(5*seconds);
  
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
  delay(10000);
  print_gsm_status();
  delay(10000);
  mySerial.println("AT+SAPBR=2,1");
  print_gsm_status();
  delay(10000);

  mySerial.println("AT");
  print_gsm_status();
  delay(2000);
  
}

//************************************************************ gsm_sendhttp
void gsm_sendhttp(int value1, float value2) {
 
  Serial.println("Iniciando HTTP ... (" + String(value1) + ")");
  
  mySerial.println("AT+HTTPINIT");
  print_gsm_status();
  delay(100);
  
  mySerial.println("AT+HTTPPARA=CID,1");
  print_gsm_status();
  delay(100);
  
  mySerial.println("AT+HTTPPARA=URL," + url);
  print_gsm_status();
  delay(100);
  
  mySerial.println("AT+HTTPPARA=CONTENT,application/x-www-form-urlencoded");
  print_gsm_status();
  delay(100);

  String PostData = "data0="+apiKey+"&data1="+String(value1)+"&data2="+String(value2,3);
  
  mySerial.println("AT+HTTPDATA=" + String(PostData.length()) + ",10000");
  print_gsm_status();
  delay(100);
  
  mySerial.println(PostData);
  print_gsm_status();
  delay(10000);
  mySerial.println("AT+HTTPACTION=1");
  print_gsm_status();
  delay(5000);
  mySerial.println("AT+HTTPREAD");
  print_gsm_status();
  delay(100);
  mySerial.println("AT+HTTPTERM");
  print_gsm_status(); 
  delay(100);

  Serial.println(" ");
  Serial.println("Finalizando HTTP ... (" + String(value1) + ")");
 
}

//************************************************************ print_gsm_status
void print_gsm_status() {
  while (mySerial.available()) {
    Serial.write(mySerial.read());
  }
}

//************************************************************ gsm_out
void gsm_out(){

  Serial.println("Finalizando conexión GPRS ...");
  mySerial.println("AT+CGATT=0"); 
  print_gsm_status();
  delay(1000);

}

void gsm_recall(){

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
