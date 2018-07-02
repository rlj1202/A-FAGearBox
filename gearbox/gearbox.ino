int lastDelta = 0;

int relay1Pr = 16;
int relay2Pr = 10;

int relay1State = LOW;
int relay2State = LOW;

int button1Time = 0;
int button2Time = 0;
int button1State = LOW;
int button2State = LOW;

int serialTime = 0;

void setup() {
  pinMode(12, INPUT);
  pinMode(11, INPUT);

  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);

  pinMode(8, OUTPUT);
  pinMode(9, INPUT);

  digitalWrite(5, HIGH);

  Serial.begin(9600);
  while (!Serial) {
  }
}

void loop() {
  digitalWrite(8, HIGH);
  delay(10);
  digitalWrite(8, LOW);

  float distance = analogRead(A1) / 1024.0 * 4.5;
  float target = analogRead(A0) / 1024.0 * 4.5;
  float delta = target - distance;
  
  //float delayTime = max(abs(delta) * 1.0 - 2 / abs(lastDelta - delta), 60.0);// PID
  float delayTime = 20.0;

  float lastDelta = delta;

  if (abs(delta) > 0.5) {
    if (delta > 0) {
    digitalWrite(3, LOW);
    digitalWrite(5, HIGH);
  } else {
    digitalWrite(3, HIGH);
    digitalWrite(5, LOW);
  }
  } else {
    digitalWrite(3, HIGH);
    digitalWrite(5, HIGH);
  }

/*
  if (abs(delta)>0.3) {
    if (delta > 0) {
    digitalWrite(5, HIGH);
    digitalWrite(3, LOW);
    delay(max(abs(delta * 64), 10.0));
    digitalWrite(3, HIGH);
    delay(100.0);
  } else {
    digitalWrite(3, HIGH);
    digitalWrite(5, LOW);
    delay(max(abs(delta * 64), 10.0));
    digitalWrite(5, HIGH);
    delay(100.0);
  }
  }*/

  
  

  //


/*
  if (button1 == HIGH && button1Time <= 0) {
    digitalWrite(3, HIGH);
    button1Time = 7;
  }
  if (button1Time > 0) {
    button1Time -= 1;
  } else {
    digitalWrite(3, LOW);
    button1Time = 0;
  }

  if (button2 == HIGH && button2Time <= 0) {
    digitalWrite(5, HIGH);
    button2Time = 7;
  }
  if (button2Time > 0) {
    button2Time -= 1;
  } else {
    digitalWrite(5, LOW);
    button2Time = 0;
  }*/
  int button1 = digitalRead(12);
  int button2 = digitalRead(11);
  
  Serial.print(target);
  Serial.print(", ");
  Serial.print(distance);
  Serial.print(", ");
  Serial.print(delta);
  //Serial.print(", ");
  //Serial.print(delayTime);
  Serial.println();
}

