#include <ArduinoBLE.h>
#include "multiplexer.h"
#include <PID_v1.h>

#include <QuickPID.h>
// BLE Service e Caratteristica
BLEService ardService("917649A0-D98E-11E5-9EEC-0002A5D5C51B");
BLECharacteristic ardCharacteristic("917649A1-D98E-11E5-9EEC-0002A5D5C51B", BLERead | BLEWrite | BLENotify, 28);

// Pin e array dati PWM
#define MOT1 7
#define MOT2 10
#define VOLT A0



// Array dei valori di tensione (in Volt)
const float V[] = {
  0, 3.63, 4.35, 5.27, 6.1, 7.125, 7.95, 8.9, 9.865, 10.5,
  11.35, 12.33, 13.015, 13.78, 14.225, 15, 15.575, 16.15, 16.6, 17,
  17.68, 18.085, 18.5, 18.925, 19.175, 19.6, 19.9, 20.25, 20.6, 20.9,
  21.3, 21.7, 22.2, 22.75, 23.35, 24
};

// Array corrispondente dei valori PWM
const float pwm[] = {
  0, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500,
  1600, 1700, 1800, 1900, 2000, 2100, 2200, 2300, 2400, 2500,
  2600, 2700, 2800, 2900, 3000, 3100, 3200, 3300, 3400, 3500,
  3600, 3700, 3800, 3900, 4000, 4095
};


const int N = sizeof(V) / sizeof(V[0]);



// Variabili globali
float qnew1 = 0, qnew2 = 0;
float q1 = 0, q2 = 0;
int pwm1 = 0, pwm2 = 0;

float V1 = 0, V2 = 0;  // Valori interpolati usati per il PWM
float f1 = 0, f2 = 0;
long previousMillis = 0;


// PID MOT1


//Define the aggressive and conservative Tuning Parameters
float kp = 0.0537;
float ki = 0.8000;
float kd = 0.25;
double setpoint1, input1, pwm1_d;
PID  pid1(&input1, &pwm1_d, &setpoint1, kp, ki, kd, DIRECT);

// PID MOT2
double setpoint2, input2, pwm2_d;
PID  pid2(&input2, &pwm2_d, &setpoint2, kp, ki, kd, DIRECT);

// Array per invio dati BLE
bool pid_flag = true;
// POMPA PICCOLA
float alfa = 0.4868;
float beta = 2.427;
float dati[7];
int pwm1_set = 0;
int pwm2_set = 0;



//#include "gestione_dati.h"
void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(MOT1, OUTPUT);
  pinMode(MOT2, OUTPUT);
  pinMode(VOLT, INPUT);

  analogWrite(MOT1, 0);
  analogWrite(MOT2, 0);

  while (!BLE.begin()) {
    Serial.println("Errore BLE, ritento...");
    delay(1000);  // Aspetta un secondo prima di riprovare
  }

  BLE.begin();
  BLE.setLocalName("ArduinoARD");
  BLE.setAdvertisedService(ardService);
  ardService.addCharacteristic(ardCharacteristic);
  BLE.addService(ardService);
  BLE.advertise();

  analogWriteResolution(12);  // PWM a 12 bit

  setup_mlpx();
  meansetup();
  scan_mlpx();

  // Inizializzazione PID
  pid1.SetOutputLimits(0, 4095);  // PWM range
  pid2.SetOutputLimits(0, 4095);
  pid1.SetMode(MANUAL);
  pid2.SetMode(MANUAL);

  Serial.println("BLE attivo, in attesa di connessione...");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    digitalWrite(LED_BUILTIN, HIGH);

    while (central.connected()) {
      gestisciComandiSeriali();
      gestisciComandiBLE();


      //  questo codice ha effetto siccome non ha PID e sensori collegati. quando verranno
      if (q1 != qnew1 || q2 != qnew2) {
        q1 = qnew1;
        q2 = qnew2;
        pwm1 = calcolaPWM(qnew1, V1, q1);
        pwm2 = calcolaPWM(qnew2, V2, q2);
        analogWrite(MOT1, pwm1);
        analogWrite(MOT2, pwm2);
      }
      // Debug flussi
      for (int i = 0; i < 5; i++) {
        Serial.print(dati[i], 1);
        Serial.print(" ");
     
       }
        Serial.println();


      //IMPLEMENTARE QUESTO CODICE QUANDO SI AVRA PID

      // Setpoint e input aggiornati



      // q1 = qnew1;
      // q2 = qnew2;

      // setpoint1 = q1;
      // setpoint2 = q2;

      // input1 = flusso_scale[0];
      // input2 = flusso_scale[1];

      // pid1.Compute();
      // pid2.Compute()

      // pwm1 = constrain((int)pwm1_d * pid_flag + pwm1_set, 0, 4095);
      // pwm2 = constrain((int)pwm2_d * pid_flag + pwm2_set, 0, 4095);

      // analogWrite(MOT1, pwm1);
      // analogWrite(MOT2, pwm2);

      // double gap1 = abs(setpoint1 - input1);  //distance away from setpoint
      // if (gap1 < 1) {                         //we're close to setpoint, use conservative tuning parameters
      //   pid1.SetTunings(kp, ki, kd);
      // } else {
      //   //we're far from setpoint, use aggressive tuning parameters
      //   pid1.SetTunings(kp * 4, ki * 4, kd * 4);
      // }
      // if (setpoint1==0){
      //   //we're close to setpoint, use conservative tuning parameters
      //   pid1.SetTunings(0, 0, 0);
      // }


      // double gap2 = abs(setpoint1 - input2);  //distance away from setpoint
      // if (gap2 < 1) {                         //we're close to setpoint, use conservative tuning parameters
      //   pid2.SetTunings(kp, ki, kd);
      // } else {
      //   //we're far from setpoint, use aggressive tuning parameters
      //   pid2.SetTunings(kp * 4, ki * 4, kd * 4);
      // }
      //  if (setpoint2==0){
      //   //we're close to setpoint, use conservative tuning parameters
      //   pid2.SetTunings(0, 0, 0);
      // }

      //read_mlpx_flux(TCA_FLUX1, &flusso_scale[0], &raw_flux[0], &flusso_media[0]);
      //read_mlpx_flux(TCA_FLUX2, &flusso_scale[1], &raw_flux[1], &flusso_media[1]);




      if (millis() - previousMillis >= 100) {
       

        //massa[0]=f_massa(flusso_scale[0], massa[0],  millis() - previousMillis );
        //massa[1]=f_massa(flusso_scale[1], massa[1],  millis() - previousMillis );

        inviaDatiBLE();
        previousMillis = millis();
        //f_massa(float _flusso, float _massa_old, int dt);
      }

      delay(10);
    }

    analogWrite(MOT1, 0);
    analogWrite(MOT2, 0);
    digitalWrite(LED_BUILTIN, LOW);
  }
}




