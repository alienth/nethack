/*	SCCS Id: @(#)nttty.c	3.1	90/05/23
/* Copyright (c) NetHack PC Development Team 1993    */
/* NetHack may be freely redistributed.  See license for details. */

/* tty.c - (Windows NT) version */
/*                                                  
 * Initial Creation - Michael Allison 93/01/31 
 *
 */

#ifdef WIN32CON

#define NEED_VARARGS /* Uses ... */
#include "hack.h"
#include "wintty.h"
#include <sys\types.h>
#include <sys\stat.h>
#pragma pack(8)
#include <windows.h>
#include <wincon.h>

void FDECL(cmov, (int, int));
void FDECL(nocmov, (int, int));

/*
 * The following WIN32 Console API routines are used in this file.
 *
 * CreateFile
 * GetConsoleScreenBufferInfo
 * GetStdHandle
 * SetConsoleCursorPosition
 * SetConsoleTextAttribute
 * PeekConsoleInput
 * ReadConsoleInput
 * WriteConsoleOutputCharacter
 */

/* Win32 Console handles for input and output */
HANDLE hConIn;
HANDLE hConOut;

/* Win32 Screen buffer,coordinate,console I/O information */
CONSOLE_SCREEN_BUFFER_INFO csbi;
COORD ntcoord;
INPUT_RECORD ir;

/* Flag for whether NetHack was launched via progman, not command line.
 * The reason we care at all, is so that we can get
 * a final RETURN at the end of the game when launched from progman
 * to prevent the scoreboard (or panic message :-| from vanishing
 * immediately after it is displayed, yet not bother when started
 * from the command line. 
 */
int ProgmanLaunched;

# ifdef TEXTCOLOR
char ttycolors[MAXCOLORS];
static void NDECL(init_ttycolor);
# endif

static char nullstr[] = "";
char erase_char,kill_char;
/* extern int LI, CO; */	/* decl.h does this alreay */

/*
 * Called after returning from ! or ^Z
 */
void
gettty(){
	register int i;

	erase_char = '\b';
	kill_char = 21;		/* cntl-U */
	flags.cbreak = TRUE;
#ifdef TEXTCOLOR
	init_ttycolor();
#endif
}

/* reset terminal to original state */
void
settty(s)
const char *s;
{
	end_screen();
	if(s) raw_print(s);
}

/* called by init_nhwindows() and resume_nhwindows() */
void
setftty()
{
	start_screen();
}

int
tgetch()
{
	int ch;
	int valid = 0;
	int metaflags = 0;
	int count;

	valid = 0;
	while (!valid)
	{
	    ReadConsoleInput(hConIn,&ir,1,&count);
	    /* We only care about ascii press-down KEY_EVENTs */
	    if ((ir.EventType == KEY_EVENT) &&
	       (ir.Event.KeyEvent.bKeyDown) &&	
	       (ir.Event.KeyEvent.uChar.AsciiChar)) valid = 1;
	}
	metaflags = ir.Event.KeyEvent.dwControlKeyState;
	/*
	 * metaflags can be checked to see if various special
         * keys were pressed at the same time as the key.
         * Currently we are using the ALT keys only.
	 *
	 *           RIGHT_ALT_PRESSED, LEFT_ALT_PRESSED,
         *           RIGHT_CTRL_PRESSED, LEFT_CTRL_PRESSED,
         *           SHIFT_PRESSED,NUMLOCK_ON, SCROLLLOCK_ON,
         *           CAPSLOCK_ON, ENHANCED_KEY
         *
	 * are all valid bit masks to use on metaflags
         * eg. (metaflags & LEFT_CTRL_PRESSED) is true if the
         *      left control key was pressed with the keystroke.
         */
	ch = (ir.Event.KeyEvent.uChar.AsciiChar == '\r') ?
			 '\n' :ir.Event.KeyEvent.uChar.AsciiChar;
	if (metaflags & (LEFT_ALT_PRESSED|RIGHT_ALT_PRESSED))
	   ch = tolower(ch) | 0x080;
	return ch;
}

int
kbhit()
{
	int done = 0;	/* true =  "stop searching"        */
	int retval;	/* true =  "we had a match"        */
	int count;	/* scratch-pad area for API call   */

	done = 0;
	retval = 0;
	while (!done)
	{
	    count = 0;
	    PeekConsoleInput(hConIn,&ir,1,&count);
	    if (count > 0)
            {
		/* Make sure its an ascii press-down KEY_EVENT */
	        if ((ir.EventType == KEY_EVENT) &&
	        (ir.Event.KeyEvent.bKeyDown) &&	
	        (ir.Event.KeyEvent.uChar.AsciiChar))
	        { 
		     /* This is what we were looking for        */
		     done = 1;	        /* Stop looking         */
		     retval = 1;        /* Found what we sought */
	        }
		/* Discard it, its an insignificant event */
	        else ReadConsoleInput(hConIn,&ir,1,&count);
	    }
	    else  /* There are no events in console event queue*/
	    {
		done = 1;	  /* Stop looking               */
		retval = 0;	  /* No ascii press-down key    */
	    }
	}
	return retval;
}

