d i th// WHEEL PINS //

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

// DISTANCE DETECTOR PINS //

#define far A5
#define near A4

// VARIABLES //

int speed = 255;  // Speed for the wheels to travel at
int colour  = 1;  // Cycles between 1, 2, 3 (RGB)
int count  = 0;   // Unique objects detected so far
unsigned int red, green, blue = 0;  // Contains colour sensor values
boolean reached = false;  // Keep track of if an object is seen/has been reached or not

int movingAvgNear[10], movingAvgFar[10];
int avgCount = 0;
int avgFar, avgNear = 0;

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

  // MOVING AND NAVIGATION //


  // Have the vehicle slow as it approaches the object
  if (avgNear > 500 && !reached)
    speed = 100;
  else
    speed = 255;

  // Determine the direction to go - Turn if a line is encountered
  if (analogRead(LEFT) >= 900 && analogRead(RIGHT) < 900)
    left();
  else if (analogRead(LEFT) < 900 && analogRead(RIGHT) >= 900)
    right();
  // Turn to the next spoke when the centre is reached
  else if (analogRead(LEFT) >= 900 && analogRead(RIGHT) >= 900) {
    // Tick off one object found
    if (reached)
      count++;

    // Prepare to find the next object
    reached = false;
    scan();
  // Car is aligned
  } else {
    // Go towards object upon finding it, else go back to the start
    if (reached)
      backward();
    else
      forward();
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

  distance();
  
  // Long distance detection (all outdated, keeping for debugging)

  /*
  if (avgCount == 10) {
    for (int i = 0; i < 10; i++)
      avg += movingAvg[i];
    //Serial.println(avg/10);

    if (avg/10 > 70 && avg/10 <= 82)
      high = 150;
    else if (avg/10 <= 102)
      high = 130;
    else if (avg/10 <= 113)
      high = 120;
    else if (avg/10 <= 123)
      high = 110;
    else if (avg/10 <= 133)
      high = 100;
    else if (avg/10 <= 153)
      high = 90;
    else if (avg/10 <= 164)
      high = 80;
    else if (avg/10 <= 184)
      high = 70;
    else if (avg/10 <= 221)
      high = 60;
    else if (avg/10 <= 256)
      high = 50;
    else if (avg/10 <= 317)
      high = 40;
    else if (avg/10 <= 409)
      high = 30;
    else if (avg/10 <= 532)
      high = 20;
    else
      Serial.println("Bad Reading");
    
    avgCount = 0;
    avg = 0;
  }


    
  movingAvg[avgCount] = analogRead(far);
  avgCount++;
  Serial.println(analogRead(near));
*/

  // This is the easiest to detect. Red goes very low while green and blue go high
  if (red < 70 && green >= 100 && blue >= 100) {
    Serial.println("The sensor detects something that is Red.");
    
    digitalWrite(R, HIGH);
    digitalWrite(G, LOW);
    digitalWrite(B, LOW);

    reached = true;
    
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
    
    reached = true;

  // Even strong blues do not cause as much change as light red. 
  // Red and green values do not go past 100, blue does not dip  very far
  // Red and default will NEVER cause this to be true, green triggers before blue
  // Reporting blue as green & vice versa does not occur with this ordering.
  } else if (blue < red && blue < green) {
    Serial.println("The sensor detects something that is Blue.");
  
    digitalWrite(R, LOW);
    digitalWrite(G, LOW);
    digitalWrite(B, HIGH);

    reached = true;
    
  } else {
    Serial.println("The sensor does not detect a strong Red, Green, or Blue.");
  
    digitalWrite(R, LOW);
    digitalWrite(G, LOW);
    digitalWrite(B, LOW);
  }

  Serial.println("---");

  // TODO: Might want a delay here

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

void distance() {
  // Keeps a moving average for the distance sensors as they can be a bit wild
  movingAvgFar[avgCount] = analogRead(far);
  movingAvgNear[avgCount] = analogRead(near);
  avgCount = (avgCount + 1) % 10;
  
  for (int i = 0; i < 10; i++) {
    avgFar += movingAvgFar[i];
    avgNear += movingAvgNear[i];
  }

  avgFar = avgFar / 10;
  avgNear = avgNear / 10;
  
}
// Scans for the next object
void scan() {
  // Complete, do nothing
  if (count == 3) {
    halt();
    delay(1000000000);
  } else {
    // Go forward if object detected. Having it travel up a bit so it doesn't have both sensors over tape
    if (analogRead(far) > 125) {
      forward();
      delay(50);
    // Otherwise, continue turning
    } else {
      right();
      distance();
      scan();
    }
  }
}


