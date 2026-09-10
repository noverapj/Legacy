

#include "stdafx.h"

#include "../io3DEngine/ioAxisAlignBox.h"

#include "ioComplexStringPrinter.h"

#include "ioCharEmoticon.h"
#include "io3DCharEmoticon.h"

#include "ioBaseChar.h"

ioHashString ioCharEmoticon::m_EmoticonBack;

int ioCharEmoticon::m_iEmoticonXPos;
int ioCharEmoticon::m_iEmoticonYPos;

int ioCharEmoticon::m_iEmoticonNameOffsetX;
int ioCharEmoticon::m_iEmoticonNameOffsetY;
int ioCharEmoticon::m_iEmoticonNameAlignType;

DWORD ioCharEmoticon::m_dwDefaultDuration;
DWORD ioCharEmoticon::m_dwChatIconDuration;
DWORD ioCharEmoticon::m_dw3DChatIconDuration;
DWORD ioCharEmoticon::m_dwLongDuration;

float ioCharEmoticon::m_fEmoticonEndScale;
float ioCharEmoticon::m_fFishingGradeEndScale;

float ioCharEmoticon::m_fOwnerGameEventScale;
float ioCharEmoticon::m_fOwnerSkillBuffScale;
float ioCharEmoticon::m_fOwnerBadStateScale;
float ioCharEmoticon::m_fOwnerMovieEventScale;

float ioCharEmoticon::m_fOtherGameEventScale;
float ioCharEmoticon::m_fOtherSkillBuffScale;
float ioCharEmoticon::m_fOtherBadStateScale;
float ioCharEmoticon::m_fOtherMovieEventScale;

DWORD ioCharEmoticon::m_dwOwnerGameEventTime;
DWORD ioCharEmoticon::m_dwOwnerSkillBuffTime;
DWORD ioCharEmoticon::m_dwOwnerBadStateTime;

DWORD ioCharEmoticon::m_dwOtherGameEventTime;
DWORD ioCharEmoticon::m_dwOtherSkillBuffTime;
DWORD ioCharEmoticon::m_dwOtherBadStateTime;

DWORD ioCharEmoticon::m_dwDisapearWhiteTime;
DWORD ioCharEmoticon::m_dwDisapearAlphaTime;

float ioCharEmoticon::m_fDurationScaleRate;
float ioCharEmoticon::m_fDurationScaleFastRate;
DWORD ioCharEmoticon::m_dwDurationScaleTime;
DWORD ioCharEmoticon::m_dwDurationScaleFastTime;



ioCharEmoticon::ioCharEmoticon()
{
	m_pEmoticon = NULL;
	m_pEmoticonBack = NULL;
	m_pEmoticonNum  = NULL;

	m_p3DChatEmoticon = NULL;

	m_pKillPointNum = NULL;
	m_pKillPointPer = NULL;

	m_pChatBubbleCenter  = NULL;
	m_pChatBubbleRight   = NULL;

	m_dwEmoticonStartTime = 0;
	m_dwCurEmoticonDuration = 0;
	m_dwCurScaleDuration = 0;
	m_fCurStartScale    = FLOAT1;
	m_iHitComboCount    = 0;
	m_iCurHitComboCount = 0;

	m_bLongTimeLiveEmoticon = false;
	m_bUnLimitedEmoticon = false;

	m_State = ES_NONE;
	m_Type  = EMT_DEFAULT;
	m_bOwnerChar = false;

	m_RenderType = UI_RENDER_NORMAL;
	m_iRenderXPos = 0;
	m_iRenderYPos = 0;

	m_bNeedRender = false;
	m_bInScreen = false;
	m_bVisible  = false;

	m_iCharRank    = 0;

	m_szEmoticonName.Clear();

	//
	m_bFastDuration = false;
	m_fCurDurationScaleRate = FLOAT1;
	m_dwDurationScaleStartTime = 0;

	m_pFishingGrade = NULL;
	m_pFishingLevelUp = NULL;
	m_fCurFishingGradeScale = FLOAT1;
	//

	// NewEmoticon
	m_pCurEmoticonInfo = NULL;
	m_ReserveEmoticonList.clear();
}

ioCharEmoticon::~ioCharEmoticon()
{
	SAFEDELETE( m_pEmoticon );
	SAFEDELETE( m_pEmoticonBack );
	SAFEDELETE( m_pEmoticonNum );
	SAFEDELETE( m_p3DChatEmoticon );

	SAFEDELETE( m_pKillPointNum );
	SAFEDELETE( m_pKillPointPer );

	SAFEDELETE( m_pChatBubbleCenter );
	SAFEDELETE( m_pChatBubbleRight );

	SAFEDELETE( m_pFishingGrade );
	SAFEDELETE( m_pFishingLevelUp );

	m_vHitComboNum.clear();
	m_vChatTextVec.clear();

	// NewEmoticon
	SAFEDELETE(m_pCurEmoticonInfo);

	EmoticonInfoList::iterator iter = m_ReserveEmoticonList.begin();
	while( iter != m_ReserveEmoticonList.end() )
	{
		EmoticonInfo *pInfo = (*iter);
		SAFEDELETE(pInfo);

		++iter;
	}
	m_ReserveEmoticonList.clear();
}

void ioCharEmoticon::LoadStaticValues()
{
	ioINILoader_e kLoader( "config/sp2_gui.ini" );

	kLoader.SetTitle_e( "emoticon" );

	char szBuf[MAX_PATH];
	kLoader.LoadString_e( "back_image", "", szBuf, MAX_PATH );
	m_EmoticonBack = szBuf;

	m_iEmoticonXPos = kLoader.LoadInt_e( "x_offset", 0 );
	m_iEmoticonYPos = kLoader.LoadInt_e( "y_offset", 46 );

	m_iEmoticonNameOffsetX   = kLoader.LoadInt_e( "title_offset_x", 0 );
	m_iEmoticonNameOffsetY   = kLoader.LoadInt_e( "title_offset_y", 0 );
	m_iEmoticonNameAlignType = kLoader.LoadInt_e( "title_align_type", 0 );

	m_dwDefaultDuration = kLoader.LoadInt_e( "default_duration", 1000 );
	m_dwChatIconDuration = kLoader.LoadInt_e( "chat_icon_duration", 2000 );
	m_dw3DChatIconDuration = kLoader.LoadInt_e( "3d_chat_icon_duration", 2500 );
	m_dwLongDuration = kLoader.LoadInt_e( "long_duration", 2000 );

	m_fEmoticonEndScale = kLoader.LoadFloat_e( "emoticon_end_scale", 0.75f );
	m_fFishingGradeEndScale = kLoader.LoadFloat_e( "fishing_grade_end_scale", 0.65f );

	m_fOwnerGameEventScale = kLoader.LoadFloat_e( "owner_game_event_scale", 0.75f );
	m_fOwnerSkillBuffScale = kLoader.LoadFloat_e( "owner_skill_buff_scale", 0.75f );
	m_fOwnerBadStateScale  = kLoader.LoadFloat_e( "owner_bad_state_scale", 0.75f );
	m_fOwnerMovieEventScale  = kLoader.LoadFloat_e( "owner_movie_event_scale", 0.75f );

	m_fOtherGameEventScale = kLoader.LoadFloat_e( "other_game_event_scale", 0.75f );
	m_fOtherSkillBuffScale = kLoader.LoadFloat_e( "other_skill_buff_scale", 0.75f );
	m_fOtherBadStateScale  = kLoader.LoadFloat_e( "other_bad_state_scale", 0.75f );
	m_fOtherMovieEventScale  = kLoader.LoadFloat_e( "other_movie_event_scale", 0.75f );

	m_dwOwnerGameEventTime = kLoader.LoadInt_e( "owner_game_event_time", 200 );
	m_dwOwnerSkillBuffTime = kLoader.LoadInt_e( "owner_skill_buff_time", 100 );
	m_dwOwnerBadStateTime  = kLoader.LoadInt_e( "owner_bad_state_time", 100 );

	m_dwOtherGameEventTime = kLoader.LoadInt_e( "other_game_event_time", 100 );
	m_dwOtherSkillBuffTime = kLoader.LoadInt_e( "other_skill_buff_time", 100 );
	m_dwOtherBadStateTime  = kLoader.LoadInt_e( "other_bad_state_time", 100 );

	m_dwDisapearWhiteTime = kLoader.LoadInt_e( "disapear_white_time", 100 );
	m_dwDisapearAlphaTime = kLoader.LoadInt_e( "disapear_alpha_time", 100 );


	kLoader.SetTitle_e( "fishing_icon" );
	m_fDurationScaleRate		= kLoader.LoadFloat_e( "duration_scale_rate", FLOAT1 );
	m_fDurationScaleFastRate	= kLoader.LoadFloat_e( "duration_scale_fast_rate", FLOAT1 );
	m_dwDurationScaleTime		= kLoader.LoadInt_e( "duration_scale_time", 0 );
	m_dwDurationScaleFastTime	= kLoader.LoadInt_e( "duration_scale_fast_time", 0 );


	io3DCharEmoticon::LoadStaticValue( kLoader );
}

