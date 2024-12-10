#include <SoftwareSerial.h>
#include "TFMini.h"
#include <AccelStepper.h>
#include <Encoder.h>
TFMini tfmini;
#define PI 3.141592653589793238462643383279502884197169399375105820974944
SoftwareSerial SerialTFMini(2, 3);  //The only value that matters here is the first one, 2, Rx
AccelStepper stepper(1, 8, 9);
#define STEPS_PER_REVOLUTION 200
#define ANGULAR_RESOLUTION 1.8  // Degrees per step

Encoder encoder(4, 5);
void getTFminiData(int* distance, int* strength) {
  static char i = 0;
  char j = 0;
  int checksum = 0;
  static int rx[9];
  if (SerialTFMini.available()) {
    rx[i] = SerialTFMini.read();
    if (rx[0] != 0x59) {
      i = 0;
    } else if (i == 1 && rx[1] != 0x59) {
      i = 0;
    } else if (i == 8) {
      for (j = 0; j < 8; j++) {
        checksum += rx[j];
      }
      if (rx[8] == (checksum % 256)) {
        *distance = rx[2] + rx[3] * 256;
        *strength = rx[4] + rx[5] * 256;
      }
      i = 0;
    } else {
      i++;
    }
  }
}


void setup() {
  Serial.begin(115200);  //Initialize hardware serial port (serial debug port)
  while (!Serial)
    ;  // wait for serial port to connect. Needed for native USB port only
  Serial.println("Initializing...");
  SerialTFMini.begin(TFMINI_BAUDRATE);  //Initialize the data rate for the SoftwareSerial port
  tfmini.begin(&SerialTFMini);          //Initialize the TF Mini sensor
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);
}

void loop() {
  int distance = 0;
  int strength = 0;
  int totalVolume = 0;
  for (int angle = 0; angle < 360; angle += ANGULAR_RESOLUTION) {
    stepper.moveTo(angle * (STEPS_PER_REVOLUTION / 360));
    stepper.runToPosition();

    getTFminiData(&distance, &strength);
    while (!distance) {
      getTFminiData(&distance, &strength);
    }

    double radius = distance / 2.0;
    double crossSectionalArea = PI * radius * radius;
    totalVolume += crossSectionalArea * ANGULAR_RESOLUTION;
  }

  Serial.print("Total Volume: ");
  Serial.println(totalVolume);

  delay(1000);  // Adjust delay as needed
}
