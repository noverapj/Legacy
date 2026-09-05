#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class ioEffect;

class ioMoveAttackBuff2 : public ioBuff
{
protected:
	enum BuffState
	{
		BS_Delay,
		BS_Attack,
	};

protected:
	struct SubWeaponInfo
	{
		ioHashString m_szWeaponEffect;
		float m_fWeaponAngle;
		D3DXVECTOR3 m_vWeaponPos;
		WeaponInfoList m_WeaponInfoList;
	};

	typedef std::vector<SubWeaponInfo> SubWeaponInfoList;

protected:
	BuffState m_BuffState;

	DWORD m_dwMotionEndTime;
	DWORD m_dwFireTime;
	bool m_bWound;
	std::vector<DWORD> m_SubWeaponEffectList;
	float m_fCurSubWeaponAngle;
	int m_nCurWeaponGrade;

protected:
	CEncrypt<bool> m_bEnableWeaponSkill;
	CEncrypt<bool> m_bEnableArmorSkill;
	CEncrypt<bool> m_bEnableHelmetSkill;
	CEncrypt<bool> m_bEnableCloakSkill;
	bool m_bDisableDownState;

	ioHashStringVec m_RemoveBuffList;

	float m_fStartJumpPower;
	float m_fEndJumpPower;
	
	DWORD m_dwCameraEvent;
	ioHashString m_szCameraBuff;

	IntVec m_EnableInputKeyState;
	IntVec m_BuffEndState;

	AttackAttribute m_NormalAttack;

	SubWeaponInfoList m_SubWeaponInfoList;
	DWORD m_dwSubWeaponRotateSpeed;

	int m_nMaxWeaponGrade;
	DWORD m_dwWeaponGradeTime;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

	bool CheckEnableUseSkill( int iSlot );

protected:
	void Init();
	void LoadExtraProperty( ioINILoader &rkLoader );
	bool CheckWoundState( ioBaseChar *pOwner );

protected:
	bool CheckEnableKey( ioBaseChar *pOwner );
	bool CheckBuffEndState( ioBaseChar *pOwner );
	void CheckInputKey( ioBaseChar *pOwner );
	bool CheckAttackKey( ioBaseChar *pOwner );

protected:
	void SetAttackState( ioBaseChar *pOwner );

protected:
	void CreateSubWeaponEffect( ioBaseChar *pOwner );
	void DestroySubWeaponEffect( ioBaseChar *pOwner );
	void UpdateSubWeaponEffect( ioBaseChar *pOwner );
	DWORD CreateEffect( ioBaseChar *pOwner, ioHashString& szEffect, D3DXVECTOR3& vPos, D3DXVECTOR3& vScale );
	void DestroyEffect( ioBaseChar *pOwner, DWORD& dwEffectID );

protected:
	void CreateSubWeapon( ioBaseChar *pOwner );

protected:
	void SetCameraBuff( ioBaseChar *pOwner );
	void EndCameraBuff( ioBaseChar *pOwner );
	void SetCameraEvent( ioBaseChar *pOwner );
	void EndCameraEvent( ioBaseChar *pOwner );

public:
	ioMoveAttackBuff2();
	ioMoveAttackBuff2( const ioMoveAttackBuff2 &rhs );
	virtual ~ioMoveAttackBuff2();
};

inline ioMoveAttackBuff2* ToMoveAttackBuff2( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_MOVE_ATTACK2 )
		return NULL;

	return dynamic_cast< ioMoveAttackBuff2* >( pBuff );
}