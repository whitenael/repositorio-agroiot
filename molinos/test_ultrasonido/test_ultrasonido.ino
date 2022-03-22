const float desc_alras = 30; // variable a confirmar
const float alto_tanque = 100;
const float diam_tanque = 560;

const double pi = 3.14159;

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

  float vol_cm3 = pi * (diam_tanque * diam_tanque) / 4 * alto_tanque;
  float vol_lit  = vol_cm3 / 1000;
  float auxiliar = vol_lit; //aux = 3141,6 Litros
  float llenado = 100.00;

  // calcular el llenado real

  vol_cm3 = pi * (diam_tanque * diam_tanque) / 4 * (alto_tanque - (distancia - desc_alras));
  vol_lit  = vol_cm3 / 1000;         
  llenado = vol_lit * 100.00 / auxiliar; 
  
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

  for (int muestras = 1; muestras <= 50; muestras++){
    
    total_medida = total_medida + medida;
    promedio = total_medida / muestras;
       
  }

  if (promedio < 0){return -1;}

  return promedio;
  
}
