

#include "stdafx.h"

#include "ioCreateChar.h"
#include "RaceSetting.h"
#include "CharacterInfo.h"

#include "io3DCharEmoticon.h"

io3DCharEmoticon::TalkSoundMap io3DCharEmoticon::m_TalkSoundMap;

float io3DCharEmoticon::m_fCharScale = FLOAT1;
int io3DCharEmoticon::m_iXOffset = 0;
int io3DCharEmoticon::m_iYOffset = 0;

ioHashString io3DCharEmoticon::m_FacePostfix;
ioHashString io3DCharEmoticon::m_KillFace;
ioHashString io3DCharEmoticon::m_DieFace;
ioHashString io3DCharEmoticon::m_ActionFace;
ioHashString io3DCharEmoticon::m_FaceChangeType;
DWORD io3DCharEmoticon::m_dwChangeStartGap = 0;
DWORD io3DCharEmoticon::m_dwChangeDuration = 0;
DWORD io3DCharEmoticon::m_dwChangeOnOffGap = 0;

DWORD io3DCharEmoticon::m_dwSpecialStartGap = 0;
DWORD io3DCharEmoticon::m_dwSpecialDuration = 0;
DWORD io3DCharEmoticon::m_dwSpecialOnOffGap = 0;

float io3DCharEmoticon::m_fHairOutLineThick = FLOAT1;
float io3DCharEmoticon::m_fHeadOutLineThick = FLOAT1;
bool  io3DCharEmoticon::m_bChangeOutLineThick = false;

io3DCharEmoticon::io3DCharEmoticon()
{
	m_pUI3DRender = NULL;
	m_pAvata = NULL;

	m_FaceChange = FC_READY;
	m_dwSetStart = 0;

	m_iMaxChangeCount = 1;
	m_iCurChangeCount = 0;

	m_SpecialType = SPA_NONE;
}

io3DCharEmoticon::~io3DCharEmoticon()
{
	SAFEDELETE( m_pAvata );
	SAFEDELETE( m_pUI3DRender );
}

void io3DCharEmoticon::LoadStaticValue( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "3d_chat_emoticon" );

	m_fCharScale = rkLoader.LoadFloat_e( "scale", FLOAT1 );
	m_iXOffset   = rkLoader.LoadInt_e( "x_offset", -40 );
	m_iYOffset   = rkLoader.LoadInt_e( "y_offset", -525 );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "change_face_postfix", "", szBuf, MAX_PATH );
	m_FacePostfix = szBuf;
	rkLoader.LoadString_e( "change_face_kill", "", szBuf, MAX_PATH );
	m_KillFace = szBuf;
	rkLoader.LoadString_e( "change_face_die", "", szBuf, MAX_PATH );
	m_DieFace = szBuf;
	rkLoader.LoadString_e( "change_face_action", "", szBuf, MAX_PATH );
	m_ActionFace = szBuf;

	rkLoader.LoadString( "change_face_type", "all", szBuf, MAX_PATH );
	m_FaceChangeType = szBuf;

	m_dwChangeStartGap = rkLoader.LoadInt_e( "change_start_gap", 0 );
	m_dwChangeDuration = rkLoader.LoadInt_e( "change_duration", 1000 );
	m_dwChangeOnOffGap = rkLoader.LoadInt_e( "change_on_off_gap", 100 );

	m_dwSpecialStartGap = rkLoader.LoadInt_e( "special_start_gap", 0 );
	m_dwSpecialDuration = rkLoader.LoadInt_e( "special_duration", 1000 );
	m_dwSpecialOnOffGap = rkLoader.LoadInt_e( "special_on_off_gap", 100 );

	m_fHairOutLineThick = rkLoader.LoadFloat_e( "hair_outline_thick", FLOAT1 );
	m_fHeadOutLineThick = rkLoader.LoadFloat_e( "head_outline_thick", FLOAT1 );
	m_bChangeOutLineThick = rkLoader.LoadBool_e( "outline_change", false );

	rkLoader.SetTitle_e( "talk_sound" );
	int iTalkLevelCount = rkLoader.LoadInt_e( "talk_word_level_count", 0 );
	for( int i=0 ; i<iTalkLevelCount ; i++ )
	{
		char szTitle[MAX_PATH];
		wsprintf_e( szTitle, "talk_word_level%d", i+1 );

		rkLoader.SetTitle( szTitle );
		int iLimitWord = rkLoader.LoadInt_e( "limit_word", 0 );
		int iTalkSoundCount = rkLoader.LoadInt_e( "sound_count", 0 );

		if( iLimitWord == 0 || iTalkSoundCount == 0 )
			continue;

		SoundList vSoundList;
		char szKey[MAX_PATH], szBuf[MAX_PATH];
		for( int j=0 ; j<iTalkSoundCount ; j++ )
		{
			wsprintf_e( szKey, "talk_sound%d", j+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			if( strcmp( szBuf, "" ) )
			{
				vSoundList.push_back( ioHashString( szBuf ) );
			}
		}

		if( !vSoundList.empty() )
		{
			m_TalkSoundMap[iLimitWord] = vSoundList;
		}
	}
}

