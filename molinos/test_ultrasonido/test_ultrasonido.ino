#include <NewPing.h>
#include <EEPROM.h>

const int PinTrig = 2;
const int PinEcho = 3;

NewPing sonar(PinTrig, PinEcho);

float distancia;
int microSeconds;
int eeAdress = 0;

void setup()
{
  // Iniciamos el monitor serie para mostrar el resultado
  Serial.begin(9600);

  Serial.println("Seteando memoria.. ");

  for (int i = 0; i < EEPROM.length(); i++){
    EEPROM.write(i,0); // seteamos la memoria EEPROM a 0
  }

  Serial.println("Memoria seteada");
}
void loop()
{ 
  float distanciaChequeada;
  
  microSeconds = sonar.ping_median(50);
  distancia = sonar.convert_cm(microSeconds);

  distanciaChequeada = chequearMedida(distancia);
  
  Serial.println(distanciaChequeada);

  if (eeAdress >= EEPROM.length()) {eeAdress = 0;}  

  // escribir dato en una posicion de memoria

  EEPROM.put(eeAdress, distancia); //grabamos

  eeAdress += sizeof(int); //movemos una posicion de memoria
  
  delay(500);
}

// Chequeamos que la medida este dentro de un rango valido

// Caso contrario, hacer una nueva medida

float chequearMedida(float medida){
  
  int counter = 0;

  if (medida < 0 || medida > 30)
  {
    int datoNuevo;
    while ((counter < 10) & ~((datoNuevo > 0 & datoNuevo < 30)))
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
    else{return datoNuevo;}
  }

  return medida;
  
}
