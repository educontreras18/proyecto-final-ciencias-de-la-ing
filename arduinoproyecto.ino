#include <LiquidCrystal.h>
#include <SoftwareSerial.h>


// === Configuración del LCD Keypad Shield ===
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

int adc_key_in  = 0;

// === Pines del sensor ultrasónico ===
const int trigPin = 19;
const int echoPin = 18;

// === Pin del relevador ===
const int relayPin = 2;

// === Variables de medición ===
long duration;
float zero = 20.4, tempzero = 20.4;
float pos_actual = 0;
float pos_anterior = 0;
float vel_actual = 0;
float vel_anterior = 0;
float p0 = 0;
float p1 = 0;
float p2 = 0;
float p3 = 0;
int test_run = 0;
int output = 0; 

// === Variables de configuración (en ms) ===
int test = 0;
unsigned long T1 = 500;   // Tiempo de encendido del relevador
unsigned long T2 = 4500;   // Tiempo total de la prueba
unsigned long Ts = 50;     // Tiempo de muestreo
float Ts_sec = 0.05, tempTs_sec = 0.05;
unsigned long tempT1, tempT2, tempTs;
unsigned long tStart;
unsigned long TIC = 0, TOC = 0; 
unsigned long ttest = 0;

// === Máquina de estados ===
enum State {
  IDLE,
  CONFIG1,
  CONFIG2,
  CONFIG3,
  CONFIG4,
  SAVE,
  SAVEOK,
  CONFIRM_INICIO,
  PRUEBA0,
  PRUEBA1,
  PRUEBA2,
  FINPRUEBA
};

State estado = IDLE;

int opcion = 0;  // para menús de Sí/No

// ---------------------------------------------------------
int read_LCD_buttons()  
{ 
  adc_key_in = analogRead(0);
  if (adc_key_in > 900) return btnNONE;  
  if (adc_key_in < 50)   return btnRIGHT; 
  if (adc_key_in < 250)  return btnUP;
  if (adc_key_in < 350)  return btnDOWN;
  if (adc_key_in < 450)  return btnLEFT;
  if (adc_key_in < 850)  return btnSELECT; 
  return btnNONE;  
}

// ---------------------------------------------------------
void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  
  lcd.setCursor(3,0);
  lcd.print("Test bench");
  lcd.setCursor(0,1);
  lcd.print("Ver 1.0 - Nov 25");
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH);
  
  delay(3000);
  lcd.clear();
  
  lcd.setCursor(0,0);
  lcd.print("Mechanics");
  lcd.setCursor(0,1);
  lcd.print("PIA Thursday V1");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Professor:");
  lcd.setCursor(0,1);
  lcd.print("Dr. R. Morales");
  delay(3000);
  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Team #10");
  delay(3000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Team members");
  delay(3000);

  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("MJPM");
  lcd.setCursor(0,1);
  lcd.print("2197611");
  delay(3000);
  
  lcd.clear(); 
  lcd.setCursor(1,0);
  lcd.print("DATT");
  lcd.setCursor(1,1);
  lcd.print("2209142");
  delay(3000);
  
  lcd.clear(); 
  lcd.setCursor(2,0);
  lcd.print("SGG");
  lcd.setCursor(2,1);
  lcd.print("2182407");
  delay(3000);
  
  lcd.clear(); 
  lcd.setCursor(3,0);
  lcd.print("RMTC");
  lcd.setCursor(3,1);
  lcd.print("2208662");
  delay(3000);
  
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("Initializing...");
  lcd.setCursor(0,1);
  lcd.write(byte(5));
  delay(200);
  lcd.setCursor(1,1);
  lcd.write(byte(5));
  delay(200);
  lcd.setCursor(2,1);
  lcd.write(byte(5));
  delay(200);
  lcd.setCursor(3,1);
  lcd.write(byte(5));
  delay(200);
  lcd.setCursor(4,1);
  lcd.write(byte(5));
  delay(200);
  lcd.setCursor(5,1);
  lcd.write(byte(5));
  delay(200);
  lcd.setCursor(6,1);
  lcd.write(byte(5));
  delay(200);
  lcd.setCursor(7,1);
  lcd.write(byte(5));
  delay(200);
  lcd.setCursor(8,1);
  lcd.write(byte(5));
  delay(200);
  lcd.setCursor(9,1);
  lcd.write(byte(5));
  delay(200);
  lcd.setCursor(10,1);
  lcd.write(byte(5));
  delay(200);  
  lcd.setCursor(11,1);
  lcd.write(byte(5));
  delay(200);
  lcd.setCursor(12,1);
  lcd.write(byte(5));
  delay(200);  
  lcd.setCursor(13,1);
  lcd.write(byte(5));
  delay(200);
  lcd.setCursor(14,1);
  lcd.write(byte(5));
  delay(200);  
  lcd.setCursor(15,1);
  lcd.write(byte(5));
  delay(200);
  lcd.setCursor(16,1);
  lcd.write(byte(5));
  delay(200);  

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("System Ready");
  delay(3000);    
  
  lcd.clear(); 
}

