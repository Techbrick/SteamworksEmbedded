
/* Ultrasonic reader
    (C) FRC 3941
    CC-BY-SA
*/
#include <Wire.h>

#define UNITIALIZED 0x00
#define TRIGGERED 0x01
#define ECHOING 0x02
#define SRIDLE 0x04
#define SRERROR 0x08

const float speedOfSound = 0.00675197; // inches per us / 2 for two way travel

unsigned long pingnumber = 0;

unsigned long curmicros = 0;
unsigned long lastpingmicros = 0;

unsigned long duration;

// Avoid 0,1 (Serial), A4, A5 (I2C) and 13 (LED)

// Front Left
unsigned long hcsr0_micros;
byte hcsr0_state = UNITIALIZED;
const int hcsr0_trig = 2;
const int hcsr0_echo = 3;
float hcsr0_distance = 0;

// Front Right
unsigned long hcsr1_micros;
byte hcsr1_state = UNITIALIZED;
const int hcsr1_trig = 4;
const int hcsr1_echo = 5;
float hcsr1_distance = 0;

// Right Side
unsigned long hcsr2_micros;
byte hcsr2_state = UNITIALIZED;
const int hcsr2_trig = 6;
const int hcsr2_echo = 7;
float hcsr2_distance = 0;

// Rear Right
unsigned long hcsr3_micros;
byte hcsr3_state = UNITIALIZED;
const int hcsr3_trig = 8;
const int hcsr3_echo = 9;
float hcsr3_distance = 0;

// Rear Left
unsigned long hcsr4_micros;
byte hcsr4_state = UNITIALIZED;
const int hcsr4_trig = A0;
const int hcsr4_echo = A1;
float hcsr4_distance = 0;

// Left Side
unsigned long hcsr5_micros;
byte hcsr5_state = UNITIALIZED;
const int hcsr5_trig = A2;
const int hcsr5_echo = A3;
float hcsr5_distance = 0;


void setup() {
  // put your setup code here, to run once:
  pinMode(hcsr0_trig, OUTPUT);
  pinMode(hcsr0_echo, INPUT);
  pinMode(hcsr1_trig, OUTPUT);
  pinMode(hcsr1_echo, INPUT);
  pinMode(hcsr2_trig, OUTPUT);
  pinMode(hcsr2_echo, INPUT);
  pinMode(hcsr3_trig, OUTPUT);
  pinMode(hcsr3_echo, INPUT);
  pinMode(hcsr4_trig, OUTPUT);
  pinMode(hcsr4_echo, INPUT);
  pinMode(hcsr5_trig, OUTPUT);
  pinMode(hcsr5_echo, INPUT);

  Wire.begin(6);  // This will be the device

  Serial.begin(115200);
}

void trigger_hcsr() {

  //trigger ultrasonics, durration 10ms
  digitalWrite(hcsr0_trig, HIGH);
  digitalWrite(hcsr1_trig, HIGH);
  digitalWrite(hcsr2_trig, HIGH);
  digitalWrite(hcsr3_trig, HIGH);
  digitalWrite(hcsr4_trig, HIGH);
  digitalWrite(hcsr5_trig, HIGH);
  delay(10);

  //Stop triggering ultrasonics
  digitalWrite(hcsr0_trig, LOW);
  digitalWrite(hcsr1_trig, LOW);
  digitalWrite(hcsr2_trig, LOW);
  digitalWrite(hcsr4_trig, LOW);
  digitalWrite(hcsr5_trig, LOW);

  hcsr0_state = TRIGGERED;
  hcsr1_state = TRIGGERED;
  hcsr2_state = TRIGGERED;
  hcsr3_state = TRIGGERED;
  hcsr4_state = TRIGGERED;
  hcsr5_state = TRIGGERED;

}

