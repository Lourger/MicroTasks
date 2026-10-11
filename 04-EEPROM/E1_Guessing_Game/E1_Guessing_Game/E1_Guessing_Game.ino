#include <Arduino.h>
#include <EEPROM.h>

// ---------- Storage ----------
#define EEPROM_SIZE    128           // bytes reserved for the emulated EEPROM
#define SCORE_COUNT    10            // the last 10 scores, one byte each, at address 0..9 (newest at 0)
#define EMPTY_A        0             // a slot holding 0 or 255 counts as empty
#define EMPTY_B        255

// ---------- Game ----------
#define SEED_PIN       34            // unconnected ADC pin: its noise seeds the random generator
#define MIN_NUMBER     1
#define MAX_NUMBER     100
#define NEW_GAME_MS    2000

int secretNumber = 0;                // the number to guess
int attempts     = 0;                // guesses so far in this game

bool isEmptySlot(uint8_t value) {
  return (value == EMPTY_A || value == EMPTY_B);
}

void saveScore(uint8_t score) {
  for (int i = SCORE_COUNT - 1; i > 0; i--) {          // shift every score one slot older
    EEPROM.write(i, EEPROM.read(i - 1));
  }
  EEPROM.write(0, score);                              // newest at address 0
  EEPROM.commit();                                     // now it is really saved in flash
  Serial.println("Score saved.");
}

void showLeaderboard(uint8_t currentScore) {
  uint8_t scores[SCORE_COUNT];
  int count = 0;
  for (int i = 0; i < SCORE_COUNT; i++) {              // collect the non-empty scores
    uint8_t value = EEPROM.read(i);
    if (!isEmptySlot(value)) {
      scores[count] = value;
      count++;
    }
  }
  if (count == 0) {
    Serial.println("No scores available.");
    return;
  }

  for (int i = 0; i < count - 1; i++) {                // sort: fewest attempts first
    for (int j = 0; j < count - 1 - i; j++) {
      if (scores[j] > scores[j + 1]) {
        uint8_t temp  = scores[j];
        scores[j]     = scores[j + 1];
        scores[j + 1] = temp;
      }
    }
  }

  Serial.println("---- Leaderboard (fewest attempts first) ----");
  bool youMarked = false;
  for (int i = 0; i < count; i++) {
    Serial.print(i + 1);
    Serial.print(". ");
    Serial.print(scores[i]);
    Serial.print(" attempts");
    if (!youMarked && scores[i] == currentScore) {
      Serial.print("  (You)");
      youMarked = true;
    }
    Serial.println();
  }
  if (!youMarked) {
    Serial.println("Your score is not in the top 10.");
  }

  Serial.print("Last 10 scores (newest first): ");
  for (int i = 0; i < SCORE_COUNT; i++) {
    uint8_t value = EEPROM.read(i);
    if (isEmptySlot(value)) {
      Serial.print("- ");
    } else {
      Serial.print(value);
      Serial.print(" ");
    }
  }
  Serial.println();
}

void startNewGame() {
  secretNumber = random(MIN_NUMBER, MAX_NUMBER + 1);   // 1..100
  attempts = 0;
  Serial.println();
  Serial.println("New game: I picked a number from 1 to 100. Type your guess.");
}

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  randomSeed(analogRead(SEED_PIN));
  Serial.println("=== Number Guessing Game ===");
  startNewGame();
}

void loop() {
  if (Serial.available() == 0) {
    return;                                            // nothing typed yet
  }

  int guess = Serial.parseInt();
  while (Serial.available() > 0) {                     // throw away the rest of the line
    Serial.read();
  }
  if (guess == 0) {
    return;                                            // an empty line or Enter on its own
  }
  if (guess < MIN_NUMBER || guess > MAX_NUMBER) {
    Serial.println("Please enter a number from 1 to 100.");
    return;
  }

  attempts++;
  if (guess > secretNumber) {
    Serial.println("Too High!");
  } else if (guess < secretNumber) {
    Serial.println("Too Low!");
  } else {
    Serial.print("Correct! You took ");
    Serial.print(attempts);
    Serial.println(" attempts.");
    uint8_t score = (attempts > 254) ? 254 : attempts;   // 255 is reserved for "empty"
    saveScore(score);
    showLeaderboard(score);
    Serial.println("Game over.");
    delay(NEW_GAME_MS);
    startNewGame();
  }
}
