#pragma once

#include "ioBuff.h"

class ioINILoader;
class ioBaseChar;

class ioAdhesiveBuff : public ioBuff
{

public:
	enum AttachType
	{
		ATTACH_CHAR,
		ATTACH_WEAPON,
	};

	enum PositionCheckType
	{
		PCT_NONE,
		PCT_LAND_OVER,
	};
protected:
	ioHashString m_StunAnimation;
	ioHashString m_CatchAnimation;

	D3DXVECTOR3 m_vRelativeAxis;
	D3DXVECTOR3 m_vRelativePos;
	float       m_fRelativeOffsetY;
	CEncrypt<float> m_fRelativeAngle;
	CEncrypt<float> m_fRelativeLength;

	bool m_bDisableCollsion;
	bool m_bRotateEqualCreater;
	bool m_bMustRunEndBuffCall;
	bool m_bWaitCreateWeaponTimeUse;
	DWORD m_dwWaitCreateWeaponTime;
	bool m_bWorked;
	bool m_bCallEndBuff2;

protected:
	PositionCheckType m_iPositionCheckType;

	AttachType m_iAttachType;
	float m_fWeaponRelativeLength;

	CEncrypt<bool> m_bAttachCharPos;
	CEncrypt<bool> m_bAttachCharPos2;
	CEncrypt<bool> m_bAttachCharPos3;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:	
	void SetCalledWeapon( ioWeapon* pWeapon );
	void SetRelativeVector( D3DXVECTOR3 &vVector );
	bool GetAdhesiveCollisionDisable();

	void SetCallEndBuff2( bool bCall );

protected:
	virtual void SetAction();
	virtual void ProcessDelay( float fTimePerSec );

	void ProcessPosition();

public:
	ioAdhesiveBuff();
	ioAdhesiveBuff( const ioAdhesiveBuff &rhs );
	virtual ~ioAdhesiveBuff();
};

inline ioAdhesiveBuff* ToAdhesiveBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_ADHESIVE_STATE )
		return NULL;

	return dynamic_cast< ioAdhesiveBuff* >( pBuff );
}

