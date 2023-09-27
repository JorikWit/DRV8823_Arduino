#define SDI MOSI  //Data
//#define SCK SCK //Data is shifted on rising edge
#define CS A5     //HIGH when data is transferd
#define STB A0 //Pulse LOW -> HIGH. When data is send and a CS is LOW aga+in
#define RST A2    //Low to disable Stepper motor
#define SLEEP A3  //Turn high?

#define amountOfSteps 64
uint16_t microsteps[amountOfSteps] = { 
  0x001D,0x0019,0x0059,0x0055,0x0155,0x0151,0x0251,0x024D,
  0x034D,0x0349,0x0449,0x0445,0x0545,0x0541,0x0641,0x0640,
  0x0740,0x0642,0x0643,0x0543,0x0547,0x0447,0x044B,0x034B,
  0x034F,0x024F,0x0253,0x0153,0x0157,0x0057,0x005B,0x001B,
  0x001F,0x009B,0x00DB,0x00D7,0x01D7,0x01D3,0x02D3,0x02CF,
  0x03CF,0x03CB,0x04CB,0x04C7,0x05C7,0x05C3,0x06C3,0x06C2,
  0x07C0,0x06C0,0x06C1,0x05C1,0x05C5,0x04C5,0x04C9,0x03C9,
  0x03CD,0x02CD,0x02D1,0x01D1,0x01D5,0x00D5,0x00D9,0x0099
};
                             
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
  sendBits(0);
}

double stepInMicrostepsArray = 4.0;   //there are 4 "fulls steps" in array 
double degreesPerStep = 1.8;          //Motor
double rotDegree = 3600.0;            //Rotation of the motor
double rotationTimeSec = .250;        //time can be a bit longer. Communication delay

void loop() {
  for (int i = 0; i < ((rotDegree / degreesPerStep / stepInMicrostepsArray) * amountOfSteps); i++) {
    sendBits(microsteps[i % amountOfSteps]);
    delayMicroseconds((rotationTimeSec * 1000000) / ((rotDegree / degreesPerStep / stepInMicrostepsArray) * amountOfSteps));
  }
  delay(50);
  pinMode(SLEEP, HIGH);
  delay(950);
  pinMode(SLEEP, LOW);
  for (int i = ((rotDegree / degreesPerStep / stepInMicrostepsArray) * amountOfSteps); i > -1; i--) {
    sendBits(microsteps[i % amountOfSteps]);
    delayMicroseconds((rotationTimeSec * 1000000.0) / ((rotDegree / degreesPerStep / stepInMicrostepsArray) * amountOfSteps));
  }
  delay(50);
  pinMode(SLEEP, HIGH);
  delay(950);
  pinMode(SLEEP, LOW);
}

void sendMotorBits(uint16_t data) {
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

void printBits(uint16_t data) {
  for (uint8_t i = 15; i != 255 ; i--){
    int mask = 1 << i;
    Serial.print(data & mask ? "1" : "0");
    if (i % 4 == 0) Serial.print(" ");
  }
  Serial.println();
}
