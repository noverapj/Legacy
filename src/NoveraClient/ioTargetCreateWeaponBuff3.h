#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class ioEffect;

class ioTargetCreateWeaponBuff3 : public ioBuff
{
protected:
	enum BuffState
	{
		BS_FindTarget,
		BS_Attack,
		BS_End,
	};

protected:
	BuffState m_BuffState;

	bool m_bReduceGauge;
	DWORD m_dwMotionEndTime;
	int m_nCurWeaponCnt;

	UniqueObjID m_dwReadyMakerID;

	ioHashStringVec m_AimTargetList;
	ioHashStringVec m_PointTargetList;

	DWORDVec m_AimMarkerList;
	DWORDVec m_PointMarkerList;

	DWORD m_dwWeaponFireTime;

	bool m_bWound;

protected:
	CEncrypt<bool> m_bEnableWeaponSkill;
	CEncrypt<bool> m_bEnableArmorSkill;
	CEncrypt<bool> m_bEnableHelmetSkill;
	CEncrypt<bool> m_bEnableCloakSkill;
	bool m_bDisableDownState;

	ioHashStringVec m_RemoveBuffList;

	ioHashString m_szReadyMarker;
	ioHashString m_szAimMarker;
	ioHashString m_szPointMarker;

	float m_fReadyMarkerRange;

	ioHashString m_szAttackAni;
	float m_fAttackRate;

	ioHashString m_szJumpAttackAni;
	float m_fJumpAttackRate;

	int m_nAimWeapon;
	//int m_nPointWeapon;
	int m_nTotalWeaponCnt;

	//D3DXVECTOR3 m_vCreateWeaponOffset;
	//D3DXVECTOR3 m_vArriveWeaponOffset;

	ioHashString m_szWeaponRedEffect;
	ioHashString m_szWeaponBlueEffect;

	float m_fTargetAngle;
	float m_fTargetRange;
	float m_fTargetUpHeight;
	float m_fTargetUnderHeight;
	int m_nMaxTargetCnt;

	float m_fJumpPower;

	//ioHashString m_szHideBuff;
	//bool m_bSetAttackBuff;

	DWORD m_dwCameraEvent;
	ioHashString m_szCameraBuff;

	IntVec m_EnableInputKeyState;
	IntVec m_BuffEndState;

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
	void LoadTargetProperty( ioINILoader &rkLoader );
	bool CheckWoundState( ioBaseChar *pOwner );
	void CheckFindTarget( ioBaseChar *pOwner );

protected:
	void CreateMarker( ioBaseChar *pOwner, ioHashString szMarker, D3DXVECTOR3 vPos, bool bVisible, DWORD& dwMarkID );
	void DestroyMarker( ioBaseChar *pOwner, DWORD& dwMarkID );
	void CreateMarkerList( ioBaseChar *pOwner, DWORDVec& MakerList, ioHashString szMarker, int nCnt );
	void DestroyMarkerList( ioBaseChar *pOwner, DWORDVec& MakerList );
	void UpdateTargetMarker( ioBaseChar *pOwner, const DWORDVec& MakerList, const ioHashStringVec& TargetList );

protected:
	void UpdateReadyMarker( ioBaseChar *pOwner );

protected:
	bool CheckEnableKey( ioBaseChar *pOwner );
	bool CheckBuffEndState( ioBaseChar *pOwner );
	void CheckInputKey( ioBaseChar *pOwner );
	bool CheckAttackKey( ioBaseChar *pOwner );

protected:
	void SetAttackState( ioBaseChar *pOwner, ioHashString szName, float fRate );
	void CheckJumpAttackState( ioBaseChar *pOwner );

protected:
	void CreateWeapon( ioBaseChar *pOwner );

protected:
	void SetCameraBuff( ioBaseChar *pOwner );
	void EndCameraBuff( ioBaseChar *pOwner );
	void SetCameraEvent( ioBaseChar *pOwner );
	void EndCameraEvent( ioBaseChar *pOwner );

public:
	ioTargetCreateWeaponBuff3();
	ioTargetCreateWeaponBuff3( const ioTargetCreateWeaponBuff3 &rhs );
	virtual ~ioTargetCreateWeaponBuff3();
};

inline ioTargetCreateWeaponBuff3* ToTargetCreateWeaponBuff3( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_TARGET_WEAPON3 )
		return NULL;

	return dynamic_cast< ioTargetCreateWeaponBuff3* >( pBuff );
}