void io3DCharEmoticon::Initialize( const ioHashString &rkCamName )
{
	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", rkCamName.c_str() );

	m_SpecialType = SPA_NONE;
}

void io3DCharEmoticon::SetAvata( const CHARACTER &rkInfo, int iChatLen, bool bNoSound, SpecialAtcionType iType )
{
	if( !m_pUI3DRender )	return;

	SAFEDELETE( m_pAvata );

	m_pAvata = m_pUI3DRender->CreateChar( rkInfo, "EmoticonChar", false );
	if( !m_pAvata )	return;

	m_pAvata->DestroyHandAndBody();

	RaceSetting::SetCurRaceDetail( rkInfo );
	m_pAvata->SetPosition( RaceSetting::AvataPosition() );
	m_pAvata->SetPitch( RaceSetting::AvataAngleX() );


	m_pAvata->SetYaw( RaceSetting::AvataAngleY() );
	m_pAvata->SetScale( m_fCharScale, m_fCharScale, m_fCharScale );

	m_pUI3DRender->SetCameraPos( RaceSetting::AvataCamPosition() );

	m_pAvata->SetLoopAni( "public_delay.ani" );
	m_pAvata->Update( 0.01f );

	ioHashString szFace = m_FacePostfix;
	m_FaceChange = FC_READY;
	m_dwSetStart = FRAMEGETTIME();
	m_SpecialType = iType;

	switch( iType )
	{
	case SPA_NONE:
		break;
	case SPA_KILL:
		szFace = m_KillFace;
		break;
	case SPA_DIE:
		szFace = m_DieFace;
		break;
	case SPA_FALL:
	case SPA_ACTION:
		szFace = m_ActionFace;
		break;
	}

	bool bMale = false;
	if( rkInfo.m_sex == 1 )
		bMale = true;
	ioHashString szNewFaceName;
	bool bGetNameOK = GetFaceChangeName( szNewFaceName,
										 szFace.c_str(),
										 bMale,
										 rkInfo.m_face );

	if( !bMale )
	{
		char szFullName[MAX_PATH]="";
		StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\texture\\%s" , szNewFaceName.c_str() );
		if( !g_ResourceLoader.IsFileExist( szFullName ) )
		{
			bGetNameOK = GetFaceChangeName( szNewFaceName, szFace.c_str(), true, rkInfo.m_face ); // 남자로 설정.
		}
	}

	if( bGetNameOK )
	{
		m_ChangeFaceTex = szNewFaceName;
	}
	else
	{
		m_ChangeFaceTex.Clear();
	}

	if( m_bChangeOutLineThick )
	{
		m_pAvata->ChangeHairOutLineThick( m_fHairOutLineThick );
		m_pAvata->ChangeHeadOutLineThick( m_fHeadOutLineThick );
	}

	m_iMaxChangeCount = ( iChatLen + 1 ) / 2;

	if( !bNoSound )
	{
		PlayTalkSound( m_iMaxChangeCount );
	}
	else
	{
		m_CurChatSound.Clear();
	}

	m_iMaxChangeCount = max( 1, m_iMaxChangeCount ) * 2;

	switch( iType )
	{
	case SPA_KILL:
	case SPA_DIE:
	case SPA_FALL:
	case SPA_ACTION:
		m_iMaxChangeCount = 2;
		break;
	}

	m_iCurChangeCount = 0;
}

void io3DCharEmoticon::SetAvata( const CHARACTER &rkInfo, int iChatLen, bool bNoSound, SpecialAtcionType iType, int iViewPortW, int iViewPortH )
{
	if( !m_pUI3DRender )	return;

	SAFEDELETE( m_pAvata );

	m_pAvata = m_pUI3DRender->CreateChar( rkInfo, "ResultChar", false );
	if( !m_pAvata )	return;

	m_pAvata->DestroyHandAndBody();
	m_pAvata->SetPosition( 0.0f, -138.0f, 800.0f );
	m_pAvata->SetYaw( FLOAT10 );
	m_pAvata->RotateY( -15.0f );
	m_pUI3DRender->SetLookAt( 0.0f, 0.0f, 900.0f );

	float fAspect = (float)iViewPortW/(float)iViewPortH;
	m_pUI3DRender->SetProjection( FLOAT1, 600.0f, 45.0f, fAspect );

	ioHashString szFace = m_FacePostfix;
	m_FaceChange = FC_READY;
	m_dwSetStart = FRAMEGETTIME();
	m_SpecialType = iType;

	switch( iType )
	{
	case SPA_NONE:
		break;
	case SPA_KILL:
		szFace = m_KillFace;
		break;
	case SPA_DIE:
		szFace = m_DieFace;
		break;
	case SPA_FALL:
	case SPA_ACTION:
		szFace = m_ActionFace;
		break;
	}

	bool bMale = false;
	if( rkInfo.m_sex == 1 )
		bMale = true;
	ioHashString szNewFaceName;
	bool bGetNameOK = GetFaceChangeName( szNewFaceName,
		szFace.c_str(),
		bMale,
		rkInfo.m_face );

	if( !bMale )
	{
		char szFullName[MAX_PATH]="";
		StringCbPrintf( szFullName, sizeof( szFullName ), "resource\\texture\\%s" , szNewFaceName.c_str() );
		if( !g_ResourceLoader.IsFileExist( szFullName ) )
		{
			bGetNameOK = GetFaceChangeName( szNewFaceName, szFace.c_str(), true, rkInfo.m_face ); // 남자로 설정.
		}
	}

	if( bGetNameOK )
	{
		m_ChangeFaceTex = szNewFaceName;
	}
	else
	{
		m_ChangeFaceTex.Clear();
	}

	if( m_bChangeOutLineThick )
	{
		m_pAvata->ChangeHairOutLineThick( m_fHairOutLineThick );
		m_pAvata->ChangeHeadOutLineThick( m_fHeadOutLineThick );
	}

	m_iMaxChangeCount = ( iChatLen + 1 ) / 2;

	if( !bNoSound )
	{
		PlayTalkSound( m_iMaxChangeCount );
	}
	else
	{
		m_CurChatSound.Clear();
	}

	m_iMaxChangeCount = max( 1, m_iMaxChangeCount ) * 2;

	switch( iType )
	{
	case SPA_KILL:
	case SPA_DIE:
	case SPA_FALL:
	case SPA_ACTION:
		m_iMaxChangeCount = 2;
		break;
	}

	m_iCurChangeCount = 0;
}

