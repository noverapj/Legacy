#pragma once

#include "ioNormalSkill.h"

class ioCircleMoveCreateDummySkill2 : public ioNormalSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_START,
		SS_LOOP,
		SS_ACTION,
		SS_END,
		SS_WAIT,
		SS_CREATE_DUMMY,
	};

	enum SkillSyncType
	{
		SST_DIR_CHANGE,
		SST_ACTION,
	};

protected:
	SkillState					m_SkillState;

	//마법진 설정
	float						m_fCircleMinRange;
	float						m_fCircleMaxRange;
	float						m_fCircleMoveSpeed;
	float						m_fCircleOffSet;
	D3DXVECTOR3					m_vCirclePos;

	//이펙트 설정
	ioHashString				m_ReadyCircle;
	ioHashString				m_szReadyCircleDropZone;
	ioHashString				m_szCurReadyCircle;
	ioHashString				m_RedEffect;
	ioHashString				m_BlueEffect;
	DWORD						m_dwCurCircleEffectID;

	//더미
	ioHashString				m_szDummyName;
	D3DXVECTOR3					m_vDummyOffset;
	int							m_iStartDummyAniCode;
	int							m_iLoopDummyAniCode;
	int							m_iActionDummyAniCode;
	DWORD						m_dwDummyIndex;

	DWORD						m_dwLoopStartTime;
	DWORD						m_dwLoopDuration;

	bool						m_bEnableDropZoneAttack;

	DWORD						m_dwFireStartTime;

	//Weapon
	CEncrypt<float>				m_fWeaponGravityRate;
	CEncrypt<float>				m_fWeaponFloatPower;
	CEncrypt<float>				m_fWeaponFloatMinPower;
	CEncrypt<float>				m_fWeaponMinSpeed;
	float						m_fUpLimitHeight;
	float						m_fUpLimitRange;
	float						m_fRevisionRate;

	D3DXVECTOR3					m_vWeaponDir;
	float						m_fCurWeaponMoveSpeed;
	float						m_fCurWeaponFloatPower;

	int							m_iMaxAttackCount;
	int							m_iCurAttackCount;

	WeaponInfo					m_CannonAttack;

	bool						m_bUseHide;
	float						m_fAlphaRate;

	bool						m_bUseSkillProtect;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

protected:
	void ClearData();

	void SetStartState( ioBaseChar* pChar, ioPlayStage *pStage );
	void SetLoopState( ioBaseChar* pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar* pChar );

	void CreateDummy( ioBaseChar *pChar, ioPlayStage *pStage );

	void CreateMagicCircle( ioBaseChar *pChar );
	float GetGrowthCircleRange( ioBaseChar *pChar );

	void UpdateMagicCircle( ioBaseChar *pChar, ioPlayStage *pStage );
	
	void CheckCircleByHeight( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckRotate( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckInputKey( ioBaseChar *pChar, ioPlayStage *pStage );
	
	void CheckWeaponFloatInfo( ioBaseChar *pChar );
	float GetFlightDuration( float fCurGravity, float fCurWeaponFloatPower, float fGapHeight );
	float GetTargetTime( IN float fCurGravity, IN float fGapHeight, OUT float &fFloatPower );
	void CreateCannonWeapon( ioBaseChar *pChar );

	bool CheckDummyLive( ioBaseChar* pChar, ioPlayStage* pStage );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pChar ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
public:
	ioCircleMoveCreateDummySkill2(void);
	ioCircleMoveCreateDummySkill2( const ioCircleMoveCreateDummySkill2 &rhs );
	virtual ~ioCircleMoveCreateDummySkill2(void);
};

inline ioCircleMoveCreateDummySkill2* ToCircleMoveCreateWeaponSkill2( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CIRCLE_MOVE_CREATE_DUMMY2 )
		return NULL;

	return static_cast< ioCircleMoveCreateDummySkill2* >( pSkill );
}
