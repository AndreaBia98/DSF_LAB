#include <Wire.h>
#include <movingAvg.h>
#include "filtroiir.h"
// parametri i2c della
int TCA_FLUX1 = 3;
int TCA_FLUX2 = 7;

// parametri idraulici
float SCALE_FACTOR_FLOW[2] = { 500, 500 };
float flux_scale[2] = {1.139 ,1.218};//{ 0.761, 1.269 };

// Variabili output
extern bool airInLine[2];
bool highFlow[2];
uint16_t flags;

// Variabili globali
float raw_flux[2] = { 0, 0 };
float flusso_scale[2] = { 0, 0 };
float flusso_media[2] = { 0, 0 };

// Tempo
unsigned long t = 0;
unsigned long t_1 = 0;
unsigned long t0 = 0;

// Media mobile con movingAvg
const int numSamples = 5;
movingAvg fluxAvg[2] = { movingAvg(numSamples), movingAvg(numSamples) };

// Setup della media mobile
void meansetup() {
  fluxAvg[0].begin();
  fluxAvg[1].begin();
}

void meanloop() {
  float newReading1 = analogRead(A0) * (5.0 / 1023.0);
  float newReading2 = analogRead(A1) * (5.0 / 1023.0);

  float average1 = fluxAvg[0].reading(newReading1);
  float average2 = fluxAvg[1].reading(newReading2);

  Serial.print("Moving average 1: ");
  Serial.print(average1, 4);
  Serial.print(", Moving average 2: ");
  Serial.println(average2, 4);

  delay(25);
}

int filterValue(int value) {
  if (abs(value) < 10) {
    return 0;
  } else {
    return value;
  }
}

void read_mlpx_flux(int i, float *output1, float *output2, float *output3) {
  int16_t raw_signal_flux;
  float scaled_flow_value;
  float _average;

  tcaselect(i);
  Wire.requestFrom(ADDRESS, 9);

  if (Wire.available() >= 9) {
    raw_signal_flux = Wire.read() << 8;
    raw_signal_flux |= Wire.read();
    
  }else{
      raw_signal_flux=NAN;

  }

    if (i == TCA_FLUX1) {
      //raw_signal_flux = IIR_filter(raw_signal_flux, x_hist1, y_hist1);
      //raw_signal_flux *= flux_scale[0];
      scaled_flow_value = ((float)raw_signal_flux)* flux_scale[0] / SCALE_FACTOR_FLOW[0];
      _average = fluxAvg[0].reading(scaled_flow_value*1000);
    }

    if (i == TCA_FLUX2) {
      //raw_signal_flux *= flux_scale[1];
     // raw_signal_flux = IIR_filter(raw_signal_flux, x_hist2, y_hist2);
      scaled_flow_value = ((float)raw_signal_flux) * flux_scale[1]/ SCALE_FACTOR_FLOW[1];
      _average = fluxAvg[1].reading(scaled_flow_value*1000);
    }

    *output1 = scaled_flow_value;
    *output2 = raw_signal_flux;
    *output3 = float(_average)/1000;
  
}

void print_datas(int _volt1, int _volt2, float _extra1, float _extra2, float _extra3) {
  t_1 = t;
  t = float(millis()) - t0;

  read_mlpx_flux(TCA_FLUX1, &flusso_scale[0], &raw_flux[0], &flusso_media[0]);
  read_mlpx_flux(TCA_FLUX2, &flusso_scale[1], &raw_flux[1], &flusso_media[1]);

  Serial.print(_volt1); Serial.print("\t");
  Serial.print(flusso_scale[0], 3); Serial.print("\t");
  Serial.print(flusso_media[0], 3); Serial.print("\t");
  Serial.print(raw_flux[0], 3); Serial.print("\t\t");

  Serial.print(_volt2); Serial.print("\t");
  Serial.print(flusso_scale[1], 3); Serial.print("\t");
  Serial.print(flusso_media[1], 3); Serial.print("\t");
  Serial.print(raw_flux[1], 3); Serial.print("\t\t");

  Serial.print(_extra1, 3); Serial.print("\t");
  Serial.print(_extra2, 3); Serial.print("\t");
  Serial.print(_extra3, 3); Serial.print("\t");

  Serial.print(float(t) / 1000, 3); Serial.print("\t");
  Serial.print(-999); Serial.print("\t");
}

float f_massa(float _flusso, float _massa_old, int dt) {
  float _mass = _flusso * dt / (60 * 1000) + _massa_old;
  return _mass;
}
