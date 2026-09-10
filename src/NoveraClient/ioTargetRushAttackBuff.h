#pragma once
#include "iobuff.h"
class ioTargetRushAttackBuff :
	public ioBuff
{
protected:
	enum Buff_State
	{
		BS_NONE,
		BS_SERACH,
		BS_ACTION,
		BS_MOVE_END,
		BS_SHOOT,
		BS_END
	};

	Buff_State m_BuffState;
	bool	   m_bStart;
	//Key Reserve Time
	DWORD		m_dwEnableAttackTime;
	DWORD		m_dwAttackReserveTime;
	DWORD		m_dwEnableAttackCancelTime;
	DWORD		m_dwAttackCancelReserveTime;
	
	//제거할 버프
	ioHashStringVec m_RemoveBuffList;

	//거리
	float	m_fMaxMoveDist;
	float	m_fCurMoveDist;
	float	m_fRunSpeed;
	float   m_fReduceGauge;

	//횟수
	int		m_iAttackMaxCount;
	int		m_iAttackCurCount;

	//Animation
	ioHashString m_szMoveAttackMotion;
	float m_fMoveAttackMotionRate;

	//Marker 관련 변수
	float m_fMarkerMinRange;
	float m_fMarkerMaxRange;
	float m_fMarkerAngle;
	bool  m_bEnableDownAim;
	ioHashString m_szBlueMarker;
	ioHashString m_szRedMarker;

	//Target
	BaseCharDistanceSqList m_vTargetInfoList;
	ioHashString m_szTargetName;
	D3DXVECTOR3 m_vTargetDir;
	D3DXVECTOR3 m_vAttackDir;
	bool		m_bChangeChar;

	//스킬 사용 관련 변수
	bool m_bEnableWeaponSkill;
	bool m_bEnableArmorSkill;
	bool m_bEnableHelmetSkill;
	bool m_bEnableCloakSkill;

	//캐릭터가 Wound상태인지 아닌지 체크하는 변수
	bool	m_bWound;

	//SKillEndJumpState의 jump_power
	float m_fJumpPower;

	//weapon 관련
	WeaponInfoList m_vWeaponInfoList;
	DWORD		   m_dwFireTime;
	DWORD		   m_dwFireWeaponIndex;
	DWORD		   m_dwWeaponBaseIndex;

	//Move End
	ioHashString	m_szMoveEndMotion;
	float			m_fMoveEndMotionRate;
	DWORD			m_dwMoveEndMotionEndTime;
	float			m_fMoveEndAttackMoveSpeed;
	DWORD			m_dwMoveEndAttackLiveTime;

	bool			m_bShootWeapon;
	IntVec m_EnableInputKeyState;
	IntVec m_BuffEndState;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();
	virtual void ApplyExtraBuffInfo( SP2Packet &rkPacket );

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();
public:
	bool CheckEnableUseSkill( int iSlot );
	//움직임
	void ProcessRushAttack( float fTimePerSec );
	void UseMoveEndAttack( ioBaseChar *pWounder );
	bool IsCharCollisionSkipState();
protected:
	//초기화
	void Init();
	//TargetMarker생성
	void CreateMarker( ioBaseChar *pOwner );
	
	//Target 검색 state
	void PorcessTargetFindState( ioBaseChar *pOwner );
	//s키 입력시 타겟 변환
	void ChangeTarget( ioBaseChar *pOwner );
	
	//weapon 발사
	void AttackWeaponFire( ioBaseChar *pOwner );
	void ShootWeapon( ioBaseChar *pOwner );
	void DestroyWeapon( ioBaseChar *pOwner );

	//상태 확인
	bool CheckWoundState();
	//타겟 검색
	void CheckFindTarget( ioBaseChar *pOwner );
	bool CheckTargetValidate( ioBaseChar *pOwner, ioHashString szTarget );
	//move collision체크
	bool CheckMove( ioBaseChar *pOwner );
	//weapon check
	bool CheckZoneEffectWeapon( ioBaseChar *pOwner );

	//타겟방향으로 회전
	void SetRotation( ioBaseChar *pOwner );
	//moveattack state로 전환
	void SetMoveAttackState( ioBaseChar *pOwner );
	//serach state로 전환
	void SetSerachState( ioBaseChar *pOwner );
	void SetMoveEndState( ioBaseChar *pOwner );
	bool CheckEnableKey( ioBaseChar *pOwner );
	bool CheckBuffEndState( ioBaseChar *pOwner );
	
public:
	ioTargetRushAttackBuff(void);
	ioTargetRushAttackBuff( const ioTargetRushAttackBuff &rhs );
	~ioTargetRushAttackBuff(void);
};

inline ioTargetRushAttackBuff* ToTargetRushAttackBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_TARGET_RUSH_ATTACK )
		return NULL;

	return dynamic_cast< ioTargetRushAttackBuff* >( pBuff );
}