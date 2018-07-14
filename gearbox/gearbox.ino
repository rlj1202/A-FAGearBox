/*
 * A-FA GearBox using Arduino Uno
 * 
 * 
 * 
 * JisuSim, ByeongHeeJang
 * Ajou University, Korea, 2018.
 */

static const float PWM_HZ = 12.5;// to control pnuematic piston pressure.

static const int PIN_OUT_RELAY_BACK_IN = 2;
static const int PIN_OUT_RELAY_BACK_OUT = 3;
static const int PIN_OUT_RELAY_FRONT_IN = 4;
static const int PIN_OUT_RELAY_FRONT_OUT = 5;

static const int PIN_IN_BUTTON_1 = 11;
static const int PIN_IN_BUTTON_2 = 12;

static const int PIN_IN_TARGET = A0;
static const int PIN_IN_POSITION = A1;

static const int WAIT_VALUE_ARRAY_SIZE = 25;

/*
class DelayTaskNode {
  public:
  DelayTaskNode(int milli, bool(*func)()) {
    this->milli = milli;
    this->func = func;
  }
  DelayTaskNode *next;
  DelayTaskNode *prev;
  long startTime;
  private:
  int milli;
  bool(*func)();
};

DelayTaskNode *root = NULL;

void addDelayTask(int milli, bool (*func)()) {
  DelayTaskNode *newTask = new DelayTaskNode(milli, func);
  newTask->startTime = millis();
  
  if (root == NULL) {
    root = newTask;
  } else {
    DelayTaskNode *lastNode = root;
    while (true) {
      if (lastNode->next != NULL) {
        lastNode = lastNode->next;
      } else {
        break;
      }
    }
    lastNode->next = newTask;
    newTask->prev = lastNode;
  }
}

void precceedDelayTasks() {
  int curTime = millis();
  DelayTaskNode *curNode = root;
  while (curNode != NULL) {
    long elapsed = curTime - curNode->startTime;
    if (elapsed >= curNode->milli) {
      curNode->func();
      if (curNode->prev != NULL && curNode->next != NULL) {
        curNode->prev->next = curNode->next;
        curNode->next->prev = curNode->prev;
      }
    }
    curNode = curNode->next;
  }
}*/

long waitValues[WAIT_VALUE_ARRAY_SIZE];

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

// DEPRECATED
void initWaitFunc() {
  for (int i = 0; i < WAIT_VALUE_ARRAY_SIZE; i++) {
    waitValues[i] = -1;
  }
}

// DEPRECATED
// wait function invokes given function at every given milli seconds.
template<typename Func> bool wait(int index, long milli, Func f) {
  long curTime = millis();
  long lastTime = waitValues[index];
  if (lastTime == -1) {
    lastTime = curTime;
    waitValues[index] = curTime;
  }
  long elapsed = curTime - lastTime;

  if (elapsed >= milli) {
    bool result = f();
    if (result == true) {
      waitValues[index] = millis();
    }
    return result;
  }

  return false;
}

void setup() {
  // Initialize I/O pins.
  pinMode(PIN_OUT_RELAY_BACK_IN, OUTPUT);
  pinMode(PIN_OUT_RELAY_BACK_OUT, OUTPUT);
  pinMode(PIN_OUT_RELAY_FRONT_IN, OUTPUT);
  pinMode(PIN_OUT_RELAY_FRONT_OUT, OUTPUT);
  digitalWrite(PIN_OUT_RELAY_BACK_IN, LOW);
  digitalWrite(PIN_OUT_RELAY_BACK_OUT, LOW);
  digitalWrite(PIN_OUT_RELAY_FRONT_IN, LOW);
  digitalWrite(PIN_OUT_RELAY_FRONT_OUT, LOW);

  pinMode(PIN_IN_BUTTON_1, INPUT);
  pinMode(PIN_IN_BUTTON_2, INPUT);

  // Initialize wait function.
  initWaitFunc();

  // Initialize Serial communication.
  Serial.begin(9600);
  while (!Serial) {// Wait until serial initialization is done.
  }
}

void __loop() {
  
}

