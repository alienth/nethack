/*	SCCS Id: @(#)ball.c	3.1	92/11/04	*/
/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1985. */
/* NetHack may be freely redistributed.  See license for details. */

/* Ball & Chain =============================================================*/

#include "hack.h"

static void NDECL(litter);

void
ballfall()
{
	boolean gets_hit;

	gets_hit = (((uball->ox != u.ux) || (uball->oy != u.uy)) &&
		    ((uwep == uball)? FALSE : (boolean)rn2(5)));
	if (carried(uball)) {
		pline("Startled, you drop the iron ball.");
		if (uwep == uball)
			setuwep((struct obj *)0);
		if (uwep != uball)
			freeinv(uball);
	}
	if(gets_hit){
		int dmg = rn1(7,25);
		pline("The iron ball falls on your %s.",
			body_part(HEAD));
		if (uarmh)
		    if(is_metallic(uarmh)) {
			pline("Fortunately, you are wearing a hard helmet.");
			dmg = 3;
		    } else if (flags.verbose)
			Your("%s does not protect you.", xname(uarmh));
		losehp(dmg, "Crunched in the head by an iron ball",
			NO_KILLER_PREFIX);
	}
}

/*
 *  To make this work, we have to mess with the hero's mind.  The rules for
 *  ball&chain are:
 *
 *	1. If the hero can see them, fine.
 *	2. If the hero can't see either, it isn't seen.
 *	3. If either is felt it is seen.
 *	4. If either is felt and moved, it disappears.
 *
 *  If the hero can see, then when a move is done, the ball and chain are
 *  first picked up, the positions under them are corrected, then they
 *  are moved after the hero moves.  Not too bad
 *
 *  If the hero is blind, then she can "feel" the ball and/or chain at any
 *  time.  However, when the hero moves, the felt ball and/or chain become
 *  unfelt and whatever was felt "under" the ball&chain appears.  Pretty
 *  nifty, but it requires that the ball&chain "remember" what was under
 *  them --- i.e. they pick-up glyphs when they are felt and drop them when
 *  moved (and felt).  When swallowed, the ball&chain are pulled completely
 *  off of the dungeon, but are still on the object chain.  They are placed
 *  under the hero when she is expelled.
 */

/*
 *  Place the ball & chain under the hero.  Make sure that the ball & chain
 *  variables are set (actually only needed when blind, but what the heck).
 *  It is assumed that when this is called, the ball and chain are NOT
 *  attached to the object list.
 */
void
placebc()
{
    if (!uchain || !uball) {
	impossible("Where are your ball and chain?");
	return;
    }

    uchain->nobj = fobj;		/* put chain on object list */
    fobj = uchain;

    if (carried(uball))		/* the ball is carried */
	u.bc_order = 0;			/* chain & ball at different pos */
    else {			/* the ball is NOT carried */
	uball->nobj = fobj;		/* put ball on object list */
	fobj = uball;

	place_object(uball, u.ux, u.uy);
	u.bc_order = 1;			/* chain on top */
    }

    place_object(uchain, u.ux, u.uy);

    u.bglyph = u.cglyph = levl[u.ux][u.uy].glyph;   /* pick up glyph */

    newsym(u.ux,u.uy);
}

void
unplacebc()
{
    if (u.uswallow) return;	/* ball&chain not placed while swallowed */

    if (!carried(uball)) {
	freeobj(uball);
	if (Blind && (u.bc_felt & BC_BALL))		/* drop glyph */
	    levl[uball->ox][uball->oy].glyph = u.bglyph;

	newsym(uball->ox,uball->oy);
    }
    freeobj(uchain);
    if (Blind && (u.bc_felt & BC_CHAIN))		/* drop glyph */
	levl[uchain->ox][uchain->oy].glyph = u.cglyph;

    newsym(uchain->ox,uchain->oy);
    u.bc_felt = 0;					/* feel nothing */
}


