#include <Arduino.h>

#define RED    0
#define YELLOW 1
#define GREEN  2
#define MODE   9


#define TIME_CONSTANT 500
#define LOOP_DELAY TIME_CONSTANT / 25

#define MODE_NORMAL   0
#define MODE_BLINKING 1
#define MODE_START 0
#define MODE_END 1

struct trafficLightState 
{
  uint16_t duration;
  uint16_t red;
  uint16_t yellow;
  uint16_t green;
};

trafficLightState TRAFFIC_LIGHT_MODES[] = {

  /*
   * Normaler Modus
   * ***************** */

  //GRÜN

   { .duration = 20, .red = LOW, .yellow = LOW, .green = HIGH},

  //GRÜN Blinken
   { .duration = 1,  .red = LOW, .yellow = LOW, .green = LOW},
   { .duration = 1,  .red = LOW, .yellow = LOW, .green = HIGH},
   { .duration = 1,  .red = LOW, .yellow = LOW, .green = LOW},
   { .duration = 1,  .red = LOW, .yellow = LOW, .green = HIGH},
   { .duration = 1,  .red = LOW, .yellow = LOW, .green = LOW},
   { .duration = 1,  .red = LOW, .yellow = LOW, .green = HIGH},
   { .duration = 1,  .red = LOW, .yellow = LOW, .green = LOW},
   { .duration = 1,  .red = LOW, .yellow = LOW, .green = HIGH},

  //GELB

   { .duration = 8, .red = LOW, .yellow = HIGH, .green = LOW},

  //ROT

   { .duration = 20, .red = HIGH, .yellow = LOW, .green = LOW},

  //GELB-ROT

   { .duration = 4, .red = HIGH, .yellow = HIGH, .green = LOW},


  
  /*
   * Gelb Blinken Modus
   * ***************** */

  { .duration = 1,  .red = LOW, .yellow = HIGH, .green = LOW},
  { .duration = 1,  .red = LOW, .yellow = LOW, .green = LOW}

};

#define MAX_MODES 2

int LIGHT_STATE_RANGE[2][2] = { {0, 12}, {12, 14} };
int lightState = 0;
int lastEvent = 0;
int mode = 0;

uint8_t modeButtonState = HIGH;

void initializeLights()
{
 //Setzen der LEDs für das rote, gelbe und Grüne Ampellicht
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(GREEN, LOW);
}

void switchMode(int newmode)
{
  /*
  digitalWrite(GREEN,HIGH); delay(250);
  digitalWrite(GREEN,LOW); delay(250);
  digitalWrite(GREEN,HIGH); delay(250);
  digitalWrite(GREEN,LOW); delay(250);
  */

  mode = (newmode >= MAX_MODES) ? 0 : newmode;

  //letzten Zeitstempel initialisieren
  lastEvent = 0;
  lightState = LIGHT_STATE_RANGE[mode][MODE_END] - 1;

  initializeLights();
}

void setup() 
{
  //sethen des Druckknopfes um den Modus der Ampel zu ändern
  pinMode(MODE, OUTPUT);
  digitalWrite(MODE, HIGH);
  pinMode(MODE, INPUT);
  digitalWrite(MODE, HIGH);

  switchMode(1);
}

void loop() 
{
  // put your main code here, to run repeatedly:
  int current = millis();
  if ((TRAFFIC_LIGHT_MODES[lightState].duration * TIME_CONSTANT) <= abs(current-lastEvent))
  {
    //digitalWrite(YELLOW,HIGH); delay(250);
    //digitalWrite(YELLOW,LOW); delay(250);
    ++lightState;
    lightState = (lightState >= LIGHT_STATE_RANGE[mode][MODE_END]) 
               ? LIGHT_STATE_RANGE[mode][MODE_START] 
               : lightState;

    lastEvent = current;

    digitalWrite(RED, TRAFFIC_LIGHT_MODES[lightState].red);
    digitalWrite(YELLOW, TRAFFIC_LIGHT_MODES[lightState].yellow);
    digitalWrite(GREEN, TRAFFIC_LIGHT_MODES[lightState].green);
  }

  
  int button = digitalRead(MODE);
  if ((LOW == button) && (HIGH == modeButtonState))
  {
    //digitalWrite(RED,HIGH); delay(1000);
    //digitalWrite(RED,LOW); delay(250);
    switchMode(mode +1);
    
  }
  else if (LOW == button)
  {
    //digitalWrite(YELLOW,HIGH); delay(250);
    //digitalWrite(YELLOW,LOW); delay(250);
  }
  
  modeButtonState = button;
  
  delay(LOOP_DELAY);
}