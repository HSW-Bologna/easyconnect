Template for esp32 projects

# TODO

 - Ottimizza la dimensione delle immagini (crop to content, composizioni varie)
 - Ottimizza l'impostazione delle immagini con una funzione che controllo se la sorgente e' cambiata
 - Prima di controllare deterministicamente ogni singola periferica manda un messaggio broadcast sperando che tutti lo ricevano per attivare tutte le uscite nello stesso momento.
 - All'accensione manda un messaggio broadcast di accensione (per riportare a uno stato fermo tutti i dispositivi)
 - Nella comunicazione modbus, nelle risposte poni dei limiti in base alla lunghezza che ti aspetti
 - Rimescola per bene la generazione casuale (mac delle due interfacce + serial number)
 - scaricare la seriale prima di mandare dei messaggi