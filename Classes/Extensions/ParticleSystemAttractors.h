#ifndef __PARTICLE_SYSTEM_ATTRACTORS_H__
#define __PARTICLE_SYSTEM_ATTRACTORS_H__

#include "cocos2d.h"

USING_NS_CC;

struct Attractor
{
	//attractor position
	CCPoint position;
	//force constant of attraction (~1/r^2)
	float strength;
	//radius at whcih the attraction is cut off (to prevent singularities, etc)
	float cutoffRadius;
	//debye damping?
	float damping;
	//maximum force
	float maxForce;
	
	Attractor(CCPoint s_position, float s_strength, float s_cutoffRadius, float s_damping, float s_maxForce)
		:position(s_position), strength(s_strength), cutoffRadius(s_cutoffRadius), damping(s_damping), maxForce(s_maxForce)
	{

	}

	CCPoint getForce(const CCPoint& particlePoint)
	{
		CCPoint dr=ccpSub(particlePoint, position);
		float length=ccpLength(dr);
		//if (length<cutoffRadius)
		{
		//	return ccp(0.0f, 0.0f);
		}
		//the minus is attractive. For repulsion, make the strength -ve
		CCPoint f=ccpMult(dr, -strength/(length*length*length));
		if (maxForce>0)
		{
			float fMag2=ccpLengthSQ(f);
			if (fMag2>maxForce*maxForce)
				f=ccpMult(f, maxForce/sqrt(fMag2));
		}
		
		return f;
	}
};

class ParticleSystemAttractors :public CCParticleSystemQuad
{
	vector<Attractor*> attractors;
public:
	static ParticleSystemAttractors * particleWithFile(const char *plistFile);
	virtual void update(ccTime dt);
	void addAttractor(Attractor* attractor){attractors.push_back(attractor);}
	void clearAttractors();
};

#endif