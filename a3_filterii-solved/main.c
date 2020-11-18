#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "msp432_boostxl_init.h"
#include "msp432_arm_dsp.h"
#include <stdlib.h>

#define MWSPT_NSEC 11
const int NL[MWSPT_NSEC] = { 1,3,1,3,1,3,1,3,1,3,1 };
const float32_t NUM[MWSPT_NSEC][3] = {
  {
       0.49548381567,                 0,                 0
  },
  {
                   1,  -0.2867178022861,                 1
  },
  {
       0.49548381567,                 0,                 0
  },
  {
                   1,   -1.182613372803,                 1
  },
  {
      0.440425157547,                 0,                 0
  },
  {
                   1,   -1.310552239418,                 1
  },
  {
      0.440425157547,                 0,                 0
  },
  {
                   1, -0.07903191447258,                 1
  },
  {
     0.1847512274981,                 0,                 0
  },
  {
                   1,                 0,                -1
  },
  {
                   1,                 0,                 0
  }
};

const int DL[MWSPT_NSEC] = { 1,3,1,3,1,3,1,3,1,3,1 };
const float32_t DEN[MWSPT_NSEC][3] = {
  {
                   1,                 0,                 0
  },
  {
                   1,  -0.3868900835514,   0.9795336723328
  },
  {
                   1,                 0,                 0
  },
  {
                   1,   -1.100977659225,   0.9826176166534
  },
  {
                   1,                 0,                 0
  },
  {
                   1,  -0.4729762077332,   0.9133484363556
  },
  {
                   1,                 0,                 0
  },
  {
                   1,  -0.9933134317398,   0.9230388402939
  },
  {
                   1,                 0,                 0
  },
  {
                   1,  -0.7248051762581,    0.857614338398
  },
  {
                   1,                 0,                 0
  }
};

float32_t iir2(float32_t x, float32_t taps[2], const float32_t G, const float32_t N[3], const float32_t D[3]) {
    float32_t k1, k2, k3, y;

    k1 = N[0] * x + taps[0];
    y  = k1;
    k2 = N[1] * x + taps[1] - D[1] * y;
    k3 = N[2] * x - D[2] * y;

    taps[0] = k2;
    taps[1] = k3;

    return y * G;
}

uint16_t processSample(uint16_t x) {
    float32_t input = adc14_to_f32(0x1800 + rand() % 0x1000);
//    float32_t input = adc14_to_f32(x);

    static float32_t filter[5][2] = {0};

    float32_t v;
    v = iir2(input, filter[0], NUM[0][0], NUM[1], DEN[1]);
    v = iir2(v,     filter[1], NUM[2][0], NUM[3], DEN[3]);
    v = iir2(v,     filter[2], NUM[4][0], NUM[5], DEN[5]);
    v = iir2(v,     filter[3], NUM[6][0], NUM[7], DEN[7]);
    v = iir2(v,     filter[4], NUM[8][0], NUM[9], DEN[9]);

    return f32_to_dac14(v);
}

#include <stdio.h>

int main(void) {
    WDT_A_hold(WDT_A_BASE);

    msp432_boostxl_init_intr(FS_16000_HZ, BOOSTXL_J1_2_IN, processSample);
    msp432_boostxl_run();

    return 1;
}
