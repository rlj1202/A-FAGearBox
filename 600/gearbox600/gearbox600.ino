/*
 * A-FA 600 GearBox using Arduino Uno
 * 
 * JisuSim, ByeongHeeJang, DongHeeLee
 * Ajou University, Suwon, Korea, 2018.
 */

#define TASK_SERIAL
// #define TASK_CLUTCH_POSITION_CONTROL
#define TASK_GEAR_SHIFTING
// #define TASK_LAUNCH_CONTROL

/*
 * Constants
 */
static const int IGNITION_CUT_TIME = 150;
static const int CYLINDER_TIME = 600;

// clutch position control
// DEPRECATED
static const int PIN_DTR_UP_SMALL = 2; // DTR = Darlington Transistor
static const int PIN_DTR_UP_BIG = 3;
static const int PIN_DTR_DOWN_A = 4;
static const int PIN_DTR_DOWN_B = 5;

static const int PIN_SS_TARGET = A0; // SS = Sensor
static const int PIN_SS_REAL = A1;
 
// gear shifting
static const int PIN_RELAY_SHIFT_UP = 2; // 6;
static const int PIN_RELAY_SHIFT_DOWN = 3; // 7;
static const int PIN_RELAY_INJECTION_CUT = 8;

static const int PIN_BTN_SHIFT_UP = 9;
static const int PIN_BTN_SHIFT_DOWN = 10;

static const int GEAR_SHIFTING_DELAY = 100;

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
  digitalWrite(PIN_RELAY_SHIFT_UP, LOW);
  digitalWrite(PIN_RELAY_SHIFT_DOWN, LOW);
  digitalWrite(PIN_RELAY_INJECTION_CUT, HIGH);

  pinMode(PIN_BTN_SHIFT_UP, INPUT_PULLUP);
  pinMode(PIN_BTN_SHIFT_DOWN, INPUT_PULLUP);

  // launch control
  pinMode(PIN_BTN_LAUNCH_CTR, INPUT);

  // clutch position control
  pinMode(PIN_DTR_UP_SMALL, OUTPUT);
  pinMode(PIN_DTR_UP_BIG, OUTPUT);
  pinMode(PIN_DTR_DOWN_A, OUTPUT);
  pinMode(PIN_DTR_DOWN_B, OUTPUT);

  //digitalWrite(PIN_RELAY_INJECTION_CUT, LOW);
}

void loop() {
  runTaskSerial();
  runTaskClutchPositionControl();
  runTaskGearShifting();
  runTaskLaunchControl();
}

void runTaskSerial() {
#ifdef TASK_SERIAL
  if (taskSerial.state == 0) {
    Serial.print("Shift up: ");
    Serial.print(!digitalRead(PIN_BTN_SHIFT_UP));
    Serial.print(", Shift down: ");
    Serial.print(!digitalRead(PIN_BTN_SHIFT_DOWN));
    Serial.print(", Clutch: ");
    Serial.print(analogRead(PIN_SS_TARGET));
    Serial.print(", Real: ");
    Serial.print(analogRead(PIN_SS_REAL));
    Serial.print(", taskClutch.state: ");
    Serial.print(taskGear.state);
    Serial.println();

    taskSerial.state++;
    taskSerial.time = millis();
  } else if (taskSerial.state == 1) {
    if (millis() - taskSerial.time >= 1000) {
      taskSerial.state = 0;
    }
  }
#else
  /* nothing */
#endif
}

