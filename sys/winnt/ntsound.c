/* NetHack 3.5	ntsound.c	$NHDT-Date$  $NHDT-Branch$:$NHDT-Revision$ */
/* NetHack 3.5	ntsound.c	$Date: 2009/05/06 10:53:33 $  $Revision: 1.7 $ */
/*   SCCS Id: @(#)ntsound.c   3.5     $NHDT-Date$                        */
/*   SCCS Id: @(#)ntsound.c   3.5     $Date: 2009/05/06 10:53:33 $                        */
/*   Copyright (c) NetHack PC Development Team 1993                 */
/*   NetHack may be freely redistributed.  See license for details. */
/*                                                                  */
/*
 * ntsound.c - Windows NT NetHack sound support
 *                                                  
 *Edit History:
 *     Initial Creation                              93/12/11
 *
 */

#include "hack.h"
#include "win32api.h"
#include <mmsystem.h>

#ifdef USER_SOUNDS

void play_usersound(filename, volume)
const char* filename;
int volume;
{
/*    pline("play_usersound: %s (%d).", filename, volume); */
	(void)sndPlaySound(filename, SND_ASYNC | SND_NODEFAULT);
}

#endif /*USER_SOUNDS*/
/* ntsound.c */
