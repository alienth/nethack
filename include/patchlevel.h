/*
 *  Patch 1, July 31, 1989
 *  add support for Atari TOS (courtesy Eric Smith) and Andrew File System
 *	(courtesy Ralf Brown)
 *  include the uuencoded version of termcap.arc for the MSDOS versions that
 *	was included with 2.2 and 2.3
 *  make a number of simple changes to accommodate various compilers
 *  fix a handful of bugs, and do some code cleaning elsewhere
 *  add more instructions for new environments and things commonly done wrong
 */

/*
 *  Patch 2, August 16, 1989
 *  add support for OS/2 (courtesy Timo Hakulinen)
 *  add a better makefile for MicroSoft C (courtesy Paul Gyugyi)
 *  more accomodation of compilers and preprocessors
 *  add better screen-size sensing
 *  expand color use for PCs and introduce it for SVR3 UNIX machines
 *  extend '/' to multiple identifications
 *  allow meta key to be used to invoke extended commands
 *  fix various minor bugs, and do further code cleaning
 */

/*
 *  Patch 3, September 6, 1989
 *  add war hammers and revise object prices
 *  extend prototypes to ANSI compilers in addition to the previous MSDOS ones
 *  move object-on-floor references into functions in preparation for planned
 *	data structures to allow faster access and better colors
 *  fix some more bugs, and extend the portability of things added in earlier
 *	patches
 */

/*
 *  Patch 4, September 27, 1989
 *  add support for VMS (courtesy David Gentzel)
 *  move monster-on-floor references into functions and implement the new
 *	lookup structure for both objects and monsters
 *  extend the definitions of objects and monsters to provide "living color"
 *	in the dungeon, instead of a single monster color
 *  ifdef varargs usage to satisfy ANSI compilers
 *  standardize on the color 'gray'
 *  assorted bug fixes
 */

#define PATCHLEVEL	4