/*
 *  bc_order()
 *
 *  Return the stacking of the hero's ball & chain.  This assumes that the
 *  hero is being punished.
 *
 *  Return values:
 *	0   ball & chain not at same location
 *	1   chain on top
 *	2   ball on top
 */
int
bc_order()
{
    int order;
    struct obj *obj;

    if (uchain->ox != uball->ox || uchain->oy != uball->oy || carried(uball))
	return 0;

    obj = level.objects[uchain->ox][uchain->oy];

    for (order = -1; obj; obj = obj->nexthere) {
	if (obj == uchain) {
	    order = 1;
	    break;
	}
	if (obj == uball) {
	    order = 2;
	    break;
	}
    }
    if (order < 0) {
	impossible("bc_order:  ball&chain not in same location!");
	order = 0;
    }
    return order;
}

/*
 *  set_bc()
 *
 *  The hero is either about to go blind or already blind and just punished.
 *  Set up the ball and chain variables so that the ball and chain are "felt".
 */
void
set_bc(already_blind)
int already_blind;
{
    int ball_on_floor = !carried(uball);
    u.bc_order = bc_order();				/* get the order */
    u.bc_felt = ball_on_floor ? BC_BALL|BC_CHAIN : BC_CHAIN;	/* felt */

    if (already_blind) {
	u.cglyph = u.bglyph = levl[u.ux][u.uy].glyph;
	return;
    }

    /*
     *  Since we can still see, remove the ball&chain and get the glyph that
     *  would be beneath them.  Then put the ball&chain back.  This is pretty
     *  disgusting, but it will work.
     */
    remove_object(uchain);
    if (ball_on_floor) remove_object(uball);

    newsym(uchain->ox, uchain->oy);
    u.cglyph = levl[uchain->ox][uchain->oy].glyph;

    if (u.bc_order) {			/* same location (ball not carried) */
	u.bglyph = u.cglyph;
	if (u.bc_order == 1) {
	    place_object(uball,  uball->ox, uball->oy);
	    place_object(uchain, uchain->ox, uchain->oy);
	} else {
	    place_object(uchain, uchain->ox, uchain->oy);
	    place_object(uball,  uball->ox, uball->oy);
	}
	newsym(uball->ox, uball->oy);
    } else {					/* different locations */
	place_object(uchain, uchain->ox, uchain->oy);
	newsym(uchain->ox, uchain->oy);
	if (ball_on_floor) {
	    newsym(uball->ox, uball->oy);		/* see under ball */
	    u.bglyph = levl[uball->ox][uball->oy].glyph;
	    place_object(uball,  uball->ox, uball->oy);
	    newsym(uball->ox, uball->oy);		/* restore ball */
	}
    }
}


/*
 *  move_bc()
 *
 *  Move the ball and chain.  This is called twice for every move.  The first
 *  time to pick up the ball and chain before the move, the second time to
 *  place the ball and chain after the move.  If the ball is carried, this
 *  function should never have BC_BALL as part of it's control.
 */
