/* NetHack 3.5	macpopup.h	$NHDT-Date$  $NHDT-Branch$:$NHDT-Revision$ */
/* NetHack 3.5	macpopup.h	$Date: 2009/05/06 10:44:51 $  $Revision: 1.6 $ */
/*	SCCS Id: @(#)macpopup.h	3.5	1999/10/25	*/
/* Copyright (c) Nethack Develpment Team, 1999.		*/
/* NetHack may be freely redistributed.  See license for details. */

#ifndef MACPOPUP_H
# define MACPOPUP_H

/* ### mmodal.c ### */

extern void FlashButton(DialogRef, short);
extern char queued_resp(char *resp);
extern char topl_yn_function(const char *query, const char *resp, char def);
extern int get_line_from_key_queue(char *bufp);

#endif /* MACPOPUP_H */
