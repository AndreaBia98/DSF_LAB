void dati_managment(float ricevuti[]){
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