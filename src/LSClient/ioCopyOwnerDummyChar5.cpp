
#include "stdafx.h"

#include "ioCopyOwnerDummyChar5.h"

#include "ioGaugeHelper.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

#include "ioAniCollisionBoxGrp.h"
#include "ioWoundedCollisionBoxGrp.h"
#include "ioCollisionBoxGrpManager.h"

#include "WeaponDefine.h"

#include "ioGuildMarkMgr.h"

#include "GUI/LadderTeamWnd.h"

ioCopyOwnerDummyChar5::ioCopyOwnerDummyChar5( ioEntityGroup *pGrp, ioPlayMode *pMode )
	: ioDummyChar( pGrp, pMode )
{
}

ioCopyOwnerDummyChar5::~ioCopyOwnerDummyChar5()
{
}

void ioCopyOwnerDummyChar5::LoadProperty( ioINILoader &rkLoader )
{
	ioDummyChar::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_bDisableWeaponCollision = rkLoader.LoadBool_e( "disable_weapon_collision", false );

	rkLoader.LoadString_e( "defult_delay_ani", "", szBuf, MAX_PATH );
	m_szDefaultDelayMotion = szBuf;
	m_fDefaultDelayMotionRate = rkLoader.LoadFloat_e( "defult_delay_ani_rate", FLOAT1 );
}

void ioCopyOwnerDummyChar5::SetCopyOwnerInfo( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	if ( m_szDefaultDelayMotion.IsEmpty() )
	{
		ioEquipSlot *pEquip = pOwner->GetEquipSlot();
		if( pEquip )
		{
			ioWeaponItem *pItem = pEquip->GetWeapon();
			if( pItem )
			{
				m_DelayAnimation = Help::ConvertTailName( pItem->GetDelayAnimation(), pOwner->IsMale() );

				char szFullName[MAX_PATH]="";
				StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\animation\\%s" , m_DelayAnimation.c_str() );
				if( !g_ResourceLoader.IsFileExist( szFullName ) )
				{
					m_DelayAnimation = Help::ConvertTailName( pItem->GetDelayAnimation(), true ); // 남자로
				}
			}
			else
			{
				ioEntityGroup *pGrp = pOwner->GetGroup();
				if( !pGrp )
					return;

				m_DelayAnimation = pGrp->GetAnimationName( pOwner->GetDelayAnimationIdx() );
			}
		}
	}
	else
	{
		m_DelayAnimation = m_szDefaultDelayMotion;
		m_fDelayAniRate = m_fDefaultDelayMotionRate;
	}
	

	m_HP.m_fMaxValue = pOwner->GetHP().m_fMaxValue;
	m_HP.m_fCurValue = pOwner->GetHP().m_fCurValue;

	// 팀셋팅
	TeamType eCurTeamType = pOwner->GetTeam();
	TeamType eCurSingleTeamType = pOwner->GetSingleConvertTeam();

	switch( eCurTeamType )
	{
	case TEAM_BLUE:
	case TEAM_RED:
		m_pGauge->SetTeam( eCurTeamType );
		break;
	default:
		m_pGauge->SetTeam( eCurSingleTeamType );
		break;
	}

	m_ArmorType = pOwner->GetArmorType();
	SetDelayState( false );
}


bool ioCopyOwnerDummyChar5::IsCollisionAvailableWeapon( ioWeapon *pWeapon )
{
	return ioDummyChar::IsCollisionAvailableWeapon( pWeapon );
}

void ioCopyOwnerDummyChar5::RenderGauge()
{
	if( !m_pGauge || !m_bGaugeVisible || m_bDisableHPVisible ) return;

	bool bCheckMode = g_GUIMgr.IsNoneUIMode();
	ioPlayStage *pStage = m_pModeParent->GetCreator();
	if( pStage && !bCheckMode )
	{
		if( pStage->HasCameraFixedStyle(CAMERA_STYLE_MOVIE) || pStage->HasCameraFixedStyle(CAMERA_STYLE_VIEW) )
			bCheckMode = true;
	}

	if( bCheckMode && !Setting::NoUIModeHPShow() ) return;

	D3DXVECTOR3 vPos = GetMidPositionByRate( FLOAT1 );
	vPos.y += 40.0f;

	int iXPos = 0, iYPos = 0;
	if( Setting::Check2DInScreen( vPos, iXPos, iYPos ) )
	{
		RenderGaugeOwnerInfo();
	}
}

void ioCopyOwnerDummyChar5::ProcessState( float fTimePerSec, DWORD dwPreTime )
{
	switch( m_State )
	{
	case DCS_START:
		ProcessStartState( fTimePerSec );
		break;
	case DCS_DELAY:
		ProcessDelayState( fTimePerSec );
		break;
	case DCS_DIE:
		ProcessDieState( fTimePerSec );
		break;
	}

	ProcessPos( fTimePerSec );

	if( m_pModeParent && m_pModeParent->IsZeroHP() )
	{
		if( m_HP.m_fCurValue > 0.0f )
			m_HP.m_fCurValue = 0.0f;
	}
}

