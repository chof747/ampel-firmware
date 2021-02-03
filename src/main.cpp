#include <Arduino.h>

#define RED1     7
#define YELLOW1  6
#define GREEN1   5
#define RED2     4
#define YELLOW2  3
#define GREEN2   2
#define MODE     10
#define TIMER1   9
#define TIMER2   8


#define TIME_CONSTANT 500
#define LOOP_DELAY TIME_CONSTANT / 25

#define MODE_NORMAL   0
#define MODE_BLINKING 1
#define MODE_START 0
#define MODE_END 1

struct trafficLightState 
{
  uint16_t duration;
  bool variable;
  uint16_t red1;
  uint16_t yellow1;
  uint16_t green1;
};

trafficLightState TRAFFIC_LIGHT_MODES[] = {

  /*
   * Normaler Modus
   * ***************** */

  //GRÜN

   { .duration = 20, .variable = true, .red1 = LOW, .yellow1 = LOW, .green1 = HIGH},

  //GRÜN Blinken
   { .duration = 1, .variable = false, .red1 = LOW, .yellow1 = LOW, .green1 = LOW},
   { .duration = 1, .variable = false,  .red1 = LOW, .yellow1 = LOW, .green1 = HIGH},
   { .duration = 1, .variable = false,  .red1 = LOW, .yellow1 = LOW, .green1 = LOW},
   { .duration = 1, .variable = false,  .red1 = LOW, .yellow1 = LOW, .green1 = HIGH},
   { .duration = 1, .variable = false,  .red1 = LOW, .yellow1 = LOW, .green1 = LOW},
   { .duration = 1, .variable = false, .red1 = LOW, .yellow1 = LOW, .green1 = HIGH},
   { .duration = 1, .variable = false,  .red1 = LOW, .yellow1 = LOW, .green1 = LOW},
   { .duration = 1, .variable = false,  .red1 = LOW, .yellow1 = LOW, .green1 = HIGH},

  //GELB

   { .duration = 8,  .variable = false, .red1 = LOW, .yellow1 = HIGH, .green1 = LOW},

  //ROT

   { .duration = 20,  .variable = true, .red1 = HIGH, .yellow1 = LOW, .green1 = LOW},

  //GELB-ROT

   { .duration = 4, .variable = false, .red1 = HIGH, .yellow1 = HIGH, .green1 = LOW},


  
  /*
   * Gelb Blinken Modus
   * ***************** */

  { .duration = 1,  .variable = false, .red1 = LOW, .yellow1 = HIGH, .green1 = LOW},
  { .duration = 1,  .variable = false, .red1 = LOW, .yellow1 = LOW, .green1 = LOW}

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
  pinMode(RED1, OUTPUT);
  pinMode(YELLOW1, OUTPUT);
  pinMode(GREEN1, OUTPUT);
  digitalWrite(RED1, LOW);
  digitalWrite(YELLOW1, LOW);
  digitalWrite(GREEN1, LOW);
}

void switchMode(int newmode)
{
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

    digitalWrite(RED1, TRAFFIC_LIGHT_MODES[lightState].red1);
    digitalWrite(YELLOW1, TRAFFIC_LIGHT_MODES[lightState].yellow1);
    digitalWrite(GREEN1, TRAFFIC_LIGHT_MODES[lightState].green1);
  }

  
  int button = digitalRead(MODE);
  if ((LOW == button) && (HIGH == modeButtonState))
  {
    switchMode(mode +1);
    
  }
  else if (LOW == button)
  {
  }
  
  modeButtonState = button;
  
  delay(LOOP_DELAY);
}