bool ioCharEmoticon::IsEmoticonEvent()
{
	if( m_State == ES_NONE )
		return false;
	return true;
}

void ioCharEmoticon::CheckEmoticonBackImage()
{
	if( !m_pEmoticonBack && !m_EmoticonBack.IsEmpty() )
	{
		m_pEmoticonBack = g_UIImageSetMgr.CreateImageByFullName( m_EmoticonBack );
	}
}

void ioCharEmoticon::SetHitCombo( const ioHashString &szEmoticon, 
								  const ioHashString &szIconComboNum,
								  int iHitCount,
								  bool bOwnerChar )
{
	m_iHitComboCount = iHitCount;

	if( m_State == ES_NONE || m_State > ES_SCALE || m_Type != EMT_HIT_COMBO )
	{
		m_iCurHitComboCount = m_iHitComboCount;
		SetEmoticon( szEmoticon, EMT_HIT_COMBO, bOwnerChar );
		m_vHitComboNum.clear();
	}
	else if( !szIconComboNum.IsEmpty() )
	{
		// 중복되는 히트수. ES_SCALE이 끝나면 바로 다음 히트수를 로드한다.
		m_vHitComboNum.push_back( szIconComboNum );
	}

	if( !szIconComboNum.IsEmpty() && !m_pEmoticonNum )
	{
		m_pEmoticonNum = g_UIImageSetMgr.CreateImageByFullName( szIconComboNum );
		if( m_pEmoticonNum )
			m_pEmoticonNum->SetScale( m_fCurStartScale );
	}
}

void ioCharEmoticon::SetKillPoint( const ioHashString &szEmoticon, const ioHashString &szIconNum, const ioHashString &szPer, int iPoint, bool bOwnerChar )
{
	m_iKillPoint = iPoint;
	SetEmoticon( szEmoticon, EMT_KILL_POINT, bOwnerChar );

	if( !szIconNum.IsEmpty() && !m_pKillPointNum )
	{
		m_pKillPointNum = g_UIImageSetMgr.CreateImageByFullName( szIconNum );
	}

	if( !szPer.IsEmpty() && !m_pKillPointPer )
	{
		m_pKillPointPer = g_UIImageSetMgr.CreateImageByFullName( szPer );
	}
}

void ioCharEmoticon::SetFishingGrade( const ioHashString &szEmoticon, const ioHashString &szGrade, bool bOwnerChar, DWORD dwDuration )
{
	SetEmoticon( szEmoticon, EMT_FISHING_GRADE, bOwnerChar );

	if( !szGrade.IsEmpty() )
	{
		SAFEDELETE( m_pFishingGrade );
		m_pFishingGrade = g_UIImageSetMgr.CreateImageByFullName( szGrade );
	}
}

void ioCharEmoticon::SetFishingLevelUp( const ioHashString &szEmoticon, const ioHashString &szLevelUp, bool bOwnerChar, DWORD dwDuration )
{
	SetEmoticon( szEmoticon, EMT_FISHING_LEVEL_UP, bOwnerChar );

	SAFEDELETE( m_pFishingLevelUp );

	if( !szLevelUp.IsEmpty() )
	{
		m_pFishingLevelUp = g_UIImageSetMgr.CreateImageByFullName( szLevelUp );
	}
}

void ioCharEmoticon::SetUserRankEmoticon( const ioHashString &szEmoticon, 
									      const ioHashString &szRankNumber,
									      int iRank,
									      bool bOwnerChar )
{
	m_iCharRank = iRank;
	SetEmoticon( szEmoticon, EMT_USER_RANK, bOwnerChar );

	if( !szRankNumber.IsEmpty() )
	{
		SAFEDELETE( m_pEmoticonNum );
		m_pEmoticonNum = g_UIImageSetMgr.CreateImageByFullName( szRankNumber );
	}
}

void ioCharEmoticon::ReSetHitCombo()
{
	m_iCurHitComboCount++;
	m_dwEmoticonStartTime = FRAMEGETTIME();
	m_dwCurEmoticonDuration = m_dwLongDuration;

	if( m_bOwnerChar )
	{
		m_fCurStartScale = m_fOwnerGameEventScale;
		m_dwCurScaleDuration = m_dwOwnerGameEventTime;
	}
	else
	{
		m_fCurStartScale = m_fOtherGameEventScale;
		m_dwCurScaleDuration = m_dwOtherGameEventTime;
	}

	if( m_pEmoticon )
		m_pEmoticon->SetScale( m_fCurStartScale );

	if( !m_vHitComboNum.empty() )
	{
		ioHashString &rkNumberName = m_vHitComboNum[0];
		if( !rkNumberName.IsEmpty() )
		{
			SAFEDELETE( m_pEmoticonNum );
			m_pEmoticonNum = g_UIImageSetMgr.CreateImageByFullName( rkNumberName );
			if( m_pEmoticonNum )
				m_pEmoticonNum->SetScale( m_fCurStartScale );
		}
		m_vHitComboNum.erase( m_vHitComboNum.begin() );
	}

	m_State = ES_SCALE;
}

void ioCharEmoticon::SetLongTimeLiveEmoticon( const ioHashString &szEmoticon )
{
	if( szEmoticon.IsEmpty() ) return;

	ioUIRenderImage *pEmoticon = g_UIImageSetMgr.CreateImageByFullName( szEmoticon );
	if( !pEmoticon ) return;

	SAFEDELETE( m_pEmoticon );
	SAFEDELETE( m_pEmoticonNum );

	m_szEmoticonName.Clear();
	m_pEmoticon = pEmoticon;

	CheckEmoticonBackImage();

	m_dwEmoticonStartTime = FRAMEGETTIME();
	m_dwCurEmoticonDuration = 100000000;
	m_dwCurScaleDuration = 0;
	m_fCurStartScale = m_fEmoticonEndScale;

	m_State = ES_DURATION;
	m_Type  = EMT_CHAT_ICON;

	if( m_pEmoticon )
		m_pEmoticon->SetScale( m_fCurStartScale );

	m_LongTimeLiveEmoticon = szEmoticon;
	m_bLongTimeLiveEmoticon = true;
}

void ioCharEmoticon::SetLongTimeLiveEmoticonAndScale( const ioHashString &szEmoticon, bool bFast )
{
	if( szEmoticon.IsEmpty() ) return;

	ioUIRenderImage *pEmoticon = g_UIImageSetMgr.CreateImageByFullName( szEmoticon );
	if( !pEmoticon ) return;

	SAFEDELETE( m_pEmoticon );
	SAFEDELETE( m_pEmoticonNum );

	m_szEmoticonName.Clear();
	m_pEmoticon = pEmoticon;

	CheckEmoticonBackImage();

	m_bFastDuration = bFast;

	m_dwEmoticonStartTime = FRAMEGETTIME();
	m_dwCurEmoticonDuration = 100000000;
	m_dwCurScaleDuration = 0;
	m_fCurStartScale = m_fEmoticonEndScale;

	m_State = ES_DURATION_SCALE;
	m_Type  = EMT_DEFAULT;

	if( m_pEmoticon )
		m_pEmoticon->SetScale( m_fCurStartScale );

	m_LongTimeLiveEmoticon = szEmoticon;
	m_bLongTimeLiveEmoticon = true;
}

void ioCharEmoticon::EndLongTimeLiveEmoticon()
{
	if( !m_bLongTimeLiveEmoticon )
		return;

	m_LongTimeLiveEmoticon.Clear();
	m_bLongTimeLiveEmoticon = false;

	EndEmoticon();
}

void ioCharEmoticon::ProcessDurationScale()
{
	if( m_dwDurationScaleStartTime == 0 )
		m_dwDurationScaleStartTime = FRAMEGETTIME();

	DWORD dwScaleTime = m_dwDurationScaleTime;
	float fScaleRate = m_fDurationScaleRate;

	if( m_bFastDuration )
	{
		dwScaleTime = m_dwDurationScaleFastTime;
		fScaleRate = m_fDurationScaleFastRate;
	}

	DWORD dwGapTime = FRAMEGETTIME() - m_dwDurationScaleStartTime;
	if( dwGapTime > dwScaleTime )
	{
		dwGapTime -= dwScaleTime;
		m_dwDurationScaleStartTime = FRAMEGETTIME() - ( dwGapTime % dwScaleTime );
	}

	float fTimeRate = (float)dwGapTime / (float)dwScaleTime;
	float fCurRate = FLOAT1 + fScaleRate * sinf( D3DX_PI * fTimeRate );
	
	m_fCurDurationScaleRate = m_fEmoticonEndScale * fCurRate;

	if( m_pEmoticon )
	{
		m_pEmoticon->SetScale( m_fCurDurationScaleRate );
	}
}