void ioCopyOwnerDummyChar5::ProcessPos( float fTimePerSec )
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_State == DCS_START )
	{
		m_dwProcessTime = dwCurTime;
		return;
	}

	ProcessForceMove( fTimePerSec );

	if( m_bOwnerRotate )
	{
		ioBaseChar *pOwner = GetOwner();
		if(	pOwner )
		{
			SetWorldOrientation( pOwner->GetTargetRot() );
		}
	}

	// 아래는 높이값 처리.

	D3DXVECTOR3 vPrePos = GetWorldPosition();
	D3DXVECTOR3 vCurPos = vPrePos;
	float fMapHeight = GetBottomHeight();

	DWORD dwGapTime = dwCurTime - m_dwProcessTime;

	do 
	{
		DWORD dwCurGap = min( dwGapTime, 10 );
		if( dwGapTime >= 10 )
		{
			dwGapTime -= 10;
		}
		else
		{
			dwCurGap = dwGapTime;
			dwGapTime = 0;
		}

		float fNewTimePerSec = (float)dwCurGap / FLOAT1000;

		if( m_fCurJumpPowerAmt > 0.0f )
		{
			vCurPos.y += m_fCurJumpPowerAmt * fNewTimePerSec;
		}

		float fBoundBlowGravityRate = FLOAT1;

		if( vCurPos.y > fMapHeight )
		{
			m_fGravityAmt += (m_pModeParent->GetCreator()->GetRoomGravity() * m_fGravityRate * fBoundBlowGravityRate)* fNewTimePerSec;
			vCurPos.y -= m_fGravityAmt * fNewTimePerSec;

			if( vCurPos.y <= fMapHeight )
			{
				m_fGravityAmt = 0.0f;
				m_fCurJumpPowerAmt = 0.0f;
			}
		}
	} while( dwGapTime > 0 );

	if( vCurPos.y < fMapHeight )
	{
		vCurPos.y = fMapHeight;
		m_fGravityAmt = 0.0f;
		m_fCurJumpPowerAmt = 0.0f;
	}

	SetWorldPosition( vCurPos );

	m_dwProcessTime = dwCurTime;
}