void loop() {
  for (int i = 0; i < 5; i++) {
    /*digitalWrite(PIN_OUT_RELAY_FRONT_OUT, HIGH);
    delay(20);
    digitalWrite(PIN_OUT_RELAY_FRONT_OUT, LOW);
    delay(500);*/
    digitalWrite(PIN_OUT_RELAY_FRONT_IN, HIGH);
    delay(50);
    digitalWrite(PIN_OUT_RELAY_FRONT_IN, LOW);
    delay(500);
    digitalWrite(PIN_OUT_RELAY_BACK_IN, HIGH);
    delay(10);
    digitalWrite(PIN_OUT_RELAY_BACK_IN, LOW);
    delay(500);
  }
  delay(2000);
  for (int i = 0; i < 30; i++) {
    digitalWrite(PIN_OUT_RELAY_FRONT_IN, HIGH);
  digitalWrite(PIN_OUT_RELAY_BACK_OUT, HIGH);
  delay(60);
  digitalWrite(PIN_OUT_RELAY_FRONT_IN, LOW);
  digitalWrite(PIN_OUT_RELAY_BACK_OUT, LOW);
  delay(20);
  }
  delay(2000);
  Serial.println("Test");
}

void _loop() {
  float distance = analogRead(A0) / 1024.0 * 4.5;
  //float target = analogRead(A1) / 1024.0 * 4.5;
  float target = (sin(millis() / 1000.0 * 4) + 1.0) / 2.0 * 4.5;
  float delta = distance - target;
  
  float lastDelta = delta;
  long delayTime = abs(delta / 4.5 * 70.0 + 10);

  Serial.print(target);
  Serial.print(",");
  Serial.print(distance);
  Serial.print(",");
  Serial.print(delta);
  Serial.println();
  
  /*
  wait(0, 500, [&delayTime]() -> bool {
      Serial.println("LOW");
      digitalWrite(PIN_OUT_RELAY_BACK, LOW);
      return wait(1, 1000, []() -> bool {
        Serial.println("HIGH");
        digitalWrite(PIN_OUT_RELAY_BACK, HIGH);
        return true;
      });
    });*/
    digitalWrite(PIN_OUT_RELAY_FRONT_IN, LOW);
  if (abs(delta) > 0.5) {
    //long delayTime = exp((delta - 4.5) / 3.0) * 80.0;
    /*
    
    
      digitalWrite(PIN_OUT_RELAY_BACK_IN, LOW);
      delay(delayTime);
      digitalWrite(PIN_OUT_RELAY_BACK_IN, HIGH);
      delay(80 - delayTime);*/
    
    if (delta < 0) {// down
      digitalWrite(PIN_OUT_RELAY_BACK_IN, LOW);
      delay(10);
      digitalWrite(PIN_OUT_RELAY_BACK_IN, HIGH);
      delay(70);
      /*
      digitalWrite(PIN_OUT_RELAY_FRONT_OUT, LOW);
      delay(3);
      digitalWrite(PIN_OUT_RELAY_FRONT_OUT, HIGH);
      delay(1000);
      digitalWrite(PIN_OUT_RELAY_BACK_IN, LOW);
      delay(10);
      digitalWrite(PIN_OUT_RELAY_BACK_IN, HIGH);
      delay(1000);*/
      /*
      digitalWrite(PIN_OUT_RELAY_FRONT_OUT, LOW);
      digitalWrite(PIN_OUT_RELAY_BACK_IN, LOW);
      delay(20);
      digitalWrite(PIN_OUT_RELAY_FRONT_OUT, HIGH);
      digitalWrite(PIN_OUT_RELAY_BACK_IN, HIGH);
      delay(60);*/
    } else {// UP
      digitalWrite(PIN_OUT_RELAY_FRONT_IN, LOW);
      digitalWrite(PIN_OUT_RELAY_BACK_OUT, LOW);
      delay(20);
      //digitalWrite(PIN_OUT_RELAY_FRONT_IN, HIGH);
      digitalWrite(PIN_OUT_RELAY_BACK_OUT, HIGH);
      delay(60);
    }
  }
  
/*
  Serial.print(target);
  Serial.print(", ");
  Serial.print(distance);
  Serial.print(", ");
  Serial.print(delta);
  Serial.println();*/

/*
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
  }*/

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
}

