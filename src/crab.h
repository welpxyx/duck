#ifndef CRAB_H
#define CRAB_H

#include "game001/game001.h"
#include "game001/misc.h"
#include "entity.h"

struct Crab {
	int counter;
	byte dir;
	struct Entity entity;
	byte flipped;
	byte frame[2];
};

struct CrabList {
	struct Crab crab;
	struct CrabList *next;
};

void crabUpdate(struct Crab *crab);
void crabInit(struct Crab *crab);
void crabListUpdate(struct CrabList *list);
void crabListRender(struct CrabList *list);

#endif
