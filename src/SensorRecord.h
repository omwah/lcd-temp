#ifndef SENSOR_RECORD
#define SENSOR_RECORD

#include <Arduino.h>

// NREC is how many records to keep, ie how big is the storage array
template <int NREC>
class SensorRecord {
public:
    // Record values very interval seconds
    SensorRecord(long Interval)
        : interval(Interval)
    {
        max_rec = 0;
        min_rec = 0;

        running_total = 0;
        running_count = 0;

        for(int idx = 0; idx < NREC; idx++) {
            records[idx] = -1;
        }
    }

    void init(int first_value) {
        max_rec = first_value;
        min_rec = first_value;

        //records[NREC-1] = first_value;
        //last_time = millis();
    }

    void record(int new_value) {

        // Always add new value to running average
        running_total += new_value;
        running_count++;

        // Record new value if enough time has passed
        long now = millis();
        if( (abs(now - last_time) / 1000) > interval ) {
            int avg_value = running_total / running_count;

            // Reinitialize max/min
            max_rec = avg_value;
            min_rec = avg_value;

            // Shift existing values down
            for(int idx = 1; idx < NREC; idx++) {
                records[idx-1] = records[idx];

                // Unrecorded values are < 0 
                if(records[idx] > 0) {
                    max_rec = max(records[idx], max_rec);
                    min_rec = min(records[idx], min_rec);
                }
            }

            records[NREC-1] = avg_value;
            last_time = now;

            // Reset running average
            running_total = 0;
            running_count = 0;
        }
    }

    int records[NREC];
    int max_rec;
    int min_rec;
private:
    // Average values taken when not recording,
    // so that value recorded represents interval
    // it is recorded for
    long running_total;
    long running_count;

    // How long before recording values
    long interval;

    // When we last recorded in milliseconds
    long last_time;
};

#endif
