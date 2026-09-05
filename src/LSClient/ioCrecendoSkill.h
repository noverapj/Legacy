#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioCrecendoSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_DELAY,
		SS_LOOP,
		SS_END,
	};

	enum WAVEKEYSTATE
	{
		WKS_NONE  = 0,
		WKS_UP	  = 1,
		WKS_RIGHT = 2,
		WKS_DOWN  = 3,
		WKS_LEFT  = 4,
		WKS_LAST  = 5,
		WKS_MAX	  = 6,
	};

	enum SkillSync
	{
		SSC_ATTACK,
		SSC_END,
	};

protected:
	struct WaveKey
	{
		DWORD m_dwStartTime;
		DWORD m_dwEndTime;
		DWORD m_dwKey;
		DWORD m_dwRandKey;

		AttackAttribute m_WaveAttack;
		float m_fOffset;
		float m_fNeedGague;
		float m_fTargetRange;

		WaveKey()
		{
			m_dwStartTime = 0;
			m_dwEndTime   = 0;
			m_dwKey       = WKS_NONE;
			m_dwRandKey   = WKS_NONE;

			m_WaveAttack.Init();
			m_fOffset      = 0.0f;
			m_fNeedGague   = 0.0f;
			m_fTargetRange = 0.0f;
		}
	};
	typedef std::vector<WaveKey> WaveKeyVec;

	struct KeyMapEffect
	{
		UniqueObjID m_MapEffectID;
		DWORD m_dwKey;

		KeyMapEffect()
		{
			Clear();
		}

		void Clear()
		{
			m_MapEffectID = -1;
			m_dwKey = WKS_NONE;
		}
	};

	typedef std::vector<KeyMapEffect> KeyMapEffectVec;

protected:
	SkillState m_SkillState;	

	ioHashString m_SkillLoopMotion;
	float m_SkillLoopMotionRate;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopDuration;
	DWORD m_dwEndTime;
	DWORD m_dwSkillProtectTime;
	DWORD m_dwCurSkillProtectTime;

protected:
	WaveKey    m_LastSoundWaveKey;
	WaveKeyVec m_SoundWaveKeyVec;

	KeyMapEffect    m_LastKeyMapEffect;
	KeyMapEffectVec m_KeyMapEffectVec;	

	ioHashString m_szUpKeyEffect;
	ioHashString m_szDownKeyEffect;
	ioHashString m_szLeftKeyEffect;
	ioHashString m_szRightKeyEffect;
	ioHashString m_szLastKeyEffect;

	ioHashString m_szUpKeyFailedEffect;
	ioHashString m_szDownKeyFailedEffect;
	ioHashString m_szLeftKeyFailedEffect;
	ioHashString m_szRightKeyFailedEffect;
	ioHashString m_szLastKeyFailedEffect;
	
	float m_fEffectYGap;
	float m_fEffectXGap;
	float m_fEffectKeyWidth;	
	
	float m_fLastEffectYGap;

	int m_iCurSoundWayIdx;	

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

protected:
	void ClearData();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );	
	virtual bool CheckUseItemMesh();
		
protected:
	void SetLoopState( ioBaseChar *pChar );
	void SetEndState( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

protected:
	void PrcoessLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void ProcessKeyInput( ioBaseChar *pChar, ioPlayStage *pStage, DWORD dwCurTime );
	void ProcessLastKeyInput( ioBaseChar *pChar, ioPlayStage *pStage, DWORD dwCurTime );	

	void ProcessKeyEffect( ioBaseChar* pChar, ioPlayStage *pStage );

protected:
	void CreateKeyRandom( ioBaseChar *pChar, ioPlayStage *pStage );
	void CreateKeyEffectByWaveKey( ioBaseChar *pChar, ioPlayStage *pStage );
	UniqueObjID CreateKeyEffect( ioBaseChar *pChar, ioPlayStage *pStage, DWORD dwCurKey, bool bFailed=false );
	void ChangeFailedKeyEffect( ioBaseChar* pChar, ioPlayStage *pStage );

protected:
	void RemoveKeyEffect( ioBaseChar* pChar );
	void RemoveLastKeyEffect( ioBaseChar* pChar );
	void DestroyKeyEffect( ioBaseChar* pChar );

protected:
	DWORD GetKey( ioBaseChar *pChar );
	DWORD GetMixKey( ioBaseChar *pChar );

protected:
	ioBaseChar* FindTarget( ioBaseChar *pChar, ioPlayStage* pStage, float fTargetRange );
	void AttackFire( ioBaseChar* pChar, const AttackAttribute& Attack, const D3DXVECTOR3& vFirePos );

public:
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

public:
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

public:
	ioCrecendoSkill();
	ioCrecendoSkill( const ioCrecendoSkill &rhs );
	virtual ~ioCrecendoSkill();
};

inline ioCrecendoSkill* ToCrecendoSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CRECENDO )
		return NULL;

	return dynamic_cast< ioCrecendoSkill* >(pAttack);
}