void
move_bc(before, control, ballx, bally, chainx, chainy)
int   before, control;
xchar ballx, bally, chainx, chainy;
{
    if (Blind) {
	/*
	 *  The hero is blind.  Time to work hard.  The ball and chain that
	 *  are attached to the hero are very special.  The hero knows that
	 *  they are attached, so when they move, the hero knows that they
	 *  aren't at the last position remembered.  This is complicated
	 *  by the fact that the hero can "feel" the surrounding locations
	 *  at any time, hence, making one or both of them show up again.
	 *  So, we have to keep track of which is felt at any one time and
	 *  act accordingly.
	 */
	if (!before) {
	    if ((control & BC_CHAIN) && (control & BC_BALL)) {
		/*
		 *  Both ball and chain moved.  If felt, drop glyph.
		 */
		if (u.bc_felt & BC_BALL)			/* ball felt */
		    levl[uball->ox][uball->oy].glyph = u.bglyph;
		if (u.bc_felt & BC_CHAIN)			/* chain felt */
		    levl[uchain->ox][uchain->oy].glyph = u.cglyph;
		u.bc_felt = 0;

		/* Pick up glyph a new location. */
		u.bglyph = levl[ballx][bally].glyph;
		u.cglyph = levl[chainx][chainy].glyph;

		movobj(uball,ballx,bally);
		movobj(uchain,chainx,chainy);
	    } else if (control & BC_BALL) {
		if (u.bc_felt & BC_BALL) {	/* ball felt */
		    if (!u.bc_order) {		    /* ball by itself */
			levl[uball->ox][uball->oy].glyph = u.bglyph;
		    } else if (u.bc_order == 2) {    /* ball on top */
			if (u.bc_felt & BC_CHAIN) {	/* chain felt */
			    map_object(uchain, 0);
			} else {
			    levl[uball->ox][uball->oy].glyph = u.bglyph;
			}
		    }
		    u.bc_felt &= ~BC_BALL;	/* no longer feel the ball */
		}

		/* Pick up glyph at new position. */
		u.bglyph = (ballx != chainx || bally != chainy) ?
					levl[ballx][bally].glyph : u.cglyph;

		movobj(uball,ballx,bally);
	    } else if (control & BC_CHAIN) {
		if (u.bc_felt & BC_CHAIN) {	/* chain felt */
		    if (!u.bc_order) {		    /* chain by itself */
			levl[uchain->ox][uchain->oy].glyph = u.cglyph;
		    } else if (u.bc_order == 1) {   /* chain on top */
			if (u.bc_felt & BC_BALL) {	/* ball felt */
			    map_object(uball, 0);
			} else {
			    levl[uchain->ox][uchain->oy].glyph = u.cglyph;
			}
		    }
		    u.bc_felt &= ~BC_CHAIN;
		}
		/* Pick up glyph beneath at new position. */
		u.cglyph = (ballx != chainx || bally != chainy) ?
					levl[chainx][chainy].glyph : u.bglyph;

		movobj(uchain,chainx,chainy);
	    }

	    u.bc_order = bc_order();	/* reset the order */
	}

    } else {
	/*
	 *  The hero is not blind.  To make this work correctly, we need to
	 *  pick up the ball and chain before the hero moves, then put them
	 *  in their new positions after the hero moves.
	 */
	if (before) {
	    /*
	     *  Neither ball nor chain moved, so remember which was on top.
	     *  Use the variable 'u.bc_order' since it is only valid when
	     *  blind.
	     */
	    if (!control) u.bc_order = bc_order();

	    remove_object(uchain);
	    newsym(uchain->ox, uchain->oy);
	    if (!carried(uball)) {
		remove_object(uball);
		newsym(uball->ox,  uball->oy);
	    }
	} else {
	    int on_floor = !carried(uball);

	    if ((control & BC_CHAIN) || (!control && u.bc_order == 1)) {
		/* If the chain moved or nothing moved & chain on top. */
		if (on_floor) place_object(uball,  ballx, bally);
		place_object(uchain, chainx, chainy);	/* chain on top */
	    } else {
		place_object(uchain, chainx, chainy);
		if (on_floor) place_object(uball,  ballx, bally);
							    /* ball on top */
	    }
	    newsym(chainx, chainy);
	    if (on_floor) newsym(ballx, bally);
	}
    }
}

