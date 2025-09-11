//Code porte : 
#include "pitches.h" // Notes de musique pour le buzzer

// Définition des broches
const int trigPin1 = 4; //capteur 1
const int echoPin1 = 3; //capteur 1
const int trigPin2 = 10; //capteur 2
const int echoPin2 = 11; //capteur 2
const int BUZZER = 9;  // Buzzer
const int Led_Rouge = 7; // Lumière rouge
const int Led_Verte = 6; // Lumière verte
const int BUTTON = 8; // Bouton-poussoir

// Variables pour le bouton poussoir
bool buttonState = false;  // État du système
bool lastButtonState = LOW; // Dernier état du bouton
bool playedOpeningMelody = false;
bool playedClosingMelody = false;

// Variables pour les capteurs
float distance1, distance2, SmallerDistance;
int frequence_buzzer;

// Variables pour la gestion du clignotement de la LED rouge après OFF
unsigned long ledRougeStartTime = 0;
unsigned long ledRougeLastBlink = 0;
bool ledRougeClignote = false;
bool ledRougeState = false;

void setup() {
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);

  pinMode(Led_Rouge, OUTPUT);
  pinMode(Led_Verte, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(BUTTON, INPUT_PULLUP);  // Active la résistance pull-up interne

  Serial.begin(9600);
}

void loop() {
  bool currentButtonState = digitalRead(BUTTON);

  // Détection du front montant (appui unique)
  if (currentButtonState == HIGH && lastButtonState == LOW) {  
    buttonState = !buttonState; // Inverse l'état du système
    Serial.println(buttonState ? "Mode Capteurs ON" : "Mode Capteurs OFF");
    delay(200);  // Anti-rebond

    if (!buttonState) {
      // Lorsque le bouton OFF est cliqué, démarrer le clignotement pour 10 secondes
      ledRougeStartTime = millis();
      ledRougeLastBlink = millis();
      ledRougeClignote = true;
      ledRougeState = false;
    }
  }

  lastButtonState = currentButtonState;  // Mise à jour de l'état précédent

  if (buttonState) { // Mode ON
    if (!playedOpeningMelody) { // Jouer la mélodie d’ouverture une seule fois
      playOpeningMelody();
      playedOpeningMelody = true;
      playedClosingMelody = false; // Réinitialiser la fermeture
    }

    digitalWrite(Led_Verte, HIGH);
    digitalWrite(Led_Rouge, LOW);
    Capteurs();
    
  } else { // Mode OFF
    if (!playedClosingMelody) { // Jouer la mélodie de fermeture une seule fois
      playClosingMelody();
      playedClosingMelody = true;
      playedOpeningMelody = false; // Réinitialiser l'ouverture
    }

    digitalWrite(Led_Verte, LOW);

    // Lorsque le bouton off est cliqué je veux que la lumière rouge attende 10 secondes et ensuite s’éteint
    if (ledRougeClignote) {
      unsigned long currentTime = millis();

      // Clignoter toutes les 500 ms
      if (currentTime - ledRougeLastBlink >= 500) {
        ledRougeLastBlink = currentTime;
        ledRougeState = !ledRougeState;
        digitalWrite(Led_Rouge, ledRougeState);
      }

      // Arrêter après 10 secondes
      if (currentTime - ledRougeStartTime >= 10000) {
        ledRougeClignote = false;
        digitalWrite(Led_Rouge, LOW);
      }
    } else {
      digitalWrite(Led_Rouge, LOW);
    }
  }
}

void Capteurs() {
  distance1 = getDistance(trigPin1, echoPin1);
  distance2 = getDistance(trigPin2, echoPin2);

  if (isValid(distance1) && isValid(distance2)) {
    SmallerDistance = min(distance1, distance2);
  } else if (isValid(distance1)) {
    SmallerDistance = distance1;
  } else if (isValid(distance2)) {
    SmallerDistance = distance2;
  } else {
    SmallerDistance = -1; // Aucune distance valide
  }

  Serial.print("Capteur 1: "); Serial.print(distance1); Serial.print(" cm, ");
  Serial.print("Capteur 2: "); Serial.print(distance2); Serial.print(" cm, ");
  Serial.print("Distance plus petite: "); Serial.println(SmallerDistance);

  if (SmallerDistance > 0) {
    frequence_buzzer = constrain(-22.1 * SmallerDistance + 2610, 400, 2500);
    tone(BUZZER, frequence_buzzer);
  } else { 
    noTone(BUZZER);
  }
}

float getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  float duration = pulseIn(echoPin, HIGH);
  return (duration * 0.0343) / 2;
}

bool isValid(float distance) {
  return (distance > 2 && distance < 200);
}

void playMelody(int melody[], int duration[], int size) {
    for (int i = 0; i < size; i++) {
        tone(BUZZER, melody[i], duration[i]);
        delay(duration[i] * 1.3);
    }
    noTone(BUZZER);
}

void playOpeningMelody() {
    int melody[] = {NOTE_C4, NOTE_D4, NOTE_E4};
    int duration[] = {300, 300, 300};
    playMelody(melody, duration, 3);
}

void playClosingMelody() {
    int melody[] = {NOTE_E4, NOTE_D4, NOTE_C4};
    int duration[] = {300, 300, 300};
    playMelody(melody, duration, 3);
}
