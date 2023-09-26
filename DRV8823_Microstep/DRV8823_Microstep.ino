#define SDI MOSI  //Data
//#define SCK SCK //Data is shifted on rising edge
#define CS A5     //HIGH when data is transferd
#define STB A0 //Pulse LOW -> HIGH. When data is send and a CS is LOW again

#define RST A2    //Low to disable Stepper motor
#define SLEEP A3  //Turn high?

#define amountOfSteps 128
uint16_t microsteps[amountOfSteps] = { 0x01D,0x01D,0x019,0x059,0x059,0x059,0x055,0x155,0x155,0x155,0x151,0x251,0x251,0x251,0x24D,0x34D,
                                       0x34D,0x34D,0x349,0x449,0x449,0x449,0x445,0x545,0x545,0x545,0x541,0x641,0x641,0x641,0x640,0x740,
                                       0x740,0x742,0x642,0x643,0x643,0x643,0x543,0x547,0x547,0x547,0x447,0x44B,0x44B,0x44B,0x34B,0x34F,
                                       0x34F,0x34F,0x24F,0x253,0x253,0x253,0x153,0x157,0x157,0x157,0x057,0x05B,0x05B,0x05B,0x01B,0x01F,
                                       0x01F,0x09F,0x09B,0x09B,0x0DB,0x0DB,0x0D7,0x1D7,0x1D7,0x1D7,0x1D3,0x2D3,0x2D3,0x2D3,0x2CF,0x3CF,
                                       0x3CF,0x3CF,0x3CB,0x4CB,0x4CB,0x4CB,0x4C7,0x5C7,0x5C7,0x5C7,0x5C3,0x6C3,0x6C3,0x6C3,0x6C2,0x7C2,
                                       0x7C0,0x7C0,0x6C0,0x6C1,0x6C1,0x6C1,0x5C1,0x5C5,0x5C5,0x5C5,0x4C5,0x4C9,0x4C9,0x4C9,0x3C9,0x3CD,
                                       0x3CD,0x3CD,0x2CD,0x2D1,0x2D1,0x2D1,0x1D1,0x1D5,0x1D5,0x1D5,0x0D5,0x0D9,0x0D9,0x0D9,0x099,0x09D };
                             
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
double rotDegree = 180.0;             //Rotation of the motor

double rotationTimeSec = 3.5;        //time can be a bit long. Communication delay

void loop() {
  for (int i = 0; i < ((rotDegree / degreesPerStep / stepInMicrostepsArray) * amountOfSteps); i++) {
    delayMicroseconds((rotationTimeSec * 1000000) / ((rotDegree / degreesPerStep / stepInMicrostepsArray) * amountOfSteps));
    sendBits(microsteps[i % amountOfSteps]);
  }
  delay(1000);

  for (int i = ((rotDegree / degreesPerStep / stepInMicrostepsArray) * amountOfSteps); i > -1; i--) {
    delayMicroseconds((rotationTimeSec * 1000000) / ((rotDegree / degreesPerStep / stepInMicrostepsArray) * amountOfSteps));
    sendBits(microsteps[i % amountOfSteps]);
  }
  delay(1000);
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
