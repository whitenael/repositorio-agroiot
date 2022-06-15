#include <NewPing.h>

unsigned long seconds = 1000L; //unsigned = solo almacena numeros positivos
unsigned long minutes = seconds * 60;
unsigned long hours = minutes * 60;

const float desc_alras = 40; // variable a confirmar
const float alto_tanque = 100;
const float diam_tanque = 560;

struct medida{
  float value;
  int validas;
};

float vol_cm3;
float vol_lit;
float auxiliar;
float llenado;

const double pi = 3.14159;

const int PinTrig = 6;
const int PinEcho = 7;

NewPing sonar(PinTrig, PinEcho);

float distancia;
int microSeconds;

void setup()
{
  // Iniciamos el monitor serie para mostrar el resultado
  Serial.begin(9600);

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
  delay(100);

}

// Tenemos que tomar un total de 12 medidas: 4 por minuto 
// De estas 12 medidas, descartamos las invalidas, y nos quedamos con las validas y las sumamos a un contador
// De estas validas, calculamos un promedio y lo devolvemos


medida tomarMedida(){
  int tiempo = 0;
  float datoNuevo;
  float totalValidas = 0;
  float promedio;
  int contadorValidas = 0; 
  boolean esValida = (datoNuevo > desc_alras | datoNuevo < (alto_tanque + desc_alras));
  
  while (tiempo < 3){
    microSeconds = sonar.ping_median(10);
    distancia = sonar.convert_cm(microSeconds);
    datoNuevo = distancia;
    if (esValida){
       contadorValidas++;
       totalValidas = totalValidas + datoNuevo;
    }
    tiempo++;
    delay(15*seconds);
  }
  promedio = totalValidas / contadorValidas;

  medida m = {promedio, contadorValidas};

  return m;
}
