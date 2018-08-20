/*
 * A-FA 650 GearBox using Arduino Uno
 * 
 * JisuSim, ByeongHeeJang, DongHeeLee
 * Ajou University, Suwon, Korea, 2018.
 */

#define TASK_SERIAL
#define TASK_GEAR_SHIFTING
// #define TASK_LAUNCH_CONTROL
// #define TASK_CC1101_COM

/*
 * Constants
 */
static const int BTN_PRESS_TIME = 100;
static const int CYLINDER_TIME = 600;
 
// serial communication
static const int SERIAL_DELAY = 500; // milli second

// gear shifting
static const int PIN_BTN_SHIFT_UP = 8;
static const int PIN_BTN_SHIFT_DOWN = 9;

static const int PIN_RELAY_SHIFT_UP = 3;
static const int PIN_RELAY_SHIFT_DOWN = 4;
static const int PIN_RELAY_CLUTCH = 5;

static const int GEAR_SHIFTING_DELAY = 100;

// ignition cut
static const int PIN_RELAY_INJECTION_CUT = 6; // injection cut -> ignition cut 으로 바뀜. 나중에 코드상에서도 바꾸도록 할 것.
static const int IGNITION_CUT_TIME = 150; // ms

// launch control
static const int PIN_BTN_LAUNCH_CTR = 7;
static const int PIN_RPM = A0;
static const int TARGET_RPM = 2000;

// CC1101 module
// VCC : 3.3V
// GND : GND
// D2  - GD0 : Signals buffer ready to read, INPUT
// D10 - CSN/SS : Must be level shfited to 3.3V, OUTPUT (arduino digital write voltage is 5V.)
// D11 - SI/MOSI : Must be level shifted to 3.3V, OUTPUT
// D12 - SO/MISO : INPUT
// D13 - SCK : Must bbe level shifted to 3.3V, OUTPUT
// total 5 gpio pins required. (and 2 power wires.)

// static const int PIN_ = ;

/*
 * Button States
 */
int btnShiftUpLast = LOW;
int btnShiftDownLast = LOW;
int btnLaunchCtrLast = LOW;

/*
 * Tasks
 */
struct Task {
  int state;
  long time;

  Task(int state = 0, long time = 0) : state(state), time(time) {}
};

Task taskSerial; // Serial communication
Task taskGear; // Gear shifting
Task taskLaunch; // Launch control
Task taskCC1101; // CC1101 Module communication

void setup() {
  // init serial communication
  Serial.begin(9600);
  while (!Serial);
  
  // gear shifting
  pinMode(PIN_BTN_SHIFT_UP, INPUT);
  pinMode(PIN_BTN_SHIFT_DOWN, INPUT);
  
  pinMode(PIN_RELAY_SHIFT_UP, OUTPUT);
  pinMode(PIN_RELAY_SHIFT_DOWN, OUTPUT);
  pinMode(PIN_RELAY_CLUTCH, OUTPUT);
  digitalWrite(PIN_RELAY_SHIFT_UP, HIGH);
  digitalWrite(PIN_RELAY_SHIFT_DOWN, HIGH);
  digitalWrite(PIN_RELAY_CLUTCH, HIGH);

  // injection cut
  pinMode(PIN_RELAY_INJECTION_CUT, OUTPUT);
  digitalWrite(PIN_RELAY_INJECTION_CUT, HIGH);

  // launch control
  pinMode(PIN_BTN_LAUNCH_CTR, INPUT);

  // CC1101 module
  // pinMode(PIN_, );
}

void loop() {
  runTaskSerial();
  runTaskGearShifting();
  runTaskLaunchControl();
  runTaskCC1101COM();
}

void runTaskSerial() {
#ifdef TASK_SERIAL
  if (taskSerial.state == 0) {
    Serial.print("BTN_SHIFT_UP: ");
    Serial.print(digitalRead(PIN_BTN_SHIFT_UP));
    Serial.print(", BTN_SHIFT_DOWN: ");
    Serial.print(digitalRead(PIN_BTN_SHIFT_DOWN));
    Serial.print(", BTN_LAUNCH_CTR: ");
    Serial.print(digitalRead(PIN_BTN_LAUNCH_CTR));
    //Serial.print(", taskStateLaunchCtr: ");
    //Serial.print(taskStateLaunchCtr);
    Serial.println();
    taskSerial.state++;
    taskSerial.time = millis();
  } else if (taskSerial.state == 1) {
    if (millis() - taskSerial.time >= SERIAL_DELAY) {
      taskSerial.state = 0;
    }
  }
#else
  /* nothing */
#endif
}

