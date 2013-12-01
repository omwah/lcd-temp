#ifndef SENSOR_RECORD
#define SENSOR_RECORD

#include <Arduino.h>

template <int NREC>
class SensorRecord {
public:
    SensorRecord(long Interval)
        : interval(Interval)
    {
        max_rec = 0;
        min_rec = 0;

        for(int idx = 0; idx < NREC; idx++) {
            records[idx] = -1;
        }
    }

    void init(int first_value) {
        max_rec = first_value;
        min_rec = first_value;

        records[NREC-1] = first_value;
        last_time = millis();
    }

    void record(int new_value) {
        // Record new value if enough time has passed
        long now = millis();
        if( (abs(now - last_time) / 1000) > interval ) {
            // Reinitialize max/min
            max_rec = new_value;
            min_rec = new_value;

            // Shift existing values down
            for(int idx = 1; idx < NREC; idx++) {
                records[idx-1] = records[idx];

                // Unrecorded values are < 0 
                if(records[idx] > 0) {
                    max_rec = max(records[idx], max_rec);
                    min_rec = min(records[idx], min_rec);
                }
            }

            records[NREC-1] = new_value;
            last_time = now;
        }
    }

    int records[NREC];
    int max_rec;
    int min_rec;
private:

    long interval;
    long last_time;
};

#endif
