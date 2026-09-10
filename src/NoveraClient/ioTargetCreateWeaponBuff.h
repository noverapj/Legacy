#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;
class ioEffect;

class ioTargetCreateWeaponBuff : public ioBuff
{
public:
	enum TargetBuffState
	{
		TBS_None,
		TBS_FindTarget,
		TBS_CreateWeapon,
	};

protected:
	TargetBuffState m_BuffState;

	//제거할 버프
	ioHashStringVec m_RemoveBuffList;
	//총 무기 생성 횟수
	CEncrypt<int> m_nTotalWeaponCnt;
	//현재 무기 사용 횟수
	CEncrypt<int> m_nCurWeaponCnt;
	//재사용 시간
	DWORD m_dwReloadFireTime;
	DWORD m_dwCheckFireTime;
	//마크 범위
	float m_fMakerShowRange;
	//마크 아이디
	UniqueObjID m_dwReadyMakerID;
	UniqueObjID m_dwAimMakerID;
	UniqueObjID m_dwZeroMakerID;
	//마크 이름
	ioHashString m_szRedReadyMarker;
	ioHashString m_szBlueReadyMarker;
	ioHashString m_szRedAimMarker;
	ioHashString m_szBlueAimMarker;
	ioHashString m_szRedZeroMarker;
	ioHashString m_szBlueZeroMarker;
	//준비 마크 설정
	bool m_bSetReadyMarkVisible;
	//마크 시간 설정
	DWORD m_dwAimTime;
	DWORD m_dwCheckAimTime;
	DWORD m_dwZeroTime;
	DWORD m_dwCheckZeroTime;
	//마크 별 공격
	bool m_bAimFire;
	bool m_bZeroFire;
	
	int m_nAimWeapon;
	int m_nZeroWeapon;
	//공격 애니메이션
	ioHashString m_szAimAttackAni;
	float m_fAimAttackRate;
	//점프 공격 애니메이션
	ioHashString m_szJumpAttackAni;
	float m_fJumpAttackRate;
	DWORD m_dwAttackEndTime;
	//타겟 검색 설정
	float m_fTargetAngle;
	float m_fTargetMaxRange;
	float m_fTargetMinRange;

	ioHashString m_AimedTarget;
	ioHashString m_NewAimedTarget;
	ioHashString m_AttackTarget;
	bool m_bEnableDownAim;

	DWORD m_dwFireTime;
	bool m_bSetFire;
	bool m_bWound;
	bool m_bIsJumpAttack;

	CEncrypt<bool> m_bEnableWeaponSkill;
	CEncrypt<bool> m_bEnableArmorSkill;
	CEncrypt<bool> m_bEnableHelmetSkill;
	CEncrypt<bool> m_bEnableCloakSkill;

	bool m_bSetAfterJump;
	float m_fGravityAmt;
	float m_fJumpPower;
	
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
	//초기화
	void Init();
	//추가 읽기
	void LoadTargetProperty( ioINILoader &rkLoader );
	//상태 확인
	bool CheckWoundState();
	//타겟 검색
	void CheckFindTarget( ioBaseChar *pOwner );
	//공인가 확인
	bool CompareAimedTarget( ioBaseChar *pOwner );
	//키입력 확인
	void CheckInputKey( ioBaseChar *pOwner );
	//애니메이션 시간 얻기
	void SetAttackTime( ioBaseChar *pOwner, ioHashString szName, float fRate );
	//무기 생성
	void CreateWeapon( ioBaseChar *pOwner, int nWeaponIndex, ioHashString TargetName );
	//무기 선택 생성
	void CheckAttack( ioBaseChar *pOwner );
	//추가 점프
	void CheckJumpAttackState( ioBaseChar *pOwner );

protected:
	//조준점 생성 및 제거
	void CreateReadyMarker( ioBaseChar *pOwner );
	void UpdateReadyMarker( ioBaseChar *pOwner );
	void CreateAimMarker( ioBaseChar *pOwner, ioHashString TargetName );
	void UpdateAimMarker( ioBaseChar *pOwner );
	void CreateZeroMarker( ioBaseChar *pOwner, ioHashString TargetName );
	void UpdateZeroMarker( ioBaseChar *pOwner );
	void DestroyMarker( ioBaseChar *pOwner, DWORD &dwMarkID );

public:
	ioTargetCreateWeaponBuff();
	ioTargetCreateWeaponBuff( const ioTargetCreateWeaponBuff &rhs );
	virtual ~ioTargetCreateWeaponBuff();
};

inline ioTargetCreateWeaponBuff* ToTargetCreateWeaponBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_TARGET_WEAPON )
		return NULL;

	return dynamic_cast< ioTargetCreateWeaponBuff* >( pBuff );
}