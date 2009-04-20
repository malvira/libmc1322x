/* Timer registers are all 16-bit wide with 16-bit access only */
#define TMR_OFFSET      (0x20)
#define TMR_BASE        (0x80007000)
#define TMR0_BASE       (TMR_BASE)
#define TMR1_BASE       (TMR_BASE + TMR_OFFSET*1)
#define TMR2_BASE       (TMR_BASE + TMR_OFFSET*2)
#define TMR3_BASE       (TMR_BASE + TMR_OFFSET*3)

#define TMR_REGOFF_COMP1    (0x0)
#define TMR_REGOFF_COMP2    (0x2)
#define TMR_REGOFF_CAPT     (0x4)
#define TMR_REGOFF_LOAD     (0x6)
#define TMR_REGOFF_HOLD     (0x8)
#define TMR_REGOFF_CNTR     (0xa)
#define TMR_REGOFF_CTRL     (0xc)
#define TMR_REGOFF_SCTRL    (0xe)
#define TMR_REGOFF_CMPLD1   (0x10)
#define TMR_REGOFF_CMPLD2   (0x12)
#define TMR_REGOFF_CSCTRL   (0x14)
#define TMR_REGOFF_ENBL     (0x1e)

/* Timer 0 registers */
#define TMR0_COMP1   (TMR0_BASE + TMR_REGOFF_COMP1)
#define TMR0_COMP_UP TMR0_COMP1
#define TMR0_COMP2   (TMR0_BASE + TMR_REGOFF_COMP2)
#define TMR0_COMP_DOWN TMR0_COMP2
#define TMR0_CAPT    (TMR0_BASE + TMR_REGOFF_CAPT)
#define TMR0_LOAD    (TMR0_BASE + TMR_REGOFF_LOAD)
#define TMR0_HOLD    (TMR0_BASE + TMR_REGOFF_HOLD)
#define TMR0_CNTR    (TMR0_BASE + TMR_REGOFF_CTRL)
#define TMR0_CTRL    (TMR0_BASE + TMR_REGOFF_CTRL)
#define TMR0_SCTRL   (TMR0_BASE + TMR_REGOFF_SCTRL)
#define TMR0_CMPLD1  (TMR0_BASE + TMR_REGOFF_CMPLD1)
#define TMR0_CMPLD2  (TMR0_BASE + TMR_REGOFF_CMPLD2)
#define TMR0_CSCTRL  (TMR0_BASE + TMR_REGOFF_CSCTRL)

/* one enable register to rule them all */
#define TMR_ENBL    TMR0_BASE + TMR_REGOFF_ENBL

#define MBAR_GPIO       0x80000000
#define GPIO_PAD_DIR0   0x80000000
#define GPIO_DATA0      0x80000008
#define UART1_DATA      0x80005008
#define DELAY 400000

#define reg32(x) (*(volatile uint32_t *)(x))
#define reg16(x) (*(volatile uint16_t *)(x))

#include "embedded_types.h"

__attribute__ ((section ("startup")))
void main(void) {

	/* pin direction */
	reg32(GPIO_PAD_DIR0) = 0x00000400;

	/* timer setup */
	/* CTRL */
#define COUNT_MODE 1      /* use rising edge of primary source */
#define PRIME_SRC  0xf    /* Perip. clock with 128 prescale (for 24Mhz = 187500Hz)*/
#define SEC_SRC    0      /* don't need this */
#define ONCE       0      /* keep counting */
#define LEN        1      /* count until compare then reload with value in LOAD */
#define DIR        0      /* count up */
#define CO_INIT    0      /* other counters cannot force a re-initialization of this counter */
#define OUT_MODE   0      /* OFLAG is asserted while counter is active */

	reg16(TMR_ENBL) = 0;                     /* tmrs reset to enabled */
	reg16(TMR0_SCTRL) = 0;
	reg16(TMR0_LOAD) = 0;                    /* reload to zero */
	reg16(TMR0_COMP_UP) = 18750;             /* trigger a reload at the end */
	reg16(TMR0_CMPLD1) = 18750;              /* compare 1 triggered reload level, 10HZ maybe? */
	reg16(TMR0_CNTR) = 0;                    /* reset count register */
	reg16(TMR0_CTRL) = (COUNT_MODE<<13) | (PRIME_SRC<<9) | (SEC_SRC<<7) | (ONCE<<6) | (LEN<<5) | (DIR<<4) | (CO_INIT<<3) | (OUT_MODE);
	reg16(TMR_ENBL) = 0xf;                   /* enable all the timers --- why not? */

	while(1) {

		/* blink on */
		reg32(GPIO_DATA0) = 0x00000400;
		
		while((reg16(TMR0_SCTRL)>>15) == 0) { continue; }
		reg16(TMR0_SCTRL) = 0; /*clear bit 15, and all the others --- should be ok, but clearly not "the right thing to do" */

		/* blink off */
		reg32(GPIO_DATA0) = 0x00000000;

		while((reg16(TMR0_SCTRL)>>15) == 0) { continue; }
		reg16(TMR0_SCTRL) = 0; /*clear bit 15, and all the others --- should be ok, but clearly not "the right thing to do" */

	};
}
