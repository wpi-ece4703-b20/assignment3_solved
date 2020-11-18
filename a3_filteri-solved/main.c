#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "msp432_boostxl_init.h"
#include "msp432_arm_dsp.h"
#include <stdlib.h>

#define MWSPT_NSEC 11
const int NL[MWSPT_NSEC] = { 1,3,1,3,1,3,1,3,1,3,1 };
const float32_t NUM[MWSPT_NSEC][3] = {
  {
      0.216574356,              0,              0
  },
  {
                1,              0,             -1
  },
  {
      0.216574356,              0,              0
  },
  {
                1,              0,             -1
  },
  {
     0.1979159862,              0,              0
  },
  {
                1,              0,             -1
  },
  {
     0.1979159862,              0,              0
  },
  {
                1,              0,             -1
  },
  {
     0.1919633895,              0,              0
  },
  {
                1,              0,             -1
  },
  {
                1,              0,              0
  }
};
const int DL[MWSPT_NSEC] = { 1,3,1,3,1,3,1,3,1,3,1 };
const float32_t DEN[MWSPT_NSEC][3] = {
  {
                1,              0,              0
  },
  {
                1,   -1.082482219,   0.8809102774
  },
  {
                1,              0,              0
  },
  {
                1,  -0.3098140955,   0.8581526875
  },
  {
                1,              0,              0
  },
  {
                1,   -0.871129334,   0.6990858912
  },
  {
                1,              0,              0
  },
  {
                1,  -0.4202696085,   0.6671988368
  },
  {
                1,              0,              0
  },
  {
                1,  -0.6305604577,   0.6160731912
  },
  {
                1,              0,              0
  }
};

float32_t iir2(float32_t x,
               float32_t ntaps[2],
               float32_t dtaps[2],
               const float32_t G,
               const float32_t N[3],
               const float32_t D[3]) {
    float32_t k1, k2, k3, y;

    k1 = N[1] * ntaps[0] + N[2] * ntaps[1];
    k3 = N[0] * x + k1;

    k2 = D[1] * dtaps[0] + D[2] * dtaps[1];
    y  = k3 - k2;

    ntaps[1] = ntaps[0];
    ntaps[0] = x;

    dtaps[1] = dtaps[0];
    dtaps[0] = y;

    return y * G;
}

uint16_t processSample(uint16_t x) {
    float32_t input = adc14_to_f32(0x1800 + rand() % 0x1000);
//    float32_t input = adc14_to_f32(x);

    static float32_t nfilter[5][2] = {0};
    static float32_t dfilter[5][2] = {0};

    float32_t v;
    v = iir2(input, nfilter[0], dfilter[0], NUM[0][0], NUM[1], DEN[1]);
    v = iir2(v,     nfilter[1], dfilter[1], NUM[2][0], NUM[3], DEN[3]);
    v = iir2(v,     nfilter[2], dfilter[2], NUM[4][0], NUM[5], DEN[5]);
    v = iir2(v,     nfilter[3], dfilter[3], NUM[6][0], NUM[7], DEN[7]);
    v = iir2(v,     nfilter[4], dfilter[4], NUM[8][0], NUM[9], DEN[9]);

    return f32_to_dac14(v);
}

#include <stdio.h>

int main(void) {
    WDT_A_hold(WDT_A_BASE);

    msp432_boostxl_init_intr(FS_16000_HZ, BOOSTXL_J1_2_IN, processSample);
    msp432_boostxl_run();

    return 1;
}
