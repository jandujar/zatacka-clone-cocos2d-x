#include "AIParameter.h"

#define GetByte(X,Y) (*(((unsigned char *)&X)+Y))

AIParameterSet* AIParameterSet::clone()
{
	AIParameterSet* cloneSet=new AIParameterSet();
	cloneSet->cowardnessFront=this->cowardnessFront;
	cloneSet->cowardnessSides=this->cowardnessSides;
	cloneSet->cowardnessDiags=this->cowardnessDiags;
	cloneSet->startingSearchDistance=this->startingSearchDistance;
	cloneSet->fitness=0;
	return cloneSet;
}

AIParameterSet* AIParameterSet::randomAIParameterSet()
{
	AIParameterSet* randSet=new AIParameterSet();
	randSet->cowardnessFront=(unsigned int)(CCRANDOM_0_1()*255.0f);
	randSet->cowardnessSides=(unsigned int)(CCRANDOM_0_1()*255.0f);
	randSet->cowardnessDiags=(unsigned int)(CCRANDOM_0_1()*255.0f);
	randSet->startingSearchDistance=(unsigned int)(CCRANDOM_0_1()*255.0f);
	randSet->fitness=0;
	return randSet;
}


static void crossover(AIParameterSet* a1, AIParameterSet* a2)
{
	float crossoverProb=0.7f;
	if (CCRANDOM_0_1()<crossoverProb)
	{
		int crossoverPlace=(int)(CCRANDOM_0_1()*32);
		int aiSet1 = (a1->cowardnessDiags << 24) + (a1->cowardnessFront << 16) + (a1->cowardnessSides << 8) + a1->startingSearchDistance; 
		int aiSet2 = (a2->cowardnessDiags << 24) + (a2->cowardnessFront << 16) + (a2->cowardnessSides << 8) + a2->startingSearchDistance;





	}
}
static void mutate(AIParameterSet* a1)
{

}
