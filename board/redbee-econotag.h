#ifndef BOARD_REDBEE_DEV_H
#define BOARD_REDBEE_DEV_H

#define LED_RED   (1 << 23)
#define LED_GREEN (1 << 24)
#define LED_BLUE  (1 << 25)

/* XTAL TUNE parameters */
/* see http://devl.org/pipermail/mc1322x/2009-December/000162.html */
/* for details about how to make this measurment */

/* Econotag also needs an addtional 12pf on board */
/* Coarse tune: add 4pf */
#define CTUNE_4PF 1
/* Coarse tune: add 0-15 pf (CTUNE is 4 bits) */
#define CTUNE 11
/* Fine tune: add FTUNE * 156fF (FTUNE is 5bits) */
#define FTUNE 3

#include <std_conf.h>

#endif