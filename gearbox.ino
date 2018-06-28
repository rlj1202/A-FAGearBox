int relay1Pr = 16;
int relay2Pr = 10;

int relay1Time = 0;
int relay2Time = 0;
int relay1State = LOW;
int relay2State = LOW;

int button1Time = 0;
int button2Time = 0;
int button1State = LOW;
int button2State = LOW;

int serialTime = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
  }
  pinMode(12, INPUT);
  pinMode(11, INPUT);

  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
}

void loop() {
  int raw = analogRead(A0);
  relay1Pr = map(raw, 0, 1023, 1, 10);
  relay2Pr = map(1024 - raw, 0, 1023, 1, 10);
  
  relay1Time += 1;
  relay2Time += 1;
  serialTime += 1;
  delay(1);

  for (int i = 0; i < 10; i++) {
    digitalWrite(3, HIGH);
    delay(100);
    digitalWrite(3, LOW);
    delay(100);
  }
  delay(100A);
  for (int i = 0; i < 10; i++) {
    digitalWrite(5, HIGH);
    delay(100);
    digitalWrite(5, LOW);
    delay(100);
  }

/*
  if (relay1Time >= relay1Pr / 2) { 
    relay1Time = 0;

    if (relay1State == HIGH) {
      digitalWrite(3, LOW);
      relay1State = LOW;
    } else {
      digitalWrite(3, HIGH);
      relay1State = HIGH;
    }
  }
  if (relay2Time >= relay2Pr / 2) {
    relay2Time = 0;

    if (relay2State == HIGH) {
      digitalWrite(5, LOW);
      relay2State = LOW;
    } else {
      digitalWrite(5, HIGH);
      relay2State = HIGH;
    }
  }*/

  //

  int button1 = digitalRead(12);
  int button2 = digitalRead(11);

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

  if (serialTime >= 1000) {
    Serial.print("Button1 : ");
    Serial.print(button1);
    Serial.print(", Button2 : ");
    Serial.print(button2);
    Serial.print(", Potentiometer : ");
    Serial.print(relay1Pr);
    Serial.println();
  }
}
