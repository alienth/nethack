/* Copyright (c) Stichting Mathematisch Centrum, Amsterdam, 1984. */

#ifdef MKLEV

savelev(){
	int fd;

#else
extern struct obj *billobjs;

savelev(fd){
#ifndef NOWORM
	register struct wseg *wtmp, *wtmp2;
	register tmp;
#endif NOWORM

#endif MKLEV

#ifdef MKLEV
	if((fd = creat(tfile,FMASK)) < 0) panic("Cannot create %s\n", tfile);
#else
	if(fd < 0) panic("Save on bad file!");
#endif MKLEV
	bwrite(fd,(char *) levl,sizeof(levl));
#ifdef MKLEV
	bwrite(fd,(char *) nul,sizeof(long));
#else
	bwrite(fd,(char *) &moves,sizeof(long));
#endif MKLEV
	bwrite(fd,(char *) &xupstair,sizeof(xupstair));
	bwrite(fd,(char *) &yupstair,sizeof(yupstair));
	bwrite(fd,(char *) &xdnstair,sizeof(xdnstair));
	bwrite(fd,(char *) &ydnstair,sizeof(ydnstair));
	savemonchn(fd, fmon);
	savegenchn(fd, fgold);
	savegenchn(fd, ftrap);
	saveobjchn(fd, fobj);
#ifdef MKLEV
	saveobjchn(fd, (struct obj *) 0);
	bwrite(fd,(char *) nul, sizeof(unsigned));
#else
	saveobjchn(fd, billobjs);
	billobjs = 0;
	save_engravings(fd);
#endif MKLEV
#ifndef QUEST
	bwrite(fd,(char *) rooms,sizeof(rooms));
	bwrite(fd,(char *) doors,sizeof(doors));
#endif QUEST
	fgold = ftrap = 0;
	fmon = 0;
	fobj = 0;
#ifndef MKLEV
/*--------------------------------------------------------------------*/
#ifndef NOWORM
	bwrite(fd,(char *) wsegs,sizeof(wsegs));
	for(tmp=1; tmp<32; tmp++){
		for(wtmp = wsegs[tmp]; wtmp; wtmp = wtmp2){
			wtmp2 = wtmp->nseg;
			bwrite(fd,(char *) wtmp,sizeof(struct wseg));
		}
		wsegs[tmp] = 0;
	}
	bwrite(fd,(char *) wgrowtime,sizeof(wgrowtime));
#endif NOWORM
/*--------------------------------------------------------------------*/
#endif MKLEV
}

bwrite(fd,loc,num)
register fd;
register char *loc;
register unsigned num;
{
/* lint wants the 3rd arg of write to be an int; lint -p an unsigned */
	if(write(fd, loc, (int) num) != num)
		panic("cannot write %d bytes to file #%d",num,fd);
}

saveobjchn(fd,otmp)
register fd;
register struct obj *otmp;
{
	register struct obj *otmp2;
	unsigned xl;
	int minusone = -1;

	while(otmp) {
		otmp2 = otmp->nobj;
		xl = otmp->onamelth;
		bwrite(fd, (char *) &xl, sizeof(int));
		bwrite(fd, (char *) otmp, xl + sizeof(struct obj));
		free((char *) otmp);
		otmp = otmp2;
	}
	bwrite(fd, (char *) &minusone, sizeof(int));
}

savemonchn(fd,mtmp)
register fd;
register struct monst *mtmp;
{
	register struct monst *mtmp2;
	unsigned xl;
	int minusone = -1;
	struct permonst *monbegin = &mons[0];

	bwrite(fd, (char *) &monbegin, sizeof(monbegin));

	while(mtmp) {
		mtmp2 = mtmp->nmon;
		xl = mtmp->mxlth + mtmp->mnamelth;
		bwrite(fd, (char *) &xl, sizeof(int));
		bwrite(fd, (char *) mtmp, xl + sizeof(struct monst));
		if(mtmp->minvent) saveobjchn(fd,mtmp->minvent);
		free((char *) mtmp);
		mtmp = mtmp2;
	}
	bwrite(fd, (char *) &minusone, sizeof(int));
}

savegenchn(fd,gtmp)
register fd;
register struct gen *gtmp;
{
	register struct gen *gtmp2;
	while(gtmp) {
		gtmp2 = gtmp->ngen;
		bwrite(fd, (char *) gtmp, sizeof(struct gen));
		free((char *) gtmp);
		gtmp = gtmp2;
	}
	bwrite(fd, nul, sizeof(struct gen));
}