void ioCharEmoticon::SetNamedUnLimitedEmoticon( ioBaseChar *pChar, const ioHashString &szEmoticon, const ioHashString &szDescName, DWORD dwColor )
{
	if( szEmoticon.IsEmpty() ) return;

	if( !pChar || pChar->GetState() != CS_FISHING )
	{
		ioUIRenderImage *pEmoticon = g_UIImageSetMgr.CreateImageByFullName( szEmoticon );
		if( !pEmoticon ) return;

		SAFEDELETE( m_pEmoticon );
		SAFEDELETE( m_pEmoticonNum );

		m_szEmoticonName.Clear();
		m_pEmoticon = pEmoticon;
	}

	m_szUnLimitedEmoticonName = szDescName;
	m_dwUnLimitedEmoticonNameColor = dwColor;

	if( !pChar || pChar->GetState() != CS_FISHING )
	{
		CheckEmoticonBackImage();

		m_dwEmoticonStartTime = FRAMEGETTIME();
		m_dwCurEmoticonDuration = 100000000;
		m_dwCurScaleDuration = 0;
		m_fCurStartScale = m_fEmoticonEndScale;

		m_State = ES_UNLIMITED;
		m_Type  = EMT_NAMED_UNLIMITED_ICON;

		if( m_pEmoticon )
			m_pEmoticon->SetScale( m_fCurStartScale );
	}

	m_UnLimitedEmoticon = szEmoticon;
	m_bUnLimitedEmoticon = true;
}

void ioCharEmoticon::EndUnLimitedEmoticon()
{
	if( !m_bUnLimitedEmoticon )
		return;

	m_UnLimitedEmoticon.Clear();
	m_szUnLimitedEmoticonName.Clear();
	m_bUnLimitedEmoticon = false;

	EndEmoticon();
}

void ioCharEmoticon::SetEmoticon( const ioHashString &szEmoticon,
								  EmoticonType eType,
								  bool bOwnerChar,
								  DWORD dwDuration )
{
	if( szEmoticon.IsEmpty() )	return;

	ioUIRenderImage *pEmoticon = g_UIImageSetMgr.CreateImageByFullName( szEmoticon );
	if( !pEmoticon ) return;

	SAFEDELETE( m_pEmoticon );
	SAFEDELETE( m_pEmoticonNum );

	m_szEmoticonName.Clear();
	m_pEmoticon = pEmoticon;

	CheckEmoticonBackImage();

	m_dwEmoticonStartTime = FRAMEGETTIME();
	m_bOwnerChar = bOwnerChar;

	switch( eType )
	{
	case EMT_DEFAULT:
		m_dwCurEmoticonDuration = m_dwDefaultDuration;
		m_fCurStartScale = m_fEmoticonEndScale;
		m_dwCurScaleDuration = 0;
		break;
	case EMT_CHAT_ICON:
		m_dwCurEmoticonDuration = m_dwChatIconDuration;
		m_fCurStartScale = m_fEmoticonEndScale;
		m_dwCurScaleDuration = 0;
		break;
	case EMT_KILL_POINT:
	case EMT_GAME_EVENT:
	case EMT_HIT_COMBO:
	case EMT_USER_RANK:
	case EMT_FISHING_GRADE:
	case EMT_FISHING_LEVEL_UP:
		m_dwCurEmoticonDuration = m_dwLongDuration;
		
		if( m_bOwnerChar )
		{
			m_fCurStartScale = m_fOwnerGameEventScale;
			m_dwCurScaleDuration = m_dwOwnerGameEventTime;
		}
		else
		{
			m_fCurStartScale = m_fOtherGameEventScale;
			m_dwCurScaleDuration = m_dwOtherGameEventTime;
		}
		break;
	case EMT_MOVIE_EVENT:
		m_dwCurEmoticonDuration = m_dwLongDuration;

		if( m_bOwnerChar )
		{
			m_fCurStartScale = m_fOwnerMovieEventScale;
			m_dwCurScaleDuration = m_dwOwnerGameEventTime;
		}
		else
		{
			m_fCurStartScale = m_fOtherMovieEventScale;
			m_dwCurScaleDuration = m_dwOtherGameEventTime;
		}
		break;
	case EMT_SKILL_BUFF:
		m_dwCurEmoticonDuration = m_dwDefaultDuration;

		if( m_bOwnerChar )
		{
			m_fCurStartScale = m_fOwnerSkillBuffScale;
			m_dwCurScaleDuration = m_dwOwnerSkillBuffTime;
		}
		else
		{
			m_fCurStartScale = m_fOtherSkillBuffScale;
			m_dwCurScaleDuration = m_dwOtherSkillBuffTime;
		}
		break;
	case EMT_BAD_STATE:
		m_dwCurEmoticonDuration = m_dwDefaultDuration;

		if( m_bOwnerChar )
		{
			m_fCurStartScale = m_fOwnerBadStateScale;
			m_dwCurScaleDuration = m_dwOwnerBadStateTime;
		}
		else
		{
			m_fCurStartScale = m_fOtherBadStateScale;
			m_dwCurScaleDuration = m_dwOtherBadStateTime;
		}
		break;
	}

	// 시스템에서 시간을 설정하였으면 그 시간으로 설정
	if( dwDuration > 0 )
		m_dwCurEmoticonDuration = dwDuration;

	if( m_pEmoticon )
		m_pEmoticon->SetScale( m_fCurStartScale );

	m_State = ES_SCALE;
	m_Type  = eType;
}

void ioCharEmoticon::SetNamedEmoticon( const ioHashString &szEmoticon, const ioHashString &szDescName, DWORD dwColor, bool bOwnerChar, DWORD dwDuration /*= 0 */ )
{
	if( szEmoticon.IsEmpty() )	return;

	ioUIRenderImage *pEmoticon = g_UIImageSetMgr.CreateImageByFullName( szEmoticon );
	if( !pEmoticon ) return;

	SAFEDELETE( m_pEmoticon );
	SAFEDELETE( m_pEmoticonNum );

	m_pEmoticon = pEmoticon;

	m_szEmoticonName = szDescName;
	m_dwEmoticonNameColor = dwColor;

	CheckEmoticonBackImage();

	m_dwEmoticonStartTime = FRAMEGETTIME();
	m_bOwnerChar = bOwnerChar;

	m_dwCurEmoticonDuration = m_dwDefaultDuration;

	if( m_bOwnerChar )
	{
		m_fCurStartScale = m_fOwnerSkillBuffScale;
		m_dwCurScaleDuration = m_dwOwnerSkillBuffTime;
	}
	else
	{
		m_fCurStartScale = m_fOtherSkillBuffScale;
		m_dwCurScaleDuration = m_dwOtherSkillBuffTime;
	}

	// 시스템에서 시간을 설정하였으면 그 시간으로 설정
	if( dwDuration > 0 )
		m_dwCurEmoticonDuration = dwDuration;

	if( m_pEmoticon )
		m_pEmoticon->SetScale( m_fCurStartScale );

	m_State = ES_SCALE;
	m_Type  = EMT_NAMED_ICON;
}

void ioCharEmoticon::Set3DChatEmoticon( const CHARACTER &rkInfo, int iChatLen, bool bOwnerChar, bool bNoSound )
{
	SAFEDELETE( m_pEmoticon );
	SAFEDELETE( m_pEmoticonNum );

	if( !m_p3DChatEmoticon )
	{
		m_p3DChatEmoticon = new io3DCharEmoticon;
		m_p3DChatEmoticon->Initialize( "char_state_cam" );
	}

	m_p3DChatEmoticon->SetAvata( rkInfo, iChatLen, bNoSound, SPA_NONE );

	m_dwEmoticonStartTime = FRAMEGETTIME();
	m_bOwnerChar = bOwnerChar;

	m_dwCurEmoticonDuration = m_dw3DChatIconDuration;
	m_fCurStartScale = m_fEmoticonEndScale;
	m_dwCurScaleDuration = 0;

	m_State = ES_3D_CHAT;
	m_Type  = EMT_3D_CHAT_ICON;
}

