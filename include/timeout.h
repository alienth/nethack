/* NetHack 3.6	timeout.h	$NHDT-Date$  $NHDT-Branch$:$NHDT-Revision$ */
/* NetHack 3.6	timeout.h	$Date: 2009/05/06 10:45:12 $  $Revision: 1.6 $ */
/*	SCCS Id: @(#)timeout.h	3.5	1999/02/13	*/
/* Copyright 1994, Dean Luick					  */
/* NetHack may be freely redistributed.  See license for details. */

#ifndef TIMEOUT_H
#define TIMEOUT_H

/* generic timeout function */
typedef void FDECL((*timeout_proc), (ANY_P *, long));

/* kind of timer */
#define TIMER_LEVEL 0   /* event specific to level */
#define TIMER_GLOBAL 1  /* event follows current play */
#define TIMER_OBJECT 2  /* event follows a object */
#define TIMER_MONSTER 3 /* event follows a monster */

/* save/restore timer ranges */
#define RANGE_LEVEL 0  /* save/restore timers staying on level */
#define RANGE_GLOBAL 1 /* save/restore timers following global play */

/*
 * Timeout functions.  Add a define here, then put it in the table
 * in timeout.c.  "One more level of indirection will fix everything."
 */
#define ROT_ORGANIC 0 /* for buried organics */
#define ROT_CORPSE 1
#define REVIVE_MON 2
#define BURN_OBJECT 3
#define HATCH_EGG 4
#define FIG_TRANSFORM 5
#define MELT_ICE_AWAY 6
#define NUM_TIME_FUNCS 7

/* used in timeout.c */
typedef struct fe {
    struct fe *next;          /* next item in chain */
    long timeout;             /* when we time out */
    unsigned long tid;        /* timer ID */
    short kind;               /* kind of use */
    short func_index;         /* what to call when we time out */
    anything arg;             /* pointer to timeout argument */
    Bitfield(needs_fixup, 1); /* does arg need to be patched? */
} timer_element;

#endif /* TIMEOUT_H */