/* return TRUE if ball could be dragged */
boolean
drag_ball(x, y, bc_control, ballx, bally, chainx, chainy)
xchar x, y;
int *bc_control;
xchar *ballx, *bally, *chainx, *chainy;
{
	struct trap *t = (struct trap *)0;

	*ballx  = uball->ox;
	*bally  = uball->oy;
	*chainx = uchain->ox;
	*chainy = uchain->oy;
	*bc_control = 0;

	if (dist2(x, y, uchain->ox, uchain->oy) <= 2) {
	    *bc_control = 0;	/* nothing moved */
	    move_bc(1, *bc_control, *ballx, *bally, *chainx, *chainy);
	    return TRUE;
	}

	if (carried(uball) || dist2(x, y, uball->ox, uball->oy) < 3 ||
		(uball->ox == uchain->ox && uball->oy == uchain->oy)) {
	    *chainx = u.ux;
	    *chainy = u.uy;
	    *bc_control = BC_CHAIN;
	    move_bc(1, *bc_control, *ballx, *bally, *chainx, *chainy);
	    return TRUE;
	}

	if (near_capacity() > SLT_ENCUMBER) {
	    You("cannot %sdrag the heavy iron ball.",
			    invent ? "carry all that and also " : "");
	    nomul(0);
	    return FALSE;
	}

	if ((is_pool(uchain->ox, uchain->oy) &&
			(levl[uchain->ox][uchain->oy].typ == POOL ||
			 !is_pool(uball->ox, uball->oy) || 
			 levl[uball->ox][uball->oy].typ == POOL))
	    || ((t = t_at(uchain->ox, uchain->oy)) && 
			(t->ttyp == PIT ||
			 t->ttyp == SPIKED_PIT ||
			 t->ttyp == TRAPDOOR)) ) {

	    if (Levitation) {
		You("feel a tug from your iron ball.");
		if (t) t->tseen = 1;
	    } else {
		struct monst *victim;

		You("are jerked back by your iron ball!");
		if (victim = m_at(uchain->ox, uchain->oy)) {
		    int tmp;

		    tmp = -2 + Luck + find_mac(victim);

		    if (victim->msleep) {
			victim->msleep = 0;
			tmp += 2;
		    }
		    if (!victim->mcanmove) {
			tmp += 4;
			if (!rn2(10)) {
			    victim->mcanmove = 1;
			    victim->mfrozen = 0;
			}
		    }
		    if (tmp >= rnd(20))
			(void) hmon(victim,uball,1); 
		    else 
			miss(xname(uball), victim);

		} 		/* now check again in case mon died */
		if (!m_at(uchain->ox, uchain->oy)) {
		    u.ux = uchain->ox;
		    u.uy = uchain->oy;
		    newsym(u.ux0, u.uy0);
		}
		nomul(0);

		*ballx = uchain->ox;
		*bally = uchain->oy;
		*bc_control = BC_BALL;
		move_bc(1, *bc_control, *ballx, *bally, *chainx, *chainy);
		move_bc(0, *bc_control, *ballx, *bally, *chainx, *chainy);
		spoteffects();
		return FALSE;
	    } 
	}

	*ballx  = uchain->ox;
	*bally  = uchain->oy;
	*chainx = u.ux;
	*chainy = u.uy;
	*bc_control = BC_BALL|BC_CHAIN;;
	nomul(-2);
	nomovemsg = "";

	move_bc(1, *bc_control, *ballx, *bally, *chainx, *chainy);
	return TRUE;
}

/*
 *  drop_ball()
 *
 *  The punished hero drops or throws her iron ball.  If the hero is
 *  blind, we must reset the order and glyph.  Check for side effects.
 *  This routine expects the ball to be already placed.
 */
