/*	SCCS Id: @(#)macerrs.c	3.1	93/01/24		  */
/* Copyright (c) Michael Hamel, 1991 */
/* NetHack may be freely redistributed.  See license for details. */

#ifdef applec	/* This needs to be resident always */
#pragma segment Main
#endif

#include "hack.h"
#include "macwin.h"
#include <Dialogs.h>
#include <TextUtils.h>
#include <Resources.h>

#define stackDepth  1
#define errAlertID 129
#define stdIOErrID 1999

static Str255 gActivities[stackDepth] = {""};
static short gTopactivity = 0;

void showerror(char * errdesc, const char * errcomment)
{
	short		itemHit;
	Str255		paserr,
				pascomment;
				
	SetCursor(&qd.arrow);
	if (errcomment == nil) errcomment = "";
	C2P (errcomment, pascomment);
	C2P (errdesc, paserr);
	ParamText(paserr,pascomment,gActivities[gTopactivity],(StringPtr)"");
	itemHit = Alert(errAlertID, (ModalFilterUPP)nil);
}


Boolean itworked(short errcode)
/* Return TRUE if it worked, do an error message and return false if it didn't. Error
   strings for native C errors are in STR#1999, Mac errs in STR 2000-errcode, e.g
   2108 for not enough memory */

{
	if (errcode != 0) {
		short		 itemHit;
		Str255 		 errdesc;
		StringHandle strh;
	
		errdesc[0] = '\0';
		if (errcode > 0) GetIndString(errdesc,stdIOErrID,errcode);  /* STDIO file rres, etc */
		else {
			strh = GetString(2000-errcode);
			if (strh != (StringHandle) nil) {
				memcpy(errdesc,*strh,256);
				ReleaseResource((Handle)strh);
			}
		}
		if (errdesc[0] == '\0') {  /* No description found, just give the number */
			sprintf((char *)&errdesc[1],"a %d error occurred",errcode);
			errdesc[0] = strlen((char*)&errdesc[1]);
		}
		SetCursor(&qd.arrow);
		ParamText(errdesc,(StringPtr)"",gActivities[gTopactivity],(StringPtr)"");
		itemHit = Alert(errAlertID, (ModalFilterUPP)nil);
	}
	return(errcode==0);
}

void mustwork(short errcode)
/* For cases where we can't recover from the error by any means */
{
	if (itworked(errcode)) ;
	else ExitToShell();
}


#if defined(USE_STDARG) || defined(USE_VARARGS)
# ifdef USE_STDARG
static void vprogerror(const char *line, va_list the_args);
# else
static void vprogerror();
# endif

/* Macro substitute for error() */
void error VA_DECL(const char *, line)
	VA_START(line);
	VA_INIT(line, char *);
	vprogerror(line, VA_ARGS);
	VA_END();
}

# ifdef USE_STDARG
static void
vprogerror(const char *line, va_list the_args) {
# else
static void
vprogerror(line, the_args) const char *line; va_list the_args; {
# endif

#else  /* USE_STDARG | USE_VARARG */

void
error VA_DECL(const char *, line)
#endif
/* Do NOT use VA_START and VA_END in here... see above */
	char pbuf[BUFSZ];

	if(index(line, '%')) {
		Vsprintf(pbuf,line,VA_ARGS);
		line = pbuf;
	}
	showerror("of an internal error",line);
}

void attemptingto(char * activity)
/* Say what we are trying to do for subsequent error-handling: will appear as x in an
   alert in the form "Could not x because y" */
{	C2P(activity,gActivities[gTopactivity]);
}

#if 0 /* Apparently unused */
void comment(char *s, long n)
{
	Str255 paserr;
	short itemHit;
	
	sprintf((char *)&paserr[1], "%s - %d",s,n);
	paserr[0] = strlen ((char*)&paserr[1]);
	ParamText(paserr,(StringPtr)"",(StringPtr)"",(StringPtr)"");
	itemHit = Alert(128, (ModalFilterUPP)nil);
}

void pushattemptingto(char * activity)
/* Push a new description onto stack so we can pop later to previous state */
{
	if (gTopactivity < stackDepth) {
		gTopactivity++;
		attemptingto(activity);
	}
	else error("activity stack overflow");
}

void popattempt(void)
/* Pop to previous state */
{
	if (gTopactivity > 1) --gTopactivity;
	else error("activity stack underflow");
}
#endif /* Apparently unused */
