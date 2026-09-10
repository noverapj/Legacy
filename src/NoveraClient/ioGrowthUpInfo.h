#pragma once



class ioINILoader;
class ioPlayStage;

class ioGrowthUpInfo
{
protected:
	GrowthType m_GrowthType;
	CEncrypt<int> m_iCurLevel;

	//int m_nNormalItemMaxReinForceLv;;

	int m_nCurItemReinForceLv;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	inline void SetType( GrowthType eType ) { m_GrowthType = eType; }
	inline GrowthType GetType() const { return m_GrowthType; }

	void SetCurLevel( int iLevel );

	//void SetItemReinForceLv( int nLv );

public:
	ioGrowthUpInfo();
	virtual ~ioGrowthUpInfo();
};

//////////////////////////////////////////////////////////////////////////////////////////////////
class ioGrowthNormalUpInfo : public ioGrowthUpInfo
{
protected:
	CEncrypt<float> m_fMaxValue;

	CEncrypt<float> m_fMaxValue_Section1;
	CEncrypt<float> m_fMaxValue_Section2;
	CEncrypt<float> m_fMaxValue_Section3;

	CEncrypt<int>   m_nMaxValue_SectionGap1;
	CEncrypt<int>   m_nMaxValue_SectionGap2;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	float GetValue( ioPlayStage *pStage );

public:
	ioGrowthNormalUpInfo();
	virtual ~ioGrowthNormalUpInfo();
};

inline ioGrowthNormalUpInfo* ToNormalUpInfo( ioGrowthUpInfo *pUpInfo )
{
	if( !pUpInfo || pUpInfo->GetType() == GT_NONE )
		return NULL;

	if( pUpInfo->GetType() == GT_CHAR_ATTACK || pUpInfo->GetType() == GT_SKILL_ATTACK )
		return NULL;
	if( pUpInfo->GetType() == GT_BLOW_PUSH )
		return NULL;
	if( pUpInfo->GetType() == GT_TELEPORT_ANI_RATE )
		return NULL;
	if( pUpInfo->GetType() == GT_SKILL_ATTACK_FORCE )
		return NULL;

	return dynamic_cast< ioGrowthNormalUpInfo* >( pUpInfo );
}

//////////////////////////////////////////////////////////////////////////////////////////////////
class ioGrowthAttackUpInfo : public ioGrowthUpInfo
{
protected:
	CEncrypt<float> m_fDamageRate;
	CEncrypt<float> m_fPushRate;
	CEncrypt<float> m_fAirPushRate;
	CEncrypt<float> m_fBlowRate;
	CEncrypt<float> m_fAirBlowRate;

	CEncrypt<float> m_fDamageRate_Section1;
	CEncrypt<float> m_fDamageRate_Section2;
	CEncrypt<float> m_fDamageRate_Section3;

	CEncrypt<int> m_nDamageRate_Section1_gap;
	CEncrypt<int> m_nDamageRate_Section2_gap;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	float GetDamageRate( ioPlayStage *pStage );
	
	float GetPushRate(ioPlayStage *pStage);
	float GetAirPushRate(ioPlayStage *pStage);
	
	float GetBlowRate(ioPlayStage *pStage);
	float GetAirBlowRate(ioPlayStage *pStage);

public:
	ioGrowthAttackUpInfo();
	virtual ~ioGrowthAttackUpInfo();
};

inline ioGrowthAttackUpInfo* ToAttackUpInfo( ioGrowthUpInfo *pUpInfo )
{
	if( !pUpInfo || (pUpInfo->GetType() != GT_CHAR_ATTACK && pUpInfo->GetType() != GT_SKILL_ATTACK) )
		return NULL;

	return dynamic_cast< ioGrowthAttackUpInfo* >( pUpInfo );
}

//////////////////////////////////////////////////////////////////////////////////////////////////

class ioGrowthBlowPushUpInfo : public ioGrowthUpInfo
{
protected:
	CEncrypt<float> m_fPushRate;
	CEncrypt<float> m_fBlowRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	float GetPushRate(ioPlayStage *pStage);
	float GetBlowRate(ioPlayStage *pStage);

public:
	ioGrowthBlowPushUpInfo();
	virtual ~ioGrowthBlowPushUpInfo();
};

inline ioGrowthBlowPushUpInfo* ToBlowPushUpInfo( ioGrowthUpInfo *pUpInfo )
{
	if( !pUpInfo || pUpInfo->GetType() != GT_BLOW_PUSH )
		return NULL;

	return dynamic_cast< ioGrowthBlowPushUpInfo* >( pUpInfo );
}

//////////////////////////////////////////////////////////////////////////////////////////////////

class ioGrowthTeleportUpInfo : public ioGrowthUpInfo
{
protected:
	CEncrypt<float> m_fAniRate;
	CEncrypt<float> m_fEndAniRate;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	float GetAniRate(ioPlayStage *pStage);
	float GetEndAniRate(ioPlayStage *pStage);

public:
	ioGrowthTeleportUpInfo();
	virtual ~ioGrowthTeleportUpInfo();
};

inline ioGrowthTeleportUpInfo* ToTeleportUpInfo( ioGrowthUpInfo *pUpInfo )
{
	if( !pUpInfo || pUpInfo->GetType() != GT_TELEPORT_ANI_RATE )
		return NULL;

	return dynamic_cast< ioGrowthTeleportUpInfo* >( pUpInfo );
}

//////////////////////////////////////////////////////////////////////////////////////////////////

class ioGrowthAttackForceUpInfo : public ioGrowthUpInfo
{
protected:
	CEncrypt<float> m_fForceValue;
	CEncrypt<float> m_fFrictionValue;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	float GetForceValue(ioPlayStage *pStage);
	float GetFrictionValue(ioPlayStage *pStage);

public:
	ioGrowthAttackForceUpInfo();
	virtual ~ioGrowthAttackForceUpInfo();
};

inline ioGrowthAttackForceUpInfo* ToAttackForceUpInfo( ioGrowthUpInfo *pUpInfo )
{
	if( !pUpInfo || pUpInfo->GetType() != GT_SKILL_ATTACK_FORCE )
		return NULL;

	return dynamic_cast< ioGrowthAttackForceUpInfo* >( pUpInfo );
}

//////////////////////////////////////////////////////////////////////////////////////////////////

