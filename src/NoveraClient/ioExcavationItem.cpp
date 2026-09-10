#include <stdafx.h>

#include "ioExcavationItem.h"
#include "ioExcavationManager.h"
#include "ioDummyChar.h"

ioExcavationItem::ioExcavationItem(void)
{
	m_dwAnimationEndTime = 0;
	m_eAniType           = AT_NONE;
	m_bExcavatingToolMesh= false;
	m_dwEquipTime        = 0;
	m_iRidingDummyCharIndex = 0;
	m_iOwnerExcavationLevel = 0;
}

ioExcavationItem::ioExcavationItem( const ioExcavationItem &rhs )
: ioObjectItem( rhs ),
  m_vExcavatingTool( rhs.m_vExcavatingTool )
{
	m_dwAnimationEndTime = 0;
	m_eAniType           = AT_NONE;
	m_dwEquipTime        = 0;
	m_bExcavatingToolMesh= false;
	m_iRidingDummyCharIndex = 0;
	m_iOwnerExcavationLevel = 0;
}

ioExcavationItem::~ioExcavationItem(void)
{
}

ioItem* ioExcavationItem::Clone()
{
	return new ioExcavationItem( *this );
}

ioObjectItem::ObjectSubType ioExcavationItem::GetObjectSubType() const
{
	return OST_EXCAVATION;
}

void ioExcavationItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH]="";
	char szKey[MAX_PATH] = "";
	int iCnt = rkLoader.LoadInt_e( "excavation_cnt", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		ToolInfo kInfo;

		wsprintf( szKey, "excavation%d_min_lv", i+1 );
		kInfo.min_lv = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "excavation%d_max_lv", i+1 );
		kInfo.max_lv = rkLoader.LoadInt( szKey, 0 );

		wsprintf( szKey, "excavation%d_equip_mesh", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.equip_mesh_name = szBuf;

		wsprintf( szKey, "excavation%d_tool_mesh", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.mesh_name = szBuf;

		wsprintf( szKey, "excavation%d_delay_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.ani_name[ANI_DELAY] = szBuf;

		wsprintf( szKey, "excavation%d_run_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.ani_name[ANI_RUN] = szBuf;

		wsprintf( szKey, "excavation%d_equip_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.ani_name[ANI_EQUIP] = szBuf;

		wsprintf( szKey, "excavation%d_release_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.ani_name[ANI_RELEASE] = szBuf;

		wsprintf( szKey, "excavation%d_start_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.ani_name[ANI_START] = szBuf;

		wsprintf( szKey, "excavation%d_digging_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.ani_name[ANI_DIGGING] = szBuf;

		wsprintf( szKey, "excavation%d_fail_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.ani_name[ANI_FAIL] = szBuf;

		wsprintf( szKey, "excavation%d_success_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.ani_name[ANI_SUCCESS] = szBuf;

		wsprintf( szKey, "excavation%d_jump_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.ani_name[ANI_JUMP] = szBuf;

		wsprintf( szKey, "excavation%d_jump_end_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.ani_name[ANI_JUMP_END] = szBuf;

		wsprintf( szKey, "excavation%d_riding_dummy", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.riding_dummy = szBuf;

		wsprintf( szKey, "excavation%d_change_skill", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.change_skill = szBuf;

		m_vExcavatingTool.push_back( kInfo );
	}
}

void ioExcavationItem::OnReleased( ioBaseChar *pOwner )
{
	ioObjectItem::OnReleased( pOwner );

	SetRiding( false, pOwner );
}

void ioExcavationItem::OnEquiped( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioObjectItem::OnEquiped( pOwner );
	
	if( SetActionAni( pOwner, m_CurToolInfo.ani_name[ANI_EQUIP], 1.0f ) )
		m_eAniType = AT_EQUIP;

	if( pOwner )
		pOwner->SetExcavating( true );

	m_dwEquipTime = REALGETTIME();
	m_iOwnerExcavationLevel = g_ExcavationMgr.GetUserExcavationLv(pOwner->GetCharName());
}

void ioExcavationItem::SetReleaseAni( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( SetActionAni( pOwner, m_CurToolInfo.ani_name[ANI_RELEASE], 1.0f ) )
		m_eAniType = AT_RELEASE;

	SetExcavatingToolMesh( pOwner, false );

	m_dwEquipTime = 0;
}

void ioExcavationItem::SetExcavatingStartAni( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	m_szToolMesh = GetMeshName( pOwner->GetCharName() );

	if( SetActionAni( pOwner, m_CurToolInfo.ani_name[ANI_START], 1.0f ) )
		m_eAniType = AT_EXCAVATING_STRT;
}

void ioExcavationItem::SetExcavatingAni( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	int iCnt = m_vExcavatingTool.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( SetActionAni( pOwner, m_CurToolInfo.ani_name[ANI_DIGGING], 1.0f, true ) )
			m_eAniType = AT_EXCAVATING;
	}
}

void ioExcavationItem::SetResultFailAni( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( SetActionAni( pOwner, m_CurToolInfo.ani_name[ANI_FAIL], 1.0f ) )
		m_eAniType = AT_RESULT_FAIL;
}

void ioExcavationItem::SetResultSuccessAni( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if( SetActionAni( pOwner, m_CurToolInfo.ani_name[ANI_SUCCESS], 1.0f ) )
		m_eAniType = AT_RESULT_SUCCESS;
}

void ioExcavationItem::SetResultEquipAni( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	ioHashString szPreEquipMesh = m_EquipMesh;

	if( !IsRiding() )
	{
		if( m_EquipMesh != szPreEquipMesh )
		{
			pOwner->DestroyEntity( szPreEquipMesh );
			pOwner->AddEntity( m_EquipMesh );
		}

		if( SetActionAni( pOwner, m_CurToolInfo.ani_name[ANI_EQUIP], 1.0f ) )
			m_eAniType = AT_RESULT_EQUIP;
	}
	else
	{
		m_eAniType = AT_RESULT_EQUIP;
	}
}

bool ioExcavationItem::IsAnimationEndTime( ioBaseChar *pChar )
{
	if( !pChar )
		return false;

	if( m_dwAnimationEndTime + pChar->GetActionStopTotalDelay() < FRAMEGETTIME() )
		return true;

	return false;
}

bool ioExcavationItem::SetActionAni( ioBaseChar * pOwner, ioHashString &rszAnimation, float fAnimationRate, bool bLoopAni /*= false */ )
{
	if( !pOwner )
		return false;

	if( rszAnimation.IsEmpty() )
		return false;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( rszAnimation );
	if( iAniID == -1 ) 
		return false;

	pOwner->SetState( CS_EXCAVATING );

	pGrp->ClearAllActionAni( 100.0f, true );
	pGrp->ClearAllLoopAni( 100.0f, true );
	bool bRtn = false;
	if( bLoopAni)
	{
		bRtn = pGrp->SetLoopAni( rszAnimation, 100.0f, 100.0f, 1.0f, 1.0f/fAnimationRate );
		m_dwAnimationEndTime = FRAMEGETTIME();
		m_dwAnimationEndTime += 120000 * fAnimationRate;
	}
	else
	{
		bRtn = pGrp->SetActionAni( rszAnimation, 100.0f, 100.0f, 1.0f, 1.0f/fAnimationRate );

		m_dwAnimationEndTime = FRAMEGETTIME();
		m_dwAnimationEndTime += pGrp->GetAnimationFullTime( iAniID ) * fAnimationRate;
	}
	
	return bRtn;
}

void ioExcavationItem::ClearAniInfo()
{
	m_dwAnimationEndTime = 0;
	m_eAniType = AT_NONE;
}


void ioExcavationItem::SetExcavatingToolMesh( ioBaseChar *pOwner, bool bSet )
{
	if( !pOwner )
		return;

	if( bSet )
	{
		if( !m_bExcavatingToolMesh )
		{
			m_bExcavatingToolMesh = true;
			pOwner->AddEntity( m_szToolMesh );
		}
	}
	else
	{
		if( m_bExcavatingToolMesh )
		{
			m_bExcavatingToolMesh = false;
			pOwner->DestroyEntity( m_szToolMesh );
			m_szToolMesh.Clear();
		}
	}
}	

void ioExcavationItem::SetNormalAttack( ioBaseChar *pOwner, int iCurCombo )
{

}

ioHashString ioExcavationItem::GetMeshName( const ioHashString &szName )
{
	ioHashString szReturnName;

	int iCnt = m_vExcavatingTool.size();
	for( int i=0; i<iCnt; ++i )
	{
		if( m_vExcavatingTool[i].EnableTool( g_ExcavationMgr.GetUserExcavationLv(szName) ) )
		{
			szReturnName = m_vExcavatingTool[i].mesh_name;
			break;
		}
	}

	if( !IsRiding() && szReturnName.IsEmpty() && !m_vExcavatingTool.empty() )
	{
		szReturnName = m_vExcavatingTool[0].mesh_name;
	}

	return szReturnName;
}

void ioExcavationItem::CheckMeshAndAni( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	int index = GetToolInfoIndex( g_ExcavationMgr.GetUserExcavationLv(pOwner->GetCharName()) );
	if( COMPARE( index, 0, (int)m_vExcavatingTool.size() ) )
	{
		m_CurToolInfo = m_vExcavatingTool[index];

		m_DelayAnimation         = m_CurToolInfo.ani_name[ANI_DELAY];
		m_RunAnimation           = m_CurToolInfo.ani_name[ANI_RUN];
		m_JumppingAnimation      = m_CurToolInfo.ani_name[ANI_JUMP];
		m_JumpEndAnimation       = m_CurToolInfo.ani_name[ANI_JUMP_END];
		m_ExtendJumpEndAnimation = m_CurToolInfo.ani_name[ANI_JUMP_END];
		m_EquipMesh              = m_CurToolInfo.equip_mesh_name;

		if( !m_CurToolInfo.riding_dummy.IsEmpty() )
		{
			SetRiding( true, pOwner );
		}

		if( !m_CurToolInfo.change_skill.IsEmpty() )
		{
			m_SkillNameMap.clear();
			SetSkillName( m_CurToolInfo.change_skill );
		}
	}
}

bool ioExcavationItem::CheckChangeMesh()
{
	int iCurExcavationLv = g_ExcavationMgr.GetUserExcavationLv( GetOwnerName() );
	int iPreIndex = GetToolInfoIndex( m_iOwnerExcavationLevel );
	int iCurIndex = GetToolInfoIndex( iCurExcavationLv );

	m_iOwnerExcavationLevel = iCurExcavationLv;

	return ( iPreIndex < iCurIndex );
}

int ioExcavationItem::GetToolInfoIndex( int iExcavationLv )
{
	int iCnt = m_vExcavatingTool.size();
	for( int i = 0; i < iCnt; ++i )
	{
		if( m_vExcavatingTool[i].EnableTool( iExcavationLv ) )
		{
			return i;
		}
	}
	return 0;
}

void ioExcavationItem::SetRiding( bool bRide, ioBaseChar *pOwner )
{
	if( pOwner == NULL )
	{
		return;
	}

	ioPlayStage *pStage = pOwner->GetCreator();
	if( pStage == NULL )
	{
		return;
	}

	if( bRide )
	{
		SetRiding( false, pOwner ); // 재귀함수 주의

		D3DXVECTOR3 vTargetPos = pOwner->GetWorldPosition();

		int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
		if( pStage->CreateDummyChar( m_CurToolInfo.riding_dummy, iCurIndex, pOwner->GetCharName(), vTargetPos, 0.0f, 0, true ) != NULL )
		{
			m_iRidingDummyCharIndex = iCurIndex;
		}
	}
	else
	{
		if( IsRiding() )
		{
			ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iRidingDummyCharIndex );
			if( pDummy )
				pDummy->SetDieState( true );

			m_iRidingDummyCharIndex = 0;
		}
	}
}

bool ioExcavationItem::IsRiding()
{
	return ( m_iRidingDummyCharIndex > 0 );
}
