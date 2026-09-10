
#include "stdafx.h"

#include "ioPet.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioPet::ioPet( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioPlayEntity( pGrp, pMode )
{	
	m_pOwner = NULL;
	m_bLive = true;

	m_pPlayStage = NULL;
	if( m_pModeParent )
		m_pPlayStage = m_pModeParent->GetCreator();	

	m_bPreForceHide = false;
	m_bForceHide = false;
	m_bPreHide = false;
	m_nPreAlphaRate = MAX_ALPHA_RATE;
}

ioPet::~ioPet()
{
	PetRankAniInfoList::iterator iter = m_PetRankAniInfoList.begin();
	for ( ; iter!=m_PetRankAniInfoList.end() ; ++iter )
		iter->second.clear();
	m_PetRankAniInfoList.clear();
	m_PetDefaultAniInfoList.clear();
}

ApplyWeaponExp ioPet::ApplyWeapon( ioWeapon *pWeapon )
{
	return AWE_NO_EXPLOSION;
}

bool ioPet::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	return false;
}

ArmorType ioPet::GetArmorType() const
{
	return AT_UNARMOR;
}

ioPlayEntity::PlaySubType ioPet::GetSubType() const
{
	return PST_PET;
}

bool ioPet::IsEntityCollisionSkipState( ioGameEntity* pColEntity ) const
{
	return true;
}

