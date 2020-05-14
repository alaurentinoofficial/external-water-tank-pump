#define TANK_LEVEL_MIN_PIN A0
#define TANK_LEVEL_MAX_PIN A1
#define EXTERNAL_TANK_LEVEL_MIN_PIN A2

#define PUMP_RELAY_PIN 2
#define BTN_TURN_PIN 3
#define BTN_STOP_PIN 4
#define LED_AUTO_PIN 5

#define TURN_OFF_PUMP_AUTO_DELAY 2000

bool autoControll = true;

void setup() {
  pinMode(TANK_LEVEL_MIN_PIN, INPUT);
  pinMode(TANK_LEVEL_MAX_PIN, INPUT);
  pinMode(EXTERNAL_TANK_LEVEL_MIN_PIN, INPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  pinMode(BTN_TURN_PIN, INPUT);
  pinMode(BTN_STOP_PIN, INPUT);
  pinMode(LED_AUTO_PIN, OUTPUT);

  turnOffPump();

  Serial.begin(9600);
}

void loop() {
  stopAutoBtnPressEvent();
  manualOnOffBtnPressEvent();
  autoOnOffPump();
}

void stopAutoBtnPressEvent() {
  if (digitalRead(BTN_STOP_PIN) == HIGH) {
    delay(500);
    while(digitalRead(BTN_STOP_PIN) == HIGH);

    autoControll = !autoControll;

    Serial.println("> Status is " + String(autoControll) + "\n");
  }
}

void manualOnOffBtnPressEvent() {
  if (digitalRead(BTN_TURN_PIN) == HIGH) {
    while(digitalRead(BTN_TURN_PIN) == HIGH);
    delay(500);
    
    Serial.println("> Turn on manually");

    if ((autoControll && tankIsAlreadyFull(true)) || !externalTankContainsMinLevel(true)) {
      delay(500);
      return;
    }

    turnOnPump();
    Serial.println("> Pump on");
    
    if (autoControll) {
      waitTankBecomeFull(true);
      delay(TURN_OFF_PUMP_AUTO_DELAY);
    }
    else {
      while(digitalRead(BTN_TURN_PIN) == LOW);
      while(digitalRead(BTN_TURN_PIN) == HIGH);
    }
      
    turnOffPump();
    Serial.println("> Pump off\n");
    delay(500);
  }
}

void autoOnOffPump() {
  if (autoControll) {
    if (digitalRead(TANK_LEVEL_MIN_PIN) == LOW && digitalRead(TANK_LEVEL_MAX_PIN) == LOW) {

      if (!externalTankContainsMinLevel(true)) {
        delay(500);
        return;
      }
    
      Serial.println("> Turn on automatically");
      Serial.println("> Pump on");
      
      turnOnPump();
      
      waitTankBecomeFull(true);
      delay(TURN_OFF_PUMP_AUTO_DELAY);
      
      turnOffPump();

      Serial.println("> Pump off\n");
      delay(500);
    }
  }  
}

bool externalTankContainsMinLevel(bool printError) {
  if (digitalRead(EXTERNAL_TANK_LEVEL_MIN_PIN) == LOW) {
    if (printError)
      Serial.println("> Exteral water tank don't contains a minimal level of water\n");
    
    return false;
  }

  return true;
}

bool tankIsAlreadyFull(bool printError) {
  if (digitalRead(TANK_LEVEL_MAX_PIN) == HIGH) {
    if(printError)
      Serial.println("> Water already full\n");
    
    return true;
  }

  return false;
}

void waitTankBecomeFull(bool printError) {
  while(true) {
    if (digitalRead(TANK_LEVEL_MAX_PIN) == HIGH) break;
    else if (digitalRead(EXTERNAL_TANK_LEVEL_MIN_PIN) == LOW) {
      if (printError)
        Serial.println("> The water level in extenal tank is above of minimum!");

      break;
    }
  }
}

void turnOnPump() {
  digitalWrite(PUMP_RELAY_PIN, LOW);
  digitalWrite(LED_AUTO_PIN, HIGH);
}

void turnOffPump() {
  digitalWrite(PUMP_RELAY_PIN, HIGH);
  digitalWrite(LED_AUTO_PIN, LOW);
}
