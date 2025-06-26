# Controllo Pompe Roller con Feedback da Flussimetri e Comunicazione BLE

Questo progetto Arduino implementa un sistema di controllo preciso per due pompe roller, gestite tramite segnali PWM. Il sistema è progettato per regolare la portata delle pompe basandosi su valori di portata desiderati, ricevuti via Bluetooth Low Energy (BLE) da un'applicazione esterna (es. MATLAB). La misurazione delle portate in tempo reale è assicurata da due flussimetri I2C, collegati tramite un multiplexer, e i dati possono essere utilizzati per un controllo in PID.

## Funzionalità Principali

* **Controllo PWM delle Pompe Roller:** Generazione di segnali PWM per controllare la velocità e, di conseguenza, la portata delle pompe roller.
* **Stima della Portata dal Voltaggio:** La portata desiderata per ciascuna pompa viene stimata in base al voltaggio applicato alle pompe.
* **Comunicazione BLE Bidirezionale:**
    * **Input:** Ricezione di comandi e valori di portata desiderata (`Q1`, `Q2`) da un'applicazione esterna (es. MATLAB) tramite BLE.
    * **Output:** Invio di dati e stati del sistema (es. letture dei flussimetri, parametri di calibrazione) all'applicazione esterna.
* **Lettura Flussimetri I2C:** Acquisizione in tempo reale delle portate da due flussimetri I2C.
* **Gestione Multiplexer I2C:** Utilizzo di un multiplexer I2C (TCA9548A o simile) per gestire la comunicazione con più sensori I2C.
* **Filtraggio Dati:** Implementazione di un filtro IIR e/o media mobile per migliorare la stabilità e l'accuratezza delle letture dei flussimetri.
* **Predisposizione per Controllo PID:** I dati di portata letti dai flussimetri sono disponibili per l'implementazione di un algoritmo di controllo PID, permettendo una regolazione automatica e precisa delle portate desiderate.
* **Gestione Comandi Remota:** La funzione `dati_managment` interpreta gli array di dati ricevuti via BLE per aggiornare parametri di sistema come le portate desiderate, i coefficienti di calibrazione dei flussimetri, o i parametri PID.

## Componenti Hardware Necessari

* **Scheda Arduino compatibile con Modulo Bluetooth Low Energy (BLE):** Arduino mkr 1010
* **2x Pompe Roller:** Controllabili via PWM.
* **2x Flussimetri:**  Sensirion SFLS1300F o simili.
* **Multiplexer I2C:** TCA9548A
* **Modulo Bluetooth Low Energy (BLE):** (Se la tua scheda Arduino non lo integra già - es. HC-05 se compatibile con BLE, o moduli dedicati come HM-10).
* **Driver Motore/MOSFET:** Per pilotare le pompe roller con i segnali PWM.
* **Cavi di collegamento, Breadboard, Alimentatore.**
  
## Schema di Collegamento (Esempio)

* **Arduino <-> Multiplexer I2C:** Collegare i pin SDA e SCL dell'Arduino ai rispettivi pin del multiplexer.
* **Multiplexer I2C <-> Flussimetri:** Collegare ciascun flussimetro a un canale separato del multiplexer.
* **Arduino <-> Pompe Roller:** Collegare i pin PWM dell'Arduino ai driver delle pompe.
* **Arduino <-> Modulo BLE:** (Se esterno) Collegare i pin TX/RX o i pin I2C/SPI appropriati.

*(Potresti voler aggiungere qui un link a un diagramma Fritzing o uno schema elettrico se ne hai uno.)*

## Software e Librerie

Questo progetto richiede l'Arduino IDE e le seguenti librerie:

