#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class ioEffect;

class ioFlyJumpBuff2 : public ioBuff
{
protected:
	ioHashStringVec m_vJumpAnimationList;
	DWORD m_dwJumpEnableTime;

	bool m_bEnableRotate;
	bool m_bContinueJump;

	float m_fJumpPower;
	float m_fJumpForce;
	float m_fJumpSpeedRate;
	float m_fGravityRate;

	float m_fEnableHeight;
	float m_fAniRate;

	int m_iMaxJumpCnt;

	bool m_bJumpAttack;

	CEncrypt<bool>	m_bEnableMoveRotate;
	CEncrypt<DWORD>	m_dwMoveRotateSpeed;

	bool m_bReduceGauge;
	CEncrypt<bool>	m_bInvisibleMeshOnFlyJump;
	bool m_bHideMesh;	

	ioHashStringVec m_RemoveBuffList;
	ioHashStringVec m_CheckBuffList;
	std::vector<int> m_CheckBuffListIndex;

	DWORD m_dwActiveTime;
	DWORD m_dwCheckActiveTime;

	ioHashString m_szEffectName;
	UniqueObjID m_dwEffectID;

	float m_fEndJumpPower;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual bool IsCanJumpAttack() const;

	virtual bool OnReduceGauge() const;

	virtual bool IsCharCollisionSkip() const;

	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

	void SetSkillGauge( float fSkillGauge );
	void ChangeMesh( bool bShow );
	
public:
	ioFlyJumpBuff2();
	ioFlyJumpBuff2( const ioFlyJumpBuff2 &rhs );
	virtual ~ioFlyJumpBuff2();
};