void ioCharEmoticon::EndEmoticon()
{
	if( m_bLongTimeLiveEmoticon )
	{
		SetLongTimeLiveEmoticon( m_LongTimeLiveEmoticon );
	}
	else if( m_bUnLimitedEmoticon )
	{
		SetNamedUnLimitedEmoticon( NULL, m_UnLimitedEmoticon, m_szUnLimitedEmoticonName, m_dwUnLimitedEmoticonNameColor );
	}
	else
	{
		SAFEDELETE( m_pEmoticon );
		SAFEDELETE( m_pEmoticonNum );
		SAFEDELETE( m_pKillPointNum );
		SAFEDELETE( m_pFishingGrade );
		SAFEDELETE( m_pFishingLevelUp );

		m_dwEmoticonStartTime = 0;
		m_dwCurEmoticonDuration = 0;
		m_dwCurScaleDuration = 0;
		m_fCurStartScale = 0.0f;
		m_fCurFishingGradeScale = 0.0f;

		m_State = ES_NONE;
		m_bOwnerChar = false;
	}

	m_vChatTextVec.clear();
	SAFEDELETE( m_pChatBubbleCenter );
	SAFEDELETE( m_pChatBubbleRight );
}

void ioCharEmoticon::EndEmoticonByName( const ioHashString &rszDescName )
{
	if( rszDescName.IsEmpty() )
		return;
	if( m_szEmoticonName != rszDescName )
		return;

	m_dwCurEmoticonDuration = FRAMEGETTIME() - m_dwEmoticonStartTime; // Duration을 현재 pasttime으로 맞춘다.
	m_State = ES_END_WHITE;
}

void ioCharEmoticon::UpdateEmoticon( const D3DXVECTOR3 &vPos,
									 const D3DXVECTOR3 &vCamTargetPos,
									 bool bVisible )
{
	if( m_State == ES_NONE )
	{
		m_bNeedRender = false;
		return;
	}

	if( m_State != ES_3D_CHAT && !m_pEmoticon )
	{
		m_bNeedRender = false;
		return;
	}

	m_bNeedRender = true;

	DWORD dwPastTime = FRAMEGETTIME() - m_dwEmoticonStartTime;
	m_RenderType = UI_RENDER_NORMAL;

	int iColor = 0;
	float fAlphaRate = FLOAT1;
	float fScale = m_fEmoticonEndScale;

	switch( m_State )
	{
	case ES_SCALE:
		if( dwPastTime < m_dwCurScaleDuration )
		{
			float fScaleRate = (float)dwPastTime / (float)m_dwCurScaleDuration;
			fScale = m_fCurStartScale * ( FLOAT1 - fScaleRate ) + m_fEmoticonEndScale * fScaleRate;

			m_pEmoticon->SetScale( fScale );
			if( m_Type == EMT_HIT_COMBO )
			{
				if( m_pEmoticonNum )
					m_pEmoticonNum->SetScale( fScale );
			}

			if( m_Type == EMT_FISHING_GRADE )
			{
				float fFishScaleRate = (float)dwPastTime / (float)m_dwCurScaleDuration;
				float fFishScale = m_fCurStartScale * ( FLOAT1 - fFishScaleRate ) + m_fFishingGradeEndScale * fFishScaleRate;

				if( m_pFishingGrade )
					m_pFishingGrade->SetScale( fFishScale );
			}
		}
		else
		{
			m_State = ES_DURATION;
			m_pEmoticon->SetScale( m_fEmoticonEndScale );

			if( m_Type == EMT_FISHING_GRADE )
			{
				if( m_pFishingGrade )
					m_pFishingGrade->SetScale( m_fFishingGradeEndScale );
			}

			if( m_Type == EMT_HIT_COMBO )
			{
				if( m_pEmoticonNum )
					m_pEmoticonNum->SetScale( m_fEmoticonEndScale );
				if( m_iHitComboCount > m_iCurHitComboCount )
				{					
					ReSetHitCombo();
					m_bNeedRender = false;
					return;
				}
			}
		}
		break;
	case ES_DURATION:
		if( dwPastTime > m_dwCurEmoticonDuration )
		{
			m_State = ES_END_WHITE;
		}
		break;
	case ES_END_WHITE:
		if( m_dwCurEmoticonDuration + m_dwDisapearWhiteTime > dwPastTime )
		{
			dwPastTime -= m_dwCurEmoticonDuration;

			iColor = 255.0f * ( (float)dwPastTime / (float)m_dwDisapearWhiteTime );
			m_pEmoticon->SetColor( iColor, iColor, iColor );
			if( m_pEmoticonNum )
				m_pEmoticonNum->SetColor( iColor, iColor, iColor );
		}
		else
		{
			m_State = ES_END_ALPHA;
			iColor = 255;
			m_pEmoticon->SetColor( 255, 255, 255 );
			if( m_pEmoticonNum )
				m_pEmoticonNum->SetColor( 255, 255, 255 );
		}

		m_RenderType = UI_RENDER_NORMAL_ADD_COLOR;
		break;
	case ES_END_ALPHA:
		if( m_dwCurEmoticonDuration + m_dwDisapearWhiteTime + m_dwDisapearAlphaTime > dwPastTime )
		{
			dwPastTime -= m_dwCurEmoticonDuration + m_dwDisapearWhiteTime;

			iColor = 255;
			fAlphaRate = FLOAT1 - (float)dwPastTime / (float)m_dwDisapearAlphaTime;
			m_pEmoticon->SetAlpha( (BYTE)(int)(255.0f * fAlphaRate) );
			if( m_pEmoticonNum )
				m_pEmoticonNum->SetAlpha( (BYTE)(int)(255.0f * fAlphaRate) );
		}
		else
		{
			EndEmoticon();
			m_bNeedRender = false;
			return;
		}

		m_RenderType = UI_RENDER_NORMAL_ADD_COLOR;
		break;
	case ES_3D_CHAT:
		if( dwPastTime > m_dwCurEmoticonDuration )
		{
			EndEmoticon();
			m_bNeedRender = false;
			return;
		}
		break;
	case ES_DURATION_SCALE:
		ProcessDurationScale();
		break;
	}

	m_iRenderXPos = m_iRenderYPos = 0;
	m_bInScreen = true;
	m_bVisible = bVisible;

	if( !Setting::Check2DInScreen( vPos, m_iRenderXPos, m_iRenderYPos ) )
	{
		m_bInScreen = false;
	}

	if( !m_bInScreen )
	{
		if( m_Type == EMT_CHAT_ICON || m_Type == EMT_3D_CHAT_ICON )
		{
			CheckOutScreen( vPos, vCamTargetPos, m_iRenderXPos, m_iRenderYPos, fScale, true );
		}
		else
		{
			m_bNeedRender = false;
			return;
		}
	}
	else if( m_bInScreen && !m_bVisible )
	{
		if( m_Type == EMT_CHAT_ICON || m_Type == EMT_3D_CHAT_ICON )
		{
			CheckOutScreen( vPos, vCamTargetPos, m_iRenderXPos, m_iRenderYPos, fScale, false );
		}
		else
		{
			m_bNeedRender = false;
			return;
		}
	}
	else
	{
		m_iRenderXPos -= m_iEmoticonXPos;
		m_iRenderYPos -= m_iEmoticonYPos;
	}

	if( m_pEmoticonNum || m_pKillPointNum )
	{
		m_iNumberOffset = 6;

		if( dwPastTime < m_dwCurScaleDuration )
		{
			float fRate = (float)dwPastTime / (float)m_dwCurScaleDuration;
			m_iNumberOffset -= (int)( 6.0f * fRate );
		}

		m_fNumberScale = fScale;
	}

	if( m_Type == EMT_3D_CHAT_ICON && m_p3DChatEmoticon )
	{
		m_p3DChatEmoticon->Update( m_iRenderXPos, m_iRenderYPos );
	}

	m_fNamedAlpha = fAlphaRate;
	m_fNamedScale = fScale;
	m_iNamedBkColor = iColor;
}

void ioCharEmoticon::CheckNamedEmoticon( float fAlphaRate, float fScale )
{
	BYTE iAlpha = (BYTE)(int)(127.0f * fAlphaRate);
	BYTE iAlpha2 = (BYTE)(int)(255.0f * fAlphaRate);
	float fCurSize = 0.0f;
	float fOffSet = 0.0f;
	
	fCurSize = 0.66f * (fScale / m_fEmoticonEndScale);
	fOffSet -= 35.0f * (fScale / m_fEmoticonEndScale);
	fOffSet -= 9.0f * (fScale / m_fEmoticonEndScale);


	g_FontMgr.SetAlignType( (TextAlignType)m_iEmoticonNameAlignType );
	g_FontMgr.SetBkColor( m_iNamedBkColor, m_iNamedBkColor, m_iNamedBkColor );

	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetTextColor( 0, 0, 0 );

	if( !m_szEmoticonName.IsEmpty() )
		g_FontMgr.PrintTextAlpha( m_iRenderXPos + m_iEmoticonNameOffsetX, m_iRenderYPos + fOffSet + m_iEmoticonNameOffsetY, fCurSize, iAlpha, m_szEmoticonName.c_str() );

	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL );
	if( m_iNamedBkColor > 0 )
		g_FontMgr.SetTextColor( m_iNamedBkColor, m_iNamedBkColor, m_iNamedBkColor );
	else
		g_FontMgr.SetTextColor( m_dwEmoticonNameColor );

	if( !m_szEmoticonName.IsEmpty() )
		g_FontMgr.PrintTextAlpha( m_iRenderXPos + m_iEmoticonNameOffsetX, m_iRenderYPos + fOffSet + m_iEmoticonNameOffsetY, fCurSize, iAlpha2, m_szEmoticonName.c_str() );
}

