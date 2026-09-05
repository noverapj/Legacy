

#include "stdafx.h"

#include "ioParticleColorTable.h"
#include "ioCPU.h"

ioParticleColorTable::ioParticleColorTable()
{
}

ioParticleColorTable::~ioParticleColorTable()
{
}

ioParticleColorTable& ioParticleColorTable::operator=( const ioParticleColorTable &rhs )
{
	ioCPU::FastMemcpy( m_aColorTable, rhs.m_aColorTable, sizeof(m_aColorTable) );

	return *this;
}