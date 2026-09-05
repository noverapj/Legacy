

#include "stdafx.h"

#include "ioParticleIterator.h"

ioParticleIterator::ioParticleIterator( std::list<ioParticle*>::iterator iStart,
									    std::list<ioParticle*>::iterator iEnd )
{
	m_Start = m_Pos = iStart;
	m_End = iEnd;
}

