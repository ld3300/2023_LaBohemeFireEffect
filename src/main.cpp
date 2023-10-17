#include <Arduino.h>

#define REDPIN D6
#define WHITEPIN D7
#define GREENPIN D5

boolean newData = false;
const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
uint16_t redMax = 100; //r
uint16_t greenMax = 255; //g
uint16_t greenRatio = 40; // use 'p' percentage green to red
uint16_t whiteMax = 255; //w
uint16_t intensityMaster = 75; // use 'i' - 0 to 100% of full intensity

long minDelay = -1000; // use m
unsigned long maxDelay = 100; // use d
unsigned long stretchDelay = 200; // use s

// void changeAmber(){
//   static unsigned long combinedDelay = 500;
//   static unsigned long lastChange = 0;
//   unsigned long currentMillis = millis();
//   if(currentMillis - lastChange >= combinedDelay){
//     uint8_t red = random(0,redMax);
//     uint8_t green = 0;
//     uint8_t greenSeed = 0;
//     if(red > 10){
//       green = random(0,greenMax);
//       green = constrain(green,0,(redMax*greenRatio)/100);
//     }
//     analogWrite(REDPIN, red);
//     analogWrite(GREENPIN, green);
//     lastChange = millis();
//     combinedDelay = random(minDelay,stretchDelay);
//     combinedDelay = constrain(combinedDelay,0,maxDelay);
//   }
// }

// void changeWhite(){
//   static unsigned long combinedDelay = 500;
//   static unsigned long lastChange = 0;
//   unsigned long currentMillis = millis();
//   if(currentMillis - lastChange >= combinedDelay){
//     uint8_t white = random(0,255);
//     analogWrite(WHITEPIN, white);
//     lastChange = millis();
//     combinedDelay = random(minDelay,stretchDelay);
//     combinedDelay = constrain(combinedDelay,0,maxDelay);
//   }
// }

void changeLeds(){
  static unsigned long combinedDelay = 500;
  static unsigned long lastChange = 0;
  unsigned long currentMillis = millis();
  if(currentMillis - lastChange >= combinedDelay){
    uint8_t white = random(0,whiteMax);
    uint8_t red = random(0,redMax);
    uint8_t green = 0;
    uint8_t greenSeed = 0;
    if(red > 10){
      green = random(0,greenMax);
      uint16_t ratioGreen = (red*greenRatio)/100;
      green = constrain(green,0,ratioGreen);
    }
    // next 3 lines adjust overall intensity as a percentage
    // if(red > 0) red = (red * intensityMaster)/100;
    // if(green > 0) green = (green * intensityMaster)/100;
    // if(white > 0) white = (white * intensityMaster)/100;
    uint16_t percentDec = (255*intensityMaster)/100;
    red = map(red, 0, 255, 0, percentDec);
    green = map(green, 0, 255, 0, percentDec);
    white = map(white, 0, 255, 0, percentDec);
    analogWrite(REDPIN, red);
    analogWrite(GREENPIN, green);
    analogWrite(WHITEPIN, white);
    lastChange = millis();
    combinedDelay = random(minDelay,stretchDelay);
    combinedDelay = constrain(combinedDelay,0,maxDelay);
  }
}

void recvWithEndMarker() {
 static byte ndx = 0;
 char endMarker = '\n';
 char rc;
 
 // if (Serial.available() > 0) {
  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (rc != endMarker) {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }
    else {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

void showNewData() {
 if (newData == true) {
  Serial.print("This just in ... ");
  Serial.println(receivedChars);
  newData = false;
 }
}

void adjustStrip(){
  if (newData == true) {
    String rcvdString = "";
    char serialColor = receivedChars[0];
    char serialValue[10];
    uint16_t strLen = sizeof(sizeof(receivedChars)/sizeof(receivedChars[0]));
    for(int i = 1; i < strLen; i++){
      rcvdString += (char)receivedChars[i];
    }
    Serial.println(rcvdString);
    long numValue = rcvdString.toInt();
    switch (serialColor){
      case 'r':
        redMax = constrain(numValue,0,255);
        break;
      case 'g':
        greenMax = constrain(numValue,0,255);
        break;
      case 'w':
        whiteMax = constrain(numValue,0,255);
        break;
      case 'd':
        maxDelay = numValue;
        break;
      case 'm':
        minDelay = numValue;
        break;
      case 's':
        stretchDelay = numValue;
        break;
      case 'p':
        greenRatio = numValue;
        break;
      case 'i':
        intensityMaster = numValue;
        break;
      default:
        break;
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(REDPIN, OUTPUT);
  pinMode(GREENPIN, OUTPUT);
  pinMode(WHITEPIN, OUTPUT);
  analogWrite(REDPIN, 10);
  analogWrite(GREENPIN, 10);
  analogWrite(WHITEPIN, 10);
  Serial.println("use d to set max flicker delay");
}

void loop() {
  recvWithEndMarker();
  adjustStrip();
  showNewData();
  // changeAmber();
  // changeWhite();
  changeLeds();
}