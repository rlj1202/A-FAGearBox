/*
 * A-FA 600 GearBox using Arduino Uno
 * 
 * JisuSim, ByeongHeeJang, DongHeeLee
 * Ajou University, Suwon, Korea, 2018.
 */

/*
 * Constants
 */
// clutch position control
static const int PIN_DTR_UP_SMALL = 2; // DTR = Darlington Transistor
static const int PIN_DTR_UP_BIG = 3;
static const int PIN_DTR_DOWN_A = 4;
static const int PIN_DTR_DOWN_B = 5;

static const int PIN_SS_TARGET = A0; // SS = Sensor
static const int PIN_SS_REAL = A1;
 
// gear shifting
static const int PIN_RELAY_SHIFT_UP = 6;
static const int PIN_RELAY_SHIFT_DOWN = 7;
static const int PIN_RELAY_INJECTION_CUT = 8;

static const int PIN_BTN_SHIFT_UP = 9;
static const int PIN_BTN_SHIFT_DOWN = 10;

// launch control
static const int PIN_BTN_LAUNCH_CTR = 11;

static const int PIN_SS_RPM = A2;

/*
 * Button states
 */
int btnShiftUpLast = LOW;
int btnShiftDownLast = LOW;
int btnLaunchCtrLast = LOW;

/*
 * Clutch position control
 */
long delayTime = 0;

// PID control
float k_p = 0.025;
float k_i = 0;
float k_d = 1.1;
double integral = 0;

/*
 * Tasks
 */
struct Task {
  int state;
  long time;

  Task(int state = 0, long time = 0) : state(state), time(time) {}
};

Task taskSerial; // serial communication
Task taskClutch; // clutch positioning, clutch position control
Task taskGear; // gear shifting
Task taskLaunch; // launch control

void setup() {
  // init serial communication
  Serial.begin(9600);
  while (!Serial);

  // gear shifting
  pinMode(PIN_RELAY_SHIFT_UP, OUTPUT);
  pinMode(PIN_RELAY_SHIFT_DOWN, OUTPUT);
  pinMode(PIN_RELAY_INJECTION_CUT, OUTPUT);
  digitalWrite(PIN_RELAY_SHIFT_UP, HIGH);
  digitalWrite(PIN_RELAY_SHIFT_DOWN, HIGH);
  digitalWrite(PIN_RELAY_INJECTION_CUT, HIGH);

  pinMode(PIN_BTN_SHIFT_UP, INPUT);
  pinMode(PIN_BTN_SHIFT_DOWN, INPUT);

  // launch contorl
  pinMode(PIN_BTN_LAUNCH_CTR, INPUT);
}