// ---------------------------------------------------------
void loop() {

  TIC = millis();
  int button = read_LCD_buttons();

  switch (estado) {

    // ==================== IDLE ====================
    case IDLE:
      lcd.setCursor(2,0);
      lcd.print("Mode:WAITING");
      lcd.setCursor(0,1);
      lcd.print("L:CONFIG   R:RUN");

      if (button == btnLEFT) {
        tempT1 = T1;
        tempT2 = T2;
        tempTs = Ts;
        tempTs_sec = Ts_sec;
        tempzero = zero;
        estado = CONFIG1;
        lcd.clear();
        delay(200);
      } 
      else if (button == btnRIGHT) {
        estado = CONFIRM_INICIO;
        lcd.clear();
        delay(200);
      }
      break;

    // ==================== CONFIG1 ====================
    case CONFIG1:
      lcd.setCursor(0,0);
      lcd.print("T_on: ");
      lcd.print(tempT1);
      lcd.print(" ms   ");
      lcd.setCursor(0,1);
      lcd.print("SEL->OK    UP/DN");
      
      if (button == btnUP)   { tempT1 += 100; delay(150); }
      if (button == btnDOWN) { if(tempT1 > 100) tempT1 -= 100; delay(150); }
      if (button == btnSELECT) { estado = CONFIG2; lcd.clear(); delay(200); }
      break;

    // ==================== CONFIG2 ====================
    case CONFIG2:
      lcd.setCursor(0,0);
      lcd.print("T_off: ");
      lcd.print(tempT2);
      lcd.print(" ms   ");
      lcd.setCursor(0,1);
      lcd.print("SEL->OK    UP/DN");
            
      if (button == btnUP)   { tempT2 += 100; delay(150); }
      if (button == btnDOWN) { if(tempT2 > 100) tempT2 -= 100; delay(150); }
      if (button == btnSELECT) { estado = CONFIG3; lcd.clear(); delay(200); }
      break;

    // ==================== CONFIG3 ====================
    case CONFIG3:
      lcd.setCursor(0,0);
      lcd.print("Sampling: ");
      lcd.print(tempTs);
      lcd.print(" ms   ");
      lcd.setCursor(0,1);
      lcd.print("SEL->OK    UP/DN");
      
      
      if (button == btnDOWN) { if (tempTs > 1) { tempTs -= 1; tempTs_sec = (tempTs*0.001); delay(150); }}
      if (button == btnUP)   { if (tempTs > 1) { tempTs += 1; tempTs_sec = (tempTs*0.001); delay(150); }}
      if (button == btnSELECT) { estado = CONFIG4; opcion = 0; lcd.clear(); delay(200); }
      
      break;

    // ==================== CONFIG4 ====================
    case CONFIG4:
      lcd.setCursor(0,0);
      lcd.print("Calibrate: ");
      lcd.print(pos_actual);
      lcd.print(" cm   ");
      lcd.setCursor(0,1);
      lcd.print("SEL->OK    UP/DN");
      
      if (button == btnUP)   { tempzero += 0.1; delay(150); }
      if (button == btnDOWN) { tempzero -= 0.1; delay(150); }
      if (button == btnSELECT) { estado = SAVE; opcion = 0; lcd.clear(); delay(200); }
      
      break;
 

    // ==================== SAVE ====================
    case SAVE:
      lcd.setCursor(0,0);
      lcd.print("Save config?");
      lcd.setCursor(13,0);
      lcd.print(opcion == 0 ? "Yes " : "No ");
      lcd.setCursor(0,1);
      lcd.print("SEL->OK    UP/DN");

      if (button == btnUP || button == btnDOWN) { opcion = !opcion; delay(150); }
      if (button == btnSELECT) {
        if (opcion == 0) estado = SAVEOK;
        else estado = IDLE;
        lcd.clear();
        delay(200);
      }
      break;

    // ==================== SAVEOK ====================
    case SAVEOK:
      T1 = tempT1;
      T2 = tempT2;
      Ts = tempTs;
      Ts_sec = tempTs_sec;
      zero = tempzero;
      lcd.setCursor(0,0);
      lcd.print("Saved");
      delay(1500);
      lcd.clear();
      estado = IDLE;
      break;

    // ==================== CONFIRM_INICIO ====================
    case CONFIRM_INICIO:
      lcd.setCursor(0,0);
      lcd.print("Start test?");
      lcd.setCursor(13,0);
      lcd.print(opcion == 0 ? "Yes " : "No ");
      lcd.setCursor(0,1);
      lcd.print("SEL->OK    UP/DN");
      
      if (button == btnUP || button == btnDOWN) { opcion = !opcion; delay(150); }
      if (button == btnSELECT) {
        if (opcion == 0) {
          estado = PRUEBA0;
          lcd.clear();
        } else {
          estado = IDLE;
          lcd.clear();
        }
        delay(200);
      }
      break;

    // ==================== PRUEBA0 ====================
    case PRUEBA0: 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Running test...");
      lcd.setCursor(0,1);
      lcd.print("Prueba 1/2");
      digitalWrite(relayPin, LOW);
      test = test + 1;
      test_run = 1;
      output = 1;
      tStart = millis();
      estado = PRUEBA1;
      break;

      
    case PRUEBA1: 
      if (ttest >= T1) {
        output = 0;
        digitalWrite(relayPin, HIGH);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Running test...");
        lcd.setCursor(0,1);
        lcd.print("Part 2/2");
        estado = PRUEBA2;
      }
      break;
      
    case PRUEBA2:
      if (ttest >= T1 + T2) {
        test_run = 0;
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Done.");
        lcd.setCursor(0,1);
        lcd.print("Press SELECT.");
        estado = FINPRUEBA;
      }
      break;

     case FINPRUEBA: 
      if (button == btnSELECT) {lcd.clear(); estado = IDLE;}   
      break;
  }

  if (estado == PRUEBA1 || estado == PRUEBA2 ) {

  // Medición ultrasónica
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  
  p0 = (zero - (duration * 0.034 / 2));
  pos_actual = (p0 + p1 + p2 + p3) / 4;
  p3 = p2;
  p2 = p1;
  p1 = p0;

  vel_actual = (pos_actual - pos_anterior)/(Ts_sec);
  pos_anterior = pos_actual;
  
  ttest = (millis() - tStart);

  Serial.print(ttest*0.001);
  Serial.print(",");
  Serial.print(pos_actual);
  Serial.print(",");
  Serial.print(vel_actual);
  Serial.print(",");
  Serial.print(output);
  Serial.print(",");
  Serial.println(test);      

  }
  else if (estado == CONFIG4) {

  // Medición ultrasónica
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  
  p0 = (tempzero - (duration * 0.034 / 2));
  pos_actual = (p0 + p1 + p2 + p3) / 4;
  p3 = p2;
  p2 = p1;
  p1 = p0;
  }
  else 
  {
  pos_actual = 0;
  p3 = 0;
  p2 = 0;
  p1 = 0;
  vel_actual = 0;
  pos_anterior = 0;
  }
  TOC = millis() - TIC;

  if (Ts >= TOC) {delay(Ts-TOC);}
  
