/*	SCCS Id: @(#)termcap.c	3.0	88/11/20
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

#include <ctype.h>	/* for isdigit() */

/* block some unused #defines to avoid overloading some cpp's */
#define MONATTK_H
#include "hack.h"	/* for ROWNO, COLNO, *HI, *HE, *AS, *AE */

#ifndef MSDOS
# define TERMLIB	/* include termcap code */
#endif

#if !defined(SYSV) || defined(TOS) || defined(UNIXPC)
# ifndef LINT
extern			/* it is defined in libtermlib (libtermcap) */
# endif
	short ospeed;	/* terminal baudrate; used by tputs */
#else
short	ospeed = 0;	/* gets around "not defined" error message */
#endif

#ifdef MICROPORT_286_BUG
#define Tgetstr(key) (tgetstr(key,tbuf))
#else
#define Tgetstr(key) (tgetstr(key,&tbufptr))
#endif /* MICROPORT_286_BUG **/

static void nocmov();
#ifdef TEXTCOLOR
# ifdef TERMLIB
static void init_hilite();
# endif
#endif

static char *HO, *CL, *CE, *UP, *CM, *ND, *XD, *BC, *SO, *SE, *TI, *TE;
static char *VS, *VE, *US, *UE;
static char *MR, *ME;
#if 0
static char *MB, *MH;
static char *MD;	/* may already be in use below */
#endif
#ifdef TERMLIB
# ifdef TEXTCOLOR
static char *MD;
# endif
static int SG;
static char PC = '\0';
static char tbuf[512];
#endif

#ifndef TERMLIB
static char tgotobuf[20];
# ifdef TOS
#define tgoto(fmt, x, y)	(Sprintf(tgotobuf, fmt, y+' ', x+' '), tgotobuf)
# else
#define tgoto(fmt, x, y)	(Sprintf(tgotobuf, fmt, y+1, x+1), tgotobuf)
# endif
#endif /* TERMLIB */

