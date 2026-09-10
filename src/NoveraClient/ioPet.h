#pragma once

#include "ioPlayEntity.h"

class ioPlayStage;
class ioPlayMode;
class ioINILoader;
class SP2Packet;
class ioBaseChar;

class ioPet : public ioPlayEntity
{
protected:
	ioBaseChar *m_pOwner;
	ioPlayStage *m_pPlayStage;
	ioHashString m_OwnerName;
	
protected:
	int m_nSubType;
	bool m_bLive;
	PetRankType m_CurRankType;
	CharState m_PreOwnerState;

	//Æê ·©Å©º° ¾Ö´Ï¸ÞÀÌ¼Ç ¸®½ºÆ®
	PetRankAniInfoList m_PetRankAniInfoList;
	
	//Æê ·©Å©º° µðÆúÆ® ¾Ö´Ï¸ÞÀÌ¼Ç ¸®½ºÆ®
	PetDefaultAniInfoList m_PetDefaultAniInfoList;
	
	bool m_bPreForceHide;
	bool m_bForceHide;
	bool m_bPreHide;

	int m_nPreAlphaRate;

public:	
	virtual ApplyWeaponExp ApplyWeapon( ioWeapon *pWeapon );
	virtual bool IsCollisionAvailableWeapon( ioWeapon *pWeapon );
	virtual ArmorType GetArmorType() const;
	virtual PlaySubType GetSubType() const;		
	virtual bool IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const;
	virtual bool IsCharCollisionSkipState( TeamType eTeamType, int iState=0 ) const;
	virtual void UpdateBottomHeight() const;
	virtual void Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime );	

protected:
	UniqueObjID PlaySound( ioBaseChar *pChar, ioHashString &szSoundName, bool bLoop );
	void StopSound( ioHashString &szSoundName, UniqueObjID eSoundID );

public:
	void InitPet( const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale, PetRankType eCurRankType );

public:
	bool IsLive() { return m_bLive; }
	void SetLive( bool bLive) { m_bLive = bLive; }

	void SetOwner( ioHashString szOwnerName );
	inline const ioHashString& GetOwnerName() { return m_OwnerName; }
	virtual int GetTypeID() const;
	ioBaseChar* GetOwner();
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual void ApplyPetSync( ioPlayStage *pStage, SP2Packet &rkPacket ) = 0;

public:
	virtual void SetPetState( int eState ) = 0;
	virtual bool OnStartState();
	virtual void OnDeadState();

protected:
	virtual void OnProcessState() = 0;

protected:
	virtual void SetStateAni( int nState );

protected:
	void LoadDefaultAniInfo( ioINILoader &rkLoader, PetDefaultAniInfoList &vList );
	bool GetDefaultAniInfo( int nPetRank, PetDefaultAniInfo &sInfo );
	void LoadStateAniInfo( ioINILoader &rkLoader, PetRankAniInfoList &vList );
	bool GetStateAniInfo( int nPetRank, int nState, PetStateAniInfo &sInfo );

public: 
	void SetForceHide( bool bForceHide );

public:
	ioEffect* AttachEffect( const ioHashString &rkEffect, ioEntityEvent *pEntityEventOwner );
	void EndEffect( const ioHashString &rkEffect, bool bGrace );

public:
	ioPet( ioEntityGroup *pGrp, ioPlayMode *pMode );
	virtual ~ioPet();
};

inline ioPet* ToPet( ioGameEntity *pObj )
{
	ioPlayEntity *pPlay = ToPlayEntity( pObj );
	if( !pPlay || pPlay->GetSubType() != ioPlayEntity::PST_PET )
		return NULL;

	return dynamic_cast< ioPet* >( pPlay );
}