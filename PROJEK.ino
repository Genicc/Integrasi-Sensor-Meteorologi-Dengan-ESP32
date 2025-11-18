#include <ModbusMaster.h>
#include <Wire.h>
#include <Adafruit_AHTX0.h>

// =============== SOIL (asli) ===============
const int SOIL_SENSOR_PIN = 34;
const int AIR_VALUE = 3200;
const int WATER_VALUE = 1500;
const int THRESHOLD_DRY = 30;
const int THRESHOLD_MODERATE = 60;

// =============== Modbus + AHT (asli) ===============
#define RXD2 16
#define TXD2 17
ModbusMaster node;

const float TIP_MM = 0.2f;
const uint16_t PULSES_PER_TIP = 2;
const float MM_PER_PULSE = TIP_MM / PULSES_PER_TIP;

const unsigned long DRY_TIMEOUT_MINUTES = 1;
const unsigned long DRY_TIMEOUT_MS = DRY_TIMEOUT_MINUTES * 60UL * 1000UL;
const unsigned long READ_PERIOD_MS = 100;

uint16_t lastRawCount = 0;
bool firstRead = true;
unsigned long lastTipTime = 0;
float sessionRainMm = 0.0f;
bool inDryState = true;

// =============== AHT (task terpisah) ===============
Adafruit_AHTX0 aht;
float lastTemp = NAN;
float lastRH   = NAN;
volatile unsigned long suppressAHTUntilMs = 0;

// =============== UTIL TIMESTAMP ===============
String nowStamp() {
  unsigned long ms = millis();
  unsigned long s  = ms / 1000UL;
  unsigned long m  = s / 60UL;
  unsigned long h  = m / 60UL;
  s %= 60UL; m %= 60UL;
  char buf[20];
  snprintf(buf, sizeof(buf), "%02lu:%02lu:%02lu.%03lu", h % 100UL, m, s, ms % 1000UL);
  return String(buf);
}

// =============== PRINTER BLOK ===============
void printRainBlock(uint32_t tippingCountSession, float rainMm, float t, float rh) {
  Serial.println("================= CURAH HUJAN =================");
  Serial.printf("[%s] Tipping    : %lu\n", nowStamp().c_str(), tippingCountSession);
  Serial.printf("[%s] Curah Hujan: %.2f mm\n", nowStamp().c_str(), rainMm);
  if (!isnan(t))  Serial.printf("[%s] Temp       : %.2f °C\n", nowStamp().c_str(), t);
  if (!isnan(rh)) Serial.printf("[%s] RH         : %.2f %%\n", nowStamp().c_str(), rh);
  Serial.println("===============================================");
  Serial.println();
}

void printRainStart() {
  Serial.println("============== HUJAN TERDETEKSI ===============");
  Serial.printf("[%s] Hujan mulai terdeteksi (sesi baru).\n", nowStamp().c_str());
  Serial.println("===============================================");
  Serial.println();
}

void printRainStopBlock(float totalMm) {
  Serial.println("============== HUJAN BERHENTI ================");
  Serial.printf("[%s] Total Curah Hujan Sesi: %.2f mm\n", nowStamp().c_str(), totalMm);
  Serial.println("===============================================");
  Serial.println();
}

void printAHTBlock(float t, float rh) {
  Serial.println("--------------- AHT SENSOR -------------------");
  Serial.printf("[%s] Temp       : %.2f °C\n", nowStamp().c_str(), t);
  Serial.printf("[%s] RH         : %.2f %%\n", nowStamp().c_str(), rh);
  Serial.println("-----------------------------------------------");
  Serial.println();
}

void printSoilBlock(int sensorValue, int moisturePercent, const String& status) {
  Serial.println("----------------- SOIL SENSOR -----------------");
  Serial.printf("[%s] Nilai ADC  : %d / 4095\n", nowStamp().c_str(), sensorValue);
  Serial.printf("[%s] Kelembaban : %d %%\n", nowStamp().c_str(), moisturePercent);
  Serial.printf("[%s] Status     : %s\n", nowStamp().c_str(), status.c_str());

  Serial.print ("["); Serial.print(nowStamp()); Serial.print("] Bar       : [");
  int bars = moisturePercent / 5;
  for (int i = 0; i < 20; i++) Serial.print(i < bars ? "=" : " ");
  Serial.println("]");
  Serial.println("-----------------------------------------------");
  Serial.println();
}

