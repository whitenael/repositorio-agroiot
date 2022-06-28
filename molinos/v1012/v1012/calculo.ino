// #### INICIO VARIABLES PARA EL CALCULO DE MEDIDA #### //

// MEDIDAS DE TODOS LOS TANQUES POR ID (en cm) // 
/*
  ID 67:
  - alto : 100
  - diametro : 560
  ID 68
  - alto : 100
  - diametro : 1012
  ID 69
  - alto : 100
  - diametro : 760
  ID 70
  - alto : 100
  - diametro : 760
*/

const float desc_alras = 40; // medida al pelo del agua
const float alto_tanque = 100; // medidas del tanque
const float diam_tanque = 755;

float vol_cm3;
float vol_lit;
float auxiliar;
float llenado;

const double pi = 3.14159;

// La funcion calcularLlenado recibe la distancia medida y devuelve el porcentaje de llenado

float calcularLlenado(float distancia){
  
  // Primero calcula el volumen del tanque (segun medidas)

  vol_cm3 = pi * (diam_tanque * diam_tanque) / 4 * alto_tanque;
  vol_lit  = vol_cm3 / 1000;
  auxiliar = vol_lit;
  llenado = 100.00;

  // Calcular el llenado real utilizando la distancia recolectada

  vol_cm3 = pi * (diam_tanque * diam_tanque) / 4 * (alto_tanque - (distancia - desc_alras));
  vol_lit  = vol_cm3 / 1000;         
  llenado = vol_lit * 100.00 / auxiliar; 

  return llenado; //Se devuelve el porcentaje de llenado
}
