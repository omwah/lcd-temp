#include <Arduino.h>

/*
  The circuit:
 * LCD RS pin to digital pin 8
 * LCD Enable pin to digital pin 9
 * LCD D4 pin to digital pin 4
 * LCD D5 pin to digital pin 5
 * LCD D6 pin to digital pin 6
 * LCD D7 pin to digital pin 7
 * LCD BL pin to digital pin 10
 * KEY pin to analogl pin 0
 */

#include <LiquidCrystal.h>
#include "DigitalSmooth.h"
#include "SensorRecord.h"
#include "level_characters.h"

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);

#define NUM_KEYS  5
int adc_key_val[NUM_KEYS] = {50, 200, 400, 600, 800 };

#define TEMP_PIN 1

#define READING_DELAY 500

#define NCURSOR 4
char cursors[] = { '.', 'o', 'O', 'o' };
int cursor_idx = 0;

// Smooth over 13 samples and keep the top 95%
DigitalSmooth<13> sensor_smoothing(0.85);

// Record last 16 minutes of values
#define N_REC_SAMPS 16
SensorRecord<N_REC_SAMPS> minute_record(60);

float convert_celsius(int sensor)
{
    // convert the sensor reading to degrees Celsius
    float voltage = (sensor * 5000.0) / 1024.0; // convert raw sensor value to millivolts
    voltage = voltage - 500; // remove voltage offset
    float celsius = voltage / 10; // convert millivolts to Celsius

    return celsius;
}

void setup()
{
    Serial.begin(9600);
    lcd.clear();
    lcd.begin(16, 2);
    lcd.setCursor(0, 0);

    // Initialize with a read value so we don't
    // get garbage initially
    int sensor = analogRead(TEMP_PIN);
    sensor_smoothing.init(sensor);
    minute_record.init(sensor);
    
    // Register characters for records
    lcd.createChar(1, level_1);
    lcd.createChar(2, level_2);
    lcd.createChar(3, level_3);
    lcd.createChar(4, level_4);
    lcd.createChar(5, level_5);
    lcd.createChar(6, level_6);
    lcd.createChar(7, level_7);
    lcd.createChar(8, level_8);
}

float convert_fahrenheit(float celsius) {
    return (celsius * 9.0) / 5.0 + 32.0;
}

// Convert ADC value to key number
int get_key(unsigned int input)
{
    int k;

    for (k = 0; k < NUM_KEYS; k++) {
        if (input < adc_key_val[k]) {
            return k;
        }
    }

    if (k >= NUM_KEYS) {
        k = -1;    // No valid key pressed
    }

    return k;
}

void display_record() {
    static int row_idx = 0;

    //Serial.println("display_record");
    //Serial.print("max = "); Serial.print(minute_record.max_rec);
    //Serial.print(" min = "); Serial.println(minute_record.min_rec);

    int rec_interval = minute_record.max_rec - minute_record.min_rec;
    //Serial.print("interval = "); Serial.println(rec_interval);
    for(int idx = 0; idx < N_REC_SAMPS; idx++) {
        // Scale value to be one of 1 - N_PIXEL_ROWS
        int rec_val = minute_record.records[idx];
        int level_height = (rec_val -  minute_record.min_rec) / float(rec_interval) * (N_PIXEL_ROWS - 1) + 1;

        //Serial.print("val = "); Serial.print(rec_val);
        //Serial.print(" height = "); Serial.println(level_height);

        lcd.setCursor(idx, row_idx);
        if(rec_val > 0 && level_height == 0 && rec_interval == 0) {
            // Special case when there is only one valid value
            lcd.write(N_PIXEL_ROWS);
        } else if(level_height > 0) {
            lcd.write(level_height);
        } else {
            lcd.print(' ');
        }
    }
}

void display_current_temp(int sensor)
{
    static int row_idx = 1;

    float celsius = convert_celsius(sensor);
    float fahrenheit = convert_fahrenheit(celsius);
    lcd.setCursor(0, row_idx);
    lcd.print(celsius, 1);
    lcd.print(" C ");

    int next_location = 8;
    if(celsius >= 100)
        next_location++;

    lcd.setCursor(next_location, row_idx);
    lcd.print(fahrenheit, 1);
    lcd.print(" F ");

    lcd.setCursor(15, row_idx);
    lcd.print(cursors[cursor_idx]);
    cursor_idx = (cursor_idx + 1) % NCURSOR; 
}

void loop()
{
    int sensor = sensor_smoothing.smooth(analogRead(TEMP_PIN));
    minute_record.record(sensor);
    display_record();
    display_current_temp(sensor);
    delay(READING_DELAY);
}
