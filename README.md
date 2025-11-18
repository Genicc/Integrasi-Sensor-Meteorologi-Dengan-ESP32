🌦️ Integrasi Sensor Meteorologi Dengan ESP32
  Monitoring Curah Hujan, Suhu & Kelembaban, dan Kelembaban Tanah
  Project ini mengintegrasikan beberapa sensor meteorologi menggunakan ESP32 dengan sistem multi-tasking:
    1. Rain Gauge RK400 (RS485 Modbus)
    2. AHT10/AHT20 (I2C – Suhu & Kelembaban)
    3. Sensor Soil Moisture (Analog ADC ESP32)
  Seluruh data ditampilkan melalui Serial Monitor dalam format log yang rapi.


✨ Fitur Utama
      1. Pembacaan curah hujan via Modbus RS485
      2. Pembacaan suhu & RH menggunakan AHT10/AHT20
      3. Pembacaan kelembaban tanah dengan status: Kering / Sedang / Basah
      4. Sistem multi-task FreeRTOS untuk pembacaan sensor yang stabil
      5. Deteksi otomatis mulai hujan dan hujan berhenti


📌 Arsitektur Sistem
      1. Rain Gauge RK400 → RS485 → ESP32 (UART2)
      2. AHT10/AHT20      → I2C   → ESP32 (SDA 21, SCL 22)
      3. Soil Sensor      → ADC   → ESP32 (GPIO 34)
      4. Serial Monitor   → USB   → PC


🔌 Wiring
      | Sensor          | Pin ESP32  |
      | --------------- | ---------- |
      | Soil Moisture   | GPIO 34    |
      | AHT SDA         | GPIO 21    |
      | AHT SCL         | GPIO 22    |
      | RS485 RO → RXD2 | GPIO 16    |
      | RS485 DI → TXD2 | GPIO 17    |
      | VCC/GND semua   | 3.3V / GND |


🖥️ Contoh Output Serial

      =================================
      Soil Moisture Monitor - ESP32
      =================================
      Pin Sensor: GPIO 34
      
      Melakukan test koneksi...
      [00:00:02.105] Nilai pembacaan test: 2875
      
      ----------------- SOIL SENSOR -----------------
      [00:00:04.120] Nilai ADC  : 2850 / 4095
      [00:00:04.120] Kelembaban : 62 %
      [00:00:04.120] Status     : BASAH - Kelembaban baik
      [00:00:04.120] Bar       : [==================  ]
      -----------------------------------------------
      
      --------------- AHT SENSOR -------------------
      [00:00:05.321] Temp       : 28.37 °C
      [00:00:05.321] RH         : 74.82 %
      -----------------------------------------------
      
      ============== HUJAN TERDETEKSI ===============
      [00:01:10.215] Hujan mulai terdeteksi (sesi baru).
      ===============================================
      
      ================= CURAH HUJAN =================
      [00:01:12.320] Tipping    : 3
      [00:01:12.320] Curah Hujan: 0.60 mm
      [00:01:12.320] Temp       : 27.95 °C
      [00:01:12.320] RH         : 79.12 %
      ===============================================
      
      ============== HUJAN BERHENTI ================
      [00:06:55.123] Total Curah Hujan Sesi: 2.80 mm
      ===============================================


📦 Library yang Dibutuhkan
      1. ModbusMaster
      2. Adafruit AHTX0
      3. ESP32 Core Arduino