void io3DCharEmoticon::PlayTalkSound( int iChangeCount )
{
	if( m_TalkSoundMap.empty() || iChangeCount == 0 )
		return;

	TalkSoundMap::iterator iter = m_TalkSoundMap.begin();
	for( ; iter!=m_TalkSoundMap.end() ; ++iter )
	{
		if( iter->first >= iChangeCount )
		{
			PlayRandomTalk( iter->second );
			return;
		}
	}

	// 못찾았다.
	iter = m_TalkSoundMap.end();
	--iter;

	PlayRandomTalk( iter->second );
}

void io3DCharEmoticon::PlayRandomTalk( const SoundList &rkList )
{
	m_CurChatSound.Clear();

	if( !rkList.empty() )
	{
		int iSoundCount = rkList.size();
		int iSoundIdx = rand() % iSoundCount;
		m_CurChatSound = rkList[iSoundIdx];
	}
}

void io3DCharEmoticon::Update( int iXPos, int iYPos )
{
	m_iXPos = iXPos;
	m_iYPos = iYPos;

	if( m_ChangeFaceTex.IsEmpty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwSetStart > dwCurTime )
		return;

	DWORD dwPastTime = dwCurTime - m_dwSetStart;
	DWORD dwStartGap = m_dwChangeStartGap;
	DWORD dwDuration = m_dwChangeDuration;
	DWORD dwOnOffGap = m_dwChangeOnOffGap;

	if( m_SpecialType != SPA_NONE )
	{
		dwStartGap = m_dwSpecialStartGap;
		dwDuration = m_dwSpecialDuration;
		dwOnOffGap = m_dwSpecialOnOffGap;
	}

	switch( m_FaceChange )
	{
	case FC_READY:
		if( dwPastTime >= dwStartGap )
		{
			m_pAvata->ChangeFaceTexture( m_ChangeFaceTex );
			m_FaceChange = FC_DURATION;
			m_iCurChangeCount = 1;
			g_SoundMgr.PlaySound( m_CurChatSound );
		}
		break;
	case FC_DURATION:
		if( dwPastTime >= dwStartGap + dwDuration )
		{
			// 현재 짝수라면 이미 복구되어 있다.
			if( m_iCurChangeCount & 1 )
			{
				m_pAvata->RestoreDefaultFace();
			}

			m_FaceChange = FC_DONE;
		}
		else
		{
			if( m_iMaxChangeCount > m_iCurChangeCount )
			{
				DWORD dwOnOffTime = dwStartGap + m_iCurChangeCount * dwOnOffGap;
				if( dwPastTime > dwOnOffTime )
				{
					m_iCurChangeCount++;

					if( m_iCurChangeCount & 1 )
					{
						m_pAvata->ChangeFaceTexture( m_ChangeFaceTex );

					}
					else
					{
						m_pAvata->RestoreDefaultFace();
					}
				}
			}
		}
		break;
	case FC_DONE:
		break;
	}
}

void io3DCharEmoticon::Render()
{
	if( m_pAvata && m_pUI3DRender )
	{
		D3DXMATRIX matUI = m_pUI3DRender->MakeUIProjection( m_iXPos + m_iXOffset, m_iYPos + m_iYOffset );			
		m_pUI3DRender->Render( &matUI, true );
	}
}

void io3DCharEmoticon::RenderViewPort( int iXPos, int iYPos, int iViewWidth, int iViewHeight )
{
	if( m_pAvata && m_pUI3DRender )
	{
		D3DRECT rcD3D;
		rcD3D.x1 = iXPos;
		rcD3D.y1 = iYPos;
		rcD3D.x2 = rcD3D.x1 + iViewWidth;
		rcD3D.y2 = rcD3D.y1 + iViewHeight;
		m_pUI3DRender->RenderViewPort( &rcD3D, true );
	}
}