void ioCharEmoticon::CheckNamedUnLimitedEmoticon( float fAlphaRate, float fScale )
{
	BYTE iAlpha = (BYTE)(int)(127.0f * fAlphaRate);
	BYTE iAlpha2 = (BYTE)(int)(255.0f * fAlphaRate);
	float fCurSize = 0.0f;
	float fOffSet = 0.0f;
	
	fCurSize = 0.66f * (fScale / m_fEmoticonEndScale);
	fOffSet -= 35.0f * (fScale / m_fEmoticonEndScale);
	fOffSet -= 9.0f * (fScale / m_fEmoticonEndScale);

	g_FontMgr.SetAlignType( (TextAlignType)m_iEmoticonNameAlignType );
	g_FontMgr.SetBkColor( m_iNamedBkColor, m_iNamedBkColor, m_iNamedBkColor );

	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetTextColor( 0, 0, 0 );

	if( !m_szUnLimitedEmoticonName.IsEmpty() )
		g_FontMgr.PrintTextAlpha( m_iRenderXPos + m_iEmoticonNameOffsetX, m_iRenderYPos + fOffSet + m_iEmoticonNameOffsetY, fCurSize, iAlpha, m_szUnLimitedEmoticonName.c_str() );

	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL );
	g_FontMgr.SetTextColor( m_dwUnLimitedEmoticonNameColor );

	if( !m_szUnLimitedEmoticonName.IsEmpty() )
		g_FontMgr.PrintTextAlpha( m_iRenderXPos + m_iEmoticonNameOffsetX, m_iRenderYPos + fOffSet + m_iEmoticonNameOffsetY, fCurSize, iAlpha2, m_szUnLimitedEmoticonName.c_str() );
}

void ioCharEmoticon::RenderEmoticon()
{
	if( !m_bNeedRender )	return;

	ErrorReport::SetPosition( 1370, 12 );
	bool bRenderChatBubble = RenderChatBubble();
	ErrorReport::SetPosition( 1370, 13 );

	if( m_Type == EMT_3D_CHAT_ICON && m_p3DChatEmoticon )
	{
		ErrorReport::SetPosition( 1370, 1 );
		m_p3DChatEmoticon->Render();
		ErrorReport::SetPosition( 1370, 2 );

		ErrorReport::SetPosition( 1370, 14 );
		if( bRenderChatBubble )
			RenderChatText();
		ErrorReport::SetPosition( 1370, 15 );

		return;
	}

	if( m_State == ES_SCALE || m_State == ES_DURATION || m_State == ES_DURATION_SCALE )
	{
		if( m_pEmoticonBack && m_bInScreen && m_bVisible )
		{
			ErrorReport::SetPosition( 1370, 3 );

			m_pEmoticonBack->Render( m_iRenderXPos,
									 m_iRenderYPos,
									 UI_RENDER_NORMAL,
									 TFO_BILINEAR );

			ErrorReport::SetPosition( 1370, 4 );
		}
	}

	if( m_pEmoticon )
	{
		ErrorReport::SetPosition( 1370, 5 );

		m_pEmoticon->Render( m_iRenderXPos,
							 m_iRenderYPos,
							 m_RenderType,
							 TFO_BILINEAR );

		ErrorReport::SetPosition( 1370, 6 );
	}

	if( m_Type == EMT_HIT_COMBO )
	{
		if( m_pEmoticonNum )
		{
			ErrorReport::SetPosition( 1370, 7 );

			m_pEmoticonNum->Render( m_iRenderXPos,
									m_iRenderYPos,
									m_RenderType,
									TFO_BILINEAR );

			ErrorReport::SetPosition( 1370, 8 );
		}	
	}
	else if( m_Type == EMT_KILL_POINT && m_pKillPointNum )
	{
		m_pKillPointNum->SetHorzFormatting( ioUIRenderImage::HF_LEFT_ALIGN );
		
		//자릿수 구분
		if( m_iKillPoint < 10 )
		{
			m_pKillPointNum->RenderNum( m_iRenderXPos - 20,	m_iRenderYPos - 14,
										m_iKillPoint, 0.0f, FLOAT1, m_RenderType, TFO_BILINEAR );

			if( m_pKillPointPer )
				m_pKillPointPer->Render( m_iRenderXPos, m_iRenderYPos -14, m_RenderType, TFO_BILINEAR );
		}
		else if( m_iKillPoint < 100 )
		{
			m_pKillPointNum->RenderNum( m_iRenderXPos - 30,	m_iRenderYPos - 14,
										m_iKillPoint, -6.0f, FLOAT1, m_RenderType, TFO_BILINEAR );

			if( m_pKillPointPer )
				m_pKillPointPer->Render( m_iRenderXPos + 10, m_iRenderYPos -14, m_RenderType, TFO_BILINEAR );
		}
		else
		{
			m_pKillPointNum->RenderNum( m_iRenderXPos - 40,	m_iRenderYPos - 14,
										m_iKillPoint, -6.0f, FLOAT1, m_RenderType, TFO_BILINEAR );

			if( m_pKillPointPer )
				m_pKillPointPer->Render( m_iRenderXPos + 20, m_iRenderYPos -14, m_RenderType, TFO_BILINEAR );
		}
	}
	else if( m_Type == EMT_FISHING_GRADE && m_pFishingGrade )
	{
		m_pFishingGrade->Render( m_iRenderXPos, m_iRenderYPos, m_RenderType, TFO_BILINEAR );
	}
	else if( m_Type == EMT_FISHING_LEVEL_UP && m_State == ES_DURATION && m_pFishingLevelUp )
	{
		m_pFishingLevelUp->Render( m_iRenderXPos - 44, m_iRenderYPos - 35, m_RenderType );
	}
	else if( m_pEmoticonNum )
	{
		ErrorReport::SetPosition( 1370, 9 );

		m_pEmoticonNum->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
		
		int iNum = m_iCharRank;

		ErrorReport::SetPosition( 1370, 10 );

		m_pEmoticonNum->RenderNum( m_iRenderXPos - 3,
								   m_iRenderYPos - ( 25 - m_iNumberOffset ),
								   iNum,
								   -6.0f,
								   m_fNumberScale,
								   m_RenderType,
								   TFO_BILINEAR );

		ErrorReport::SetPosition( 1370, 11 );
	}	

	// ErrorReport::SetPosition 12, 13, 14, 15 상단에서 사용
	ErrorReport::SetPosition( 1370, 16 );
	if( bRenderChatBubble )
		RenderChatText();
	ErrorReport::SetPosition( 1370, 17 );

	if( m_Type == EMT_NAMED_ICON )
		CheckNamedEmoticon( m_fNamedAlpha, m_fNamedScale );
	else if( m_Type == EMT_NAMED_UNLIMITED_ICON )
		CheckNamedUnLimitedEmoticon( m_fNamedAlpha, m_fNamedScale );
}

RECT ioCharEmoticon::GetEmoticonRenderRect() const
{
	RECT rcRect;
	
	if( !m_pEmoticon )
	{
		SetRect( &rcRect, m_iRenderXPos - 20,
						  m_iRenderYPos - 20,
						  m_iRenderXPos + 20,
						  m_iRenderYPos + 20 );

		return rcRect;
	}

	D3DXVECTOR2 vSize = m_pEmoticon->GetSize();
	rcRect.left = m_iRenderXPos - (int)m_pEmoticon->GetOffsetX();
	rcRect.top  = m_iRenderYPos - (int)m_pEmoticon->GetOffsetY();
	rcRect.right = rcRect.left + (int)vSize.x;
	rcRect.bottom = rcRect.top + (int)vSize.y;

	return rcRect;
}

