#define pinritardo 16
#define pindurata 15
#define pinavvio 12
#define rele 14
#define luce 13

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);
const unsigned long ora_in_millis = 3600000;  // 1000 millisecondi * 60 secondi * 60 minuti
const unsigned long minuti_in_millis = 60000; // 1000 millisecondi * 60 secondi
const unsigned long secondi_in_millis = 500;  // tempo lampeggio barra secondi
unsigned long now = 0;
int ritardo = 0;
int durata = 12;
int duepunti = LOW;

void(* riavvio)(void) = 0;

void setup() {

  pinMode(pinritardo, INPUT_PULLUP);
  pinMode(pindurata, INPUT_PULLUP);
  pinMode(pinavvio, INPUT_PULLUP);
  pinMode(rele, OUTPUT);
  digitalWrite(rele, LOW);
  pinMode(luce, OUTPUT);
  digitalWrite(luce, LOW);
  lcd.init();
  lcd.noBacklight();
  lcd.setCursor(0, 0);
  lcd.print("RITARDO:     0 h");
  lcd.setCursor(0, 1);
  lcd.print("DURATA:      12h");

}

void loop() {
  while ((digitalRead(pinavvio)) == HIGH) {
    if ((digitalRead(pinritardo)) == LOW) {  // imposta ritardo da 0 (parte subito) a 12 ore
      if (ritardo == 12) {
        ritardo = 0;
      } else {
        ritardo = ritardo + 1;
      }
      lcd.setCursor(0, 0);
      lcd.print("RITARDO:       h");
      lcd.setCursor(13, 0);
      lcd.print(ritardo);
      delay(300);    // delay per debounce
    }
    if ((digitalRead(pindurata)) == LOW) {  // imposta durata di ricarica da 1 ora a massimo 12 ore
      if (durata == 12) {
        durata = 1;
      } else {
        durata = durata + 1;
      }
      lcd.setCursor(0, 1);
      lcd.print("DURATA:        h");
      lcd.setCursor(13, 1);
      lcd.print(durata);
      delay(300);  // delay per debounce
    }
  }

  // premuto pulsante di avvio esco dal while e comincia a contare il ritardo

  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("RITARDO ->   :00");
  unsigned long tik_secondi;
  unsigned long tik_minuti;
  unsigned long tik_ore;
  int minuti = 0;

  while ((ritardo != -1)) {
    now = millis();
    if (now - tik_secondi >= secondi_in_millis) {  // if utile al lampeggio dei secondi
      tik_secondi = now;
      if (duepunti == LOW) {
        duepunti = HIGH;
        lcd.setCursor(13, 0);
        lcd.print(":");
      } else {
        duepunti = LOW;
        lcd.setCursor(13, 0);
        lcd.print(" ");
      }

    }
    if (now - tik_minuti >= minuti_in_millis) {  // if utile a scrivere i minuti mancanti
      tik_minuti = now;
      minuti = minuti - 1;
      if (minuti == -1) {
        minuti = 59;
      }

      if (minuti < 10) {
        lcd.setCursor(14, 0);
        lcd.print("0");
        lcd.setCursor(15, 0);
        lcd.print(minuti);
      } else {

        lcd.setCursor(14, 0);
        lcd.print(minuti);

      }

    }


    if (now - tik_ore >= ora_in_millis) {  // if utile a scrivere le ore mancanti
      tik_ore = now;
      ritardo = ritardo - 1;
      if (ritardo < 10) {
        lcd.setCursor(11, 0);
        lcd.print(" ");
        lcd.setCursor(12, 0);
        lcd.print(ritardo);
      } else {

        lcd.setCursor(11, 0);
        lcd.print(ritardo);

      }
    }


  }

  // conteggio ritardo finito, parte il rele e comincia a contare la durata

  lcd.setCursor(0, 0);
  lcd.print("RITARDO:  FINITO");
  lcd.setCursor(0, 1);
  lcd.print("DURATA ->   0:00");
  digitalWrite(rele, HIGH);
  digitalWrite(luce, HIGH);


  minuti = 0;
  int accensione = 0;

  while ((durata != accensione)) {
    now = millis();
    if (now - tik_secondi >= secondi_in_millis) {  // if utile al lampeggio dei secondi
      tik_secondi = now;
      if (duepunti == LOW) {
        duepunti = HIGH;
        lcd.setCursor(13, 1);
        lcd.print(":");
      } else {
        duepunti = LOW;
        lcd.setCursor(13, 1);
        lcd.print(" ");
      }

    }
    if (now - tik_minuti >= minuti_in_millis) {  // if utile al conteggio dei minuti e ore trascorsi
      tik_minuti = now;
      minuti = minuti + 1;
      if (minuti == 60) {
        minuti = 0;
        accensione = accensione + 1;
        if (accensione < 10) {
          lcd.setCursor(11, 1);
          lcd.print(" ");
          lcd.setCursor(12, 1);
          lcd.print(accensione);
        } else {

          lcd.setCursor(11, 1);
          lcd.print(accensione);

        }
      }

      if (minuti < 10) {
        lcd.setCursor(14, 1);
        lcd.print("0");
        lcd.setCursor(15, 1);
        lcd.print(minuti);
      } else {

        lcd.setCursor(14, 1);
        lcd.print(minuti);

      }

    }



  }

  // tempo finito, spegne rele e resetta la scheda

  lcd.setCursor(0, 1);
  lcd.print("DURATA:   FINITO");
  lcd.noBacklight();
  digitalWrite(rele, LOW);
  digitalWrite(luce, LOW);
  delay(1000);

  riavvio();
}
