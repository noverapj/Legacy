#pragma once

#include "ioWeapon.h"
#include "IORandom.h"

class ioEffect;
class ioUroborusGuideArrow;

class ioUroborusWeapon3 : public ioWeapon
{
public:
	enum UroborusState
	{
		US_MOVE,
		//US_WAIT,
		US_TARGET_MOVE,
		US_TARGET_ROTATE,
		US_TARGET_ROTATE_MOVE,
		US_TARGET_ROTATE_ATTACK,
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
	bool m_bNoFloatRangeCheck;

	float m_fCurMoveSpeed;
	float m_fCurMoveRange;

	D3DXVECTOR3 m_vTargetMovePos;
	float m_fTargetHeight;
	float m_fTargetHeightGap;
	CEncrypt<float> m_fTargetMoveSpeed;
	CEncrypt<float> m_fTargetRotateSpeed;
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
	bool m_bNoFireMeshCheck;

	//칼리때 추가
	D3DXVECTOR3		m_StartPos;
	bool			m_bUseEffectStartPosMode;
	bool			m_bUseStartReturnPos;
	bool			m_bUseRotateWeapon;
	float			m_fRotateAngle;
	float			m_fRotateAngle2;

	bool			m_bUseReturnWeapon;//드래곤 라이더AS처럼 일정시간 뒤에 돌아 온다.
	float			m_fReturnMaxLength;//해당 거리만큼 이동한 후에 다시 뒤로 돌아 온다.

	bool			m_bUseDelayStartTime;
	DWORD			m_dwDelayStartTime;
	bool m_bNoItemCheck;
	bool m_bNoAllCheck;
	bool m_bCheckLand;//지형보다 아래이면 안된다.

	bool m_bMoveAfterRotate;
	D3DXVECTOR3 m_vTargetRotateOffset;
	float m_fCurRotateAngle;

	//체인매지션모자때 추가
	//공정된 방향으로 변환
	bool			m_bFixedDir;
public:
	static ActionStopType m_ActionStopType;
	static ActionStopType m_DieActionStopType;

protected:
	bool SetFireMesh( const ioHashString &szFileName );

	void OnFloating( float fTimePerSec, ioPlayStage *pStage );
	void OnTargetMove( float fTimePerSec, ioPlayStage *pStage );
	void OnTargetRotate( float fTimePerSec, ioPlayStage *pStage );
	void OnTargetRotateMove( float fTimePerSec, ioPlayStage *pStage );
	void OnTargetRotateAttack( float fTimePerSec, ioPlayStage *pStage );

	float GetMoveSpeed();
	bool CheckOwnerState();

	void ProcessGuidArrow();
	void SetHadesTypePosition( ioPlayStage* pStage );

	void SetGuidArrowPosMode();
	bool CheckLandHeight( ioPlayStage *pStage );
	
	//void SetWaitState( ioBaseChar *pChar );
	void SetMoveAfterRotateState( bool bSendNet );

public:
	void SetTargetMoveState( bool bSendNet );
	void SetEndState( bool bSkillEnd, bool bCheckCombo, bool bSendNet );
	void SetRotateAttackState( bool bSendNet );
	void SetRotateMoveState( D3DXVECTOR3 vPos, bool bSendNet );

	void CheckTargetMoveState( ioBaseChar *pChar, bool bApplyCol );
	void SetTraceTargetName( const ioHashString &szName );

	bool IsUroborusMove();
	bool IsUroborusTargetMove();

	float GetMaxRange();
	inline float GetTargetMoveSpeed() const { return m_fTargetMoveSpeed; }

	void SetSkillWeapon( bool b ){ m_bSkillWeapon = b;}
	void SetOneTimeColCheck(bool b ){ m_bOneTimeColCheck = b;}

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

	virtual D3DXVECTOR3 GetAttackDir( const ioPlayEntity *pEntity ) const;

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

protected:
	virtual void UpdateWorldAttackBox( float fTimePerSec, bool bSetNull );

public:
	ioUroborusWeapon3( ioBaseChar *pOwner, const WeaponAttribute *pAttr );
	virtual ~ioUroborusWeapon3();
};

inline ioUroborusWeapon3* ToUroborusWeapon3( ioWeapon *pWeapon )
{
	if( !pWeapon || pWeapon->GetType() != ioWeapon::WT_UROBORUS3 )
		return NULL;

	return dynamic_cast< ioUroborusWeapon3* >( pWeapon );
}

