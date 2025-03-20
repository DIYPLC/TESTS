// Straight Key Morse
// https://r9a.ru/forum/index.php?topic=1120.0

#define SOUND_PIN 2
#define SOUND_FR_HZ 1000
#define PIN_CW_KEY 3

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_CW_KEY, INPUT_PULLUP);
}

void loop() {

  bool cw_key = not(digitalRead(PIN_CW_KEY));

  if (cw_key) {
    tone(SOUND_PIN, SOUND_FR_HZ);
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    noTone(SOUND_PIN);
    digitalWrite(LED_BUILTIN, LOW);
  }
  
  // delay(1);
}