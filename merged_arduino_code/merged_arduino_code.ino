/* Ultrasonic reader
    (C) FRC 3941
    CC-BY-SA
  Address 6
  Returns the following data over I2C:
  7x4 = 28 Bytes
  Bytes       Data
  4           Unsigned Long Ping #
  4           Float Inches Ultrasonic 0
  4           Float Inches Ultrasonic 1
  4           Float Inches Ultrasonic 2
  4           Float Inches Ultrasonic 3
  4           Float Inches Ultrasonic 4
  4           Float Inches Ultrasonic 5
  
  Send to me
  Bytes       Data
  1           byte brightness of light cannon 1
  1           byte brightness of light cannon 2
  1           byte brightness of light cannon 3
  1           byte brightness of light cannon 4
*/

#include <Wire.h>

#define UNITIALIZED 0x00
#define TRIGGERED 0x01
#define ECHOING 0x02
#define SRIDLE 0x04
#define SRERROR 0x08

const int LED1pin=6;
const int LED2pin=9;
const int LED3pin=10;
const int LED4pin=11;
byte LEDbrightness1=0;
byte LEDbrightness2=0;
byte LEDbrightness3=0;
byte LEDbrightness4=0;

const int I2CSlaveAddr = 6;



const float speedOfSound = 0.00675197; // inches per us / 2 for two way travel

unsigned long pingnumber = 0;

unsigned long curmicros = 0;
unsigned long lastpingmicros = 0;

unsigned long duration;

// Avoid 0,1 (Serial), A4, A5 (I2C) and 13 (LED)

// Front Left
unsigned long hcsr0_micros;
byte hcsr0_state = UNITIALIZED;
const int hcsr0_trig = 27;
const int hcsr0_echo = 26;
float hcsr0_distance = 0;

// Front Right
unsigned long hcsr1_micros;
byte hcsr1_state = UNITIALIZED;
const int hcsr1_trig = 31;
const int hcsr1_echo = 30;
float hcsr1_distance = 0;

// Right Side
unsigned long hcsr2_micros;
byte hcsr2_state = UNITIALIZED;
const int hcsr2_trig = 35;
const int hcsr2_echo = 34;
float hcsr2_distance = 0;

// Rear Right
unsigned long hcsr3_micros;
byte hcsr3_state = UNITIALIZED;
const int hcsr3_trig = 39;
const int hcsr3_echo = 38;
float hcsr3_distance = 0;

// Rear Left
unsigned long hcsr4_micros;
byte hcsr4_state = UNITIALIZED;
const int hcsr4_trig = 43;
const int hcsr4_echo = 42;
float hcsr4_distance = 0;

// Left Side
unsigned long hcsr5_micros;
byte hcsr5_state = UNITIALIZED;
const int hcsr5_trig = 47;
const int hcsr5_echo = 46;
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

  Wire.begin(I2CSlaveAddr);  // This will be the device
  Wire.onRequest(reportDistancesI2C);
  Serial.begin(115200);

  Wire.begin(8);
  Wire.onReceive(receiveEvent);
  
  Serial.begin(115200);
  
  pinMode(LED1pin,OUTPUT);
  pinMode(LED2pin,OUTPUT);
  pinMode(LED3pin,OUTPUT);
  pinMode(LED4pin,OUTPUT);
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
  digitalWrite(hcsr3_trig, LOW);
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

void reportDistancesI2C(){
  byte Data[28];
  volatile byte* ptr_pingnum = (byte*) &pingnumber;
  volatile byte* ptr_hcsr0_distance = (byte*) &hcsr0_distance;
  volatile byte* ptr_hcsr1_distance = (byte*) &hcsr1_distance;
  volatile byte* ptr_hcsr2_distance = (byte*) &hcsr2_distance;
  volatile byte* ptr_hcsr3_distance = (byte*) &hcsr3_distance;
  volatile byte* ptr_hcsr4_distance = (byte*) &hcsr4_distance;
  volatile byte* ptr_hcsr5_distance = (byte*) &hcsr5_distance;
  Data[0] = ptr_pingnum[0];
  Data[1] = ptr_pingnum[1];
  Data[2] = ptr_pingnum[2];
  Data[3] = ptr_pingnum[4];
  Data[4] = ptr_hcsr0_distance[0];
  Data[5] = ptr_hcsr0_distance[1];
  Data[6] = ptr_hcsr0_distance[2];
  Data[7] = ptr_hcsr0_distance[3];
  Data[8] = ptr_hcsr1_distance[0];
  Data[9] = ptr_hcsr1_distance[1];
  Data[10] = ptr_hcsr1_distance[2];
  Data[11] = ptr_hcsr1_distance[3];
  Data[12] = ptr_hcsr2_distance[0];
  Data[13] = ptr_hcsr2_distance[1];
  Data[14] = ptr_hcsr2_distance[2];
  Data[15] = ptr_hcsr2_distance[3];
  Data[16] = ptr_hcsr3_distance[0];
  Data[17] = ptr_hcsr3_distance[1];
  Data[18] = ptr_hcsr3_distance[2];
  Data[19] = ptr_hcsr3_distance[3];
  Data[20] = ptr_hcsr4_distance[0];
  Data[21] = ptr_hcsr4_distance[1];
  Data[22] = ptr_hcsr4_distance[2];
  Data[23] = ptr_hcsr4_distance[3];
  Data[24] = ptr_hcsr5_distance[0];
  Data[25] = ptr_hcsr5_distance[1];
  Data[26] = ptr_hcsr5_distance[2];
  Data[27] = ptr_hcsr5_distance[3];
  Wire.write(Data, 28);
}

void receiveEvent(int howMany) {
  if ( Wire.available() == 4){
    // look for four bytes
    LEDbrightness1 = Wire.read();
    LEDbrightness2 = Wire.read();
    LEDbrightness3 = Wire.read();
    LEDbrightness4 = Wire.read();


    
  }
  Serial.print("LED 1 = ");
  Serial.println(LEDbrightness1);
  
  Serial.print("LED 2 = ");
  Serial.println(LEDbrightness2);

  Serial.print("LED 3= ");
  Serial.println(LEDbrightness3);

  Serial.print("LED 4= ");
  Serial.println(LEDbrightness4);

  analogWrite(LED1pin,LEDbrightness1);
  analogWrite(LED2pin,LEDbrightness2);
  analogWrite(LED3pin,LEDbrightness3);
  analogWrite(LED4pin,LEDbrightness4);
  
}

