/*	SCCS Id: @(#)getline.c	3.1	90/22/02
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include "hack.h"
#include "mactty.h"
#include "macwin.h"
#include "macpopup.h"
#include "func_tab.h"

typedef Boolean FDECL ((* key_func), (unsigned char));

int
get_line_from_key_queue (char * bufp) {
	* bufp = 0;
	if (try_key_queue (bufp)) {
		while (* bufp) {
			if (* bufp == 10 || * bufp == 13) {
				* bufp = 0;
			}
			bufp ++;
		}
		return true;
	}
	return false;
}


static void
topl_getlin(const char *query, char *bufp, Boolean ext) {
	int q_len = strlen(query);

	if (get_line_from_key_queue (bufp))
		return;

	enter_topl_mode((char *) query);
	while (topl_key(nhgetch(), ext))
		;
	leave_topl_mode(bufp);
}


/*
 * Read a line closed with '\n' into the array char bufp[BUFSZ].
 * (The '\n' is not stored. The string is closed with a '\0'.)
 * Reading can be interrupted by an escape ('\033') - now the
 * resulting string is "\033".
 */
void
mac_getlin(const char *query, char *bufp) {

#if ENABLE_MAC_POPUP
	if (iflags.popup_dialog)
		popup_getlin (query, bufp);
	else
#endif
		topl_getlin (query, bufp, false);
}


/* Read in an extended command - doing command line completion for
 * when enough characters have been entered to make a unique command.
 * This is just a modified getlin() followed by a lookup.   -jsb
 */
int
mac_get_ext_cmd() {
	char bufp[BUFSZ];
	int i;

	topl_getlin("# ", bufp, true);
	for (i = 0; extcmdlist[i].ef_txt != (char *)0; i++)
		if (!strcmp(bufp, extcmdlist[i].ef_txt)) break;
	if (extcmdlist[i].ef_txt == (char *)0) i = -1;    /* not found */

	return i;
}


/* macgetline.c */
