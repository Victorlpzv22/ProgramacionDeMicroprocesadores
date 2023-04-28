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
static DigitalIn      g_swr(SWR_PIN);

// mux stuff
static Ticker         g_mux_tick;
static bool volatile  gb_mux_evnt;

static void mux_isr (void) {
  gb_mux_evnt = true;
}

// switch management
static Ticker         g_swr_tick;
static bool volatile  gb_swr_evnt;

static void swr_isr (void) {
  gb_swr_evnt = true;
}

int main (void) {
  uint8_t cnt = 0;                        // 0 to 99
  bool    b_right = false;
  bool    b_swr_state = false;
  uint8_t swr_cnt = 0;

  g_swr.mode(PullUp);
  g_mux_tick.attach_us(mux_isr, 4000);    // 250 Hz
  g_swr_tick.attach_us(swr_isr, 1000);    // 1 ms, 1000 Hz

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
      gb_swr_evnt = false;                // here every 1 ms
      if (b_swr_state != !g_swr) {        // swr changing? (active low)
        if (swr_cnt++ > 3) {              // this means 5 times, not 3
          b_swr_state = !b_swr_state;
          if (b_swr_state) {
            cnt += ((cnt >= 99) ? -cnt : 1);
          }
        }
      } else {
        swr_cnt = 0;
      }
    }
    __disable_irq();
    if (!gb_mux_evnt && !gb_swr_evnt) {
      __WFI();
    }
    __enable_irq();
  }
}

