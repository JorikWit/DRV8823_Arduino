#define SDI MOSI  //Data
//#define SCK SCK //Data is shifted on rising edge
#define CS A5     //HIGH when data is transferd
#define STB A0 //Pulse LOW -> HIGH. When data is send and a CS is LOW again

#define RST A2    //Low to disable Stepper motor
#define SLEEP A3  //Turn high?

uint8_t motorAB = 0b0000;

bool decayB = 0;
uint8_t stepB = 0b000;
bool phaseB = 0;
bool enableB = 0;

bool decayA = 0;
uint8_t stepA = 0b000;
bool phaseA = 0;
bool enableA = 0;

void setup() {
  //PinModes
  pinMode(SDI, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(STB, OUTPUT);

  pinMode(RST, OUTPUT);
  pinMode(SLEEP, OUTPUT);

  //Setup IO
  digitalWrite(SDI, LOW);
  digitalWrite(SCK, LOW);
  digitalWrite(CS, LOW);
  digitalWrite(STB, LOW);

  pinMode(RST, LOW);
  pinMode(SLEEP, LOW);

  Serial.begin(115200);

  //Reset
  sendBits(parseMotorBits());

  enableA = true;
  enableB = true;
  stepA = 0b011;  //If both on max 71% power
  stepB = 0b011;  //If both on max 71% power
  decayA = 0;
  decayB = 0;
}

// ramp up accelaration curve steppermotor use geogebra. y=((3000)/(x^(0.0002 x)))+5
double delayRampUp;
double delayMin = 0;            //Delay in operation
double delayStartUp = 500;      //Start delay to start turning
double delayCurve = 0.00005;     //Small number is a slower curve
int stopDelayCurve = 10000;     //Stop rampup after ... cycles (overflow protection)
int i = 0;
//delayMicroseconds((delayStartUp /pow(i, (delayCurve*(i > stopDelayCurve ? i : i++)))) + delayMin);

bool dirMotor = 0;


void loop() {
  //ramp up motor
  //Pos 0
  for (int j = 0; j < 25; j++) {
    phaseA = dirMotor;
    phaseB = dirMotor;
    sendBits(parseMotorBits());
    delay(50);
    //Pos 1
    phaseA = 1;
    phaseB = 0;
    sendBits(parseMotorBits());
    delay(50);
    //Pos 2
    phaseA = !dirMotor;
    phaseB = !dirMotor;
    sendBits(parseMotorBits());
    delay(50);
    //Pos 3
    phaseA = 0;
    phaseB = 1;
    sendBits(parseMotorBits());
    delay(50);
  }
  i = 0;
  delay(1000);
  dirMotor = !dirMotor;
}

void printBits(uint16_t data) {
    for (uint8_t i = 15; i != 255 ; i--){
    int mask = 1 << i;
    Serial.print(data & mask ? "1" : "0");
    if (i % 4 == 0) Serial.print(" ");
  }
  Serial.println();
}

void sendBits(uint16_t data) {
    digitalWrite(CS, HIGH);
    for (uint8_t i = 0; i < 16 ; i++){
      int mask = 1 << i;
      digitalWrite(SDI, data & mask ? 1 : 0);
      digitalWrite(SCK, HIGH);
      digitalWrite(SCK, LOW);

    }
    digitalWrite(CS, LOW);
    digitalWrite(STB, HIGH);
    digitalWrite(STB, LOW);   
}

uint16_t parseMotorBits() {
    return (motorAB << 12 | 
         decayB << 11 | stepB << 8 | phaseB << 7 | enableB << 6 |
         decayA << 5 | stepA << 2 | phaseA << 1 | enableA);
}
