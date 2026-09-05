#pragma once

class ioWorldEventReceiver
{
public:
	virtual void OnTelepotation( const D3DXVECTOR3 &vTargetPos ) = 0;
	virtual void OnFireZoneDamage( float fDamage, const ioHashString &szBuff ) = 0;
	virtual void OnPoisonZoneDamage() = 0;

public:
	ioWorldEventReceiver(){}
	virtual ~ioWorldEventReceiver(){}
};

