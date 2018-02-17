#include <avr/io.h>   // les librairies
#include <avr/wdt.h> 
#include <avr/sleep.h>


// ----- DECLARATION DES VARIABLES ------

// Pour les LED
int pinLeds1 = 0; 
int pinLeds2 = 1;
int pinLeds3 = 2;
int pinLed4 = 3; 

                    
int buttonPin = 4;  // on indique la pin du capteur d'inclinaison

int buttonState;   // On vérifie l'état du capteur d'inclinaison
                           
long ranim;      // Variable pour le random de l'animation 
         
    
long ran;           // Variable pour le random du résultat final


int last = 0;     // Il n'y a pas encore eu de lancement de dé
                    
int timo = 10000;  // Temps que le dé reste affiché 10000 ms = 10 seconde

volatile uint8_t wdt_count;  // Incremented by watchdog interrupt <----------

// ----- DECLARATION ENTREES ET SORTIES ------
void setup ()
{
      //arret des services   
  ACSR  |= _BV(ACD);     // switch Analog Compartaror OFF

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);// Configure attiny85 sleep mode
  // Reset watchdog interrupt counter
  wdt_count = 255; //max value

  //On indique que les LED sont des sorties
  pinMode (pinLeds1, OUTPUT);
  pinMode (pinLeds2, OUTPUT);
  pinMode (pinLeds3, OUTPUT);
  pinMode (pinLed4, OUTPUT);
  // On indique que le detecteur d'inclinaison est une entrée.
  pinMode (buttonPin, INPUT);

   // on initialise le fait de pouvoir lancer des randoms
 randomSeed(analogRead(0));
  
}

// ----- ACTIONS A EFFECTUER ------

void blink(uint8_t flash)

{  
  buttonState = digitalRead(buttonPin);
 if (buttonState == HIGH){              // Si on bouge le dé  

   for (int i=0; i <= 7; i++){             // Animation pour 8 affichages de dé avec 200 ms entre chaque  
     
  ranim = random(1, 7);                   // on effectue un random pour chaque affichage
 
  while (ranim == last){                   // On verifie que deux affichages consécutifs ne sont pas les mêmes 
    ranim = random(1, 7);                 // Sinon on random un autre chiffre
           }  
  last = ranim; // On stock le chiffre pour éviter d'afficher les 2 mêmes valeurs de dé pendant l'animation
   
    
    if (ranim == 1){
      digitalWrite (pinLed4, HIGH);
    }
    if (ranim == 2){
      digitalWrite (pinLeds1, HIGH);
    }
    if (ranim == 3){
      digitalWrite (pinLeds3, HIGH);
      digitalWrite (pinLed4, HIGH);
    }
    if (ranim == 4){
      digitalWrite (pinLeds1, HIGH);
      digitalWrite (pinLeds3, HIGH);
    }
    if (ranim == 5){
      digitalWrite (pinLeds1, HIGH);
      digitalWrite (pinLeds3, HIGH);
      digitalWrite (pinLed4, HIGH);
   }
   if (ranim == 6){
      digitalWrite (pinLeds1, HIGH);
      digitalWrite (pinLeds2, HIGH);
      digitalWrite (pinLeds3, HIGH);
   }
      delay(200); // Pause entre l'affichage de l'animation : 200ms led éteintes
    digitalWrite (pinLeds1, LOW);
    digitalWrite (pinLeds2, LOW);
    digitalWrite (pinLeds3, LOW);
    digitalWrite (pinLed4, LOW);
      delay(50);
                          } 
    
  
// RANDOM DU RESULTAT FINAL
        ran = random(1, 7);
    
   if (ran == 1){
      digitalWrite (pinLed4, HIGH);
      delay(timo); // time étant la variable qui vaut 7000ms donc affichage du résultat pendant 7 secondes
    }
    if (ran == 2){
      digitalWrite (pinLeds1, HIGH);
      delay(timo);
    }
    if (ran == 3){
      digitalWrite (pinLeds3, HIGH);
      digitalWrite (pinLed4, HIGH);
      delay(timo);
    }
    if (ran == 4){
      digitalWrite (pinLeds1, HIGH);
      digitalWrite (pinLeds3, HIGH);
      delay(timo);
    }
    if (ran == 5){
      digitalWrite (pinLeds1, HIGH);
      digitalWrite (pinLeds3, HIGH);
      digitalWrite (pinLed4, HIGH);
      delay(timo);
   }
   if (ran == 6){
      digitalWrite (pinLeds1, HIGH);
      digitalWrite (pinLeds2, HIGH);
      digitalWrite (pinLeds3, HIGH);
      delay(timo);
   }
  }  
  // On éteint tout pour pouvoir relancer le dé
  digitalWrite (pinLeds1, LOW);
  digitalWrite (pinLeds2, LOW);
  digitalWrite (pinLeds3, LOW);
  digitalWrite (pinLed4, LOW);
  
}  
void loop() {  
               //on lance le code lumiere
              
             blink(3);
             wdt_count = 0;
             watchdog_start_interrupt(6);      // prescale of 6 ~= 1sec //on lance les interruptions cpu
       if(buttonState == HIGH) {       // si le capeteur et enclenché
                                watchdog_stop();
                                ADCSRA |= (1<<ADEN); //Enable ADC
                               }
                          else{       // veille en atendant signal de mise en marche
                              ADCSRA &= ~(1<<ADEN); //Disable ADC, saves ~230uA
                               sleep_mode(); // Make CPU sleep until next WDT interrupt
                             }
        }           
/* Turn off WDT */
void watchdog_stop() {
  WDTCR |= _BV(WDCE) | _BV(WDE);
  WDTCR = 0x00;
}
/* Turn onn WDT (with interupt) */
void watchdog_start_interrupt(uint8_t wd_prescaler) {
  if(wd_prescaler > 9) wd_prescaler = 9;
  byte _prescaler = wd_prescaler & 0x7;
  if (wd_prescaler > 7 ) _prescaler |= _BV(WDP3); 
  // ^ fourth bit of the prescaler is somewhere else in the register...
  // set new watchdog timer prescaler value
  WDTCR = _prescaler;
  // start timed sequence
  WDTCR |= _BV(WDIE) | _BV(WDCE) | _BV(WDE);
}
// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
  wdt_count++;
  WDTCR |= _BV(WDIE);           // Watchdog goes to interrupt not reset
}
