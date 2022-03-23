const int PinTrig = 2;
const int PinEcho = 3;

// Constante velocidad sonido en cm/s
const float VelSon = 34000.0;

float distancia;

void setup()
{
  // Iniciamos el monitor serie para mostrar el resultado
  Serial.begin(9600);
  // Ponemos el pin Trig en modo salida
  pinMode(PinTrig, OUTPUT);
  // Ponemos el pin Echo en modo entrada
  pinMode(PinEcho, INPUT_PULLUP);
}
void loop()
{
  iniciarTrigger();
  
  // La función pulseIn obtiene el tiempo que tarda en cambiar entre estados, en este caso a HIGH
  unsigned long tiempo = pulseIn(PinEcho, HIGH);
  
  // Obtenemos la distancia en cm, hay que convertir el tiempo en segudos ya que está en microsegundos
  // por eso se multiplica por 0.000001
  distancia = tiempo * 0.000001 * VelSon / 2.0;
  
  Serial.print(promedio(distancia));
  Serial.print("cm");
  Serial.println();
  delay(100);
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

  for (int muestras = 1; muestras <= 100; muestras++){
    
    total_medida = total_medida + medida;
    promedio = total_medida / muestras; 
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
