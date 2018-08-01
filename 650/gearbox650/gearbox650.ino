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
// serial
int taskStateSerial = 0;
long taskTimeSerial = 0;
 
// gear shifting
int taskStateGearShifting = 0;
long taskTimeGearShifting = 0;

// launch control
int taskStateLaunchCtr = 0;
long taskTimeLaunchCtr = 0;

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
  if (taskStateSerial == 0) {
    Serial.print("BTN_SHIFT_UP: ");
    Serial.print(digitalRead(PIN_BTN_SHIFT_UP));
    Serial.print(", BTN_SHIFT_DOWN: ");
    Serial.print(digitalRead(PIN_BTN_SHIFT_DOWN));
    Serial.print(", BTN_LAUNCH_CTR: ");
    Serial.print(digitalRead(PIN_BTN_LAUNCH_CTR));
    //Serial.print(", taskStateLaunchCtr: ");
    //Serial.print(taskStateLaunchCtr);
    Serial.println();
    taskStateSerial++;
    taskTimeSerial = millis();
  } else if (taskStateSerial == 1) {
    if (millis() - taskTimeSerial >= 100) {
      taskStateSerial = 0;
    }
  }
  
  // gear shifting
  int btnShiftUp, btnShiftDown;
  btnShiftUp = digitalRead(PIN_BTN_SHIFT_UP);
  btnShiftDown = digitalRead(PIN_BTN_SHIFT_DOWN);
  if (taskStateGearShifting == 0) {
    if (!btnShiftUp || !btnShiftDown) {
      if (btnShiftUpLast == LOW && btnShiftUp == HIGH) {
        digitalWrite(PIN_RELAY_SHIFT_UP, LOW);
        taskStateGearShifting = 100;
        taskTimeGearShifting = millis();
      } else if (btnShiftDownLast == LOW && btnShiftDown == HIGH) {
        digitalWrite(PIN_RELAY_SHIFT_DOWN, LOW);
        taskStateGearShifting = 200;
        taskTimeGearShifting = millis();
      }
    }
  } else if (taskStateGearShifting == 100) {
    long cur = millis();
    if (cur - taskTimeGearShifting >= 11) {
      digitalWrite(PIN_RELAY_SHIFT_UP, HIGH);
      taskStateGearShifting++;
      taskTimeGearShifting = cur;
    }
  } else if (taskStateGearShifting == 101) {
    if (millis() - taskTimeGearShifting >= 1000) {
      taskStateGearShifting = 0;
    }
  } else if (taskStateGearShifting == 200) {
    long cur = millis();
    if (cur - taskTimeGearShifting >= 11) {
      digitalWrite(PIN_RELAY_SHIFT_DOWN, HIGH);
      taskStateGearShifting++;
      taskTimeGearShifting = cur;
    }
  } else if (taskStateGearShifting == 201) {
    if (millis() - taskTimeGearShifting >= 1000) {
      taskStateGearShifting = 0;
    }
  }
  btnShiftUpLast = btnShiftUp;
  btnShiftDownLast = btnShiftDown;

  // launch control
  int btnLaunchCtr;
  btnLaunchCtr = digitalRead(PIN_BTN_LAUNCH_CTR);
  if (taskStateLaunchCtr == 0) {
    if (btnLaunchCtr) {
      int rpm = analogRead(PIN_RPM) / 1024.0 * 17000.0;

      if (rpm > TARGET_RPM) {
        digitalWrite(PIN_RELAY_INJECTION_CUT, LOW);
      } else {
        digitalWrite(PIN_RELAY_INJECTION_CUT, HIGH);
      }

      taskStateLaunchCtr++;
      taskTimeLaunchCtr = millis();
    } else {
      digitalWrite(PIN_RELAY_INJECTION_CUT, HIGH);
    }
  } else if (taskStateLaunchCtr == 1) {
    if (millis() - taskTimeLaunchCtr >= 10) {
      taskStateLaunchCtr = 0;
    }
  }
  btnLaunchCtrLast = btnLaunchCtr;

  // CC1101 module
  
}

