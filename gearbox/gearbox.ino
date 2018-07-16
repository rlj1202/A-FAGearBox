/*
 * A-FA GearBox using Arduino Uno
 * 
 * 
 * 
 * JisuSim, ByeongHeeJang
 * Ajou University, Korea, 2018.
 */

// pins
static const int PIN_UP_SLOW = 2;
static const int PIN_UP_FAST = 3;
static const int PIN_DOWN_A = 4;
static const int PIN_DOWN_B = 5;

static const int PIN_TARGET = A0;
static const int PIN_REAL = A1;

// sensors and difference between the two
float target = 0;
float real = 0;
float delta = 0;

// amount of control volume
float amount = 0;
float delayTime = 0;

// constants of PID control
float k_p = 0.025;
float k_i = 0; //0.00005;
float k_d = 1.1;
float integral = 0;

float lastTime;

void setup() {
  Serial.begin(9600);
  while (!Serial) {}

  pinMode(PIN_UP_SLOW, OUTPUT);
  pinMode(PIN_UP_FAST, OUTPUT);
  pinMode(PIN_DOWN_A, OUTPUT);
  pinMode(PIN_DOWN_B, OUTPUT);

  digitalWrite(PIN_UP_SLOW, LOW);
  digitalWrite(PIN_UP_FAST, LOW);
  digitalWrite(PIN_DOWN_A, LOW);
  digitalWrite(PIN_DOWN_B, LOW);

  float lastTime = millis();
}

void loop() {
  // return;

  float curTime = millis();
  float elapsed = curTime - lastTime;

  target = 1024.0 - analogRead(PIN_TARGET);
  real = analogRead(PIN_REAL);
  delta = target - real;
/*
  if (abs(delta) < 50) {
    delta = 0;
  }*/
  
  integral += delta * elapsed / 100000.0;

  amount = k_p * delta + k_i * integral + k_d * (delta / elapsed);// PID control
  delayTime = min(max(abs(amount), 11.0), 40.0);

  if (abs(delta) > 50) {
    if (amount < 0) {// upward
      if (abs(delta) > 200) {
        digitalWrite(PIN_UP_FAST, HIGH);
        delay(delayTime);
        digitalWrite(PIN_UP_FAST, LOW);
        delay(100 - delayTime);
      } else {
        digitalWrite(PIN_UP_SLOW, HIGH);
        delay(delayTime);
        digitalWrite(PIN_UP_SLOW, LOW);
        delay(100 - delayTime);
      }
    } else {// downward
      digitalWrite(PIN_DOWN_A, HIGH);
      delay(delayTime);
      digitalWrite(PIN_DOWN_A, LOW);
      delay(100 - delayTime);
      digitalWrite(PIN_DOWN_B, HIGH);
      delay(delayTime);
      digitalWrite(PIN_DOWN_B, LOW);
      delay(100 - delayTime);
    }
  }

  Serial.print(target);
  Serial.print(",");
  Serial.print(real);
  Serial.print(",");
  Serial.print(delta);
  Serial.print(",");
  Serial.print(amount);
  Serial.print(",");
  Serial.print(integral);
  Serial.println();
}