void ioCharEmoticon::CheckOutScreen( const D3DXVECTOR3 &vPos,
									 const D3DXVECTOR3 &vCamTargetPos,
									 int &iXPos,
									 int &iYPos,
									 float fScale,
									 bool bVisible )
{
	int iCenterX = Setting::Width() / 2;
	int iCenterY = Setting::Height() / 2;

	int iEmoticonWidth = 65;
	int iEmoticonHeight = 68;

	int iCurXPos, iCurYPos;
	iCurXPos = iXPos;
	iCurYPos = iYPos;

	iEmoticonWidth /= 2;
	iEmoticonHeight /= 2;
	iEmoticonWidth *= fScale;
	iEmoticonHeight *= fScale;

	int iXGap = (int)(vCamTargetPos.x - vPos.x);
	int iZGap = (int)(vPos.z - vCamTargetPos.z);

	iCurXPos = iCenterX - iXGap;
	iCurYPos = iCenterY - iZGap;

	iXPos = iCurXPos;
	iYPos = iCurYPos;

	bool bAxisX = false;
	if( !bVisible )
	{
		D3DXVECTOR3 v2DPos = Setting::Convert3DTo2D( vPos );
		if( v2DPos.z > 1 )
		{
			v2DPos.x = -v2DPos.x;
			v2DPos.y = fabs( v2DPos.y );

			if( COMPARE( v2DPos.x, 0.0f, Setting::Width() ) )
				v2DPos.x += Setting::Width();

			if( COMPARE( v2DPos.y, 0.0f, Setting::Height() ) )
				v2DPos.y += Setting::Height();
		}

		float fXRate = fabs( (float)(iCenterX - v2DPos.x) ) / (float)Setting::Width();
		float fYRate = fabs( (float)(iCenterY - v2DPos.y) ) / (float)Setting::Height();

		if( fXRate >= fYRate )
			bAxisX = true;

		if( bAxisX )
		{
			if( v2DPos.x < iCenterX )
				iXPos = iEmoticonWidth;
			else
				iXPos = Setting::Width() - iEmoticonWidth;
		}
		else
		{
			if( v2DPos.y < iCenterY )
			{
				iYPos = iEmoticonHeight;
			}
			else
			{
				iYPos = Setting::Height() - iEmoticonHeight;
				iYPos -= 32;
			}
		}

		return;
	}

	bool bRePosition = true;
	if( COMPARE( iCurXPos, 0, Setting::Width() ) &&
		COMPARE( iCurYPos, 0, Setting::Height() ) )
	{
		bRePosition = false;
		float fXRate = fabs( (float)(iCenterX - iCurXPos) ) / (float)Setting::Width();
		float fYRate = fabs( (float)(iCenterY - iCurYPos) ) / (float)Setting::Height();

		if( fXRate >= fYRate )
			bAxisX = true;
	}

	if( iCurXPos < 0 )
	{
		iXPos = iEmoticonWidth;
	}
	else if( iCurXPos > Setting::Width() )
	{
		iXPos = Setting::Width() - iEmoticonWidth;
	}

	if( iCurYPos < 0 )
	{
		iYPos = iEmoticonHeight;
	}
	else if( iCurYPos > Setting::Height() )
	{
		iYPos = Setting::Height() - iEmoticonHeight;
		iYPos -= 32;
	}

	if( !bRePosition )
	{
		if( bAxisX )
		{
			if( iCenterX < iCenterX )
				iXPos = iEmoticonWidth;
			else
				iXPos = Setting::Width() - iEmoticonWidth;
		}
		else
		{
			if( iCenterY < iCenterY )
			{
				iYPos = iEmoticonHeight;
			}
			else
			{
				iYPos = Setting::Height() - iEmoticonHeight;
				iYPos -= 32;
			}
		}
	}
}

bool ioCharEmoticon::RenderChatBubble()
{
	if( !m_bInScreen )
		return false;

	if(    m_State == ES_END_WHITE 
		|| m_State == ES_END_ALPHA )
	{
		return false;
	}

	if(    m_Type != EMT_CHAT_ICON 
		&& m_Type != EMT_3D_CHAT_ICON )
	{
		return false;
	}

	if( m_vChatTextVec.empty() )
		return false;

	if(	   !m_pChatBubbleCenter 
		|| !m_pChatBubbleRight )
	{
		return false;
	}
	
	enum 
	{
		ONE_LINE_YOFFSET      = -12,
		TWO_LINE_YOFFSET      = -20,
	};

	int iSzie = m_vChatTextVec.size();
	if( iSzie == 1 )
	{
		m_pChatBubbleCenter->Render( m_iRenderXPos, m_iRenderYPos + ONE_LINE_YOFFSET );
		m_pChatBubbleRight->Render( m_iRenderXPos + m_pChatBubbleCenter->GetWidth(), m_iRenderYPos + ONE_LINE_YOFFSET );
	}
	else if( iSzie == 2 )
	{
		m_pChatBubbleCenter->Render( m_iRenderXPos, m_iRenderYPos + TWO_LINE_YOFFSET );
		m_pChatBubbleRight->Render( m_iRenderXPos + m_pChatBubbleCenter->GetWidth(), m_iRenderYPos + TWO_LINE_YOFFSET );
	}

	return true;
}

void ioCharEmoticon::RenderChatText()
{
	if( m_vChatTextVec.empty() )
		return;

	enum 
	{
		ONE_LINE_TEXT_XOFFSET     =  26,
		ONE_LINE_TEXT_YOFFSET     = -10,

		TWO_LINE_TEXT_1_XOFFSET   =  26,
		TWO_LINE_TEXT_1_YOFFSET   = -18,
		TWO_LINE_TEXT_2_XOFFSET   =  26,
		TWO_LINE_TEXT_2_YOFFSET   =  -2,

		MAX_TEXT_WIDTH            = 95,
	};

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );

	int iSzie = m_vChatTextVec.size();
	if( iSzie == 1 )
	{
		g_FontMgr.PrintText( m_iRenderXPos + ONE_LINE_TEXT_XOFFSET, m_iRenderYPos + ONE_LINE_TEXT_YOFFSET, FONT_SIZE_13, m_vChatTextVec[0].c_str() );
	}
	else if( iSzie == 2 )
	{
		g_FontMgr.PrintText( m_iRenderXPos + TWO_LINE_TEXT_1_XOFFSET, m_iRenderYPos + TWO_LINE_TEXT_1_YOFFSET, FONT_SIZE_13, m_vChatTextVec[0].c_str() );
		g_FontMgr.PrintTextWidthCut( m_iRenderXPos + TWO_LINE_TEXT_2_XOFFSET, m_iRenderYPos + TWO_LINE_TEXT_2_YOFFSET, FONT_SIZE_13, MAX_TEXT_WIDTH, m_vChatTextVec[1].c_str() );
	}
}

void ioCharEmoticon::SetChatTextBubble( const ioHashString &rszChatText )
{
	enum 
	{
		MAX_TEXT_WIDTH        = 95,
		ONE_LINE_WIDTH_OFFSET = 26,
		TWO_LINE_WIDTH_OFFSET = 21,
		WIDTH_OFFSET          = 5,
	};

	SAFEDELETE( m_pChatBubbleCenter );
	SAFEDELETE( m_pChatBubbleRight );
	m_vChatTextVec.clear();

	int iWidth = g_FontMgr.GetTextWidth( rszChatText.c_str(), TS_OUTLINE_FULL_2X, FONT_SIZE_13 );

	// 1줄에 모두 기록
	if( iWidth <= MAX_TEXT_WIDTH ) 
	{
		m_vChatTextVec.push_back( rszChatText );

		// image
//		m_pChatBubbleCenter = g_UIImageSetMgr.CreateImageByFullName( ioHashString( "ImageSet003#Chatbox1_Center" ) );
//		m_pChatBubbleRight  = g_UIImageSetMgr.CreateImageByFullName( ioHashString( "ImageSet003#Chatbox1_Right" ) );
		if( m_pChatBubbleCenter )
		{
			m_pChatBubbleCenter->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pChatBubbleCenter->SetSize( iWidth+ONE_LINE_WIDTH_OFFSET, m_pChatBubbleCenter->GetHeight() );
		}
		return;
	}

	// 2줄에 나누어서 기록
	int iOneLineWidth = iWidth / 2;
	if( iOneLineWidth > MAX_TEXT_WIDTH )
		iOneLineWidth = MAX_TEXT_WIDTH;
	else
		iOneLineWidth += WIDTH_OFFSET; // 한글이 짤릴때 1번째 라인에 글자 길이를 길게 하기 위해서

	char szFirstDst[MAX_PATH]="";
	int iFirstLineSize = Help::StringCutFun( FONT_SIZE_13, iOneLineWidth, TS_OUTLINE_FULL_2X, szFirstDst, sizeof(szFirstDst), rszChatText.c_str() );
	m_vChatTextVec.push_back( szFirstDst );
	char szSecondDst[MAX_PATH]="";
	
	int iChatSize = rszChatText.Length();
 	int iSecondCnt = 0;
 	for (int i = iFirstLineSize; i < iChatSize ; i++) // 다음 어레이 부터 설정해야 하므로 size값을 그대로사용한다. i = iFirstLineSize
 	{
 		szSecondDst[iSecondCnt] = rszChatText.At(i);	
		iSecondCnt++;
		if( iSecondCnt >= MAX_PATH )
			break;
 	}

	m_vChatTextVec.push_back( szSecondDst );

	// image
//	m_pChatBubbleCenter = g_UIImageSetMgr.CreateImageByFullName( ioHashString( "ImageSet003#Chatbox2_Center" ) );
//	m_pChatBubbleRight  = g_UIImageSetMgr.CreateImageByFullName( ioHashString( "ImageSet003#Chatbox2_Right" ) );
	if( m_pChatBubbleCenter )
	{
		m_pChatBubbleCenter->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pChatBubbleCenter->SetSize( iOneLineWidth+TWO_LINE_WIDTH_OFFSET, m_pChatBubbleCenter->GetHeight() );
	}
}

