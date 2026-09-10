

#include "stdafx.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"
#include "ioUroborusGuideArrow.h"

ioUroborusGuideArrow::ioUroborusGuideArrow( ioPlayStage *pCreator )
{
	m_pCreator = pCreator;
	m_bStartPosMode = false;
}

ioUroborusGuideArrow::~ioUroborusGuideArrow()
{
	DestroyArrow();
}

void ioUroborusGuideArrow::DestroyArrow()
{
	ArrowList::iterator iter = m_ArrowList.begin();
	for( ; iter!=m_ArrowList.end() ; ++iter )
	{
		ioMapEffect *pMapEffect = m_pCreator->FindMapEffect( *iter );
		if( pMapEffect )
		{
			if( m_bDestroyForce )
				pMapEffect->EndEffectForce();
			else
				pMapEffect->EndEffectGrace();
		}
	}
	m_ArrowList.clear();
}

void ioUroborusGuideArrow::Initialize( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH];

	rkLoader.LoadString_e( "guide_red_arrow", "", szBuf, MAX_PATH );
	m_GuideRedArrow = szBuf;

	rkLoader.LoadString_e( "guide_blue_arrow", "", szBuf, MAX_PATH );
	m_GuideBlueArrow = szBuf;

	m_iMaxArrowCount = rkLoader.LoadInt_e( "guide_max_arrow", 1 );
	m_fMaxArrowRange = 0.0f;

	m_bStartPosMode = false;
	m_bSetStartInvisible = rkLoader.LoadBool_e( "guide_start_invisible", false );
	m_bSetStartCharPos = rkLoader.LoadBool_e( "guide_start_char_pos", false );
}

void ioUroborusGuideArrow::SetMaxArrowRange( float fMaxRange )
{
	m_fMaxArrowRange = fMaxRange;
}

void ioUroborusGuideArrow::SetStartPosMode( bool bEnable, D3DXVECTOR3 vStartPos )
{
	m_bStartPosMode = bEnable;
	m_StartPos = vStartPos;
}

void ioUroborusGuideArrow::Process( ioBaseChar *pOwner, D3DXVECTOR3 vTargetPos )
{
	if( m_GuideRedArrow.IsEmpty() || m_GuideBlueArrow.IsEmpty() )
		return;

	if( !CheckArrowCreated( pOwner ) )
		return;

	if( m_bStartPosMode )
		ArrangeArrowsPosMode( pOwner, vTargetPos );
	else
		ArrangeArrows( pOwner, vTargetPos );
}

bool ioUroborusGuideArrow::CheckArrowCreated( ioBaseChar *pOwner )
{
	TeamType eTeam = pOwner->GetTeam();
	ioHashString szFileName;
	if( eTeam == TEAM_BLUE )
		szFileName = m_GuideBlueArrow;
	else if( eTeam == TEAM_RED )
		szFileName = m_GuideRedArrow;
	else //개인전
	{
		if( pOwner->GetSingleConvertTeam() == TEAM_BLUE )
			szFileName = m_GuideBlueArrow;
		else
			szFileName = m_GuideRedArrow;
	}

	if( szFileName.IsEmpty() )
		return false;

	if( !m_ArrowList.empty() )
	{
		if( m_GuideArrow == szFileName )
			return true;
		
		// 이전에 사용하던 Arrow  삭제
		DestroyArrow();
	}

    m_GuideArrow = szFileName;
	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );

	m_ArrowList.reserve( m_iMaxArrowCount );
	for( int i=0 ; i<m_iMaxArrowCount ; i++ )
	{
		ioMapEffect *pEffect = m_pCreator->CreateMapEffect( szFileName, vScale );
		if( pEffect )
		{
			m_ArrowList.push_back( pEffect->GetUniqueID() );
		}
	}

	SetEffectVisble( !m_bSetStartInvisible );
	
	return !m_ArrowList.empty();
}


