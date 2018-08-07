/*
 * A-FA 650 GearBox using Arduino Uno
 * 
 * JisuSim, ByeongHeeJang, DongHeeLee
 * Ajou University, Suwon, Korea, 2018.
 */

/*
 * Constants
 */
// gear shifting
static const int PIN_BTN_SHIFT_UP = 8;
static const int PIN_BTN_SHIFT_DOWN = 9;

static const int PIN_RELAY_SHIFT_UP = 3;
static const int PIN_RELAY_SHIFT_DOWN = 4;
static const int PIN_RELAY_CLUTCH = 5;

// injection cut
static const int PIN_RELAY_INJECTION_CUT = 6;

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
  // serial
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
    if (millis() - taskSerial.time >= 100) {
      taskSerial.state = 0;
    }
  }
  
  // gear shifting
  int btnShiftUp, btnShiftDown;
  btnShiftUp = digitalRead(PIN_BTN_SHIFT_UP);
  btnShiftDown = digitalRead(PIN_BTN_SHIFT_DOWN);
  if (taskGear.state == 0) {
    if (!btnShiftUp || !btnShiftDown) {
      if (btnShiftUpLast == LOW && btnShiftUp == HIGH) {
        digitalWrite(PIN_RELAY_INJECTION_CUT, LOW);// TODO
        digitalWrite(PIN_RELAY_SHIFT_UP, LOW);
        taskGear.state = 100;
        taskGear.time = millis();
      } else if (btnShiftDownLast == LOW && btnShiftDown == HIGH) {
        digitalWrite(PIN_RELAY_SHIFT_DOWN, LOW);
        taskGear.state = 200;
        taskGear.time = millis();
      }
    }
  } else if (taskGear.state == 100) {// shift up
    long cur = millis();
    if (cur - taskGear.time >= 11) {
      digitalWrite(PIN_RELAY_SHIFT_UP, HIGH);
      taskGear.state++;
      taskGear.time = cur;
    }
  } else if (taskGear.state == 101) {
    if (millis() - taskGear.time >= 1000) {
      taskGear.state = 0;
    }
  } else if (taskGear.state == 102) {
    
  } else if (taskGear.state == 200) {// shift down
    long cur = millis();
    if (cur - taskGear.time >= 11) {
      digitalWrite(PIN_RELAY_SHIFT_DOWN, HIGH);
      taskGear.state++;
      taskGear.time = cur;
    }
  } else if (taskGear.state == 201) {
    if (millis() - taskGear.time >= 1000) {
      taskGear.state = 0;
    }
  }
  btnShiftUpLast = btnShiftUp;
  btnShiftDownLast = btnShiftDown;

  // launch control
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

  // CC1101 module
  
}

