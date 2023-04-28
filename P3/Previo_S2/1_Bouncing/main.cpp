#include "mbed.h"
#include "pinout.h"
#include "to_7seg.h"

// seven segment display anodes
// when in a int8_t, they are 0b-GFEDCBA
static BusOut         g_seven_seg(SGA_PIN, SGB_PIN, SGC_PIN, SGD_PIN,
                                  SGE_PIN, SGF_PIN, SGG_PIN);

// display catodes
static DigitalOut     g_dsl(DSL_PIN);
static DigitalOut     g_dsr(DSR_PIN);

// switch
static InterruptIn    g_swr(SWR_PIN);

// mux stuff
static Ticker         g_mux_tick;
static bool volatile  gb_mux_evnt;

static void mux_isr (void) {
  gb_mux_evnt = true;
}

// switch management
static bool volatile  gb_swr_evnt;

static void swr_isr (void) {
  gb_swr_evnt = true;
}

int main (void) {
  uint8_t cnt = 0;                        // 0 to 99
  bool    b_right = false;

  g_swr.mode(PullUp);
  g_mux_tick.attach_us(mux_isr, 4000);    // 250 Hz
  g_swr.fall(swr_isr);

  for (;;) {
    if (gb_mux_evnt) {
      gb_mux_evnt = false;
      if (b_right) {
        g_dsr = 0;
        g_seven_seg = to_7seg(cnt / 10);
        g_dsl = 1;
      } else {
        g_dsl = 0;
        g_seven_seg = to_7seg(cnt % 10);
        g_dsr = 1;
      }
      b_right = !b_right;
    }
    if (gb_swr_evnt) {
      gb_swr_evnt = false;
      cnt += ((cnt >= 99) ? -cnt : 1);
    }
    __disable_irq();
    if (!gb_mux_evnt && !gb_swr_evnt) {
      __WFI();
    }
    __enable_irq();
  }
}