void runTaskClutchPositionControl() {
#ifdef TASK_CLUTCH_POSITION_CONTROL
  if (taskClutch.state == 0) {
    long curTime = millis();
    long elapsed = curTime - taskClutch.time;

    double target = analogRead(PIN_SS_TARGET);
    double real = analogRead(PIN_SS_REAL) / 365.0 * 1024.0;
    double delta = target - real;

    /*
    if (abs(delta) < 50) {
      delta = 0;
    }
     */

    integral += delta * elapsed / 100000.0;

    double amount = k_p * delta + k_i * integral + k_d * (delta / elapsed);
    delayTime = min(max(abs(amount), 11), 40);

    if (abs(delta) > 20) {
      if (amount > 0) {// upward
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
      Serial.println("Up big next 1");
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
#else
  /* nothing */
#endif
}

void runTaskGearShifting() {
#ifdef TASK_GEAR_SHIFTING
  int btnShiftUp, btnShiftDown;
  btnShiftUp = !digitalRead(PIN_BTN_SHIFT_UP); // pull up
  btnShiftDown = !digitalRead(PIN_BTN_SHIFT_DOWN); // pull up
  if (taskGear.state == 0) {
    if (!btnShiftUp || !btnShiftDown) {
      if (btnShiftUp) { // shift up
        taskGear.time = millis();
        taskGear.state = 1000;
      } else if (btnShiftDown) { // shift down
        taskGear.time = millis();
        taskGear.state = 2000;
      }
    }
  } else if (taskGear.state == 1000) { // check shift up signal
    long cur = millis();
    if (cur - taskGear.time >= 150) {
      if (btnShiftUp) {
        Serial.println("Shift Up");
        digitalWrite(PIN_RELAY_INJECTION_CUT, LOW);
        digitalWrite(PIN_RELAY_SHIFT_UP, HIGH); // relay -> DTR 로 바꿈. low, high 반대
        taskGear.state = 100;
        taskGear.time = cur;
      } else {
        taskGear.state = 0;
      }
    }
  } else if (taskGear.state == 2000) { // check shift down signal
    long cur = millis();
    if (cur - taskGear.time >= 150) {
      if (btnShiftDown) {
        Serial.println("Shift Down");
        digitalWrite(PIN_RELAY_SHIFT_DOWN, HIGH);
        taskGear.state = 200;
        taskGear.time = cur;
      } else {
        taskGear.state = 0;
      }
    }
  } else if (taskGear.state == 100) { // shift up
    long cur = millis();
    if (cur - taskGear.time >= IGNITION_CUT_TIME) {
      digitalWrite(PIN_RELAY_INJECTION_CUT, HIGH);
      taskGear.time = cur;
      taskGear.state++;
    }
  } else if (taskGear.state == 101) {
    if (millis() - taskGear.time >= CYLINDER_TIME - IGNITION_CUT_TIME) { // 릴레이 50 + 550 = 600ms
      digitalWrite(PIN_RELAY_SHIFT_UP, LOW);
      taskGear.state++;
    }
  } else if (taskGear.state == 102) {
    if (!btnShiftUp) {
      taskGear.state++;
      taskGear.time = millis();
    }
  } else if (taskGear.state == 104) {
    if (millis() - taskGear.time >= GEAR_SHIFTING_DELAY) {
      taskGear.state = 0;
    }
  } else if (taskGear.state == 200) { // shift down
    if (millis() - taskGear.time >= CYLINDER_TIME) { // DTR 600ms
      digitalWrite(PIN_RELAY_SHIFT_DOWN, LOW);
      taskGear.state++;
    }
  } else if (taskGear.state == 201) {
    if (!btnShiftDown) {
      taskGear.time = millis();
      taskGear.state++;
    }
    
  } else if (taskGear.state == 202) {
    if (millis() - taskGear.time >= GEAR_SHIFTING_DELAY) {
      taskGear.state = 0;
    }
  }
  btnShiftUpLast = btnShiftUp;
  btnShiftDownLast = btnShiftDown;
#else
  /* nothing */
#endif
}

void runTaskLaunchControl() {
#ifdef TASK_LAUNCH_CONTROL
  int btnLaunchCtr = digitalRead(PIN_BTN_LAUNCH_CTR);
  if (taskLaunch.state == 0) {
    if (btnLaunchCtr) {
      
    }
  }
  btnLaunchCtrLast = btnLaunchCtr;
#else
  /* nothing */
#endif
}

