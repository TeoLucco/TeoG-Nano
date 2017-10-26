#include <Filters.h>
#include <CapacitiveSensor.h>

//CAPACITIVES
#define HEAD_SCALIBRATION_TIMER 5000
#define BODY_SCALIBRATION_TIMER 5000
#define N_HEAD_SENSORS 4
#define N_BODY_SENSORS 3
//BODY CAPACITIVES
#define BODY_FRONT_S 13
#define BODY_FRONT_R A1
#define BODY_SX_S 10
#define BODY_SX_R 11
#define BODY_DX_S 12
#define BODY_DX_R A0

FilterOnePole left_body_f( LOWPASS, 1.0 );
FilterOnePole right_body_f( LOWPASS, 1.0 );
FilterOnePole front_body_f( LOWPASS, 1.0 );

int lowBodyThreshold[3]={200,200,200};
int highBodyThreshold[3]{380,380,380};

#define  HUGTIME 4000
//#define  MIN_PAT_TIME 250
//#define  MAX_PAT_TIME 5000
//#define  MIN_HIT_TIME 10
#define  MAX_HIT_TIME 500
int hugsCount = 0;
enum touchTypes {nothing, hug, hitPat0, hitPat1, hitPat2};
touchTypes touchState = nothing;

CapacitiveSensor* bodySensor[N_BODY_SENSORS];
long bodySensorValue_nf[N_BODY_SENSORS];
long bodySensorValue[N_BODY_SENSORS];
enum bodyCapacitiveStates {no_touch, soft_touch, strong_touch};
bodyCapacitiveStates capacitiveState[N_BODY_SENSORS];
bodyCapacitiveStates previousCapacitiveState[N_BODY_SENSORS];
bodyCapacitiveStates previousDynamicCapacitiveState[N_BODY_SENSORS];
unsigned long int stateStartTime[N_BODY_SENSORS] = {0, 0, 0};
unsigned long int previousStateStartTime[N_BODY_SENSORS] = {0, 0, 0};

//HEAD CAPACITIVES
#define HEAD_BUTTON_0S 2
#define HEAD_BUTTON_0R 3
#define HEAD_BUTTON_1S 4
#define HEAD_BUTTON_1R 5
#define HEAD_BUTTON_2S 6
#define HEAD_BUTTON_2R 7
#define HEAD_BUTTON_3S 8
#define HEAD_BUTTON_3R 9

CapacitiveSensor* headSensor[N_HEAD_SENSORS];
long headSensorValue[N_HEAD_SENSORS];
int pressedButton = -1;
long headThreshold = 800;
unsigned long int releaseButtonTime=0;

enum warKingsCapacitives {noOne, head, body, both};
warKingsCapacitives workingCapacitives =body;


void updateState(bodyCapacitiveStates state, int i) {
  if (capacitiveState[i] != state) {
    previousCapacitiveState[i] = capacitiveState[i];
    capacitiveState[i] = state;
    previousStateStartTime[i] = stateStartTime[i];
    stateStartTime[i] = millis();
  }
}

void setup() {
  bodyCapacitiveSetup();
  headCapacitiveSetup();
  Serial.begin(38400); //serialSetup ;)
}

void loop() {
  switch(workingCapacitives){
    case noOne: break;
    case  head: headCapacitiveLoop();break;
    case  body: bodyCapacitiveLoop();break;
    case  both: bodyCapacitiveLoop();headCapacitiveLoop();break;
  }
  SerialLoop();
}
