#pragma once
#include "ioDummyChar.h"

class ioCopyOwnerDummyChar4 : public ioDummyChar
{
protected:
	int						m_iCurAttackCount;
	std::vector<int>		m_vAttackCode;

	ioHashString			m_szCheckBuffName;

	ioHashString			m_ExplosionEffect;
	ioHashString			m_ExplosionSound;

	ioAniCollisionBoxGrp	*m_pAniCollisionBoxGrp;

	bool					m_bNoDropZoneMove;

	bool					m_bDisableWeapon;

	DWORD					m_dwFirstSensingMoveDuration;
	DWORD					m_dwFirstSensingMoveTime;
	float					m_dwFirstSensingMoveSpeed;
	bool					m_bFirstSensingMove;
	float					m_fCurTraceSpeed;

	CEncrypt<bool>			m_bShowOwnerName;

public:
	void				SetCopyOwnerInfo( ioBaseChar *pOwner );

protected:
	void				SetDummyState( DummyCharState eState );
	void				SetTraceState();
	void				SetTraceAttack( ioBaseChar *pOwner );

	void				ProcessTraceState( ioBaseChar *pOwner, ioPlayStage *pStage, float fTimePerSec, DWORD dwPreTime );
	void				ProcessTraceSensing( ioBaseChar *pOwner, ioPlayStage *pStage, float fTimePerSec );
	void				ProcessTraceTracing( ioBaseChar *pOwner, ioPlayStage *pStage, float fTimePerSec );
	void				ProcessTraceAttack( ioBaseChar *pOwner, float fTimePerSec, DWORD dwPreTime );
	void				ProcessTraceMoving( ioBaseChar *pOwner, ioPlayStage *pStage, D3DXVECTOR3 vMoveDir, float fMoveAmt, bool bNoDropZoneMove );

	void				CheckTargetTraceAttack( ioBaseChar *pOwner, ioPlayStage *pStage );

	void				UpdateWorldAxisBox() const;

	ioHashString		GetSearchTarget( ioBaseChar *pOwner, ioPlayStage *pStage, bool bSecond );

public:
	virtual void		LoadProperty( ioINILoader &rkLoader );

public:
	virtual void		SetDieState( bool bEffect, bool bWoundedDie = false );

	virtual ioOrientBox	GetAttackCollisionBox( AniCollisionType eType = ACT_NONE, bool bNoScale=false );
	virtual void		UpdateBound() const;
	
	virtual void		ProcessState( float fTimePerSec, DWORD dwPreTime );
	virtual void		ProcessDelayState( ioBaseChar* pOwner, float fTimePerSec );

	virtual void		ApplyDummyCharSync( ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	virtual void		DropZoneCheck();

public:
	virtual void RenderGauge();

private:
	void RenderGaugeOwnerInfo( int iXPos, int iYPos );

public:
	ioCopyOwnerDummyChar4( ioEntityGroup *pGrp, ioPlayMode *pMode );
	~ioCopyOwnerDummyChar4(void);
};

inline ioCopyOwnerDummyChar4* ToCopyOwnerDummyChar4( ioDummyChar *pDummy )
{
	if( !pDummy || pDummy->GetType() != DCT_COPY_OWNER4 )
		return NULL;

	return dynamic_cast< ioCopyOwnerDummyChar4* >( pDummy );
}