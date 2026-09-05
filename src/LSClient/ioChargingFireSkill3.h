#pragma once

#include "ioNormalSkill.h"
#include "ioUserKeyInput.h"
#include "ioAttackAttribute.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;


class ioChargingFireSkill3 : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_ACTION,
		SS_END,
	};

	enum SkillSync
	{
		SSC_LOOP,
		SSC_ACTION,
		SSC_END,
		SSC_DIR,
	};

protected:
	typedef struct tagFireCntInfo
	{
		DWORD m_dwCheckTime;
		int m_iCreateCnt;

		tagFireCntInfo()
		{
			m_dwCheckTime = 0;
			m_iCreateCnt = 0;
		}
	} FireCntInfo;

	typedef std::vector< FireCntInfo > FireCntInfoList;

protected:
	CEncrypt<int> m_State;
	FireCntInfoList m_FireCntInfoList;
	FloatVec m_AttackAngleList;

	// Attack
	WeaponInfo m_WeaponInfo;

	DWORD m_dwFireStartTime;

	int m_iCurMaxCnt;
	int m_iCurWayCnt;
	
	// Loop
	ioHashString m_SkillLoopMotion;
	float m_fSkillLoopMotionRate;

	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopDuration;

	ioHashString m_SkillChargingEffect;
	UniqueObjID m_dwChargingEffect;

	// End
	ioHashString m_SkillEndAni;
	float m_fSkillEndAniRate;

	//
	bool m_bTargetRot;
	DWORD m_dwCharRotateSpeed;

	D3DXVECTOR3 m_vCharMoveDir;
	ioUserKeyInput::DirKeyInput m_CurDirKey;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetLoopState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage, bool bSend );
	void SetEndState( ioBaseChar *pChar );

	void CheckCharRotateKey( ioBaseChar *pChar );
	void ProcessCharRotateDir( ioBaseChar *pChar );

	int GetCurFireMaxCnt();

	void ClearData();

public:
	ioChargingFireSkill3();
	ioChargingFireSkill3( const ioChargingFireSkill3 &rhs );
	virtual ~ioChargingFireSkill3();
};

inline ioChargingFireSkill3* ToChargingFireSkill3( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_CHARGING_FIRE3 )
		return NULL;

	return dynamic_cast< ioChargingFireSkill3* >( pSkill );
}

