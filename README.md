🌦️ INTEGRASI SENSOR METEOROLOGI DENGAN ESP32
  
      Monitoring Curah Hujan, Suhu & Kelembaban, dan Kelembaban Tanah
      Project ini mengintegrasikan beberapa sensor meteorologi menggunakan ESP32 dengan sistem multi-tasking:
      Rain Gauge RK400 (RS485 Modbus), 
      AHT10/AHT20 (I2C – Suhu & Kelembaban), 
      Sensor Soil Moisture (Analog ADC ESP32), 
      Seluruh data ditampilkan melalui Serial Monitor dalam format log yang rapi.

✨ FITUR UTAMA

      Pembacaan curah hujan via Modbus RS485,
      Pembacaan suhu & RH menggunakan AHT10/AHT20,
      Pembacaan kelembaban tanah dengan status: Kering / Sedang / Basah,
      Sistem multi-task FreeRTOS untuk pembacaan sensor yang stabil,
      Deteksi otomatis mulai hujan dan hujan berhenti.


📌 ARSITEKTUR SISTEM

      Rain Gauge RK400 → RS485 → ESP32 (UART2)
      AHT10/AHT20      → I2C   → ESP32 (SDA 21, SCL 22)
      Soil Sensor      → ADC   → ESP32 (GPIO 34)
      Serial Monitor   → USB   → PC


🔌 WIRING

      | Sensor          | Pin ESP32  |
      | --------------- | ---------- |
      | Soil Moisture   | GPIO 34    |
      | AHT SDA         | GPIO 21    |
      | AHT SCL         | GPIO 22    |
      | RS485 RO → RXD2 | GPIO 16    |
      | RS485 DI → TXD2 | GPIO 17    |
      | VCC/GND semua   | 3.3V / GND |


🖥️ OUTPUT SERIAL MONITOR

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


📦 LIBRARY YANG DIBUTUHKAN

      1. ModbusMaster
      2. Adafruit AHTX0
      3. ESP32 Core Arduino