void ioCharEmoticon::AddReserveEmoticon( EmoticonInfo* pInfo )
{
	if( !pInfo ) return;

	m_ReserveEmoticonList.push_back( pInfo );

	std::sort( m_ReserveEmoticonList.begin(), m_ReserveEmoticonList.end(), EmoticonInfoSort() );
}

EmoticonInfo* ioCharEmoticon::GetEmoticonInfoByList()
{
	int iSize = m_ReserveEmoticonList.size();
	if( iSize == 0 ) return NULL;

	EmoticonInfoList::iterator iter = m_ReserveEmoticonList.begin();

	EmoticonInfo *pInfo = (*iter);
	m_ReserveEmoticonList.erase( iter );

	return pInfo;
}

void ioCharEmoticon::UpdateInfoList()
{
	DWORD dwCurTime = FRAMEGETTIME();
	EmoticonInfoList::iterator iter = m_ReserveEmoticonList.begin();

	while( iter != m_ReserveEmoticonList.end() )
	{
		EmoticonInfo* pInfo = (*iter);
		if( pInfo->m_dwStartTime > 0 )
		{
			DWORD dwEndTime = pInfo->m_dwStartTime + pInfo->m_dwDuration;
			if( dwEndTime < dwCurTime )
			{
				SAFEDELETE(pInfo);
				iter = m_ReserveEmoticonList.erase(iter);
				continue;
			}
		}

		++iter;
	}
}

void ioCharEmoticon::SetNewEmoticon( const ioHashString &szEmoticon,
									 ioComplexStringPrinter DescString,
									 EmoticonType eType,
									 int iPriority,
									 bool bOwnerChar,
									 DWORD dwDuration )
{
	if( szEmoticon.IsEmpty() )	return;
	ioUIRenderImage *pEmoticon = g_UIImageSetMgr.CreateImageByFullName( szEmoticon );
	if( !pEmoticon ) return;

	EmoticonInfo *pInfo = new EmoticonInfo;
	if( !pInfo ) return;

	pInfo->m_EmoticonName = szEmoticon;
	if( !DescString.IsEmpty() )
		DescString.CopyFullText( pInfo->m_DescString  );
	pInfo->m_Type = eType;
	pInfo->m_bOwner = bOwnerChar;
	pInfo->m_iPriority = iPriority;
	pInfo->m_dwStartTime = FRAMEGETTIME();

	// Duration
	switch( eType )
	{
	case EMT_DEFAULT:
		pInfo->m_dwDuration = m_dwDefaultDuration;
		break;
	case EMT_CHAT_ICON:
		pInfo->m_dwDuration = m_dwChatIconDuration;
		break;
	case EMT_KILL_POINT:
	case EMT_GAME_EVENT:
	case EMT_HIT_COMBO:
	case EMT_USER_RANK:
		pInfo->m_dwDuration = m_dwLongDuration;
		break;
	case EMT_MOVIE_EVENT:
		pInfo->m_dwDuration = m_dwLongDuration;
		break;
	case EMT_SKILL_BUFF:
		pInfo->m_dwDuration = m_dwDefaultDuration;
		break;
	case EMT_BAD_STATE:
		pInfo->m_dwDuration = m_dwDefaultDuration;
		break;
	}

	// 시스템에서 시간을 설정하였으면 그 시간으로 설정
	if( dwDuration > 0 )
		pInfo->m_dwDuration = dwDuration;


	if( m_pCurEmoticonInfo )
	{
		if( m_pCurEmoticonInfo->m_iPriority < pInfo->m_iPriority )
		{
			if( pInfo->m_Type == EMT_NAMED_UNLIMITED_ICON )
				AddReserveEmoticon( pInfo );
			else
				SAFEDELETE(pInfo);

			SAFEDELETE(pEmoticon);
			return;
		}
		else
		{
			if( m_pCurEmoticonInfo->m_Type == EMT_NAMED_UNLIMITED_ICON )
				AddReserveEmoticon( m_pCurEmoticonInfo );
			else
				SAFEDELETE( m_pCurEmoticonInfo );

			m_pCurEmoticonInfo = pInfo;
		}
	}
	else
	{
		m_pCurEmoticonInfo = pInfo;
	}

	SAFEDELETE( m_pEmoticon );
	SAFEDELETE( m_pEmoticonNum );

	m_pEmoticon = pEmoticon;

	CheckEmoticonBackImage();

	// Scale
	switch( eType )
	{
	case EMT_DEFAULT:
		m_fCurStartScale = m_fEmoticonEndScale;
		m_dwCurScaleDuration = 0;
		break;
	case EMT_CHAT_ICON:
		m_fCurStartScale = m_fEmoticonEndScale;
		m_dwCurScaleDuration = 0;
		break;
	case EMT_KILL_POINT:
	case EMT_GAME_EVENT:
	case EMT_HIT_COMBO:
	case EMT_USER_RANK:
		if( m_bOwnerChar )
		{
			m_fCurStartScale = m_fOwnerGameEventScale;
			m_dwCurScaleDuration = m_dwOwnerGameEventTime;
		}
		else
		{
			m_fCurStartScale = m_fOtherGameEventScale;
			m_dwCurScaleDuration = m_dwOtherGameEventTime;
		}
		break;
	case EMT_MOVIE_EVENT:
		if( m_bOwnerChar )
		{
			m_fCurStartScale = m_fOwnerMovieEventScale;
			m_dwCurScaleDuration = m_dwOwnerGameEventTime;
		}
		else
		{
			m_fCurStartScale = m_fOtherMovieEventScale;
			m_dwCurScaleDuration = m_dwOtherGameEventTime;
		}
		break;
	case EMT_SKILL_BUFF:
		if( m_bOwnerChar )
		{
			m_fCurStartScale = m_fOwnerSkillBuffScale;
			m_dwCurScaleDuration = m_dwOwnerSkillBuffTime;
		}
		else
		{
			m_fCurStartScale = m_fOtherSkillBuffScale;
			m_dwCurScaleDuration = m_dwOtherSkillBuffTime;
		}
		break;
	case EMT_BAD_STATE:
		if( m_bOwnerChar )
		{
			m_fCurStartScale = m_fOwnerBadStateScale;
			m_dwCurScaleDuration = m_dwOwnerBadStateTime;
		}
		else
		{
			m_fCurStartScale = m_fOtherBadStateScale;
			m_dwCurScaleDuration = m_dwOtherBadStateTime;
		}
		break;
	}

	if( m_pEmoticon )
		m_pEmoticon->SetScale( m_fCurStartScale );

	m_State = ES_SCALE;
}

