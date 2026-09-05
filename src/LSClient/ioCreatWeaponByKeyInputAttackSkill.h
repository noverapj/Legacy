#pragma once
#include "ioAttackSkill.h"

struct RoomOptionInfo22
{
	CEncrypt<float> m_fValue;
	ioHashString m_Desc;
	ioHashString m_UIDesc;

	RoomOptionInfo22()
	{
		m_fValue = FLOAT1;
	}
};

typedef std::vector< RoomOptionInfo22 > RoomOptionInfoList22; 

class ioCreatWeaponByKeyInputAttackSkill :	public ioAttackSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_PRE_ACTION,
		SS_LOOP,
		SS_ACTION,
		SS_END,
	};
	
	enum KeyInputState
	{
		KIS_NONE,
		KIS_S,
		KIS_D,
		KIS_DIRECTION_D_KEY,
	};
protected:
	SkillState			m_SkillState;
	KeyInputState		m_KeyInputState;
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	BuffInfoList		m_OwnerBuffList;

	//Pre
	DWORD				m_dwPreActionFireStartTime;
	ioHashString		m_szPreActionAreaWeaponName;
	D3DXVECTOR3			m_vPreActionAreaWeaponOffset;

	//Loop
	ioHashString		m_szLoopAnimation;
	float				m_fLoopAniRate;
	DWORD				m_dwLoopStartTime;
	DWORD				m_dwLoopDuration;
	bool				m_bCharRotateByDirKey;

	//Attack
	int					m_iMaxAttackCount;
	int					m_iCurAttackCount;
	AttackAttribute		m_PreAttribute;
	AttributeList		m_DKeyAttributeList;
	AttributeList		m_SKeyAttributeList;
	AttributeList		m_DirectionKeyWithDKeyAttributeList;

	//Use Key
	bool				m_bUseDirectionKeyCheck;
	bool				m_bUseAttackKey;
	bool				m_bUseDefenseKey;

	//Effect
	ioHashString		m_szMapEffectName;
	int					m_iMaxMapEffectCount;
	DWORDVec			m_dwMapEffectIDList;
	Vector3Vec			m_vMapEffectOffsetList;

	//End
	ioHashString m_szEndAnimation;
	float		 m_fEndAniRate;

	//dummy
	vDummyCharLoadInfoList			m_DummyCharList;
	ioHashStringVec					m_vDestroyDummyCharList;
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();
public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
protected:
	virtual AttackAttribute& GetAttribute();
	virtual const AttackAttribute& GetAttributeConst() const;
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
protected:
	//Data Clear
	void ClearData();
	//INI Load
	void LoadBuffList( ioINILoader &rkLoader );
	void LoadKeyAttribute( ioINILoader &rkLoader );
	//Set function
	bool SetPreAttackState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar, bool bFirst );
	void SetEndState( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar );
	//Get Function
	const AttackAttribute& GetAttributeNormalConst() const;
	//Check Key
	bool CheckKeyInput( ioBaseChar *pChar );
	//Owner Buff
	void AddOwnerBuff( ioBaseChar *pChar );
	void RemoveOwnerBuff( ioBaseChar *pChar );
	//Create
	void CreateAreaWeapon( ioBaseChar *pChar, ioHashString szAreaWeaponName, D3DXVECTOR3 vOffset );
	void CreateSkillMapEfffect( ioBaseChar *pChar );
	//Destroy
	void DestroySkillMapEffect( ioBaseChar *pChar, DWORD& dwMapEffectID );

	//Dummy
	void CreateDummyChar( ioBaseChar *pChar );
public:
	ioCreatWeaponByKeyInputAttackSkill(void);
	ioCreatWeaponByKeyInputAttackSkill( const ioCreatWeaponByKeyInputAttackSkill &rhs );
	virtual ~ioCreatWeaponByKeyInputAttackSkill(void);
};

inline ioCreatWeaponByKeyInputAttackSkill* ToCreatWeaponByKeyInputSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CREAT_WEPON_BY_INPUT_KEY )
		return NULL;

	return dynamic_cast< ioCreatWeaponByKeyInputAttackSkill* >( pSkill );
}