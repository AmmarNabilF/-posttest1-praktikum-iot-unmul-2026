#define BLYNK_TEMPLATE_ID "TMPL6RLLFeu_O"
#define BLYNK_TEMPLATE_NAME "Gas sensor"
#define BLYNK_AUTH_TOKEN "AVg8OCyxoX-KPbBFSGBziTYqfoKxNJ6R"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// WIFI
char ssid[] = "cruxx";
char pass[] = "siapatawu";

// PIN (AMAN UNTUK ESP8266)
#define LED_MERAH D1
#define LED_KUNING D2
#define LED_HIJAU D3
#define MQ_PIN A0

// MODE
bool modeOtomatis = false;

// STATE MANUAL (BLYNK)
bool ledHijauState = 0;
bool ledKuningState = 0;
bool ledMerahState = 0;

// BLINK SYSTEM (millis)
unsigned long previousMillis = 0;
unsigned long interval = 0;

int blinkCount = 0;
int maxBlink = 0;
bool ledState = LOW;
int currentLED = -1;

// =======================
// BLYNK CONTROL (MANUAL)
// =======================
BLYNK_WRITE(V0) { // HIJAU
  if (!modeOtomatis) {
    ledHijauState = param.asInt();
  }
}

BLYNK_WRITE(V1) { // KUNING
  if (!modeOtomatis) {
    ledKuningState = param.asInt();
  }
}

BLYNK_WRITE(V2) { // MERAH
  if (!modeOtomatis) {
    ledMerahState = param.asInt();
  }
}

// =======================
// SET BLINK
// =======================
void setBlink(int ledPin, int jumlah, int durasiTotal) {
  currentLED = ledPin;
  maxBlink = jumlah * 2;   // ON + OFF
  blinkCount = 0;
  interval = durasiTotal / maxBlink;
  previousMillis = millis();
  delay(1000);
}

// =======================
// HANDLE BLINK TANPA DELAY
// =======================
void handleBlink() {
  if (currentLED == -1) return;

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    ledState = !ledState;
    digitalWrite(currentLED, ledState);

    blinkCount++;

    if (blinkCount >= maxBlink) {
      digitalWrite(currentLED, LOW);
      currentLED = -1; // selesai
    }
  }
}

// =======================
// SETUP
// =======================
void setup() {
  Serial.begin(115200);

  pinMode(LED_MERAH, OUTPUT);
  pinMode(LED_KUNING, OUTPUT);
  pinMode(LED_HIJAU, OUTPUT);

  digitalWrite(LED_MERAH, LOW);
  digitalWrite(LED_KUNING, LOW);
  digitalWrite(LED_HIJAU, LOW);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

// =======================
// LOOP
// =======================
void loop() {
  Blynk.run();
  

  int gasValue = analogRead(MQ_PIN);
  Blynk.virtualWrite(V3, gasValue);

  Serial.print("Gas Value: ");
  Serial.print(gasValue);
  Serial.print(" | Mode: ");
  Serial.println(modeOtomatis ? "AUTO" : "MANUAL");

  // =======================
  // DETEKSI MODE
  // =======================
  if (gasValue > 400) {
    modeOtomatis = true;
  } else {
    modeOtomatis = false;
  }

  // =======================
  // MODE OTOMATIS
  // =======================
  if (modeOtomatis) {

    // matikan semua dulu
    digitalWrite(LED_MERAH, LOW);
    digitalWrite(LED_KUNING, LOW);
    digitalWrite(LED_HIJAU, LOW);

    // set blink hanya jika belum berjalan
    if (currentLED == -1) {
      if (gasValue > 600) {
        // sangat tinggi
        setBlink(LED_MERAH, 70, 7000);
      } 
      else if (gasValue > 500 && gasValue <= 600) {
        // sedang
        setBlink(LED_KUNING, 50, 5000);
      } 
      else if (gasValue > 400 && gasValue <= 500) {
        // ringan
        setBlink(LED_HIJAU, 30, 3000);
      }
    }

    handleBlink();
  }

  // =======================
  // MODE MANUAL
  // =======================
  else {
    currentLED = -1; // stop blinking

    digitalWrite(LED_HIJAU, ledHijauState);
    digitalWrite(LED_KUNING, ledKuningState);
    digitalWrite(LED_MERAH, ledMerahState);
  }
}