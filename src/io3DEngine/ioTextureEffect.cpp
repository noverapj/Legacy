

#include "stdafx.h"

#include "ioTextureEffect.h"
#include "ioTextureUnitState.h"

ioTextureEffect::ioTextureEffect( ioTextureUnitState *pState )
{
	m_pUnitState = pState;
	m_bStoppedUpdate = false;
}

ioTextureEffect::~ioTextureEffect()
{
}
