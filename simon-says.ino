#include "pitches.h"
#include <List.hpp>

int VRx = A0;
int VRy = A1;
int SW = 2;

int GREEN = 3;
int RED = 5;
int YELLOW = 6;
int BLUE = 9;
int PINS[] = { 3, 5, 6, 9 };

int turnPin = 10;

int buzzer = 11;

int xPosition = 0;
int yPosition = 0;
int SW_state = 0;
int mapX = 0;
int mapY = 0;

bool userTurn = false;
bool canMove = true;

bool failed = false;

List<long> sequence;
int sequenceIndex = 0;

void setup() {
  Serial.begin(9600);

  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP);

  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(BLUE, OUTPUT);

  pinMode(turnPin, OUTPUT);

  pinMode(buzzer, OUTPUT);
}

void loop() {
  if (failed) {
    GameOver();
    return;
  }
  
  if (userTurn) {
    UserTurn();
  } else {
    GameTurn();
  }
}

void UserTurn() {
  GetInput();

  analogWrite(turnPin, 255);

  bool lightOn = false;
  if (canMove) {
    if (mapX < -256) {
      if (!lightOn) {
        analogWrite(GREEN, 255);
        lightOn = true;
        Reset(GREEN);

        MakeTurn(GREEN);
      }
    } else if (mapX > 256) {
      if (!lightOn) {
        analogWrite(YELLOW, 255);
        lightOn = true;
        Reset(YELLOW);

        MakeTurn(YELLOW);
      }
    }

    if (mapY > 256) {
      if (!lightOn) {
        analogWrite(RED, 255);
        lightOn = true;
        Reset(RED);

        MakeTurn(RED);
      }
    } else if (mapY < -256) {
      if (!lightOn) {
        analogWrite(BLUE, 255);
        lightOn = true;
        Reset(BLUE);

        MakeTurn(BLUE);
      }
    }
  }

  if (lightOn) {
    canMove = false;
  }
  if (mapX < 128 && mapX > -128) {
    if (mapY < 128 && mapY > -128) {
      canMove = true;
    }
  }
}

void GameTurn() {
  analogWrite(turnPin, 0);
  NextSequence();

  userTurn = true;
}

void MakeTurn(int light) {
  int correct = sequence.getValue(sequenceIndex);
  if (PINS[correct] == light) {
    Serial.println("Correct");
  } else {
    failed = true;
  }

  sequenceIndex++;
  if (sequenceIndex >= sequence.getSize()) {
    sequenceIndex = 0;
    userTurn = false;
  }

  delay(250);
  ResetAll();
}

void NextSequence() {
  delay(500);
  
  long r = random(0, 4);
  sequence.add(r);

  ResetAll();
  for (int i = 0; i < sequence.getSize(); i++) {
    analogWrite(PINS[sequence.getValue(i)], 255);
    delay(1000);
    analogWrite(PINS[sequence.getValue(i)], 0);
    delay(250);

    randomSeed(millis());
  }
}

void GetInput() {
  xPosition = analogRead(VRx);
  yPosition = analogRead(VRy);
  SW_state = digitalRead(SW);
  mapX = map(xPosition, 0, 1023, -512, 512);
  mapY = map(yPosition, 0, 1023, -512, 512);
}

void Reset(int except) {
  for (int i = 0; i < sizeof(PINS) / sizeof(int); i++) {
    if (PINS[i] == except) continue;

    analogWrite(PINS[i], 0);
  }
}

void ResetAll() {
  analogWrite(GREEN, 0);
  analogWrite(BLUE, 0);
  analogWrite(YELLOW, 0);
  analogWrite(RED, 0);
}

String GetName(int pin) {
  switch (pin) {
    case 0:
      return "Green";
    case 1:
      return "Red";
    case 2:
      return "Yellow";
    case 3:
      return "Blue";
    default:
      return "Unknown Light";
  }
}

bool flash = false;
void GameOver() {
  Beep();
  analogWrite(turnPin, flash ? 255 : 0);
  flash = !flash;

  delay(50);
}

void Beep() {
  tone(buzzer, NOTE_B5, 1000);
}
