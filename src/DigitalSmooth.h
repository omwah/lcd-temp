#ifndef DIGITAL_SMOOTH
#define DIGITAL_SMOOTH

/* digitalSmooth
 Paul Badger 2007
 A digital smoothing filter for smoothing sensor jitter
 This filter accepts one new piece of data each time through a loop, which the
 filter inputs into a rolling array, replacing the oldest data with the latest reading.
 The array is then transferred to another array, and that array is sorted from low to high.
 Then the highest and lowest %15 of samples are thrown out. The remaining data is averaged
 and the result is returned.

 Every sensor used with the digitalSmooth function needs to have its own array to hold
 the raw sensor values. This array is then passed to the function, for it's use.
 This is done with the name of the array associated with the particular sensor.
 */

// NSAMP should  be an odd number, no smaller than 3
template <int NSAMP>
class DigitalSmooth {

public:
    DigitalSmooth(float perc_keep)
        : perc_keep_(perc_keep), counter_(0) 
    {
        // Initialize so that we don't have undefined values on first use
        for(int i; i < NSAMP; i++) {
            samples_[i] = 0;
        }
    }

    void init(int initial_value)
    {
        for(int i; i < NSAMP; i++) {
            samples_[i] = initial_value;
        }

    }

    int smooth(int value)
    {
        // increment counter and roll over if necc.
        // % (modulo operator) rolls over variable
        counter_ = (counter_ + 1) % NSAMP;

        // input new data into the oldest slot
        samples_[counter_] = value;

        // transfer data array into anther array for sorting and averaging
        static int sorted[NSAMP];

        for (int j = 0; j < NSAMP; j++) {
            sorted[j] = samples_[j];
        }

        // flag to know when we're done sorting
        bool done = false;

        // simple swap sort, sorts numbers from lowest to highest
        int temp;

        while(!done) {
            done = true;

            for (int j = 0; j < (NSAMP - 1); j++) {
                // numbers are out of order - swap
                if (sorted[j] > sorted[j + 1]) {
                    temp = sorted[j + 1];
                    sorted[j + 1] =  sorted[j];
                    sorted[j] = temp;
                    done = false;
                }
            }
        }

        // throw out top and bottom perc_keep% of samples
        // limit to throw out at least one from top and bottom
        int bottom = max(NSAMP * (1 - perc_keep_), 1);
        int top = min((NSAMP * perc_keep_) + 1, NSAMP - 1);   // the + 1 is to make up for asymmetry caused by integer rounding
        int num_samples = 0;
        long total = 0;

        for (int j = bottom; j < top; j++) {
            total += sorted[j];  // total remaining indices
            num_samples++;
        }

        return total / num_samples;    // divide by number of samples
    }

private:
    float perc_keep_;
    int counter_;
    int samples_[NSAMP];

};

#endif