* `Wire.h`: Per la comunicazione I2C (inclusa nell'IDE Arduino).
* `movingAvg`: Per il calcolo della media mobile dei flussi. Disponibile su GitHub: [https://github.com/JChristensen/movingAvg/tree/master](https://github.com/JChristensen/movingAvg/tree/master)
* **_(Altre librerie se utilizzate, es. per BLE o PID)_**

### Installazione delle Librerie

1.  Apri l'Arduino IDE.
2.  Vai su `Sketch > Includi Libreria > Gestisci Librerie...`
3.  Cerca "movingAvg" e installa la libreria di Jan H. (o quella specifica che usi).
4.  Installa eventuali altre librerie necessarie.

## Struttura del Progetto

* `main_sketch.ino` (o il nome del tuo file .ino principale): Lo sketch Arduino principale contenente le funzioni `setup()` e `loop()`.
* `multiplexer.h`: Gestione del multiplexer I2C e delle operazioni di base con i sensori I2C.
* `Flux_Print.h`: Funzioni per la lettura e il processamento dei dati dai flussimetri, inclusa la scalatura e la media mobile. Contiene anche logiche di stampa su seriale.
* `filtroiir.h`: Implementazione di un filtro IIR per il segnale dei flussimetri.
* `gestione_dati.h`: Funzione per l'interpretazione dei comandi e dei dati ricevuti via BLE.

## Riassunto delle Funzioni Utilizzate

Questo progetto si basa su un'architettura modulare, con diverse funzioni organizzate in file header (`.h`) per una migliore leggibilità e gestione del codice. Di seguito, un riepilogo delle funzioni principali implementate.

#### Funzioni in `multiplexer.h`

* `scan_i2c()`: Esegue una scansione del bus I2C per rilevare e stampare gli indirizzi dei dispositivi collegati. Questa funzione è estremamente utile per il debug e per verificare che i sensori siano correttamente connessi e rilevati.
* `tcaselect(uint8_t i)`: Seleziona uno degli 8 canali del multiplexer I2C (con indirizzo `TCAADDR` 0x70) per abilitare la comunicazione con il sensore collegato a quella specifica porta.
* `reset_mlpx(int flag_print)`: Tenta di eseguire un soft reset sui sensori collegati a ciascuna delle 8 porte del multiplexer I2C. Controlla la presenza di un sensore su ogni porta e stampa un messaggio di conferma o di errore sulla seriale se `flag_print` è impostato a 1.
* `setup_mlpx()`: Configura i sensori di flusso collegati al multiplexer inviando loro un comando di setup (0x36, 0x08) per avviare la misurazione continua.
* `scan_mlpx()`: Esegue una scansione dettagliata del bus I2C, stampando lo stato di ciascuna porta e verificando la corretta impostazione della modalità di misurazione continua per ogni sensore.

#### Funzioni in `Flux_Print.h`

* `meansetup()`: Inizializza gli oggetti `movingAvg` per i due flussimetri, preparandoli a calcolare la media mobile.
* `meanloop()`: Legge i valori analogici dai pin A0 e A1, calcola la media mobile e la stampa sulla seriale.
* `filterValue(int value)`: Applica una soglia al valore in ingresso, ritornando 0 se il valore assoluto è inferiore a 10.
* `read_mlpx_flux(int i, float *output1, float *output2, float *output3)`: Legge i dati di flusso grezzi dal flussimetro collegato al canale `i` del multiplexer, li scala e calcola la media mobile, salvando i valori nei puntatori passati come argomenti.
* `print_datas(int _volt1, int _volt2, float _extra1, float _extra2, float _extra3)`: Acquisisce e stampa i dati di flusso dai due flussimetri (`TCA_FLUX1` e `TCA_FLUX2`) sulla seriale, insieme ad altri parametri come voltaggi e tempo, in un formato tabulato.
* `f_massa(float _flusso, float _massa_old, int dt)`: Calcola la massa (o il volume) integrando il flusso in un intervallo di tempo `dt`.

#### Funzioni in `filtroiir.h`

* `IIR_filter(float x, float* x_hist, float* y_hist)`: Implementa un filtro IIR (Infinite Impulse Response) per il segnale in ingresso `x`. Utilizza gli array `x_hist` (storico degli ingressi) e `y_hist` (storico delle uscite) per calcolare il nuovo valore filtrato.

#### Funzioni in `gestione_dati.h`

* `dati_managment(float ricevuti[])`: Interpreta i dati ricevuti in un array di float (probabilmente via BLE), utilizzando il primo elemento (`ricevuti[0]`) come codice per l'operazione da eseguire:
    * **`0`**: Imposta nuove portate desiderate (`qnew1`, `qnew2`) e calcola i nuovi valori PWM.
    * **`1`**: Imposta i parametri di calibrazione `alfa` e `beta`.
    * **`2`**: Attiva o disattiva il controllo PID e imposta i coefficienti `kp`, `ki`, `kd`.
    * **`3`**: Aggiorna i fattori di scala (`flux_scale`) utilizzati per calibrare i flussimetri.
    * **`-1`**: Forza l'invio delle impostazioni di sistema tramite BLE.