// injection cut |----------------------------------|
// cylinder       |---------------|
// clutch         |
void runTaskGearShifting() {
#ifdef TASK_GEAR_SHIFTING
  int btnShiftUp, btnShiftDown;
  btnShiftUp = digitalRead(PIN_BTN_SHIFT_UP);
  btnShiftDown = digitalRead(PIN_BTN_SHIFT_DOWN);
  if (taskGear.state == 0) {
    if (!btnShiftUp || !btnShiftDown) {
      if (btnShiftUpLast == LOW && btnShiftUp == HIGH) { // shift up
        taskGear.state = 1000;
        taskGear.time = millis();
      } else if (btnShiftDownLast == LOW && btnShiftDown == HIGH) { // shift down
        taskGear.state = 2000;
        taskGear.time = millis();
      }
    }
  } else if (taskGear.state == 1000) { // check shift up signal
    long cur = millis();
    if (cur - taskGear.time >= BTN_PRESS_TIME) {
      if (btnShiftUp) {
        digitalWrite(PIN_RELAY_INJECTION_CUT, LOW);
        digitalWrite(PIN_RELAY_SHIFT_UP, LOW);
        // digitalWrite(PIN_RELAY_CLUTCH, LOW); // TODO clutch
        taskGear.state = 100;
        taskGear.time = cur;
      } else {
        taskGear.state = 0;
      }
    }
  } else if (taskGear.state == 2000) { // check shift down signal
    long cur = millis();
    if (cur - taskGear.time >= BTN_PRESS_TIME) {
      if (btnShiftDown) {
        digitalWrite(PIN_RELAY_SHIFT_DOWN, LOW);
        taskGear.state = 200;
        taskGear.time = cur;
      } else {
        taskGear.state = 0;
      }
    }
  } else if (taskGear.state == 100) { // shift up
    long cur = millis();
    if (cur - taskGear.time >= IGNITION_CUT_TIME) {
      Serial.println("Shift Up");
      digitalWrite(PIN_RELAY_INJECTION_CUT, HIGH);
      taskGear.state++;
      taskGear.time = cur;
    }
  } else if (taskGear.state == 101) {
    long cur = millis();
    if (cur - taskGear.time >= CYLINDER_TIME - IGNITION_CUT_TIME) {
      digitalWrite(PIN_RELAY_SHIFT_UP, HIGH);
      taskGear.state++;
    }
  } else if (taskGear.state == 102) {
    if (!btnShiftUp) {
      taskGear.state++;
      taskGear.time = millis();
    }
  } else if (taskGear.state == 103) {
    if (millis() - taskGear.time >= GEAR_SHIFTING_DELAY) {
      taskGear.state = 0;
    }
  } else if (taskGear.state == 200) {// shift down
    long cur = millis();
    if (cur - taskGear.time >= CYLINDER_TIME) {
      Serial.println("Shift Down");
      digitalWrite(PIN_RELAY_SHIFT_DOWN, HIGH);
      taskGear.state++;
    }
  } else if (taskGear.state == 201) {
    if (!btnShiftDown) {
      taskGear.state++;
      taskGear.time = millis();
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
  int btnLaunchCtr;
  btnLaunchCtr = digitalRead(PIN_BTN_LAUNCH_CTR);
  if (taskLaunch.state == 0) {
    if (btnLaunchCtr) {
      int rpm = analogRead(PIN_RPM) / 1024.0 * 17000.0;

      if (rpm > TARGET_RPM) {
        digitalWrite(PIN_RELAY_INJECTION_CUT, LOW);
      } else {
        digitalWrite(PIN_RELAY_INJECTION_CUT, HIGH);
      }

      taskLaunch.state++;
      taskLaunch.time = millis();
    } else {
      digitalWrite(PIN_RELAY_INJECTION_CUT, HIGH);
    }
  } else if (taskLaunch.state == 1) {
    if (millis() - taskLaunch.time >= 10) {
      taskLaunch.state = 0;
    }
  }
  btnLaunchCtrLast = btnLaunchCtr;
#else
  /* nothing */
#endif
}

void runTaskCC1101COM() {
#ifdef TASK_CC1101_COM
  
#else
  /* nothing */
#endif
}

