#ifndef MAIN_H
#define MAIN_H
// The precompiled header include MUST appear as the first non-commented line. See 
// arCallingConventions.h for details.
#include "arPrecompiled.h"

#define SZG_DO_NOT_EXPORT
#include <stdio.h>
#include <stdlib.h>
#include "arMasterSlaveFramework.h"
#include "arInteractableThing.h"
#include "arInteractionUtilities.h"
#include "arNavigationUtilities.h"
#include "arGlut.h"
#include "arOBJ.h"


extern double currentTimeGlobal;
// music note to be used for animation
extern arOBJRenderer musicNotey;
extern bool coneselection;
extern int selectionMode;


#endif