void loop() {
  // serial communication
  if (taskSerial.state == 0) {
    Serial.print("test");
    Serial.println();

    taskSerial.state++;
    taskSerial.time = millis();
  } else if (taskSerial.state == 1) {
    if (millis() - taskSerial.time >= 100) {
      taskSerial.state = 0;
    }
  }
  
  // clutch position control
  if (taskClutch.state == 0) {
    long curTime = millis();
    long elapsed = curTime - taskClutch.time;

    double target = 1024.0 - analogRead(PIN_SS_TARGET);
    double real = analogRead(PIN_SS_REAL);
    double delta = target - real;

    /*
    if (abs(delta) < 50) {
      delta = 0;
    }
     */

    integral += delta * elapsed / 100000.0;

    double amount = k_p * delta + k_i * integral + k_d * (delta / elapsed);
    delayTime = min(max(abs(amount), 11), 40);

    if (abs(delta) > 50) {
      if (amount < 0) {// upward
        if (abs(delta) > 200) {
          digitalWrite(PIN_DTR_UP_BIG, HIGH);
          taskClutch.state = 1000;
          taskClutch.time = millis();
        } else {
          digitalWrite(PIN_DTR_UP_SMALL, HIGH);
          taskClutch.state = 1100;
          taskClutch.time = millis();
        }
      } else {// downward
        digitalWrite(PIN_DTR_DOWN_A, HIGH);
        taskClutch.state = 2000;
        taskClutch.time = millis();
      }
    }
  } else if (taskClutch.state == 1000) { // up big
    long cur = millis();
    if (cur - taskClutch.time >= delayTime) {
      digitalWrite(PIN_DTR_UP_BIG, LOW);
      taskClutch.state++;
      taskClutch.time = cur;
    }
  } else if(taskClutch.state == 1001) {
    if (millis() - taskClutch.time >= 100 - delayTime) {
      taskClutch.state = 0;
    }
  } else if (taskClutch.state == 1100) { // up small
    long cur = millis();
    if (cur - taskClutch.time >= delayTime) {
      digitalWrite(PIN_DTR_UP_SMALL, LOW);
      taskClutch.state++;
      taskClutch.time = cur;
    }
  } else if (taskClutch.state == 1101) {
    if (millis() - taskClutch.time >= 100 - delayTime) {
      taskClutch.state = 0;
    }
  } else if (taskClutch.state == 2000) { // down
    long cur = millis();
    if (cur - taskClutch.time >= delayTime) {
      digitalWrite(PIN_DTR_DOWN_A, LOW);
      taskClutch.state++;
      taskClutch.time = cur;
    }
  } else if (taskClutch.state == 2001) {
    long cur = millis();
    if (cur - taskClutch.time >= 100 - delayTime) {
      digitalWrite(PIN_DTR_DOWN_B, HIGH);
      taskClutch.state++;
      taskClutch.time = cur;
    }
  } else if (taskClutch.state == 2002) {
    long cur = millis();
    if (cur - taskClutch.time >= delayTime) {
      digitalWrite(PIN_DTR_DOWN_B, LOW);
      taskClutch.state++;
      taskClutch.time = cur;
    }
  } else if (taskClutch.state == 2003) {
    if (millis() - taskClutch.time >= 100 - delayTime) {
      taskClutch.state = 0;
    }
  }
  
  // gear shifting
  int btnShiftUp, btnShiftDown;
  btnShiftUp = digitalRead(PIN_BTN_SHIFT_UP);
  btnShiftDown = digitalRead(PIN_BTN_SHIFT_DOWN);
  if (taskGear.state == 0) {
    if (!btnShiftUp || !btnShiftDown) {
      if (btnShiftUp && !btnShiftUpLast) {
        digitalWrite(PIN_RELAY_INJECTION_CUT, LOW);
        taskGear.time = millis();
        taskGear.state = 100;
      } else if (btnShiftDown && !btnShiftDownLast) {
        digitalWrite(PIN_RELAY_SHIFT_DOWN, LOW);
        taskGear.time = millis();
        taskGear.state = 200;
      }
    }
  } else if (taskGear.state == 100) { // shift up
    long cur = millis();
    if (cur - taskGear.time >= 10) {
      digitalWrite(PIN_RELAY_SHIFT_UP, LOW);
      taskGear.time = cur;
      taskGear.state++;
    }
  } else if (taskGear.state == 101) {
    long cur = millis();
    if (cur - taskGear.time >= 10) {
      digitalWrite(PIN_RELAY_SHIFT_UP, HIGH);
      taskGear.time = cur;
      taskGear.state++;
    }
  } else if (taskGear.state == 102) {
    long cur = millis();
    if (cur - taskGear.time >= 10) {
      digitalWrite(PIN_RELAY_INJECTION_CUT, HIGH);
      taskGear.time = cur;
      taskGear.state++;
    }
  } else if (taskGear.state == 103) {
    if (millis() - taskGear.time >= 1000) {
      taskGear.state = 0;
    }
  } else if (taskGear.state == 200) { // shift down
    long cur = millis();
    if (cur - taskGear.time >= 10) {
      digitalWrite(PIN_RELAY_SHIFT_DOWN, HIGH);
      taskGear.time = cur;
      taskGear.state++;
    }
  } else if (taskGear.state == 201) {
    if (millis() - taskGear.time >= 1000) {
      taskGear.state = 0;
    }
  }
  btnShiftUpLast = btnShiftUp;
  btnShiftDownLast = btnShiftDown;

  // launch control
  int btnLaunchCtr = digitalRead(PIN_BTN_LAUNCH_CTR);
  if (taskLaunch.state == 0) {
    if (btnLaunchCtr) {
      
    }
  }
  btnLaunchCtrLast = btnLaunchCtr;
}