/* called by init_tty in wintty.c for WIN32CON port only */
void
nttty_open()
{
        HANDLE hStdOut;

        /* The following 6 lines of code were suggested by 
         * Bob Landau of Microsoft WIN32 Developer support,
         * as the only current means of determining whether
         * we were launched from the command prompt, or from
         * the NT program manager. M. Allison
         */
        hStdOut = GetStdHandle( STD_OUTPUT_HANDLE );
        GetConsoleScreenBufferInfo( hStdOut, &csbi);
        ProgmanLaunched = ((csbi.dwCursorPosition.X == 0) &&
                           (csbi.dwCursorPosition.Y == 0));
        if ((csbi.dwSize.X <= 0) || (csbi.dwSize.Y <= 0))
            ProgmanLaunched = 0;


        /* Obtain handles for the standard Console I/O devices */
	hConIn = CreateFile("CONIN$",
			GENERIC_READ |GENERIC_WRITE,
			FILE_SHARE_READ |FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 0, NULL);					

	hConOut = CreateFile("CONOUT$",
			GENERIC_READ |GENERIC_WRITE,
			FILE_SHARE_READ |FILE_SHARE_WRITE,
			NULL, OPEN_EXISTING, 0, NULL);					
	get_scr_size();
}

void
get_scr_size()
{
	if (GetConsoleScreenBufferInfo(hConOut,&csbi))
	{
	    LI = csbi.dwSize.Y;
	    CO = csbi.dwSize.X;
	}
	else
	{	
		LI = 25;
		CO = 80;
	}
}


/* fatal error */
/*VARARGS1*/

void
error VA_DECL(const char *,s)
	VA_START(s);
	VA_INIT(s, const char *);
	/* error() may get called before tty is initialized */
	if (flags.window_inited) end_screen();
	putchar('\n');
	Vprintf(s,VA_ARGS);
	putchar('\n');
	VA_END();
	exit(1);
}


void
tty_startup(wid, hgt)
    int *wid, *hgt;
{
	register int i;

	*wid = CO;
	*hgt = LI;
}

void
tty_number_pad(state)
int state;
{
}

void
tty_start_screen()
{
	if (flags.num_pad) tty_number_pad(1);	/* make keypad send digits */
}

void
tty_end_screen()
{
	clear_screen();
}

void
nocmov(x, y)
int x,y;
{
	ntcoord.X = x;
	ntcoord.Y = y;
	SetConsoleCursorPosition(hConOut,ntcoord);
}

void
cmov(x, y)
register int x, y;
{
	ntcoord.X = x;
	ntcoord.Y = y;
	SetConsoleCursorPosition(hConOut,ntcoord);
	ttyDisplay->cury = y;
	ttyDisplay->curx = x;
}

void
xputc(c)
char c;
{
	int count;

	ntcoord.X = ttyDisplay->curx;
	ntcoord.Y = ttyDisplay->cury;
	WriteConsoleOutputCharacter(hConOut,&c,1,ntcoord,&count);
}

void
xputs(s)
const char *s;
{
	int count;
	
	ntcoord.X = ttyDisplay->curx;
	ntcoord.Y = ttyDisplay->cury;
	WriteConsoleOutputCharacter(hConOut,s,
			strlen(s),ntcoord,&count);
}

void
cl_end()
{
		int count;

		ntcoord.X = ttyDisplay->curx;
		ntcoord.Y = ttyDisplay->cury;
		FillConsoleOutputCharacter(hConOut,' ',
			CO - ntcoord.X,ntcoord,&count);
		tty_curs(BASE_WINDOW, (int)ttyDisplay->curx+1,
						(int)ttyDisplay->cury);
}


void
clear_screen()
{
	int count;

	ntcoord.X = 0;
	ntcoord.Y = 0;
	FillConsoleOutputCharacter(hConOut,' ',CO * LI,
		ntcoord, &count);
	home();
}


void
home()
{
	tty_curs(BASE_WINDOW, 1, 0);
	ttyDisplay->curx = ttyDisplay->cury = 0;
}


void
backsp()
{
	DWORD count;

	GetConsoleScreenBufferInfo(hConOut,&csbi);
	if (csbi.dwCursorPosition.X > 0)
		ntcoord.X = csbi.dwCursorPosition.X-1;
	ntcoord.Y = csbi.dwCursorPosition.Y;
	WriteConsoleOutputCharacter(hConOut," ",1,ntcoord,&count);
	SetConsoleCursorPosition(hConOut,ntcoord);	
}