void
drop_ball(x, y)
xchar x, y;
{
    if (Blind) {
	u.bc_order = bc_order();			/* get the order */
							/* pick up glyph */
	u.bglyph = (u.bc_order) ? u.cglyph : levl[x][y].glyph;
    }

    if (x != u.ux || y != u.uy) {
	struct trap *t;
	const char *pullmsg = "The ball pulls you out of the %s!";

	if (u.utrap && u.utraptype != TT_INFLOOR) {
	    switch(u.utraptype) {
	    case TT_PIT:
		pline(pullmsg, "pit");
		break;
	    case TT_WEB:
		pline(pullmsg, "web");
		pline("The web is destroyed!");
		deltrap(t_at(u.ux,u.uy));
		break;
	    case TT_LAVA:
		pline(pullmsg, "lava");
		break;
	    case TT_BEARTRAP: {
		register long side = rn2(3) ? LEFT_SIDE : RIGHT_SIDE;
		pline(pullmsg, "bear trap");
		Your("%s %s is severely damaged.",
					(side == LEFT_SIDE) ? "left" : "right",
					body_part(LEG));
		set_wounded_legs(side, rn1(1000, 500));
		losehp(2, "leg damage from being pulled out of a bear trap",
					KILLED_BY);
		break;
	      }
	    }
	    u.utrap = 0;
	    fill_pit(u.ux, u.uy);
	}

	u.ux0 = u.ux;
	u.uy0 = u.uy;
	if (!Levitation && !MON_AT(x, y) && !u.utrap &&
			    (is_pool(x, y) ||
			     ((t = t_at(x, y)) &&
			      ((t->ttyp == PIT) || (t->ttyp == SPIKED_PIT) ||
			       (t->ttyp == TRAPDOOR))))) {
	    u.ux = x;
	    u.uy = y;
	} else {
	    u.ux = x - u.dx;
	    u.uy = y - u.dy;
	}
	vision_full_recalc = 1;	/* hero has moved, recalculate vision later */

	if (Blind) {
	    /* drop glyph under the chain */
	    if (u.bc_felt & BC_CHAIN)
		levl[uchain->ox][uchain->oy].glyph = u.cglyph;
	    u.bc_felt  = 0;		/* feel nothing */
	    u.bc_order = bc_order();	/* reset bc order */
	}
	movobj(uchain,u.ux,u.uy);	/* has a newsym */
	newsym(u.ux0,u.uy0);		/* clean up old position */
	spoteffects();
    }
}


static void
litter()
{
	struct obj *otmp = invent, *nextobj;
	int capacity = weight_cap();

	while (otmp) {
		nextobj = otmp->nobj;
		if ((otmp != uball) && (rnd(capacity) <= otmp->owt)) {
			if (otmp == uwep)
				setuwep((struct obj *)0);
			if ((otmp != uwep) && (canletgo(otmp, ""))) {
				Your("%s you down the stairs.",
				     aobjnam(otmp, "follow"));
				dropx(otmp);
			}
		}
		otmp = nextobj;
	}
}

void
drag_down()
{
	boolean forward;
	uchar dragchance = 3;

	/* 
	 *	Assume that the ball falls forward if:
	 *
	 *	a) the character is wielding it, or
	 *	b) the character has both hands available to hold it (i.e. is 
	 *	   not wielding any weapon), or 
	 *	c) (perhaps) it falls forward out of his non-weapon hand
	 */

	forward = (!(carried(uball))? 
		  FALSE : ((uwep == uball) || (!uwep))? 
			  TRUE : (boolean)(rn2(3) / 2));

	if (carried(uball)) 
		You("lose your grip on the iron ball.");

	if (forward) {
		if(rn2(6)) {
			You("get dragged downstairs by the iron ball.");
			losehp(rnd(6), "dragged downstairs by an iron ball",
				NO_KILLER_PREFIX);
			litter();
		}
	} else {
		if(rn2(2)) {
			pline("The iron ball smacks into you!");
			losehp(rnd(20), "iron ball collision", KILLED_BY_AN);
			exercise(A_STR, FALSE);
			dragchance -= 2;
		} 
		if( (int) dragchance >= rnd(6)) {
			You("get dragged downstairs by the iron ball.");
			losehp(rnd(3), "dragged downstairs by an iron ball",
				NO_KILLER_PREFIX);
			exercise(A_STR, FALSE);
			litter();
		}
	}
}

/*ball.c*/
