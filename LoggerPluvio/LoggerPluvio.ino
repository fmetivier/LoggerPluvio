/*
 * Pluvio Logger
 * 
 * Lecteur de carte Upesy 
 * Horloge RTC DS1307
 * interrupteur pullup du pluvio 
 *
 * port OFF du lecteur utilisé pour éteindre le lecteur de carte SD
 * 
 * Dernier test le 01/10/2024
 * 
 * F. Métivier
 * CC BY-SA 4.0
 */
#include "LowPower.h"

/*
 * Horloge RTC
 */
#include "Wire.h"
#include <TimeLib.h>
#include <DS1307RTC.h>


/*
* Pluvio
*/

const int pluvioPin = 2;
tmElements_t t0;

/*
 * Carte SD
 */
#include <SPI.h>
#include <SD.h>

const int chipSelect = 10; // CS pin du lecteur SD sur Nano
const int SDOFFPIN = 3; // pin de sommeil du lecteur

Sd2Card card;
SdVolume volume;
SdFile root;

void setup()
{
  Serial.begin(57600); // pour affichage dans le moniteur série
  delay(10);
  
  pinMode(pluvioPin, INPUT_PULLUP);

  pinMode(SDOFFPIN,OUTPUT);
  digitalWrite(SDOFFPIN,HIGH);
  
  delay(1000);

  if (!SD.begin(chipSelect)){
    Serial.println("Problème avec le lecteur SD");
  }
  else {
    Serial.println("Lecteur SD ok");
  }
  
  tmElements_t tm;
  if (RTC.read(tm)){
    Serial.print(tm.Day);
    Serial.print('/');
    Serial.print(tm.Month);
    Serial.print('/');
    Serial.print(tm.Year+1970);
    Serial.print(' ');
    Serial.print(tm.Hour);
    Serial.print(":");
    Serial.print(tm.Minute);
    Serial.print(":");
    Serial.println(tm.Second);
  }

  t0 = tm;

  delay(100);

}

void loop()
{
  
  attachInterrupt(digitalPinToInterrupt(pluvioPin),jeCompte,FALLING);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  detachInterrupt(digitalPinToInterrupt(pluvioPin));

  Serial.begin(57600);

  tmElements_t tm;
  if (RTC.read(tm)){
    if (tm.Year != t0.Year | tm.Month != t0.Month | tm.Day != t0.Day | tm.Second != t0.Second | tm.Minute != t0.Minute | tm.Hour != t0.Hour ){

        float Voltage = analogRead(A0)*5.0/1023.0;
                  
        Serial.print(tm.Year+1970);
        Serial.print(F(","));
        Serial.print(tm.Month);
        Serial.print(F(","));
        Serial.print(tm.Day);
        Serial.print(F(","));
        Serial.print(tm.Hour);
        Serial.print(F(","));
        Serial.print(tm.Minute);
        Serial.print(F(","));
        Serial.print(tm.Second);
        Serial.print(F(","));
        Serial.print(Voltage);
        Serial.print("\n");

        // réveil lecteur SD
        digitalWrite(SDOFFPIN,HIGH);
        delay(100);
        // écriture carte
        if (SD.begin(chipSelect)){
          File dataFile = SD.open("DATALOG.TXT", FILE_WRITE);
        
          if (dataFile) {
          // if the file is available, write to it:
            dataFile.print(tm.Year+1970);
            dataFile.print(F(","));
            dataFile.print(tm.Month);
            dataFile.print(F(","));
            dataFile.print(tm.Day);
            dataFile.print(F(","));
            dataFile.print(tm.Hour);
            dataFile.print(F(","));
            dataFile.print(tm.Minute);
            dataFile.print(F(","));
            dataFile.print(tm.Second);
            dataFile.print(F(","));
            dataFile.print(Voltage);
            dataFile.print("\n");     
            dataFile.close();
        } else {
          Serial.println("Pas de fichier ?");  
        }
        } else {
          Serial.println("Pas de carte ?");
        }
        
        t0=tm;
        delay(10);
        //mise en sommeil lecteur SD
        digitalWrite(SDOFFPIN,LOW);

        // arrêt Série
        Serial.end();
        // redémarrage interruptions en début de boucle
      }
  }

} //end void

void jeCompte(){
  // do nothing
}
