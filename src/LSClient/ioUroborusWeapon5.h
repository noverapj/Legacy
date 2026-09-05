#pragma once

#include "ioWeapon.h"
#include "IORandom.h"

class ioEffect;
class ioUroborusGuideArrow;

class ioUroborusWeapon5 : public ioWeapon
{
public:
	enum UroborusState
	{
		US_MOVE,
		US_TARGET_MOVE,
		US_END,
	};

protected:
	UroborusState m_UroborusState;

	ioEffect *m_pFireMesh;
	ioUroborusGuideArrow *m_pGuideArrow;

	ioOrientBox m_LocalAttackBox;
	D3DXVECTOR3 m_vMoveDir;
	D3DXVECTOR3 m_vPrePos;

	ioHashString m_RedTeamEffect;
	ioHashString m_BlueTeamEffect;

	bool m_bFirst;

	float m_fCurMoveSpeed;
	float m_fCurMoveRange;

	D3DXVECTOR3 m_vTargetMovePos;
	CEncrypt<float> m_fTargetMoveSpeed;
	CEncrypt<float> m_fTargetMaxRange;
	CEncrypt<float> m_fTargetCurRange;

	ioHashString m_TargetMoveEffect;

	//타겟이 있다면
	bool m_bTraceTarget;
	ioHashString m_TraceTargetName;

	//스킬에서 생성한 웨폰
	bool m_bSkillWeapon;

	//충돌체를 검출하면 이후에는 안한다.
	bool m_bOneTimeColCheck;
	bool m_bColOccur;


	//칼리때 추가
	D3DXVECTOR3		m_StartPos;
	bool			m_bUseEffectStartPosMode;
	bool			m_bUseStartReturnPos;
	bool			m_bUseRotateWeapon;
	float			m_fRotateAngle;

	bool			m_bUseReturnWeapon;//드래곤 라이더AS처럼 일정시간 뒤에 돌아 온다.
	float			m_fReturnMaxLength;//해당 거리만큼 이동한 후에 다시 뒤로 돌아 온다.

	bool			m_bUseDelayStartTime;
	DWORD			m_dwDelayStartTime;

	ioHashString	m_stPassTargetName;

public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool SetFireMesh( const ioHashString &szFileName );

	void OnFloating( float fTimePerSec, ioPlayStage *pStage );
	void OnTargetMove( float fTimePerSec, ioPlayStage *pStage );

	float GetMoveSpeed();
	bool CheckOwnerState();

	void ProcessGuidArrow();

	void SetGuidArrowPosMode();

public:
	void SetEndState( bool bSkillEnd, bool bCheckCombo, bool bSendNet );

	void SetTraceTargetName( const ioHashString &szName );

	bool IsUroborusMove();
	bool IsUroborusTargetMove();

	float GetMaxRange();
	inline float GetTargetMoveSpeed() const { return m_fTargetMoveSpeed; }

	void SetSkillWeapon( bool b ){ m_bSkillWeapon = b;}
	void SetOneTimeColCheck(bool b ){ m_bOneTimeColCheck = b;}

	void SetPassTargetName( const ioHashString& stPassName );

public:
	virtual bool InitResource();
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual void SetMoveDir( const D3DXVECTOR3 &vMoveDir );
	virtual void SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage );

	virtual void Process( float fTimePerSec, ioPlayStage *pStage );
	virtual void DestroyResource( ioPlayStage *pStage=NULL );
	virtual void SetWeaponDead();

	virtual D3DXVECTOR3 GetMoveDir();

	virtual void SetTeam( ioPlayStage *pStage, TeamType eType );
	virtual void SetWeaponIndex( DWORD dwIndex );

public:
	virtual const ioOrientBox& GetLocalAttackBox() const;

	virtual WeaponType GetType() const;
	virtual ActionStopType GetActionStopType() const;
	virtual ActionStopType GetDieActionStopType() const;

	virtual bool IsCollision( ioPlayEntity *pEntity, bool bMapCollision=false );
	virtual void CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage );

	virtual DWORD GetCollisionEnableTime() const;

	virtual bool TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat );

	virtual void SetOwnerChargeScaleRate( float fRate );
	virtual void ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage );

	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioUroborusWeapon5( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioUroborusWeapon5();
};

inline ioUroborusWeapon5* ToUroborusWeapon5( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_UROBORUS5 )
		return NULL;

	return dynamic_cast< ioUroborusWeapon5* >( pWeapon );
}