void
startup()
{
#ifdef TERMLIB
	register char *term = getenv("TERM");
	register char *tptr;
	char *tbufptr, *pc;
#endif
	register int i;

	/* Set the default map symbols */
	(void) memcpy((genericptr_t) &showsyms, 
		(genericptr_t) &defsyms, sizeof(struct symbols));

#if !defined(AMIGA) && !defined(TOS)
# if defined(TERMLIB) || !(defined(DECRAINBOW) || defined(OS2))
#  define IBMXASCII
# endif
#endif

#ifdef IBMXASCII
	/*
	 * If we're on an IBM box, default to the nice IBM Extended ASCII
	 * line-drawing characters (codepage 437).
	 *
	 * OS/2 defaults to a multilingual character set (codepage 850,
	 * corresponding to the ISO 8859 character set.  We should probably
	 * do a VioSetCp() call to set the codepage to 437.
	 *
	 * Someday we should do a full terminfo(4) check for ACS forms
	 * characters.
	 */
# if !defined(MSDOS) || defined(DECRAINBOW) || defined(OS2)
	if (strncmp("AT", term, 2) == 0)
# endif
	{
	    showsyms.vwall = 0xb3;	/* meta-3, vertical rule */
	    showsyms.hwall = 0xc4;	/* meta-D, horizontal rule */
	    showsyms.tlcorn = 0xda;	/* meta-Z, top left corner */
	    showsyms.trcorn = 0xbf;	/* meta-?, top right corner */
	    showsyms.blcorn = 0xc0;	/* meta-@, bottom left */
	    showsyms.brcorn = 0xd9;	/* meta-Y, bottom right */
	    showsyms.crwall = 0xc5;	/* meta-E, cross */
	    showsyms.tuwall = 0xc1;	/* meta-A, T up */
	    showsyms.tdwall = 0xc2;	/* meta-B, T down */
	    showsyms.tlwall = 0xb4;	/* meta-4, T left */
	    showsyms.trwall = 0xc3;	/* meta-C, T right */
	    showsyms.vbeam = 0xb3;	/* meta-3, vertical rule */
	    showsyms.hbeam = 0xc4;	/* meta-D, horizontal rule */
	    showsyms.room = 0xfa;	/* meta-z, centered dot */
	    showsyms.pool = 0xf7;	/* meta-w, approx. equals */
	}
#endif /* IBMXASCII */
#undef IBMXASCII

#ifdef TERMLIB
	if(!term)
#endif
#if defined(TOS) && defined(__GNUC__) && defined(TERMLIB)
		term = "st52";		/* library has a default */
#else
# ifdef ANSI_DEFAULT
#  ifdef TOS
	{
		HO = "\033H";
		CL = "\033E";		/* the VT52 termcap */
		CE = "\033K";
		UP = "\033A";
		CM = "\033Y%c%c";	/* used with function tgoto() */
		ND = "\033C";
		XD = "\033B";
		BC = "\033D";
		SO = "\033p";
		SE = "\033q";
		HI = "\033p";
		HE = "\033q\033b\020";
		for (i = 0; i < SIZE(hilites); i++) {
			hilites[i] = (char *) alloc(sizeof("Eb1"));
			Sprintf(hilites[i], (i%4)?"\033b%c","\033p", i);
		}
	}
#  else /* TOS */
	{
#   ifdef DGK
		get_scr_size();
		if(CO < COLNO || LI < ROWNO+3)
			setclipped();
#   endif
		HO = "\033[H";
		CL = "\033[2J";		/* the ANSI termcap */
/*		CD = "\033[J"; */
		CE = "\033[K";
#   ifndef TERMLIB
		CM = "\033[%d;%dH";
#   else
		CM = "\033[%i%d;%dH";
#   endif
		UP = "\033[A";
		ND = "\033[C";
		XD = "\033[B";
#   ifdef MSDOS	/* backspaces are non-destructive */
		BC = "\b";
#   else
		BC = "\033[D";
#   endif
		HI = SO = "\033[1m";
		US = "\033[4m";
		MR = "\033[7m";
		TI = HE = SE = UE = ME = "\033[0m";
		/* strictly, SE should be 2, and UE should be 24,
		   but we can't trust all ANSI emulators to be
		   that complete.  -3. */
#   if !defined(MSDOS) || defined(DECRAINBOW)
		AS = "\016";
		AE = "\017";
#   endif
		TE = VS = VE = "";
#   ifdef TEXTCOLOR
		for (i = 0; i < MAXCOLORS / 2; i++) {
			hilites[i] = (char *) alloc(sizeof("\033[0;3%dm"));
			hilites[i+BRIGHT] = (char *) alloc(sizeof("\033[1;3%dm"));
			Sprintf(hilites[i], "\033[0;3%dm", i);
			Sprintf(hilites[i+BRIGHT], "\033[1;3%dm", i);
		}
#   endif
		return;
	}
#  endif /* TOS */
# else
		error("Can't get TERM.");
# endif /* ANSI_DEFAULT */
#endif /* __GNUC__ */
#ifdef TERMLIB
	tptr = (char *) alloc(1024);

	tbufptr = tbuf;
	if(!strncmp(term, "5620", 4))
		flags.nonull = 1;	/* this should be a termcap flag */
	if(tgetent(tptr, term) < 1)
		error("Unknown terminal type: %s.", term);
	if(pc = Tgetstr("pc"))
		PC = *pc;
# ifdef TERMINFO
	if(!(BC = Tgetstr("le"))) {	
# else
	if(!(BC = Tgetstr("bc"))) {	
# endif
# if !defined(MINIMAL_TERM) && !defined(HISX)
		if(!tgetflag("bs"))
			error("Terminal must backspace.");
# endif
		BC = tbufptr;
		tbufptr += 2;
		*BC = '\b';
	}
# ifdef MINIMAL_TERM
	HO = NULL;
# else
	HO = Tgetstr("ho");
# endif
	/*
	 * LI and CO are set in ioctl.c via a TIOCGWINSZ if available.  If
	 * the kernel has values for either we should use them rather than
	 * the values from TERMCAP ...
	 */
# ifndef DGK
	if (!CO) CO = tgetnum("co");
	if (!LI) LI = tgetnum("li");
# else
	CO = tgetnum("co");
	LI = tgetnum("li");
	if (!LI || !CO)			/* if we don't override it */
		get_scr_size();
# endif
	if(CO < COLNO || LI < ROWNO+3)
		setclipped();
	if(!(CL = Tgetstr("cl")))
		error("Hack needs CL.");
	ND = Tgetstr("nd");
	if(tgetflag("os"))
		error("Hack can't have OS.");
	CE = Tgetstr("ce");
	UP = Tgetstr("up");
	/* It seems that xd is no longer supported, and we should use
	   a linefeed instead; unfortunately this requires resetting
	   CRMOD, and many output routines will have to be modified
	   slightly. Let's leave that till the next release. */
	XD = Tgetstr("xd");
/* not: 		XD = Tgetstr("do"); */
	if(!(CM = Tgetstr("cm"))) {
		if(!UP && !HO)
			error("Hack needs CM or UP or HO.");
		Printf("Playing hack on terminals without cm is suspect...\n");
		getret();
	}
	SO = Tgetstr("so");
	SE = Tgetstr("se");
	US = Tgetstr("us");
	UE = Tgetstr("ue");
	SG = tgetnum("sg");	/* -1: not fnd; else # of spaces left by so */
	if(!SO || !SE || (SG > 0)) SO = SE = US = UE = "";
	TI = Tgetstr("ti");
	TE = Tgetstr("te");
	VS = VE = "";
# if 0
	MB = Tgetstr("mb");	/* blink */
	MD = Tgetstr("md");	/* boldface */
	MH = Tgetstr("mh");	/* dim */
# endif
	MR = Tgetstr("mr");	/* reverse */
	ME = Tgetstr("me");

	/* Get rid of padding numbers for HI and HE.  Hope they
	 * aren't really needed!!!  HI and HE are ouputted to the
	 * pager as a string - so how can you send it NULLS???
	 *  -jsb
	 */
	    HI = (char *) alloc((unsigned)(strlen(SO)+1));
	    HE = (char *) alloc((unsigned)(strlen(SE)+1));
	    i = 0;
	    while(isdigit(SO[i])) i++;
	    Strcpy(HI, &SO[i]);
	    i = 0;
	    while(isdigit(SE[i])) i++;
	    Strcpy(HE, &SE[i]);
	AS = Tgetstr("as");
	AE = Tgetstr("ae");
	CD = Tgetstr("cd");
# ifdef TEXTCOLOR
	MD = Tgetstr("md");
# endif
	set_whole_screen();		/* uses LI and CD */
	if(tbufptr-tbuf > sizeof(tbuf)) error("TERMCAP entry too big...\n");
	free((genericptr_t)tptr);
# ifdef TEXTCOLOR
	init_hilite();
# endif
#endif /* TERMLIB */
}

void
start_screen()
{
	xputs(TI);
	xputs(VS);
#ifdef DECRAINBOW
	/* Select normal ASCII and line drawing character sets.
	 */
	if (flags.DECRainbow) {
		xputs("\033(B\033)0");
		if (!AS) {
			AS = "\016";
			AE = "\017";
		}
	}
#endif /* DECRAINBOW */
}

void
end_screen()
{
	clear_screen();
	xputs(VE);
	xputs(TE);
}

/* Cursor movements */

void
curs(x, y)
register int x, y;	/* not xchar: perhaps xchar is unsigned and
			   curx-x would be unsigned as well */
{

	if (y == cury && x == curx)
		return;
	if(!ND && (curx != x || x <= 3)) {	/* Extremely primitive */
		cmov(x, y);			/* bunker!wtm */
		return;
	}
	if(abs(cury-y) <= 3 && abs(curx-x) <= 3)
		nocmov(x, y);
	else if((x <= 3 && abs(cury-y)<= 3) || (!CM && x<abs(curx-x))) {
		(void) putchar('\r');
		curx = 1;
		nocmov(x, y);
	} else if(!CM) {
		nocmov(x, y);
	} else
		cmov(x, y);
}

static void
nocmov(x, y)
{
	if (cury > y) {
		if(UP) {
			while (cury > y) {	/* Go up. */
				xputs(UP);
				cury--;
			}
		} else if(CM) {
			cmov(x, y);
		} else if(HO) {
			home();
			curs(x, y);
		} /* else impossible("..."); */
	} else if (cury < y) {
		if(XD) {
			while(cury < y) {
				xputs(XD);
				cury++;
			}
		} else if(CM) {
			cmov(x, y);
		} else {
			while(cury < y) {
				xputc('\n');
				curx = 1;
				cury++;
			}
		}
	}
	if (curx < x) {		/* Go to the right. */
		if(!ND) cmov(x, y); else	/* bah */
			/* should instead print what is there already */
		while (curx < x) {
			xputs(ND);
			curx++;
		}
	} else if (curx > x) {
		while (curx > x) {	/* Go to the left. */
			xputs(BC);
			curx--;
		}
	}
}

void
cmov(x, y)
register int x, y;
{
	xputs(tgoto(CM, x-1, y-1));
	cury = y;
	curx = x;
}

void
xputc(c)
char c;
{
	(void) fputc(c, stdout);
}

void
xputs(s)
char *s;
{
#ifndef TERMLIB
	(void) fputs(s, stdout);
#else
# ifdef __STDC__
	tputs(s, 1, (int (*)())xputc);
# else
	tputs(s, 1, xputc);
# endif
#endif
}

void
cl_end() {
	if(CE)
		xputs(CE);
	else {	/* no-CE fix - free after Harold Rynes */
		/* this looks terrible, especially on a slow terminal
		   but is better than nothing */
		register int cx = curx, cy = cury;

		while(curx < COLNO) {
			xputc(' ');
			curx++;
		}
		curs(cx, cy);
	}
}

void
clear_screen() {
	xputs(CL);
	home();
}

void
home()
{
	if(HO)
		xputs(HO);
	else if(CM)
		xputs(tgoto(CM, 0, 0));
	else
		curs(1, 1);	/* using UP ... */
	curx = cury = 1;
}

void
standoutbeg()
{
	if(SO) xputs(SO);
}

void
standoutend()
{
	if(SE) xputs(SE);
}

void
revbeg()
{
	if(MR) xputs(MR);
}

#if 0	/* if you need one of these, uncomment it (here and in extern.h) */
void
boldbeg()
{
	if(MD) xputs(MD);
}

void
blinkbeg()
{
	if(MB) xputs(MB);
}

void
dimbeg()
/* not in most termcap entries */
{
	if(MH) xputs(MH);
}
#endif

void
m_end()
{
	if(ME) xputs(ME);
}

void
backsp()
{
	xputs(BC);
}

void
bell()
{
	if (flags.silent) return;
	(void) putchar('\007');		/* curx does not change */
	(void) fflush(stdout);
}

#if defined(TERMLIB) || defined(DECRAINBOW)
void
graph_on() {
	if (AS) xputs(AS);
}

void
graph_off() {
	if (AE) xputs(AE);
}
#endif

#ifndef MSDOS
# ifdef VMS
static const short tmspc10[] = {		/* from termcap */
	0, 2000, 1333, 909, 743, 666, 333, 166, 83, 55, 50, 41, 27, 20, 13, 10,
	5
};
# else
static const short tmspc10[] = {		/* from termcap */
	0, 2000, 1333, 909, 743, 666, 500, 333, 166, 83, 55, 41, 20, 10, 5
};
# endif
#endif

void
delay_output() {
	/* delay 50 ms - could also use a 'nap'-system call */
	/* BUG: if the padding character is visible, as it is on the 5620
	   then this looks terrible. */
#ifdef MSDOS
	/* simulate the delay with "cursor here" */
	register int i;
	for (i = 0; i < 3; i++) {
		cmov(curx, cury);
		(void) fflush(stdout);
	}
#else /* MSDOS /**/
	if(!flags.nonull)
#ifdef TERMINFO
		/* cbosgd!cbcephus!pds for SYS V R2 */
# ifdef __STDC__
		tputs("$<50>", 1, (int (*)())xputc);
# else
		tputs("$<50>", 1, xputc);
# endif
#else
		tputs("50", 1, xputs);
#endif

	else if(ospeed > 0 || ospeed < SIZE(tmspc10)) if(CM) {
		/* delay by sending cm(here) an appropriate number of times */
		register int cmlen = strlen(tgoto(CM, curx-1, cury-1));
		register int i = 500 + tmspc10[ospeed]/2;

		while(i > 0) {
			cmov(curx, cury);
			i -= cmlen*tmspc10[ospeed];
		}
	}
#endif /* MSDOS /**/
}

void
cl_eos()			/* free after Robert Viduya */
{				/* must only be called with curx = 1 */

	if(CD)
		xputs(CD);
	else {
		register int cx = curx, cy = cury;
		while(cury <= LI-2) {
			cl_end();
			xputc('\n');
			curx = 1;
			cury++;
		}
		cl_end();
		curs(cx, cy);
	}
}

#if defined(TEXTCOLOR) && defined(TERMLIB)
# ifdef UNIX
/*
 * Sets up color highlighting, using terminfo(4) escape sequences (highlight
 * code found in pri.c).  It is assumed that the background color is black.
 */
/* terminfo indexes for the basic colors it guarantees */
#define COLOR_BLACK   1		/* fake out to avoid black on black */
#define COLOR_BLUE    1
#define COLOR_GREEN   2
#define COLOR_CYAN    3
#define COLOR_RED     4
#define COLOR_MAGENTA 5
#define COLOR_YELLOW  6
#define COLOR_WHITE   7

/* map ANSI RGB to terminfo BGR */
const int ti_map[8] = {
	COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW,
	COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE };

static void
init_hilite()
{
	register int c;
#  ifdef TERMINFO
	char *setf, *scratch;
	extern char *tparm();
#  endif

	for (c = 0; c < MAXCOLORS; c++)
		hilites[c] = HI;

#  ifdef TERMINFO
	if (tgetnum("Co") < 8 || (setf = tgetstr("Sf", 0)) == NULL)
		return;

	for (c = 0; c < MAXCOLORS / 2; c++) {
  		scratch = tparm(setf, ti_map[c]);
		hilites[c] = (char *) alloc(strlen(scratch) + 1);
		hilites[c+BRIGHT] = (char*) alloc(strlen(scratch)+strlen(MD)+1);
		Strcpy(hilites[c], scratch);
		Strcpy(hilites[c+BRIGHT], MD);
		Strcat(hilites[c+BRIGHT], scratch);
	}
#  endif
}

# else /* UNIX */

/*
 * Sets up highlighting sequences, using ANSI escape sequences (highlight code
 * found in pri.c).  The termcap entry for HI (from SO) is scanned to find the
 * background color.
 */

static void
init_hilite()
{
	int backg = BLACK, foreg = WHITE, len;
	register int c, color;

	for (c = 0; c < SIZE(hilites); c++)
		hilites[c] = HI;

#  ifdef TOS
	hilites[RED] = hilites[BRIGHT+RED] = "\033b1";
	hilites[BLUE] = hilites[BRIGHT+BLUE] = "\033b2";
	hilites[CYAN] = hilites[BRIGHT+CYAN] = "\033b3\033c2";
	hilites[ORANGE_COLORED] = hilites[RED];
	hilites[WHITE] = hilites[GRAY] = "\033b3";
	hilites[MAGENTA] = hilites[BRIGHT+MAGENTA] = "\033b1\033c2";
	HE = "\033q\033b3\033c0";	/* to turn off the color stuff too */
#  else /* TOS */
	/* find the background color, HI[len] == 'm' */
	len = strlen(HI) - 1;

	if (HI[len] != 'm' || len < 3) return;

	c = 2;
	while (c < len) {
	    if ((color = atoi(&HI[c])) == 0) {
		/* this also catches errors */
		foreg = WHITE; backg = BLACK;
	    /*
	    } else if (color == 1) {
		foreg |= BRIGHT;
	    */
	    } else if (color >= 30 && color <= 37) {
		foreg = color - 30;
	    } else if (color >= 40 && color <= 47) {
		backg = color - 40;
	    }
	    while (isdigit(HI[++c]));
	    c++;
	}

	for (c = 0; c < MAXCOLORS / 2; c++)
	    /* avoid invisibility */
	    if (foreg != c && backg != c) {
		hilites[c] = (char *) alloc(sizeof("\033[0;3%d;4%dm"));
		hilites[c+BRIGHT] = (char *) alloc(sizeof("\033[1;3%d;4%dm"));
		Sprintf(hilites[c], "\033[0;3%d;4%dm", c, backg);
		Sprintf(hilites[c+BRIGHT], "\033[1;3%d;4%dm", c, backg);
	    }
#  endif /* TOS */
}
# endif /* UNIX */
#endif /* TEXTCOLOR */
