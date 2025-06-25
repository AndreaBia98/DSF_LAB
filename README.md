# DSF_LAB
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

* **Scheda Arduino compatibile:** (Es. Arduino Nano, ESP32, etc. - *specifica il modello che usi*)
* **2x Pompe Roller:** Controllabili via PWM.
* **2x Flussimetri:** Con interfaccia I2C (es. Sensirion SFM3xxx, o compatibile).
* **Multiplexer I2C:** (Es. PCA9548A Breakout Board)
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
* `movingAvg`: Per il calcolo della media mobile dei flussi. [Link alla libreria se disponibile, es. GitHub o Arduino Library Manager]
* **(Altre librerie se utilizzate, es. per BLE o PID)**

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

## Come Usare

1.  **Carica il codice:** Apri il file `.ino` principale nell'Arduino IDE e caricalo sulla tua scheda Arduino.
2.  **Connessione BLE:** Connetti il tuo dispositivo (es. PC con MATLAB) al modulo BLE dell'Arduino.
3.  **Invio Comandi:** Utilizza la tua applicazione MATLAB (o simile) per inviare array di float tramite BLE secondo il protocollo definito nella funzione `dati_managment`.
    * `[0, Q1_desiderato, Q2_desiderato]` per impostare le portate desiderate.
    * `[1, alfa, beta]` per impostare parametri di calibrazione.
    * `[2, Kp, Ki, Kd]` per impostare i parametri PID (se `Kp=Ki=Kd=0`, il PID viene disattivato).
    * `[3, flux_scale_0, flux_scale_1]` per aggiornare i fattori di scala dei flussimetri.
    * `[-1]` per forzare l'invio delle impostazioni via BLE.
4.  **Monitor Seriale:** Apri il Monitor Seriale (o Plotter Seriale) nell'Arduino IDE per visualizzare i dati in tempo reale dai flussimetri e altri parametri stampati dal sistema.

## Contribuzioni

Sentiti libero di aprire issue per segnalare bug, suggerire miglioramenti o proporre nuove funzionalità tramite pull request.

---
