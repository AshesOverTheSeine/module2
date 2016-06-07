// WHEEL PINS //

// Pins controlling wheel 1
#define m1p1 3   // Forward
#define m1p2 5   // Backward
// Pins controlling wheel 2
#define m2p1 6  // Forward
#define m2p2 9  // Backward


// LINE DETECTOR PINS //

#define LEFT A0  // Sensor output voltage (left)
#define RIGHT A1 // Sensor output voltage (right)


// COLOUR DETECTOR PINS //

#define S0 10
#define S1 11
#define S2 12
#define S3 13
#define reading 8


// RGB LED PINS //

#define R 2
#define G 4
#define B 7


// VARIABLES //

int speed = 255;  // Speed for the wheels to travel at
int colour  = 1;  // Cycles between 1, 2, 3 (RGB)
unsigned int red, green, blue = 0;  // Contains colour sensor values


// The setup function runs once when you press reset or power the board
void setup() {
  pinMode(m1p1, OUTPUT);
  pinMode(m1p2, OUTPUT);
  pinMode(m2p1, OUTPUT);
  pinMode(m2p2, OUTPUT);

  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(reading, INPUT);

  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);

  // Setting frequency selection to 20% for colour sensor
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {
  // Read from line sensor 1
  int proximityADC1 = analogRead(LEFT);
  float proximityV1 = (float)proximityADC1 * 5.0 / 1023.0;

  // Read from line sensor 2
  int proximityADC2 = analogRead(RIGHT);
  float proximityV2 = (float)proximityADC2 * 5.0 / 1023.0;

  // Determine the direction to go - Turn if a line is encountered
  if (proximityV1 <= 4.4 && proximityV2 > 4.4)
    left();
  else if (proximityV1 > 4.4 && proximityV2 <= 4.4)
    right();
  else {
    forward();
    Serial.println(proximityV1);
    Serial.println(proximityV2);
  }


  // Take a color sensor reading (just from 1 of R, G, or B per cycle)
  if (colour == 1) {
    // Settings for RED color sensor
    digitalWrite(S2,LOW);
    digitalWrite(S3,LOW);
    red = pulseIn(reading, LOW);
    Serial.print("Red: ");
    Serial.println(red);
    colour = 2;
  } else if (colour == 2) {
    // Settings for BLUE color sensor
    digitalWrite(S2,LOW);
    digitalWrite(S3,HIGH);
    blue = pulseIn(reading, LOW);
    Serial.print("Blue: ");
    Serial.println(blue);
    colour = 3;
  } else if (colour == 3) {
    // Settings for GREEN color sensor
    digitalWrite(S2,HIGH);
    digitalWrite(S3,HIGH);
    green = pulseIn(reading, LOW);
    Serial.print("Green: ");
    Serial.println(green);
    colour = 1;
  }

  delay(100);

  // This is the easiest to detect. Red goes very low while green and blue go high
  if (red < 70 && green >= 100 && blue >= 100) {
    Serial.println("The sensor detects something that is Red.");
    digitalWrite(R, HIGH);
    digitalWrite(G, LOW);
    digitalWrite(B, LOW);

    // The hardest detection of all, all three values drop significantly, at roughly equal levels
    // No one value seems particularly favored, any of them may be the lowest/highest
    // The orange lights in the lab keep the red value low at default; with no environmental
    // bias this would give too many false positives, need to be changed
  } else if ((abs(blue - red) <= (blue * .15) && abs(blue - green) <= (blue * .15))
      || (abs(red - blue) <= (red * .15) && abs(red - green) <= (red * .15))
      || (abs(green - blue) <= (green * .15) && abs(green - red) <= (green * .15))) {
    Serial.println("The sensor detects something that is Green.");
    digitalWrite(R, LOW);
    digitalWrite(G, HIGH);
    digitalWrite(B, LOW);

    // Even strong blues do not cause as much change as light red. 
    // Red and green values do not go past 100, blue does not dip  very far
    // Red and default will NEVER cause this to be true, green triggers before blue
    // Reporting blue as green & vice versa does not occur with this ordering.
  } else if (blue < red && blue < green) {
    Serial.println("The sensor detects something that is Blue.");
    digitalWrite(R, LOW);
    digitalWrite(G, LOW);
    digitalWrite(B, HIGH);
  } else {
    Serial.println("The sensor does not detect a strong Red, Green, or Blue.");
    digitalWrite(R, LOW);
    digitalWrite(G, LOW);
    digitalWrite(B, LOW);
  }

  Serial.println("---");

}

// Sets the motors moving forward
void right() {
  analogWrite(m1p1, speed);
  analogWrite(m1p2, LOW);
  analogWrite(m2p1, LOW);
  analogWrite(m2p2, speed);
}

// Sets the motors moving backward
void backward() {
  analogWrite(m1p1, speed);
  analogWrite(m1p2, LOW);
  analogWrite(m2p1, speed);
  analogWrite(m2p2, LOW);
}

void left() {
  analogWrite(m1p1, LOW);
  analogWrite(m1p2, speed);
  analogWrite(m2p1, speed);
  analogWrite(m2p2, LOW);
}

void forward() {
  analogWrite(m1p1, LOW);
  analogWrite(m1p2, speed);
  analogWrite(m2p1, LOW);
  analogWrite(m2p2, speed);
}

void halt() {
  analogWrite(m1p1, speed);
  analogWrite(m1p2, speed);
  analogWrite(m2p1, speed);
  analogWrite(m2p2, speed);
}

