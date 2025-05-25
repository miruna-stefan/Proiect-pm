#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <TroykaDHT.h>
#include <SD.h>

#include "timers.h"
#include "usart.h"
#include "sound.h"

const int ALERT_TEMP = 26;

const int chipSelect = 10;

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int DHT22_PIN = 8;
DHT dht(DHT22_PIN, DHT22);

void wait(uint32_t time) {
    uint32_t start = systicks;
    uint32_t end = start + time;

    // wait for the specified amount of time or for a button to be pressed
    while(end > systicks);

    return;
}

void setup() {
  USART0_init(CALC_USART_UBRR(9600));

  Timer1_init_ctc();
  Timer2_init_systicks();
  sei();

  lcd.init();
  lcd.backlight();
  if (!SD.begin(chipSelect)) {
    USART0_print("initialization failed!");
    while (1);
  }
  USART0_println("initialization done.");
  dht.begin();
}

void loop() {
  static uint32_t last_read = 0;
  static uint8_t last_note = 0;
  static uint32_t last_note_started = 0;
  static bool speaker_enabled = false;
  if (SYSTICKS_PASSED(last_read, 2000)) {
    last_read = systicks;
    int temperature;
    int humidity;
    dht.read();
    switch(dht.getState()) {
      case DHT_OK:
        temperature = dht.getTemperatureC();
        humidity = dht.getHumidity();
        USART0_print("Temperature = ");
        USART0_print_int(temperature);
        USART0_println(" C");
        USART0_print("Temperature = ");
        USART0_print_int(dht.getTemperatureK());
        USART0_println(" K");
        USART0_print("Temperature = ");
        USART0_print_int(dht.getTemperatureF());
        USART0_println(" F");
        USART0_print("Humidity = ");
        USART0_print_int(humidity);
        USART0_println(" %");
        break;
      case DHT_ERROR_CHECKSUM:
        USART0_println("Checksum error");
        break;
      case DHT_ERROR_TIMEOUT:
        USART0_println("Time out error");
        break;
      case DHT_ERROR_NO_REPLY:
        USART0_println("Sensor not connected");
        break;
    }

    if (temperature >= ALERT_TEMP && speaker_enabled==false) {
      enable_PB1_pwm();
      last_note = 0;
      speaker_enabled = true;
    } else if (temperature < ALERT_TEMP && speaker_enabled==true) {
      disable_PB1_pwm();
      speaker_enabled = false;
    }

    lcd.setCursor(0, 1);
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print("C");
    lcd.print("Hum: ");
    lcd.print(humidity);
    lcd.print("H");

    File myFile = SD.open("test_pm.txt", FILE_WRITE);
    if (myFile) {
      myFile.print("Temperature: ");
      myFile.print(temperature);
      myFile.print("C ");
      myFile.print("Humidity: ");
      myFile.print(humidity);
      myFile.println("H");
      myFile.close();
    } else {
      USART0_println("error opening test_pm.txt");
    }
  }

  if (SYSTICKS_PASSED(last_note_started, alert_durations[last_note])) {
      last_note_started = systicks;
      last_note++;
      if (last_note > num_notes)
        last_note = 0;
      set_PB1_freq(alert_notes[last_note]);
  }
}

void findI2C() {
  char hex[10];
	int nDevices = 0;

  USART0_println("Scanning...");

  for (byte address = 1; address < 127; ++address) {
		USART0_println(address);
    // The i2c_scanner uses the return value of
    // the Wire.endTransmission to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      USART0_print("I2C device found at address 0x");
      if (address < 16) {
        USART0_print("0");
      }
      sprintf(hex, "0x%x", address);
      USART0_print(hex);
      USART0_println("  !");

      ++nDevices;
    } else if (error == 4) {
      USART0_print("Unknown error at address 0x");
      if (address < 16) {
        USART0_print("0");
      }
      sprintf(hex, "0x%x", address);
      USART0_print(hex);
    }
  }
  if (nDevices == 0) {
    USART0_println("No I2C devices found\n");
  } else {
    USART0_println("done\n");
  }
  wait(5000); // Wait 5 seconds for next scan
}