void loop() {
  // put your main code here, to run repeatedly:

  curmicros = micros();
  if (curmicros > lastpingmicros + 99850) {
    // Check for faulty (still high) ultrasonics
    if (hcsr0_state != SRIDLE) {
      hcsr0_state = SRERROR;
      hcsr0_distance = -1;
    }
    if (hcsr1_state != SRIDLE) {
      hcsr1_state = SRERROR;
      hcsr1_distance = -1;
    }
    if (hcsr2_state != SRIDLE) {
      hcsr2_state = SRERROR;
      hcsr2_distance = -1;
    }
    if (hcsr3_state != SRIDLE) {
      hcsr3_state = SRERROR;
      hcsr3_distance = -1;
    }
    if (hcsr4_state != SRIDLE) {
      hcsr4_state = SRERROR;
      hcsr4_distance = -1;
    }
    if (hcsr5_state != SRIDLE) {
      hcsr5_state = SRERROR;
      hcsr5_distance = -1;
    }

    report_dist_serial();
    // if we have waited at least 50ms, trigger an ultrasonic burst
    lastpingmicros = micros();
    trigger_hcsr();
    pingnumber++;
  }

  // Ultrasonic 0
  if (hcsr0_state == TRIGGERED) {
    if (digitalRead(hcsr0_echo) == HIGH) {
      hcsr0_state = ECHOING;
      hcsr0_micros = curmicros;
    }
  } else if (hcsr0_state == ECHOING) {
    if (digitalRead(hcsr0_echo) == LOW) {
      hcsr0_state = SRIDLE;
      hcsr0_distance = float(micros() - hcsr0_micros) * speedOfSound;
      //hcsr0_distance = (micros() - hcsr0_micros);
    }
  }
  // Ultrasonic 1
  if (hcsr1_state == TRIGGERED) {
    if (digitalRead(hcsr1_echo) == HIGH) {
      hcsr1_state = ECHOING;
      hcsr1_micros = curmicros;
    }
  } else if (hcsr1_state == ECHOING) {
    if (digitalRead(hcsr1_echo) == LOW) {
      hcsr1_state = SRIDLE;
      hcsr1_distance = float(micros() - hcsr1_micros) * speedOfSound;
    }
  }
  // Ultrasonic 2
  if (hcsr2_state == TRIGGERED) {
    if (digitalRead(hcsr2_echo) == HIGH) {
      hcsr2_state = ECHOING;
      hcsr2_micros = curmicros;
    }
  } else if (hcsr2_state == ECHOING) {
    if (digitalRead(hcsr2_echo) == LOW) {
      hcsr2_state = SRIDLE;
      hcsr2_distance = float(micros() - hcsr2_micros) * speedOfSound;
    }
  }
  // Ultrasonic 3
  if (hcsr3_state == TRIGGERED) {
    if (digitalRead(hcsr3_echo) == HIGH) {
      hcsr3_state = ECHOING;
      hcsr3_micros = curmicros;
    }
  } else if (hcsr3_state == ECHOING) {
    if (digitalRead(hcsr3_echo) == LOW) {
      hcsr3_state = SRIDLE;
      hcsr3_distance = float(micros() - hcsr3_micros) * speedOfSound;
    }
  }
  // Ultrasonic 4
  if (hcsr4_state == TRIGGERED) {
    if (digitalRead(hcsr4_echo) == HIGH) {
      hcsr4_state = ECHOING;
      hcsr4_micros = curmicros;
    }
  } else if (hcsr4_state == ECHOING) {
    if (digitalRead(hcsr4_echo) == LOW) {
      hcsr4_state = SRIDLE;
      hcsr4_distance = float(micros() - hcsr4_micros) * speedOfSound;
    }
  }
  // Ultrasonic 5
  if (hcsr5_state == TRIGGERED) {
    if (digitalRead(hcsr5_echo) == HIGH) {
      hcsr5_state = ECHOING;
      hcsr5_micros = curmicros;
    }
  } else if (hcsr5_state == ECHOING) {
    if (digitalRead(hcsr5_echo) == LOW) {
      hcsr5_state = SRIDLE;
      hcsr5_distance = float(micros() - hcsr5_micros) * speedOfSound;
    }
  }



}

void report_dist_serial() {

  Serial.print(pingnumber);
  Serial.print(",");
  Serial.print(micros() - lastpingmicros);
  Serial.print(",");
  //Return distance
  Serial.print(hcsr0_distance);
  Serial.print(",");
  Serial.print(hcsr1_distance);
  Serial.print(",");
  Serial.print(hcsr2_distance);
  Serial.print(",");
  Serial.print(hcsr3_distance);
  Serial.print(",");
  Serial.print(hcsr4_distance);
  Serial.print(",");
  Serial.println(hcsr5_distance);
}