int calcolaPWM(float q, float &Vout, float &qfix) {

  // lo  voglio qua un pid con ingresso flusso_media e ilset point q
  if (q == 0.0f) {
    scan_mlpx();//reset
    Vout = 0.0f;
    return 0;
  }

  //float x = 0.57 * q + 3.52;

  //alfa= 0.626;
  //beta=2.349;
  float x = alfa * q + beta;
  Vout = x;

  if (Vout >= 24) {
    Vout = 24;
    qfix = (Vout - beta) / alfa;

    return 4095;
  }
  if (x < V[0]) x = V[0];
  if (x > V[N - 1]) x = V[N - 1];

  int i = 0;
  while (i < N - 1 && x > V[i + 1]) i++;

  float y = pwm[i] + (x - V[i]) * (pwm[i + 1] - pwm[i]) / (V[i + 1] - V[i]);
  return constrain((int)y, 0, 4095);
}



float pwm2volt(float pwm_value) {

  // Clamp del valore PWM ai limiti validi
  if (pwm_value <= pwm[0]) return V[0];
  if (pwm_value >= pwm[N - 1]) return V[N - 1];

  int i = 0;
  while (i < N - 1 && pwm_value > pwm[i + 1]) i++;

  // Interpolazione lineare inversa
  float v = V[i] + (pwm_value - pwm[i]) * (V[i + 1] - V[i]) / (pwm[i + 1] - pwm[i]);
  return v;
}

void input_managment(float ricevuti[]){
      if (ricevuti[0] == 0) {
      qnew1 = ricevuti[1];
      qnew2 = ricevuti[2];

      pwm1_set = calcolaPWM(qnew1, V1, q1);
      pwm2_set = calcolaPWM(qnew2, V2, q2);
    } 
    
    
    else if (ricevuti[0] == 1) {
      alfa = ricevuti[1];
      beta = ricevuti[2];
    } 
    
    
    else if (ricevuti[0] == 2) {
      pid_flag = true;
      kp = ricevuti[1];
      ki = ricevuti[2];
      kd = ricevuti[3];
      if (kp == 0 && ki == 0 && kd == 0) {
        pid_flag = false;
      }
    } 
    
    else if (ricevuti[0] == 3) {
      flux_scale[0] = ricevuti[1];
      flux_scale[1] = ricevuti[2];
    }
    
    else if (ricevuti[0] == -1) {
      for(int i = 0; i < 4; i++){
        inviaSettingBLE();
        delay(100);
        }
      //flux_scale[0] = ricevuti[1];
      //flux_scale[1] = ricevuti[2];
    }


}
void gestisciComandiSeriali() {
  if (Serial.available() >= 4 * sizeof(float)) {
    float ricevuti[4];
    for (int i = 0; i < 4; i++) {
      ricevuti[i] = Serial.parseFloat();
    }

    for (int i = 0; i < 4; i++) {
      Serial.print(ricevuti[i], 1);
      Serial.print(" ");
    }
    Serial.println();

    input_managment(ricevuti);
  }
}


void gestisciComandiBLE() {
  float ricevuti[5];  // Se BLE ne manda 5
  if (ardCharacteristic.written()) {
    ardCharacteristic.readValue((byte *)&ricevuti, sizeof(ricevuti));

    for (int i = 0; i < 4; i++) {
      Serial.print(ricevuti[i], 1);
      Serial.print(" ");
    }
    Serial.println();

    input_managment(ricevuti);
  }
}


void inviaDatiBLE() {
  dati[0] = analogRead(VOLT) * (3.3 / 1023.0);  // tensione di riferimento (opzionale)
  dati[1] = raw_flux[0];
  dati[2] = pwm2volt(pwm1);  //V1;
  dati[3] = raw_flux[1];
  dati[4] = pwm2volt(pwm2);
  dati[5] = input1;
  dati[6] = input2;

  ardCharacteristic.setValue((byte *)&dati, sizeof(dati));
}


void inviaSettingBLE() {
  dati[0] = alfa;  // tensione di riferimento (opzionale)
  dati[1] = beta;
  //
  dati[2] = kp;
  dati[3] = ki;
  dati[4] = kd;
  //
  dati[5] = flux_scale[0];
  dati[6] = flux_scale[1];

  ardCharacteristic.setValue((byte *)&dati, sizeof(dati));
}


