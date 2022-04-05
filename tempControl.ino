// Isaac Gallegos -> Sistema control de temperatura
// Agregando las librerías necesarias
#include <LCD_I2C.h>

LCD_I2C lcd(0x20, 16, 2);

// Creando las variables necesarias para la temperatura dada por el sensor y la establecida por el potenciómetro
int valor;
float voltaje=0.0;
int temperatura;
int tempFija;

// Creando variables e indicando pines para el motor DC
const int pinENA = 6;
const int pinIN1 = 7;
const int pinIN2 = 8;
int speed; //velocidad de giro 80% (200/255)

//Variable para leer datos dados por el potenciómetro
int infoPot;

// Almacenando el estado y modo del sistema en variables para control
String estado = "Auto";
String modo;

// Declarando pines del switch para calefacción, Aire Acondicionado y estado de sistema
int switchCal = 12;
int switchAire = 11;
int switchStatus = 10;

//Variables extra para el sistema
float diferencia;
int intensidadLed; 

void setup() {
  // Inicializando las variables y dispositivos
  lcd.begin();
  lcd.backlight();
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  pinMode(pinIN1, OUTPUT);
  pinMode(pinIN2, OUTPUT);
  pinMode(pinENA, OUTPUT);

}

void loop() {
  // Obteniendo y calculando la temperatura dada por el sensor LM35
  valor=analogRead(A0);
  voltaje=(5.0/1023.0)*valor;
  temperatura=100*voltaje;
  infoPot = analogRead(A1); // Leyendo datos del potenciómetro 
  if (digitalRead(switchStatus) == HIGH) { //Si el switch está levantado indica el estado en automático
    estado = "Auto";
    tempFija = map(infoPot, 0, 1023, -55, 150); // Mapeando los datos dados por el potenciómetro, para obtener un dato de entre -55 a 150 (similares a los dados por el sensor LM35)

    //Comprobando si la temperatura dada por el LM35 es menor a la temperatura establecida por el Potenciómetro para encender Calefacción (LED) 
    if (temperatura < tempFija) {
      modo = "Cal";
      diferencia = tempFija - temperatura;
        if (diferencia >= 5 && diferencia < 8) {
        intensidadLed = map(50, 0, 100, 0, 255); //Mapeando el porcentaje que se dará al LED para obtener un dato de entre 0 a 255
        }
        if (diferencia >= 8 && diferencia < 10) {
        intensidadLed = map(75, 0, 100, 0, 255);
        }
        if (diferencia >= 10) {
        intensidadLed = 255;
        }
    //Mandándole los datos obtenidos antes al LED y al MOTOR
      digitalWrite(pinIN1, LOW);
      digitalWrite(pinIN2, LOW);
      analogWrite(pinENA, 0);
      digitalWrite(13, intensidadLed);
    }
    //Comprobando si la temperatura dada por el LM35 es mayor a la temperatura establecida por el Potenciómetro para encender Aire Acondicionado (MOTOR)
    if (temperatura > tempFija) {
      diferencia = temperatura - tempFija;
      modo = "AA";
      if (diferencia >= 5 && diferencia < 8) {
        speed = map(50, 0, 100, 0, 255); //Mapeando el porcentaje que se dará al MOTOR para obtener un dato de entre 0 a 255
      }
      if (diferencia >= 8 && diferencia < 10) {
        speed = map(75, 0, 100, 0, 255);
      }
      if (diferencia >= 10) {
        speed = 255;
      }
//Mandándole los datos obtenidos antes al LED y al MOTOR
      digitalWrite(pinIN1, HIGH);
      digitalWrite(pinIN2, LOW);
      analogWrite(pinENA, speed);
      digitalWrite(13, LOW);
    }
    //Comprobando si la diferencia entre las temperaturas es menor que los 5 grados requeridos para apagar ambos dispositivos
    if (temperatura - tempFija >= 0 && temperatura - tempFija <= 4 || tempFija - temperatura >= 0 && tempFija - temperatura <= 4) {
        modo = "Off";
        digitalWrite(pinIN1, LOW);
        digitalWrite(pinIN2, LOW);
        analogWrite(pinENA, 0);
        digitalWrite(13, LOW);
    }
  }

  if (digitalRead(switchStatus) == LOW) {//Switch accionado, lo cual encenderá el modo MANUAL
    estado = "Man";
    tempFija = map(infoPot, 0, 1023, -55, 150); //Mapeando los datos del potenciómetro nuevamente
      if (digitalRead(switchCal) == LOW && digitalRead(switchAire) == HIGH) { //Condicionando estados de ambos Switch para encender Calefacción al 100% 
        modo = "Cal";
        digitalWrite(13, HIGH);
        digitalWrite(pinIN1, LOW);
        digitalWrite(pinIN2, LOW);
        analogWrite(pinENA, 0);
      }
      if (digitalRead(switchAire) == LOW && digitalRead(switchCal) == HIGH) { //Condicionando ambos Switch para encender Aire acondicionado al 100%
        modo = "AA";
        digitalWrite(13, LOW);
        digitalWrite(pinIN1, HIGH);
        digitalWrite(pinIN2, LOW);
        analogWrite(pinENA, 255);
      }
      if (digitalRead(switchAire) == HIGH && digitalRead(switchCal) == HIGH) { //Condicionando ambos Switch para Apagar ambos modos, si ambos están levantados
        modo = "Off";
        digitalWrite(13, LOW);
        digitalWrite(pinIN1, LOW);
        digitalWrite(pinIN2, LOW);
        analogWrite(pinENA, 0);
      }
      if (digitalRead(switchAire) == LOW && digitalRead(switchCal) == LOW) { //Condicionando ambos Switch para Apagar ambos modos, si ambos están accionados
        modo = "Off";
        digitalWrite(13, LOW);
        digitalWrite(pinIN1, LOW);
        digitalWrite(pinIN2, LOW);
        analogWrite(pinENA, 0);
      }
  }
  //Imprimiendo los datos obtenidos en el LCD
  lcd.print("Est:"+estado+ " Tmp:"+String(temperatura));
  lcd.setCursor(0, 1);
  lcd.print("Mod:" +modo + " TmpD:"+tempFija);
  delay(500);

  for (int i = 0; i < 5; ++i) {
      lcd.backlight();
      delay(50);
      lcd.noBacklight();
      delay(50);
  }

  lcd.backlight();
  lcd.clear();
  delay(500);
}