bool ioPet::IsCharCollisionSkipState( TeamType eTeamType, int iState ) const
{
	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPet::UpdateBottomHeight() const
{
	D3DXVECTOR3 vCurPos = GetWorldPosition();
	m_fCurBottomHeight = m_pModeParent->GetMapHeight( vCurPos.x, vCurPos.z, this );
}

void ioPet::Update( float fTimePerSec, DWORD dwPreTime, DWORD dwCurTime )
{
	float fSecPerTime = fTimePerSec * FLOAT1000;
	if( m_pGroup )
		m_pGroup->Update( fSecPerTime );
	
	OnProcessState();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPet::SetOwner( ioHashString szOwnerName )
{
	m_OwnerName = szOwnerName;

	if ( m_pPlayStage )
		m_pOwner = m_pPlayStage->GetBaseChar( m_OwnerName );
}

int ioPet::GetTypeID() const
{
	return m_nSubType;
}

ioBaseChar* ioPet::GetOwner()
{
	if( !m_pPlayStage )
		return NULL;

	m_pOwner = m_pPlayStage->GetBaseChar( m_OwnerName );

	return m_pOwner;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

UniqueObjID ioPet::PlaySound( ioBaseChar *pChar, ioHashString &szSoundName, bool bLoop )
{
	UniqueObjID eSoundID = 0;
	if( szSoundName.IsEmpty() )
		return eSoundID;

	ioEntityGroup *pGrp = pChar->GetGroup();
	if( bLoop )
		eSoundID = g_SoundMgr.PlaySound( szSoundName, pGrp->GetParentSceneNode(), PSM_LOOP | PSM_THREAD );
	else
		eSoundID = g_SoundMgr.PlaySound( szSoundName, pGrp->GetParentSceneNode(), PSM_THREAD );

	return eSoundID;
}

void ioPet::StopSound( ioHashString &szSoundName, UniqueObjID eSoundID )
{
	if( szSoundName.IsEmpty() )	
		return;

	g_SoundMgr.StopSound( szSoundName, eSoundID );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPet::InitPet( const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vScale, PetRankType eCurRankType )
{
	m_CurRankType = eCurRankType;

	SetWorldPosition( vPos );
	SetWorldScale( vScale );

	if ( m_pOwner )
		SetWorldOrientation( m_pOwner->GetTargetRot() );	
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

void ioPet::LoadProperty( ioINILoader &rkLoader )
{
	m_nSubType = rkLoader.LoadInt_e( "pet_sub_type", 0 );
	 
	//애니메이션 설정
	LoadDefaultAniInfo( rkLoader, m_PetDefaultAniInfoList );
	LoadStateAniInfo( rkLoader, m_PetRankAniInfoList );	
}

bool ioPet::OnStartState()
{
	return true;	
}

void ioPet::OnDeadState()
{
	SetLive( false );
}

void ioPet::SetStateAni( int nState )
{
	PetStateAniInfo sPetStateAniInfo;
	GetStateAniInfo( m_CurRankType, nState, sPetStateAniInfo );

	if( !sPetStateAniInfo.m_szAnimation.IsEmpty() )
	{
		int iAniID = m_pGroup->GetAnimationIdx( sPetStateAniInfo.m_szAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );
			m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/sPetStateAniInfo.m_fAniRate );
		}
	}
	else
	{
		PetDefaultAniInfo sPetDefaultAniInfo;
		GetDefaultAniInfo( m_CurRankType, sPetDefaultAniInfo );

		if ( sPetDefaultAniInfo.m_szAnimation.IsEmpty() )
			return;

		int iAniID = m_pGroup->GetAnimationIdx( sPetDefaultAniInfo.m_szAnimation );
		if( iAniID != -1 )
		{
			m_pGroup->ClearAllActionAni( FLOAT10, true );
			m_pGroup->ClearAllLoopAni( FLOAT10, true );
			m_pGroup->SetLoopAni( iAniID, FLOAT10, FLOAT1, FLOAT1/sPetDefaultAniInfo.m_fAniRate );
		}
	}
}

void ioPet::LoadDefaultAniInfo( ioINILoader &rkLoader, PetDefaultAniInfoList &vList )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];
	int nMaxAniInfo = rkLoader.LoadInt_e( "max_default_ani", 0 );

	m_PetDefaultAniInfoList.clear();
	vList.clear();

	for (int i=0; i<nMaxAniInfo; ++i )
	{
		wsprintf_e( szKey, "default_ani%d_rank", i+1 );
		int nPetRankType = rkLoader.LoadInt( szKey, PRT_NONE );

		PetDefaultAniInfo sPetDefaultAniInfo;
		wsprintf_e( szKey, "default_ani%d_ani_name", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sPetDefaultAniInfo.m_szAnimation = szBuf;

		wsprintf_e( szKey, "default_ani%d_ani_rate", i+1 );
		sPetDefaultAniInfo.m_fAniRate = rkLoader.LoadFloat( szKey, FLOAT1 );

		vList.insert( std::make_pair( nPetRankType, sPetDefaultAniInfo ) );
	}
}

bool ioPet::GetDefaultAniInfo( int nPetRank, PetDefaultAniInfo &sInfo )
{
	PetDefaultAniInfoList::iterator FindIter = m_PetDefaultAniInfoList.find( nPetRank );
	if ( FindIter != m_PetDefaultAniInfoList.end() )
	{
		PetDefaultAniInfo sPetDefaultAniInfo = FindIter->second;
		sInfo = sPetDefaultAniInfo;
		return true;
	}
	return false;
}

void ioPet::LoadStateAniInfo( ioINILoader &rkLoader, PetRankAniInfoList &vList )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];
	int nMaxAniInfo = rkLoader.LoadInt_e( "max_state_ani", 0 );

	m_PetRankAniInfoList.clear();
	vList.clear();
	for (int i=0; i<nMaxAniInfo; ++i )
	{
		wsprintf_e( szKey, "state_ani%d_rank", i+1 );
		int nPetRankType = rkLoader.LoadInt( szKey, PRT_NONE );

		wsprintf_e( szKey, "state_ani%d_cnt", i+1 );
		int nMaxStateInfo = rkLoader.LoadInt( szKey, 0 );
		PetStateAniInfoList vPetStateAniInfoList;

		vPetStateAniInfoList.clear();
		for ( int j=0; j<nMaxStateInfo; ++j )
		{
			PetStateAniInfo sPetStateAniInfo;
			wsprintf_e( szKey, "state_ani%d_info%d_state", i+1, j+1 );
			sPetStateAniInfo.m_nState = rkLoader.LoadInt( szKey, 0 );

			wsprintf_e( szKey, "state_ani%d_info%d_ani_name", i+1, j+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			sPetStateAniInfo.m_szAnimation = szBuf;

			wsprintf_e( szKey, "state_ani%d_info%d_ani_rate", i+1, j+1 );
			sPetStateAniInfo.m_fAniRate = rkLoader.LoadFloat( szKey, FLOAT1 );

			vPetStateAniInfoList.push_back( sPetStateAniInfo );
		}
		vList.insert( std::make_pair( nPetRankType, vPetStateAniInfoList ) );
	}
}

bool ioPet::GetStateAniInfo( int nPetRank, int nState, PetStateAniInfo &sInfo )
{
	PetRankAniInfoList::iterator FindIter = m_PetRankAniInfoList.find( nPetRank );
	if ( FindIter != m_PetRankAniInfoList.end() )
	{
		PetStateAniInfoList vPetStateAniInfoList = FindIter->second;
		int nMaxCnt = (int)vPetStateAniInfoList.size();
		for ( int i=0; i<nMaxCnt; ++i )
		{
			if ( vPetStateAniInfoList[i].m_nState == nState )
			{
				sInfo = vPetStateAniInfoList[i];
				return true;
			}
		}
		return false;
	}
	return false;
}

void ioPet::SetForceHide( bool bForceHide )
{
	m_bForceHide = bForceHide;
}

ioEffect* ioPet::AttachEffect( const ioHashString &rkEffect, ioEntityEvent *pEntityEventOwner )
{
	if( !m_pGroup ) 
		return NULL;

	if( !rkEffect.IsEmpty() )
		return m_pGroup->AttachEffect( rkEffect, pEntityEventOwner );

	return NULL;
}

void ioPet::EndEffect( const ioHashString &rkEffect, bool bGrace )
{
	if( !m_pGroup ) 
		return;

	m_pGroup->EndEffect( rkEffect, bGrace );
}