// =============== TASK AHT ===============
void AHTTask(void *pv) {
  delay(500);
  Serial.println("Booting...");
  Wire.begin(21, 22);
  if (!aht.begin(&Wire)) {
    Serial.println("ERROR: AHT10/AHT20 tidak terdeteksi di I2C! Cek alamat/pull-up/kabel.");
    while (1) delay(10);
  }
  Serial.println("AHT terhubung.");

  for (;;) {
    sensors_event_t humidity, temp;
    if (aht.getEvent(&humidity, &temp)) {
      lastTemp = temp.temperature;
      lastRH   = humidity.relative_humidity;
      if (millis() >= suppressAHTUntilMs) {
        printAHTBlock(lastTemp, lastRH);
      }
    }
    delay(2000);
  }
}

// =============== TASK SOIL ===============
void SoilTask(void *pv) {
  delay(1000);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  Serial.println("=================================");
  Serial.println("Soil Moisture Monitor - ESP32");
  Serial.println("=================================");
  Serial.println(String("Pin Sensor: GPIO ") + String(SOIL_SENSOR_PIN));
  Serial.println();

  Serial.println("Melakukan test koneksi...");
  delay(500);
  int testRead = analogRead(SOIL_SENSOR_PIN);
  Serial.printf("[%s] Nilai pembacaan test: %d\n\n", nowStamp().c_str(), testRead);

  for(;;) {
    int total = 0;
    for(int i = 0; i < 10; i++) { total += analogRead(SOIL_SENSOR_PIN); delay(10); }
    int sensorValue = total / 10;

    if (sensorValue == 0) {
      Serial.printf("[%s] ERROR: Sensor tidak terbaca (ADC = 0)\n", nowStamp().c_str());
      Serial.println("Periksa koneksi kabel sensor!\n");
      delay(2000);
    } else {
      int moisturePercent = map(sensorValue, AIR_VALUE, WATER_VALUE, 0, 100);
      moisturePercent = constrain(moisturePercent, 0, 100);

      String status;
      if (moisturePercent < THRESHOLD_DRY)       status = "KERING - Perlu disiram!";
      else if (moisturePercent < THRESHOLD_MODERATE) status = "SEDANG - Kelembaban cukup";
      else                                        status = "BASAH - Kelembaban baik";

      printSoilBlock(sensorValue, moisturePercent, status);
    }
    delay(2000);
  }
}

// =============== SETUP/LOOP (Modbus tetap) ===============
void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  node.begin(2, Serial2);
  Serial.printf("[%s] Inisialisasi RK400 RS485...\n", nowStamp().c_str());

  xTaskCreatePinnedToCore(AHTTask,  "AHTTask",  4096, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(SoilTask, "SoilTask", 4096, NULL, 1, NULL, 1);
}

void loop() {
  uint8_t result = node.readHoldingRegisters(0x0000, 1);

  if (result == node.ku8MBSuccess) {
    uint16_t rawCount = node.getResponseBuffer(0);
    unsigned long now = millis();

    if (firstRead) {
      lastRawCount = rawCount;
      lastTipTime  = now;
      firstRead = false;
    } else {
      uint16_t deltaPulses = (rawCount >= lastRawCount)
                           ? (rawCount - lastRawCount)
                           : (uint16_t)(rawCount + (uint32_t)65536 - lastRawCount);

      if (deltaPulses > 0) {
        float deltaMm = deltaPulses * MM_PER_PULSE;
        sessionRainMm += deltaMm;
        lastTipTime = now;
        lastRawCount = rawCount;

        if (inDryState) {
          inDryState = false;
          printRainStart();
        }

        uint32_t tippingCountSession = (uint32_t)((sessionRainMm / TIP_MM) + 0.5f);
        printRainBlock(tippingCountSession, sessionRainMm, lastTemp, lastRH);

        suppressAHTUntilMs = millis() + 1500;

      } else {
        lastRawCount = rawCount;

        if (!inDryState && (now - lastTipTime >= DRY_TIMEOUT_MS)) {
          printRainStopBlock(sessionRainMm);
          sessionRainMm = 0.0f;
          inDryState = true;
        }
      }
    }
  } else {
    Serial.printf("[%s] Modbus Error: %u\n", nowStamp().c_str(), result);
  }

  delay(READ_PERIOD_MS);
}