void ioUroborusGuideArrow::ArrangeArrowsPosMode( ioBaseChar *pOwner, D3DXVECTOR3 vTargetPos )
{
	if( m_iMaxArrowCount <= 0 || m_ArrowList.empty() )
		return;

	D3DXVECTOR3 vStartPos = m_StartPos;

	ArrowList::iterator iter = m_ArrowList.begin();
	// effect link 체크
	ioMapEffect *pMapEff = GetEffect( *iter );
	if( pMapEff )
	{
		ioEffect *pEff = pMapEff->GetEffect();
		if( pEff )
		{
			ioHashString szLinkBiped;
			D3DXVECTOR3 vLinkPosOffset;
			D3DXQUATERNION qtLinkRotOffset;

			if( pEff->GetSkeletonName().IsEmpty() && pEff->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
			{
				/*ioBiped *pLinked = pOwner->GetGroup()->GetBiped( szLinkBiped );
				if( pLinked )
				{
					vLinkPosOffset = pLinked->GetDerivedOrientation() * vLinkPosOffset + pLinked->GetDerivedPosition();
					vLinkPosOffset = vLinkPosOffset * pOwner->GetWorldScale();
					vLinkPosOffset = pOwner->GetWorldOrientation() * vLinkPosOffset + pOwner->GetWorldPosition();

					vStartPos = vLinkPosOffset;
				}*/
			}
		}
	}

	D3DXVECTOR3 vDiff = vTargetPos - vStartPos;

	float fTargetLength = D3DXVec3Length( &vDiff );
	D3DXVec3Normalize( &vDiff, &vDiff );

	// Update Arrow Pos and Rotation
	D3DXVECTOR3 vCurPos = vStartPos;
	float fGapLength = min( m_fMaxArrowRange, fTargetLength ) / m_iMaxArrowCount;

	float fCurRate = FLOAT1;
	if( m_fMaxArrowRange > 0.0f )
		fCurRate = fTargetLength / m_fMaxArrowRange;

	float fCurScale = max( 0.1f, fCurRate );
	for( ; iter!=m_ArrowList.end() ; ++iter )
	{
		vCurPos += vDiff * fGapLength;

		ioMapEffect *pMapEff = GetEffect( *iter );
		if( pMapEff )
		{
			pMapEff->SetWorldPosition( vCurPos );

			ioEffect *pEff = pMapEff->GetEffect();
			if( pEff )
			{
				D3DXVECTOR3 vScale( FLOAT1, FLOAT1, fCurScale );
				pEff->GetParentNode()->SetScale( vScale );

				D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vDiff );
				pEff->GetParentNode()->SetOrientation( qtRot );
			}
		}
	}
}

void ioUroborusGuideArrow::ArrangeArrows( ioBaseChar *pOwner, D3DXVECTOR3 vTargetPos )
{
	if( m_iMaxArrowCount <= 0 || m_ArrowList.empty() )
		return;

	D3DXVECTOR3 vStartPos = pOwner->GetMidPositionByRate();
	
	ArrowList::iterator iter = m_ArrowList.begin();
	if ( !m_bSetStartCharPos )
	{
		// effect link 체크
		ioMapEffect *pMapEff = GetEffect( *iter );
		if( pMapEff )
		{
			ioEffect *pEff = pMapEff->GetEffect();
			if( pEff )
			{
				ioHashString szLinkBiped;
				D3DXVECTOR3 vLinkPosOffset;
				D3DXQUATERNION qtLinkRotOffset;

				if( pEff->GetSkeletonName().IsEmpty() && pEff->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
				{
					ioBiped *pLinked = pOwner->GetGroup()->GetBiped( szLinkBiped );
					if( pLinked )
					{
						vLinkPosOffset = pLinked->GetDerivedOrientation() * vLinkPosOffset + pLinked->GetDerivedPosition();
						vLinkPosOffset = vLinkPosOffset * pOwner->GetWorldScale();
						vLinkPosOffset = pOwner->GetWorldOrientation() * vLinkPosOffset + pOwner->GetWorldPosition();

						vStartPos = vLinkPosOffset;
					}
				}
			}
		}
	}

	D3DXVECTOR3 vDiff = vTargetPos - vStartPos;

	float fTargetLength = D3DXVec3Length( &vDiff );
	D3DXVec3Normalize( &vDiff, &vDiff );

	// Update Arrow Pos and Rotation
	D3DXVECTOR3 vCurPos = vStartPos;
	float fGapLength = min( m_fMaxArrowRange, fTargetLength ) / m_iMaxArrowCount;

	float fCurRate = FLOAT1;
	if( m_fMaxArrowRange > 0.0f )
		fCurRate = fTargetLength / m_fMaxArrowRange;

	float fCurScale = max( 0.1f, fCurRate );
	for( ; iter!=m_ArrowList.end() ; ++iter )
	{
		vCurPos += vDiff * fGapLength;

		ioMapEffect *pMapEff = GetEffect( *iter );
		if( pMapEff )
		{
			pMapEff->SetWorldPosition( vCurPos );

			ioEffect *pEff = pMapEff->GetEffect();
			if( pEff )
			{
				D3DXVECTOR3 vScale( FLOAT1, FLOAT1, fCurScale );
				pEff->GetParentNode()->SetScale( vScale );

				D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vDiff );
				pEff->GetParentNode()->SetOrientation( qtRot );
			}
		}
	}
}

ioMapEffect* ioUroborusGuideArrow::GetEffect( UniqueObjID eObjID )
{
	return m_pCreator->FindMapEffect( eObjID );
}

//이펙트 설정
void ioUroborusGuideArrow::SetEffectVisble( bool bSetVisble )
{
	if( m_iMaxArrowCount <= 0 || m_ArrowList.empty() )
		return;

	ArrowList::iterator iter = m_ArrowList.begin();
	for( ; iter!=m_ArrowList.end() ; ++iter )
	{		
		ioMapEffect *pMapEff = GetEffect( *iter );
		if( pMapEff )
		{			
			ioEffect *pEff = pMapEff->GetEffect();
			if( pEff )
			{
				pEff->SetVisible( bSetVisble );
			}
		}
	}
}

//리스트가 있는가 확인
bool ioUroborusGuideArrow::IsHasArrowList() const
{
	return !m_ArrowList.empty();
}


void ioUroborusGuideArrow::SetDestroyForceType( bool b)
{
	m_bDestroyForce = b;
}