void ioCharEmoticon::UpdateNewEmoticon( const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vCamTargetPos, bool bVisible )
{
	if( m_State == ES_NONE )
	{
		m_bNeedRender = false;
		return;
	}

	if( m_State != ES_3D_CHAT )
	{
		if( !m_pEmoticon || !m_pCurEmoticonInfo )
		{
			m_bNeedRender = false;
			return;
		}
	}

	m_bNeedRender = true;

	DWORD dwPastTime = FRAMEGETTIME() - m_pCurEmoticonInfo->m_dwStartTime;
	DWORD dwCurDuration = m_pCurEmoticonInfo->m_dwDuration;

	m_RenderType = UI_RENDER_NORMAL;

	int iColor = 0;
	float fAlphaRate = FLOAT1;
	float fScale = m_fEmoticonEndScale;

	switch( m_State )
	{
	case ES_SCALE:
		if( dwPastTime < m_dwCurScaleDuration )
		{
			float fScaleRate = (float)dwPastTime / (float)m_dwCurScaleDuration;
			fScale = m_fCurStartScale * ( FLOAT1 - fScaleRate ) +
				m_fEmoticonEndScale * fScaleRate;

			m_pEmoticon->SetScale( fScale );
			if( m_Type == EMT_HIT_COMBO )
			{
				if( m_pEmoticonNum )
					m_pEmoticonNum->SetScale( fScale );
			}
		}
		else
		{
			m_State = ES_DURATION;
			m_pEmoticon->SetScale( m_fEmoticonEndScale );

			if( m_Type == EMT_HIT_COMBO )
			{
				if( m_pEmoticonNum )
					m_pEmoticonNum->SetScale( m_fEmoticonEndScale );
				if( m_iHitComboCount > m_iCurHitComboCount )
				{					
					ReSetHitCombo();
					m_bNeedRender = false;
					return;
				}
			}
		}
		break;
	case ES_DURATION:
		if( dwPastTime > dwCurDuration )
		{
			m_State = ES_END_WHITE;
		}
		break;
	case ES_END_WHITE:
		if( dwCurDuration + m_dwDisapearWhiteTime > dwPastTime )
		{
			dwPastTime -= dwCurDuration;

			iColor = 255.0f * ( (float)dwPastTime / (float)m_dwDisapearWhiteTime );
			m_pEmoticon->SetColor( iColor, iColor, iColor );
			if( m_pEmoticonNum )
				m_pEmoticonNum->SetColor( iColor, iColor, iColor );
		}
		else
		{
			m_State = ES_END_ALPHA;
			iColor = 255;
			m_pEmoticon->SetColor( 255, 255, 255 );
			if( m_pEmoticonNum )
				m_pEmoticonNum->SetColor( 255, 255, 255 );
		}

		m_RenderType = UI_RENDER_NORMAL_ADD_COLOR;
		break;
	case ES_END_ALPHA:
		if( dwCurDuration + m_dwDisapearWhiteTime + m_dwDisapearAlphaTime > dwPastTime )
		{
			dwPastTime -= dwCurDuration + m_dwDisapearWhiteTime;

			iColor = 255;
			fAlphaRate = FLOAT1 - (float)dwPastTime / (float)m_dwDisapearAlphaTime;
			m_pEmoticon->SetAlpha( (BYTE)(int)(255.0f * fAlphaRate) );
			if( m_pEmoticonNum )
				m_pEmoticonNum->SetAlpha( (BYTE)(int)(255.0f * fAlphaRate) );
		}
		else
		{
			EndEmoticon();
			m_bNeedRender = false;
			return;
		}

		m_RenderType = UI_RENDER_NORMAL_ADD_COLOR;
		break;
	case ES_3D_CHAT:
		if( dwPastTime > dwCurDuration )
		{
			EndEmoticon();
			m_bNeedRender = false;
			return;
		}
		break;
	}

	m_iRenderXPos = m_iRenderYPos = 0;
	m_bInScreen = true;
	m_bVisible = bVisible;

	if( !Setting::Check2DInScreen( vPos, m_iRenderXPos, m_iRenderYPos ) )
	{
		m_bInScreen = false;
	}

	if( !m_bInScreen )
	{
		if( m_Type == EMT_CHAT_ICON || m_Type == EMT_3D_CHAT_ICON )
		{
			CheckOutScreen( vPos, vCamTargetPos, m_iRenderXPos, m_iRenderYPos, fScale, true );
		}
		else
		{
			m_bNeedRender = false;
			return;
		}
	}
	else if( m_bInScreen && !m_bVisible )
	{
		if( m_Type == EMT_CHAT_ICON || m_Type == EMT_3D_CHAT_ICON )
		{
			CheckOutScreen( vPos, vCamTargetPos, m_iRenderXPos, m_iRenderYPos, fScale, false );
		}
		else
		{
			m_bNeedRender = false;
			return;
		}
	}
	else
	{
		m_iRenderXPos -= m_iEmoticonXPos;
		m_iRenderYPos -= m_iEmoticonYPos;
	}

	if( m_pEmoticonNum || m_pKillPointNum )
	{
		m_iNumberOffset = 6;

		if( dwPastTime < m_dwCurScaleDuration )
		{
			float fRate = (float)dwPastTime / (float)m_dwCurScaleDuration;
			m_iNumberOffset -= (int)( 6.0f * fRate );
		}

		m_fNumberScale = fScale;
	}

	if( m_Type == EMT_3D_CHAT_ICON && m_p3DChatEmoticon )
	{
		m_p3DChatEmoticon->Update( m_iRenderXPos, m_iRenderYPos );
	}

	m_fNamedAlpha = fAlphaRate;
	m_fNamedScale = fScale;
	m_iNamedBkColor = iColor;
}

void ioCharEmoticon::EndNewEmoticon()
{
	if( m_pCurEmoticonInfo )
		SAFEDELETE(m_pCurEmoticonInfo);

	SAFEDELETE( m_pEmoticon );
	SAFEDELETE( m_pEmoticonNum );
	SAFEDELETE( m_pKillPointNum );

	m_dwCurScaleDuration = 0;
	m_fCurStartScale = 0.0f;

	m_State = ES_NONE;

	m_vChatTextVec.clear();
	SAFEDELETE( m_pChatBubbleCenter );
	SAFEDELETE( m_pChatBubbleRight );

	SetReserveEmoticon();
}

void ioCharEmoticon::SetReserveEmoticon()
{
	EmoticonInfo *pInfo = GetEmoticonInfoByList();
	if( !pInfo ) return;

	ioUIRenderImage *pEmoticon = g_UIImageSetMgr.CreateImageByFullName( pInfo->m_EmoticonName );
	if( !pEmoticon )
	{
		SAFEDELETE(pInfo);
		return;
	}

	m_pCurEmoticonInfo = pInfo;
	m_pEmoticon = pEmoticon;

	CheckEmoticonBackImage();

	// Scale
	m_fCurStartScale = m_fEmoticonEndScale;
	m_dwCurScaleDuration = 0;

	if( m_pEmoticon )
		m_pEmoticon->SetScale( m_fCurStartScale );

	switch( m_pCurEmoticonInfo->m_Type )
	{
	case EMT_NAMED_UNLIMITED_ICON:
		m_State = ES_UNLIMITED;
		break;
	default:
		m_State = ES_SCALE;
		break;
	}
}

void ioCharEmoticon::CheckEmoticonDesc( float fAlphaRate, float fScale )
{
	if( !m_pCurEmoticonInfo )
		return;

	BYTE iAlpha = (BYTE)(int)(127.0f * fAlphaRate);
	BYTE iAlpha2 = (BYTE)(int)(255.0f * fAlphaRate);
	float fCurSize = 0.0f;
	float fOffSet = 0.0f;

	fCurSize = 0.66f * (fScale / m_fEmoticonEndScale);
	fOffSet -= 35.0f * (fScale / m_fEmoticonEndScale);
	fOffSet -= 9.0f * (fScale / m_fEmoticonEndScale);

	g_FontMgr.SetAlignType( (TextAlignType)m_iEmoticonNameAlignType );
	g_FontMgr.SetBkColor( m_iNamedBkColor, m_iNamedBkColor, m_iNamedBkColor );

	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetTextColor( 0, 0, 0 );

	if( !m_pCurEmoticonInfo->m_DescString.IsEmpty() )
	{
		DWORD dwBkColor = D3DCOLOR_XRGB( m_iNamedBkColor, m_iNamedBkColor, m_iNamedBkColor );
		DWORD dwTextColor = D3DCOLOR_XRGB( 0, 0, 0 );

		m_pCurEmoticonInfo->m_DescString.PrintFullModifyText( m_iRenderXPos + m_iEmoticonNameOffsetX,
															  m_iRenderYPos + fOffSet + m_iEmoticonNameOffsetY,
															  dwTextColor,
															  TS_OUTLINE_FULL_2X,
															  (TextAlignType)m_iEmoticonNameAlignType,
															  iAlpha );

		if( m_iNamedBkColor > 0 )
		{
			dwTextColor = D3DCOLOR_XRGB( m_iNamedBkColor, m_iNamedBkColor, m_iNamedBkColor );

			m_pCurEmoticonInfo->m_DescString.PrintFullModifyText( m_iRenderXPos + m_iEmoticonNameOffsetX,
																  m_iRenderYPos + fOffSet + m_iEmoticonNameOffsetY,
																  dwTextColor,
																  TS_OUTLINE_FULL_2X,
																  (TextAlignType)m_iEmoticonNameAlignType,
																  iAlpha2 );
		}
		else
		{
			m_pCurEmoticonInfo->m_DescString.PrintFullText( m_iRenderXPos + m_iEmoticonNameOffsetX,
															m_iRenderYPos + fOffSet + m_iEmoticonNameOffsetY,
															(TextAlignType)m_iEmoticonNameAlignType,
															iAlpha2 );
		}
	}
}