void
tty_nhbell()
{
	if (flags.silent) return;
	Beep(8000,500);
}


void
tty_delay_output()
{
	/* delay 50 ms - not implimented */
}

void
cl_eos()
{

		register int cy = ttyDisplay->cury+1;
		while(cy <= LI-2) {
			cl_end();
			xputc('\n');
			cy++;
		}
		cl_end();
		tty_curs(BASE_WINDOW, (int)ttyDisplay->curx+1,
						(int)ttyDisplay->cury);
}


# ifdef TEXTCOLOR
/*
 * BLACK		0
 * RED			1
 * GREEN		2
 * BROWN		3	low-intensity yellow
 * BLUE			4
 * MAGENTA 		5
 * CYAN			6
 * GRAY			7	low-intensity white
 * NO_COLOR		8
 * ORANGE_COLORED	9
 * BRIGHT_GREEN		10
 * YELLOW		11
 * BRIGHT_BLUE		12
 * BRIGHT_MAGENTA  	13
 * BRIGHT_CYAN		14
 * WHITE		15
 * MAXCOLORS		16
 * BRIGHT		8
 */

static void
init_ttycolor()
{
	ttycolors[BLACK] = FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED;
	ttycolors[RED] = FOREGROUND_RED;
	ttycolors[GREEN] = FOREGROUND_GREEN;
	ttycolors[BROWN] = FOREGROUND_GREEN|FOREGROUND_RED;
	ttycolors[BLUE] = FOREGROUND_BLUE|FOREGROUND_INTENSITY;
	ttycolors[MAGENTA] = FOREGROUND_BLUE|FOREGROUND_RED;
	ttycolors[CYAN] = FOREGROUND_GREEN|FOREGROUND_BLUE;
	ttycolors[GRAY] = FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_BLUE;
	ttycolors[BRIGHT] = FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
	ttycolors[ORANGE_COLORED] = FOREGROUND_RED|FOREGROUND_INTENSITY;
	ttycolors[BRIGHT_GREEN] = FOREGROUND_GREEN|FOREGROUND_INTENSITY;
	ttycolors[YELLOW] = FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY;
	ttycolors[BRIGHT_BLUE] = FOREGROUND_BLUE|FOREGROUND_INTENSITY;
	ttycolors[BRIGHT_MAGENTA] = FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
	ttycolors[BRIGHT_CYAN] = FOREGROUND_GREEN|FOREGROUND_BLUE;
	ttycolors[WHITE] = FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_INTENSITY;
}

# endif /* TEXTCOLOR */

int
has_color(int color)
{
# ifdef TEXTCOLOR
    return 1;
# else
    return 0;
# endif 
}

void
term_end_attr(int attr)
{
	/* Mix all three colors for white on NT console */
	SetConsoleTextAttribute(hConOut,
		FOREGROUND_RED|FOREGROUND_BLUE|
		FOREGROUND_GREEN);    
}

void
term_start_attr(int attr)
{
    switch(attr){

        case ATR_ULINE:
        case ATR_BOLD:
    	        /* Mix all three colors for white on NT console */
	        SetConsoleTextAttribute(hConOut,
		    FOREGROUND_RED|FOREGROUND_BLUE|
		    FOREGROUND_GREEN|FOREGROUND_INTENSITY );
                break;
        case ATR_BLINK:
        case ATR_INVERSE:
        default:
                term_end_attr(0);
                break;
    }                
}

void
term_end_raw_bold(void)
{
    standoutend();    
}

void
term_start_raw_bold(void)
{
    standoutbeg();
}

void
term_start_color(int color)
{
# ifdef TEXTCOLOR
	WORD attr;

        if (color >= 0 && color < MAXCOLORS) {
            attr = ttycolors[color];
	    SetConsoleTextAttribute(hConOut,attr);
	}
# endif
}

void
term_end_color(void)
{
# ifdef TEXTCOLOR
	SetConsoleTextAttribute(hConOut,
		FOREGROUND_RED|FOREGROUND_BLUE|
		FOREGROUND_GREEN);
# endif       
}


void
standoutbeg()
{
	/* Mix all three colors for white on NT console */
	SetConsoleTextAttribute(hConOut,
		FOREGROUND_RED|FOREGROUND_BLUE|
		FOREGROUND_GREEN|FOREGROUND_INTENSITY );
}


void
standoutend()
{
	/* Mix all three colors for white on NT console */
	SetConsoleTextAttribute(hConOut,
		FOREGROUND_RED|FOREGROUND_BLUE|
		FOREGROUND_GREEN);
}

#endif /* WIN32CON */

