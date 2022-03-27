#include <EEPROM.h>

const int PinTrig = 2;
const int PinEcho = 3;

// Constante velocidad sonido en cm/s
const float VelSon = 34000.0;

float distancia;
int eeAdress = 0;
float cont[10] = {0,0,0,0,0,0,0,0,0,0,0}; // el ultimo bit sirve de referencia {inicio[0], dato, dato, dato , dato, dato, dato, dato, dato, dato, ref[0]}

void setup()
{
  // Iniciamos el monitor serie para mostrar el resultado
  Serial.begin(9600);
  // Ponemos el pin Trig en modo salida
  pinMode(PinTrig, OUTPUT);
  // Ponemos el pin Echo en modo entrada
  pinMode(PinEcho, INPUT_PULLUP);

  Serial.println("Seteando memoria.. ");

  for (int i = 0; i < EEPROM.length(); i++){
    EEPROM.write(i,0); // seteamos la memoria EEPROM a 0
  }

  Serial.println("Memoria seteada");
}
void loop()
{
  iniciarTrigger();
  
  // La función pulseIn obtiene el tiempo que tarda en cambiar entre estados, en este caso a HIGH
  unsigned long tiempo = pulseIn(PinEcho, HIGH);
  
  // Obtenemos la distancia en cm, hay que convertir el tiempo en segudos ya que está en microsegundos
  // por eso se multiplica por 0.000001
  distancia = tiempo * 0.000001 * VelSon / 2.0;
  
  if (eeAdress >= EEPROM.length()) {eeAdress = 0;}  

  // escribir dato en una posicion de memoria

  EEPROM.put(eeAdress, distancia); //grabamos
  float medida = EEPROM.get(eeAdress, distancia); //retornamos

  eeAdress += sizeof(float); //movemos una posicion de memoria
  
  // iterar toda la memoria (de 4 bits en 4 bits)

  setearMedidas(cont, medida, eeAdress);

  // a cada posicion asignarle una posicion en el array cont

  delay(500);
}


// Método que inicia la secuencia del Trigger para comenzar a medir
void iniciarTrigger()
{
  // Ponemos el Triiger en estado bajo y esperamos 2 ms
  digitalWrite(PinTrig, LOW);
  delayMicroseconds(6);
  
  // Ponemos el pin Trigger a estado alto y esperamos 10 ms
  digitalWrite(PinTrig, HIGH);
  delayMicroseconds(10);
  
  // Comenzamos poniendo el pin Trigger en estado bajo
  digitalWrite(PinTrig, LOW);
}

float promedio(float medida){

  float promedio = 0;
  float total_medida = 0;
  int counter = 0;

  for (int muestras = 1; muestras <= 10; muestras++){
    
    total_medida = total_medida + medida;
    promedio = total_medida / muestras; 
    Serial.print("Medida en loop: #");
    Serial.println(muestras);
    Serial.print(medida);
    Serial.println();
    delay(200);
  }

  if (promedio < 0 || promedio > 100)
  {
    int promedioNuevo;
    while (counter < 100 && ~((promedioNuevo > 0 || promedioNuevo < 100)))
    {
      Serial.print("Retomando muestra intento #");
      Serial.print(counter);
      Serial.println();
      Serial.println(medida);
      for (int muestras = 1; muestras <= 100; muestras++){
        total_medida = total_medida + medida;
        promedioNuevo = total_medida / muestras;  
      }
      counter++;
      delay(100);
    }

    if (counter == 100){return -1;}
    else{return promedioNuevo;}
  }

  return promedio;
  
}

// iterar a traves de la memoria

// cada dato en la memoria, debe ser guardado en un espacio del array

void setearMedidas(float arr[], float medida, float address){

  for (int i=0; i < 1024; i=i+4){
    Serial.print();
  }
   
}