void ioCopyOwnerDummyChar5::RenderGaugeOwnerInfo()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if( !pStage )
		return;

	int iXPos = 0;
	int iYPos = 0;

	if( m_pGauge )
	{
		D3DXVECTOR3 vPos = GetMidPositionByRate( FLOAT1 );
		vPos.y += 40.0f;

		if( Setting::Check2DInScreen( vPos, iXPos, iYPos ) )
		{
			g_FontMgr.SetTextStyle( TS_BOLD_OUTLINE_FULL );
			g_FontMgr.SetBkColor( 0, 0, 0 );

			TeamType eCurTeamType = pOwner->GetTeam();
			TeamType eCurSingleTeamType = pOwner->GetSingleConvertTeam();

			switch( eCurTeamType )
			{
			case TEAM_BLUE:
				g_FontMgr.SetTextColor( 0, 150, 255 );
				break;
			case TEAM_RED:
				g_FontMgr.SetTextColor( 255, 60, 0 );
				break;
			default: //개인전
				{
					if( eCurSingleTeamType == TEAM_BLUE )
						g_FontMgr.SetTextColor( 0, 150, 255 );
					else
						g_FontMgr.SetTextColor( 255, 60, 0 );
				}				
				break;
			}

			enum { UPPER_Y_OFFSET = -12, GUILD_Y_OFFSET = -11, LADDER_TITLE_Y_OFFSET = -16, };
			if( pStage->GetModeType() == MT_MYROOM && !pStage->IsNetworkMode() )
			{
				if( pOwner->IsOwnerChar() )
				{
					g_FontMgr.SetAlignType( TAT_LEFT );

					DWORD dwGuildIndex, dwGuildMark;
					g_UserInfoMgr.GetGuildInfo( g_MyInfo.GetPublicID(), dwGuildIndex, dwGuildMark );
					int iNameWidth = g_FontMgr.GetTextWidth( g_MyInfo.GetPublicID().c_str(), TS_BOLD_OUTLINE_FULL, FONT_SIZE_12 );
					int iHalfWidth = ( iNameWidth + 20 + g_GuildMarkMgr.GetGuildSmallMarkSize( dwGuildIndex, dwGuildMark ) + 1 ) / 2;	// 20 은 계급사이즈

					g_LevelMgr.RenderGrade( g_MyInfo.GetPublicID(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, pOwner->GetLevel(), TEAM_BLUE );
					g_FontMgr.PrintText( iXPos - iHalfWidth + 20 - 2, iYPos + UPPER_Y_OFFSET, FONT_SIZE_12, g_MyInfo.GetPublicID().c_str() );

					g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, ( iXPos - iHalfWidth ) + 20 + iNameWidth + 2, iYPos + GUILD_Y_OFFSET );

					// 래더전 타이틀
					if( g_LadderTeamMgr.IsLadderTeam() && g_LadderTeamMgr.IsHeroMatchMode() && pOwner->GetHeroTitle() != 0 )
					{
						g_FontMgr.SetAlignType( TAT_CENTER );
						g_FontMgr.PrintText( iXPos, (iYPos + UPPER_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, "[%s]", g_LevelMgr.GetHeroTitleString( pOwner->GetHeroTitle() ).c_str() );
					}
				}
			}
			else
			{
				g_FontMgr.SetAlignType( TAT_LEFT );

				char szCharName[MAX_PATH] = "";
				sprintf( szCharName, "%s", pOwner->GetCharName().c_str() );
				DWORD dwGuildIndex, dwGuildMark;
				g_UserInfoMgr.GetGuildInfo( pOwner->GetCharName(), dwGuildIndex, dwGuildMark );
				int iNameWidth = g_FontMgr.GetTextWidth( szCharName, TS_BOLD_OUTLINE_FULL, FONT_SIZE_12 );
				int iHalfWidth = ( iNameWidth + 20+ g_GuildMarkMgr.GetGuildSmallMarkSize( dwGuildIndex, dwGuildMark ) + 1 ) / 2;	// 20 은 계급사이즈

				switch( eCurTeamType )
				{
				case TEAM_BLUE:
					g_LevelMgr.RenderGrade( pOwner->GetCharName(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, pOwner->GetLevel(), TEAM_BLUE );
					break;
				case TEAM_RED:
					g_LevelMgr.RenderGrade( pOwner->GetCharName(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, pOwner->GetLevel(), TEAM_RED );
					break;
				default: //개인전
					if( eCurSingleTeamType == TEAM_BLUE )
						g_LevelMgr.RenderGrade( pOwner->GetCharName(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, pOwner->GetLevel(), TEAM_BLUE );
					else
						g_LevelMgr.RenderGrade( pOwner->GetCharName(), iXPos - iHalfWidth, iYPos + UPPER_Y_OFFSET, pOwner->GetLevel(), TEAM_RED );
					break;
				}

				g_FontMgr.PrintText( iXPos - iHalfWidth + 20 - 2, iYPos + UPPER_Y_OFFSET, FONT_SIZE_12, szCharName );
				g_GuildMarkMgr.RenderSmallMark( dwGuildIndex, dwGuildMark, ( iXPos - iHalfWidth ) + 20 + iNameWidth + 2, iYPos + GUILD_Y_OFFSET );

				// 래더전 타이틀
				if( g_LadderTeamMgr.IsLadderTeam() && g_LadderTeamMgr.IsHeroMatchMode() && pOwner->GetHeroTitle() != 0 )
				{
					g_FontMgr.SetAlignType( TAT_CENTER );
					g_FontMgr.PrintText( iXPos, (iYPos + UPPER_Y_OFFSET)+LADDER_TITLE_Y_OFFSET, FONT_SIZE_12, "[%s]", g_LevelMgr.GetHeroTitleString( pOwner->GetHeroTitle() ).c_str() );
				}
			}

			enum { LEVEL_X_OFFSET = -1, LEVEL_Y_OFFSET = 16, };
			switch( eCurTeamType )
			{
			case TEAM_BLUE:
			case TEAM_RED:
				if(Help::IsMonsterDungeonMode(pStage->GetModeType()))
				{
					g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, pOwner->GetGrowthInstantLevel()+1, eCurTeamType, pOwner->IsOwnerChar() );
				}
				else
				{
					g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, pOwner->GetClassLevel(), eCurTeamType, pOwner->IsOwnerChar() );
				}
				break;
			default: //개인전
				if( eCurSingleTeamType == TEAM_BLUE )
					g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, pOwner->GetClassLevel(), TEAM_BLUE, pOwner->IsOwnerChar() );
				else
					g_LevelMgr.RenderClassLevel( iXPos + LEVEL_X_OFFSET, iYPos + LEVEL_Y_OFFSET, pOwner->GetClassLevel(), TEAM_RED, pOwner->IsOwnerChar() );
				break;
			}

			enum { GAUGE_Y_OFFSET = 6, };
			m_pGauge->RenderGauge( iXPos, iYPos + GAUGE_Y_OFFSET, false );
		}
	}
}

void ioCopyOwnerDummyChar5::DropZoneCheck()
{
	return;
}