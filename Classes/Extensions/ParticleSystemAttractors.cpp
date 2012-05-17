#include "ParticleSystemAttractors.h"

ParticleSystemAttractors * ParticleSystemAttractors::particleWithFile(const char *plistFile)
{
	ParticleSystemAttractors *pRet = new ParticleSystemAttractors();
	if (pRet && pRet->initWithFile(plistFile))
	{
		pRet->autorelease();
		return pRet;
	}
	CC_SAFE_DELETE(pRet)
	return pRet;
}


// ParticleSystem - MainLoop
void ParticleSystemAttractors::update(ccTime dt)
{
	if( m_bIsActive && m_fEmissionRate )
	{
		float rate = 1.0f / m_fEmissionRate;
		m_fEmitCounter += dt;
		while( m_uParticleCount < m_uTotalParticles && m_fEmitCounter > rate ) 
		{
			this->addParticle();
			m_fEmitCounter -= rate;
		}

		m_fElapsed += dt;
		if(m_fDuration != -1 && m_fDuration < m_fElapsed)
		{
			this->stopSystem();
		}
	}

	m_uParticleIdx = 0;


#if CC_ENABLE_PROFILERS
	/// @todo CCProfilingBeginTimingBlock(_profilingTimer);
#endif


	CCPoint currentPosition = CCPointZero;
	if( m_ePositionType == kCCPositionTypeFree )
	{
		currentPosition = this->convertToWorldSpace(CCPointZero);
        currentPosition.x *= CC_CONTENT_SCALE_FACTOR();
        currentPosition.y *= CC_CONTENT_SCALE_FACTOR();
	}
    else if ( m_ePositionType == kCCPositionTypeRelative )
    {
        currentPosition = m_tPosition;
        currentPosition.x *= CC_CONTENT_SCALE_FACTOR();
        currentPosition.y *= CC_CONTENT_SCALE_FACTOR();
    }

	while( m_uParticleIdx < m_uParticleCount )
	{
		tCCParticle *p = &m_pParticles[m_uParticleIdx];

		// life
		p->timeToLive -= dt;

		if( p->timeToLive > 0 ) 
		{
			// Mode A: gravity, direction, tangential accel & radial accel
			if( m_nEmitterMode == kCCParticleModeGravity ) 
			{
				CCPoint tmp, radial, tangential, attract;

				radial = CCPointZero;
				// radial acceleration
				if(p->pos.x || p->pos.y)
					radial = ccpNormalize(p->pos);
				tangential = radial;
				radial = ccpMult(radial, p->modeA.radialAccel);

				// tangential acceleration
				float newy = tangential.x;
				tangential.x = -tangential.y;
				tangential.y = newy;
				tangential = ccpMult(tangential, p->modeA.tangentialAccel);

				//attractors
				attract = CCPointZero;
				vector<Attractor*>::iterator it;

				for(it = attractors.begin(); it != attractors.end(); it++)
				{
					attract=ccpAdd(attract, (*it)->getForce(p->pos));
					if ((*it)->cutoffRadius>=ccpDistance(p->pos, (*it)->position))
						p->timeToLive=0;
				}

				// (gravity + radial + tangential) * dt
				//tmp = ccpAdd( ccpAdd( radial, ccpAdd(tangential, attract)), modeA.gravity);
				tmp = ccpMult( attract, dt);
				p->modeA.dir = ccpAdd( p->modeA.dir, tmp);
				tmp = ccpMult(p->modeA.dir, dt);
				p->pos = ccpAdd( p->pos, tmp );
				
			}

			// Mode B: radius movement
			else {				
				// Update the angle and radius of the particle.
				p->modeB.angle += p->modeB.degreesPerSecond * dt;
				p->modeB.radius += p->modeB.deltaRadius * dt;

				p->pos.x = - cosf(p->modeB.angle) * p->modeB.radius;
				p->pos.y = - sinf(p->modeB.angle) * p->modeB.radius;
			}

			// color
			p->color.r += (p->deltaColor.r * dt);
			p->color.g += (p->deltaColor.g * dt);
			p->color.b += (p->deltaColor.b * dt);
			p->color.a += (p->deltaColor.a * dt);

			// size
			p->size += (p->deltaSize * dt);
			p->size = MAX( 0, p->size );

			// angle
			p->rotation += (p->deltaRotation * dt);

			//
			// update values in quad
			//

			CCPoint	newPos;

			if( m_ePositionType == kCCPositionTypeFree || m_ePositionType == kCCPositionTypeRelative ) 
			{
				CCPoint diff = ccpSub( currentPosition, p->startPos );
				newPos = ccpSub(p->pos, diff);
			} 
			else
			{
				newPos = p->pos;
			}

			updateQuadWithParticle(p, newPos);
			//updateParticleImp(self, updateParticleSel, p, newPos);

			// update particle counter
			++m_uParticleIdx;

		} 
		else 
		{
			// life < 0
			if( m_uParticleIdx != m_uParticleCount-1 )
			{
				m_pParticles[m_uParticleIdx] = m_pParticles[m_uParticleCount-1];
			}
			--m_uParticleCount;

			if( m_uParticleCount == 0 && m_bIsAutoRemoveOnFinish )
			{
				this->unscheduleUpdate();
				m_pParent->removeChild(this, true);
				return;
			}
		}
	}

#if CC_ENABLE_PROFILERS
	/// @todo CCProfilingEndTimingBlock(_profilingTimer);
#endif

//#ifdef CC_USES_VBO
	this->postStep();
//#endif
}

void ParticleSystemAttractors::clearAttractors()
{
	vector<Attractor*>::iterator it;
	for(it = attractors.begin(); it != attractors.end(); it++)
		CC_SAFE_DELETE(*it);
	attractors.clear();
}