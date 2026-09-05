
#include "stdafx.h"

#include "ioMovableObject.h"
#include "ioModelParticle.h"

#include "ioModelParticleIterator.h"

ioModelParticleIterator::ioModelParticleIterator( std::list< ioModelParticle* >::iterator iStart,
												  std::list< ioModelParticle* >::iterator iEnd )
{
	m_Start = m_Pos = iStart;
	m_End = iEnd;
}
