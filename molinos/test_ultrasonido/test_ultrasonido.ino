#include <NewPing.h>

unsigned long seconds = 1000L; //unsigned = solo almacena numeros positivos
unsigned long minutes = seconds * 60;
unsigned long hours = minutes * 60;

const float desc_alras = 50; // variable a confirmar
const float alto_tanque = 200;
const float diam_tanque = 440;

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

  float distanciaChequeada;

  microSeconds = sonar.ping_median(50);
  distancia = sonar.convert_cm(microSeconds);
  distanciaChequeada = chequearMedida(distancia);

  if (distanciaChequeada == -1){llenado=-1;}

  else{llenado = calcularLlenado(distanciaChequeada);}

  Serial.println("########## RESULTADOS: ##########");
  Serial.print(llenado);
  Serial.print("%");
  Serial.println();
  delay(1000);

}

// Chequeamos que la medida este dentro de un rango valido

// Caso contrario, hacer una nueva medida

float chequearMedida(float medida){

  Serial.print("Medida actual: ");
  Serial.println(medida);
  
  int counter = 0;
  int datoNuevo = medida;

  if (datoNuevo < desc_alras | datoNuevo > (alto_tanque + desc_alras))
  {  
    while ((counter < 10) & (datoNuevo < desc_alras | datoNuevo > (alto_tanque + desc_alras)))
    {
      Serial.print("Retomando muestra intento #");
      Serial.print(counter);
      Serial.println();
      microSeconds = sonar.ping_median(50);
      datoNuevo = sonar.convert_cm(microSeconds);
      Serial.println(datoNuevo);
      
      counter++;
      delay(200);
    }

    if (counter == 10){return -1;}
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
