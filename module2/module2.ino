// WHEEL PINS //

// Pins controlling wheel 1
#define m1p1 3   // Forward
#define m1p2 5   // Backward
// Pins controlling wheel 2
#define m2p1 6  // Forward
#define m2p2 9  // Backward


// LINE DETECTOR PINS //

#define RIGHT A0  // Sensor output voltage (left)
#define LEFT A1 // Sensor output voltage (right)


// COLOUR DETECTOR PINS //

#define S2 12
#define S3 13
#define reading 8


// RGB LED PINS //

#define R 7
#define G 2
#define B 4


// DISTANCE DETECTOR PINS //

#define trigPin 10
#define echoPin 11


// VARIABLES //

int vel = 255;  // Speed for the wheels to travel at
int colour  = 1;  // Cycles between 1, 2, 3 (RGB)
int count;   // Unique objects detected so far
unsigned int red, green, blue;  // Contains colour sensor values
boolean reached;  // Keep track of if an object is seen/has been reached or not
int wait;
int dist;

bool flag;

// The setup function runs once when you press reset or power the board
void setup() {
  pinMode(m1p1, OUTPUT);
  pinMode(m1p2, OUTPUT);
  pinMode(m2p1, OUTPUT);
  pinMode(m2p2, OUTPUT);

  pinMode(LEFT, INPUT);
  pinMode(RIGHT, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, OUTPUT);
  
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(reading, INPUT);

  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);

  count = 0;
  red = 0;
  green = 0;
  blue = 0;
  wait = 0;
  reached = false;
  flag = false;

  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() {

  dist = distance();
  Serial.println(dist);

  // MOVING AND NAVIGATION //

  // Have the vehicle slow as it approaches the object
  if (dist < 5 && !reached)
    vel = 100;
  else
    vel = 255;

  // Determine the direction to go - Turn if a line is encountered
  if (analogRead(LEFT) >= 1015 && analogRead(RIGHT) < 1015)
    if (reached)
      right();
    else
      left();
  else if (analogRead(LEFT) < 1015 && analogRead(RIGHT) >= 1015)
    if (reached)
      left();
    else
      right();
  // Turn to the next spoke when the centre is reached
  else if (analogRead(LEFT) >= 1015 && analogRead(RIGHT) >= 1000 && wait > 300) {
    // Tick off one object found
    count++;
    // Prepare to find the next object
    reached = false;
    wait = 0;
    flag = false;
    scan();
    // Car is aligned
  } else {
    // Go towards object upon finding it, else go back to the start
    if (reached)
      backward();
    else
      forward();
  }

  //Serial.println(analogRead(LEFT));
  //Serial.println(analogRead(RIGHT));

  // Take a color sensor reading (just from 1 of R, G, or B per cycle)
  if (colour == 1) {
    // Settings for RED color sensor
    digitalWrite(S2,LOW);
    digitalWrite(S3,LOW);
    red = pulseIn(reading, LOW);
    colour = 2;
  } else if (colour == 2) {
    // Settings for BLUE color sensor
    digitalWrite(S2,LOW);
    digitalWrite(S3,HIGH);
    blue = pulseIn(reading, LOW);
    colour = 3;
  } else if (colour == 3) {
    // Settings for GREEN color sensor
    digitalWrite(S2,HIGH);
    digitalWrite(S3,HIGH);
    green = pulseIn(reading, LOW);
    colour = 1;
  }

  // This is the easiest to detect. Red goes very low while green and blue go high
  if (red < 70 && green >= 100 && blue >= 100) {
    digitalWrite(R, HIGH);
    digitalWrite(G, LOW);
    digitalWrite(B, LOW);

    reached = true;

    backward();

    // The hardest detection of all, all three values drop significantly, at roughly equal levels
    // No one value seems particularly favored, any of them may be the lowest/highest
    // The orange lights in the lab keep the red value low at default; with no environmental
    // bias this would give too many false positives, need to be changed
  } else if ((abs(blue - red) <= (blue * .15) && abs(blue - green) <= (blue * .15))
      || (abs(red - blue) <= (red * .15) && abs(red - green) <= (red * .15))
      || (abs(green - blue) <= (green * .15) && abs(green - red) <= (green * .15))) {    
    digitalWrite(R, LOW);
    digitalWrite(G, HIGH);
    digitalWrite(B, LOW);

    reached = true;

    backward();

    // Even strong blues do not cause as much change as light red. 
    // Red and green values do not go past 100, blue does not dip  very far
    // Red and default will NEVER cause this to be true, green triggers before blue
    // Reporting blue as green & vice versa does not occur with this ordering.
  } else if (blue < red && blue < green) {
    digitalWrite(R, LOW);
    digitalWrite(G, LOW);
    digitalWrite(B, HIGH);

    reached = true;

    backward();
  }

  if (reached)
    wait++;

  delay(50);
}

void forward() {
  analogWrite(m1p1, LOW);
  analogWrite(m1p2, vel);
  analogWrite(m2p1, LOW);
  analogWrite(m2p2, vel);
}

void backward() {
  analogWrite(m1p1, vel);
  analogWrite(m1p2, LOW);
  analogWrite(m2p1, vel);
  analogWrite(m2p2, LOW);
}

void right() {
  analogWrite(m1p1, vel);
  analogWrite(m1p2, LOW);
  analogWrite(m2p1, LOW);
  analogWrite(m2p2, vel);
}

void left() {
  analogWrite(m1p1, LOW);
  analogWrite(m1p2, vel);
  analogWrite(m2p1, vel);
  analogWrite(m2p2, LOW);
}

void halt() {
  analogWrite(m1p1, vel);
  analogWrite(m1p2, vel);
  analogWrite(m2p1, vel);
  analogWrite(m2p2, vel);
}

// Scans for the next object
void scan() {
  // Complete, do nothing
  if (count == 3) {
    halt();
    delay(1000000000);
    //TODO: Replace this with a wait for a button input, then show the Serial log
  } else {
    dist = distance();
    if (!flag) {
      right();
      flag = true;
      delay(1500);
      scan();
    } else if (dist < 110) {
      forward();
      delay(250);
      // Otherwise, continue turning
    } else {
      delay(250);
      scan();
    }
  }
}

int distance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  //soundTime = pulseIn(echoPin, HIGH);
  return pulseIn(echoPin, HIGH) * 171 / 10000;
}


