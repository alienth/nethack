/*	SCCS Id: @(#)youprop.h	3.0	89/06/24
/* NetHack may be freely redistributed.  See license for details. */
/* Copyright (c) 1989 Mike Threepoint */

#ifndef YOUPROP_H
#define YOUPROP_H

#ifndef PROP_H
#include "prop.h"
#endif
#ifndef PERMONST_H
#include "permonst.h"
#endif
#ifndef MONDATA_H
#include "mondata.h"
#endif
#ifndef PM_H
#include "pm.h"
#endif

#ifndef NAMED_ITEMS
# define defends(attk,uwep)	0
#endif

/* two pseudo-properties */
#define Blindfolded	(ublindf)
#define Punished	(uball)

/* perhaps these #define's should also be generated by makedefs */
#define HFire_resistance	u.uprops[FIRE_RES].p_flgs
#ifdef POLYSELF
#define Fire_resistance	((HFire_resistance) || resists_fire(uasmon) || defends(AD_FIRE,uwep))
#else
#define Fire_resistance	((HFire_resistance) || defends(AD_FIRE,uwep))
#endif

#define HSleep_resistance	u.uprops[SLEEP_RES].p_flgs
#ifdef POLYSELF
#define Sleep_resistance	((HSleep_resistance) || resists_sleep(uasmon))
#else
#define Sleep_resistance	HSleep_resistance
#endif

#define HCold_resistance	u.uprops[COLD_RES].p_flgs
#ifdef POLYSELF
#define Cold_resistance	((HCold_resistance) || resists_cold(uasmon) || defends(AD_COLD,uwep))
#else
#define Cold_resistance	((HCold_resistance) || defends(AD_COLD,uwep))
#endif

#define HDisint_resistance	u.uprops[DISINT_RES].p_flgs
#ifdef POLYSELF
#define Disint_resistance	((HDisint_resistance) || resists_disint(uasmon))
#else
#define Disint_resistance	HDisint_resistance
#endif

#define HShock_resistance	u.uprops[SHOCK_RES].p_flgs
#ifdef POLYSELF
#define Shock_resistance	((HShock_resistance) || resists_elec(uasmon) || defends(AD_ELEC,uwep))
#else
#define Shock_resistance	((HShock_resistance) || defends(AD_ELEC,uwep))
#endif

#define HPoison_resistance	u.uprops[POISON_RES].p_flgs
#ifdef POLYSELF
#define Poison_resistance	((HPoison_resistance) || resists_poison(uasmon))
#else
#define Poison_resistance	(HPoison_resistance)
#endif

#define Adornment		u.uprops[ADORNED].p_flgs

#define HRegeneration		u.uprops[REGENERATION].p_flgs
#ifdef POLYSELF
#define Regeneration		((HRegeneration) || regenerates(uasmon))
#else
#define Regeneration		(HRegeneration)
#endif

#define Searching		u.uprops[SEARCHING].p_flgs

#define HSee_invisible		u.uprops[SEE_INVIS].p_flgs
#ifdef POLYSELF
#define See_invisible		((HSee_invisible) || perceives(uasmon))
#else
#define See_invisible		(HSee_invisible)
#endif

#define HInvis			u.uprops[INVIS].p_flgs
#ifdef POLYSELF
#define Invis			((HInvis) || u.usym == S_STALKER)
#else
#define Invis			(HInvis)
#endif
#define Invisible		(Invis && !See_invisible)

#define HTeleportation		u.uprops[TELEPORT].p_flgs
#ifdef POLYSELF
#define Teleportation		((HTeleportation) || can_teleport(uasmon))
#else
#define Teleportation		(HTeleportation)
#endif

#define HTeleport_control	u.uprops[TELEPORT_CONTROL].p_flgs
#ifdef POLYSELF
#define Teleport_control	((HTeleport_control) || control_teleport(uasmon))
#else
#define Teleport_control	(HTeleport_control)
#endif

#define Polymorph		u.uprops[POLYMORPH].p_flgs
#define Polymorph_control	u.uprops[POLYMORPH_CONTROL].p_flgs

#define HLevitation		u.uprops[LEVITATION].p_flgs
#ifdef POLYSELF
#define Levitation		((HLevitation) || is_floater(uasmon))
#else
#define Levitation		(HLevitation)
#endif

#define Stealth 		u.uprops[STEALTH].p_flgs
#define Aggravate_monster	u.uprops[AGGRAVATE_MONSTER].p_flgs
#define Conflict		u.uprops[CONFLICT].p_flgs
#define Protection		u.uprops[PROTECTION].p_flgs
#define Protection_from_shape_changers	u.uprops[PROT_FROM_SHAPE_CHANGERS].p_flgs
#define Warning 		u.uprops[WARNING].p_flgs

#define HTelepat		u.uprops[TELEPAT].p_flgs
#ifdef POLYSELF
#define Telepat 		((HTelepat) || (u.umonnum == PM_FLOATING_EYE))
#else
#define Telepat 		(HTelepat)
#endif

#define Fast			u.uprops[FAST].p_flgs

#define HStun			u.uprops[STUN].p_flgs
#ifdef POLYSELF
#define Stunned 	((HStun) || u.usym == S_BAT || u.usym == S_STALKER)
#else
#define Stunned 		(HStun)
#endif

#define HConfusion		u.uprops[CONFUSION].p_flgs
#define Confusion		(HConfusion)

#define Sick			u.uprops[SICK].p_flgs
#define Blinded 		u.uprops[BLINDED].p_flgs
#define Blind			(Blinded || Blindfolded)
#define Sleeping		u.uprops[SLEEPING].p_flgs
#define Wounded_legs		u.uprops[WOUNDED_LEGS].p_flgs
#define Stoned			u.uprops[STONED].p_flgs
#define Strangled		u.uprops[STRANGLED].p_flgs
#define Hallucination		u.uprops[HALLUC].p_flgs
#define Fumbling		u.uprops[FUMBLING].p_flgs
#define Jumping 		u.uprops[JUMPING].p_flgs
#define Wwalking		u.uprops[WWALKING].p_flgs
#define Hunger			u.uprops[HUNGER].p_flgs
#define Glib			u.uprops[GLIB].p_flgs
#define Reflecting		u.uprops[REFLECTING].p_flgs
#define Lifesaved		u.uprops[LIFESAVED].p_flgs
#define Antimagic		u.uprops[ANTIMAGIC].p_flgs
#define Displaced		u.uprops[DISPLACED].p_flgs
#define Clairvoyant		u.uprops[CLAIRVOYANT].p_flgs

#endif /* YOUPROP_H /**/
