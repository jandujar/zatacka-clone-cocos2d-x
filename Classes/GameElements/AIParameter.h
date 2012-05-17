#ifndef  _AI_PARAMETER_H_
#define  _AI_PARAMETER_H_

#include "cocos2d.h"


struct AIParameterSet
{
public:	
	int cowardnessFront;
	int cowardnessSides;
	int cowardnessDiags;
	int startingSearchDistance;
	int index;
	//fitness is determined by score
	int fitness;

	AIParameterSet* clone();
	static AIParameterSet* randomAIParameterSet();
	static void crossover(AIParameterSet* a1, AIParameterSet* a2);
	static void mutate(AIParameterSet* a1);


};



#endif