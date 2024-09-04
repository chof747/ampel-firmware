#include <Arduino.h>
#include "config.h"

#define RED1 7
#define YELLOW1 6
#define GREEN1 5
#define RED2 4
#define YELLOW2 3
#define GREEN2 2
#define MODE 10


#define DEBUG_PIN 1

#define MODE_NORMAL 0
#define MODE_BLINKING 1
#define MODE_START 0
#define MODE_END 1

#ifdef DEBUG
  #include <SoftwareSerial.h>
  SoftwareSerial softserial(99, DEBUG_PIN);
#endif

struct trafficLightState
{
  uint16_t duration;
  uint8_t variable;
  uint8_t red1;
  uint8_t yellow1;
  uint8_t green1;
  uint8_t red2;
  uint8_t yellow2;
  uint8_t green2;
};

trafficLightState TRAFFIC_LIGHT_MODES[] = {

    /*
   * Normaler Modus
   * ***************** */

    //  Time, Variable, Red1, Yel1, Grn1, Red2, Yel2, Grn2
    //GRÜN 1
    {10, 1, LOW, LOW, HIGH, HIGH, LOW, LOW},
    //GRÜN Blinken 1
    {1, 0, LOW, LOW, LOW, HIGH, LOW, LOW},
    {1, 0, LOW, LOW, HIGH, HIGH, LOW, LOW},
    {1, 0, LOW, LOW, LOW, HIGH, LOW, LOW},
    {1, 0, LOW, LOW, HIGH, HIGH, LOW, LOW},
    {1, 0, LOW, LOW, LOW, HIGH, LOW, LOW},
    {1, 0, LOW, LOW, HIGH, HIGH, LOW, LOW},
    {1, 0, LOW, LOW, LOW, HIGH, LOW, LOW},
    {1, 0, LOW, LOW, HIGH, HIGH, LOW, LOW},
    //GELB 1
    {4, 0, LOW, HIGH, LOW, HIGH, LOW, LOW},
    //ROT 1
    {2, 0, HIGH, LOW, LOW, HIGH, LOW, LOW},
    //GELB-ROT 2
    {4, 0, HIGH, LOW, LOW, HIGH, HIGH, LOW},
    //GRÜN 2
    {10, 2, HIGH, LOW, LOW, LOW, LOW, HIGH},
    //GRÜN Blinken 2
    {1, 0, HIGH, LOW, LOW, LOW, LOW, LOW},
    {1, 0, HIGH, LOW, LOW, LOW, LOW, HIGH},
    {1, 0, HIGH, LOW, LOW, LOW, LOW, LOW},
    {1, 0, HIGH, LOW, LOW, LOW, LOW, HIGH},
    {1, 0, HIGH, LOW, LOW, LOW, LOW, LOW},
    {1, 0, HIGH, LOW, LOW, LOW, LOW, HIGH},
    {1, 0, HIGH, LOW, LOW, LOW, LOW, LOW},
    {1, 0, HIGH, LOW, LOW, LOW, LOW, HIGH},
    //GELB 2
    {4, 0, HIGH, LOW, LOW, LOW, HIGH, LOW},
    //ROT 2
    {2, 0, HIGH, LOW, LOW, HIGH, LOW, LOW},
    //GELB-ROT 1
    {4, 0, HIGH, HIGH, LOW, HIGH, LOW, LOW},
    /*
   * Gelb Blinken Modus
   * ***************** */

    {.duration = 1, .variable = 0, .red1 = LOW, .yellow1 = HIGH, .green1 = LOW, .red2 = LOW, .yellow2 = LOW, .green2 = LOW},
    {.duration = 1, .variable = 0, .red1 = LOW, .yellow1 = LOW, .green1 = LOW, .red2 = LOW, .yellow2 = HIGH, .green2 = LOW},

};

#define MAX_MODES 2

int LIGHT_STATE_RANGE[2][2] = {{0, 24}, {24, 26}};
int lightState = 0;
int lastEvent = 0;
int mode = 0;
bool printing = true;

uint8_t modeButtonState = HIGH;

void initializeLights()
//******************************************************************************
{
  //Setzen der LEDs für das rote, gelbe und Grüne Ampellicht
  pinMode(RED1, OUTPUT);
  pinMode(YELLOW1, OUTPUT);
  pinMode(GREEN1, OUTPUT);
  pinMode(RED2, OUTPUT);
  pinMode(YELLOW2, OUTPUT);
  pinMode(GREEN2, OUTPUT);
  digitalWrite(RED1, LOW);
  digitalWrite(YELLOW1, LOW);
  digitalWrite(GREEN1, LOW);
  digitalWrite(RED2, LOW);
  digitalWrite(YELLOW2, LOW);
  digitalWrite(GREEN2, LOW);
}

void switchMode(int newmode)
//******************************************************************************
{
  mode = (newmode >= MAX_MODES) ? 0 : newmode;

  //letzten Zeitstempel initialisieren
  #ifdef DEBUG
    softserial.print("[TL] Mode ");
    softserial.println(newmode);
  #endif
  lastEvent = 0;
  lightState = LIGHT_STATE_RANGE[mode][MODE_END] - 1;

  initializeLights();
}

void checkBtnPress()
//******************************************************************************
{
  int button = digitalRead(MODE);
  if ((LOW == button) && (HIGH == modeButtonState))
  {
    switchMode(mode + 1);
  }
  else if (LOW == button)
  {
  }
  modeButtonState = button;
}

void setup()
{
  //sethen des Druckknopfes um den Modus der Ampel zu ändern
  pinMode(MODE, OUTPUT);
  digitalWrite(MODE, HIGH);
  pinMode(MODE, INPUT);
  digitalWrite(MODE, HIGH);

#ifdef DEBUG
  softserial.begin(9600);
  softserial.println("[TL] Starting");
#endif

  switchMode(1);
}

void loop()
//******************************************************************************
{
  // put your main code here, to run repeatedly:
  int current = millis();
  uint8_t timer[2];
  timer[0] = map(analogRead(TIMER1), 0, 1023, 0, MAX_VARIABLE_TIME);
  timer[1] = map(analogRead(TIMER2), 0, 1023, 0, MAX_VARIABLE_TIME);

  uint16_t duration;

  if (0 < TRAFFIC_LIGHT_MODES[lightState].variable)
  {

    duration = timer[TRAFFIC_LIGHT_MODES[lightState].variable - 1] +
               TRAFFIC_LIGHT_MODES[lightState].duration;

    if (printing)
    {

      #ifdef DEBUG
        softserial.print("[TRAFFIC LIGHT] Using Timer ");
        softserial.print(TRAFFIC_LIGHT_MODES[lightState].variable - 1);
        softserial.print(" = ");

        softserial.print(duration * TIME_CONSTANT / 1000);
        softserial.println(" sec.");
      #endif

      printing = false;
    }
  }
  else
  {
    duration = TRAFFIC_LIGHT_MODES[lightState].duration;
    printing = true;
  }

  if ((duration * TIME_CONSTANT) <= abs(current - lastEvent))
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
    digitalWrite(RED2, TRAFFIC_LIGHT_MODES[lightState].red2);
    digitalWrite(YELLOW2, TRAFFIC_LIGHT_MODES[lightState].yellow2);
    digitalWrite(GREEN2, TRAFFIC_LIGHT_MODES[lightState].green2);
  }

  checkBtnPress();

  delay(LOOP_DELAY);
}
