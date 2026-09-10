#include "stdafx.h"

#include "GUI/GameOptionWnd.h"
#include "GUI/GameOptionSettingTab.h"

#include "ioMovieMaker.h"
#include <sys/stat.h>

template<> ioMovieMaker* Singleton< ioMovieMaker >::ms_Singleton = 0;

ioMovieMaker::ioMovieMaker()
{
	m_bMovieCapture = false;
	m_dwMovieStartCurTime = 0;
	m_dwMovieStartTime = 0;
	m_iMovieStartSec   =-1;
	m_dwMovieCutDelayCurTime = 0;
	m_dwMovieCutDelayTime    = 0;
	m_dwMoviePlayTime        = 0;
	m_dwScreenShotCurTime	 = 0;
	m_dwCompleteTime		 = 0;
	m_bRecDotShow = false;
	m_dwRecDotShowTime = 0;
	memset( m_szSaveFileInfo, 0, sizeof( m_szSaveFileInfo ) );

	m_pBackground      = NULL;
	m_pRecDot          = NULL;
	m_pMovieIcon       = NULL;
	m_pKeyOutline      = NULL;
	m_pScreenShotIcon  = NULL;

	m_hMovieFile = NULL;
	memset( &m_lFileSize, 0, sizeof(LARGE_INTEGER) );
	m_bCreateFile = false;
}

ioMovieMaker::~ioMovieMaker()
{	
	SAFEDELETE( m_pBackground );
	SAFEDELETE( m_pRecDot );
	SAFEDELETE( m_pMovieIcon );
	SAFEDELETE( m_pKeyOutline );
	SAFEDELETE( m_pScreenShotIcon );
	if( m_CapBandiLibrary.IsCapturing() )
	{
		CloseFileHandle();
		m_CapBandiLibrary.Stop();
	}
//	m_CapBandiLibrary.Destory();
}

ioMovieMaker& ioMovieMaker::GetSingleton()
{
	return Singleton< ioMovieMaker >::GetSingleton();
}

void ioMovieMaker::Initialize()
{
	ioINILoader_e kLoader( "config/sp2_movie_maker.ini" );
	char szBuf[MAX_PATH];
	kLoader.SetTitle_e( "Info" );
	m_dwMovieStartTime = kLoader.LoadInt_e( "StartTime", 0 );
	m_dwMovieCutDelayTime = max( 1000, kLoader.LoadInt_e( "CutDelayTime", 1000 ) );
	m_dwCompleteTime = kLoader.LoadInt_e( "CompleteTime", 2000 );
	

	kLoader.SetTitle_e( "Sound" );
//	kLoader.LoadString_e( "Count_5", "", szBuf, MAX_PATH );
//	m_vMovieSoundCount.push_back( szBuf );
//	kLoader.LoadString_e( "Count_4", "", szBuf, MAX_PATH );
//	m_vMovieSoundCount.push_back( szBuf );
	kLoader.LoadString_e( "Count_3", "", szBuf, MAX_PATH );
	m_vMovieSoundCount.push_back( szBuf );
	kLoader.LoadString_e( "Count_2", "", szBuf, MAX_PATH );
	m_vMovieSoundCount.push_back( szBuf );
	kLoader.LoadString_e( "Count_1", "", szBuf, MAX_PATH );
	m_vMovieSoundCount.push_back( szBuf );
	kLoader.LoadString_e( "Count_Start", "", szBuf, MAX_PATH );
	m_vMovieSoundCount.push_back( szBuf );
	kLoader.LoadString_e( "Count_Cut", "", szBuf, MAX_PATH );
	m_vMovieSoundCount.push_back( szBuf );
    
	kLoader.SetTitle_e( "Resource" );	
	SAFEDELETE( m_pBackground );
	kLoader.LoadString_e( "Background", "", szBuf, MAX_PATH );
	m_pBackground = g_UIImageSetMgr.CreateImageByFullName( szBuf );

	SAFEDELETE( m_pRecDot );
	kLoader.LoadString_e( "RecDot", "", szBuf, MAX_PATH );
	m_pRecDot = g_UIImageSetMgr.CreateImageByFullName( szBuf );

//	kLoader.LoadString_e( "Count_5", "", szBuf, MAX_PATH );
//	m_vMovieCount.push_back( szBuf );
//	kLoader.LoadString_e( "Count_4", "", szBuf, MAX_PATH );
//	m_vMovieCount.push_back( szBuf );
	kLoader.LoadString_e( "Count_3", "", szBuf, MAX_PATH );
	m_vMovieCount.push_back( szBuf );
	kLoader.LoadString_e( "Count_2", "", szBuf, MAX_PATH );
	m_vMovieCount.push_back( szBuf );
	kLoader.LoadString_e( "Count_1", "", szBuf, MAX_PATH );
	m_vMovieCount.push_back( szBuf );
	kLoader.LoadString_e( "Count_Start", "", szBuf, MAX_PATH );
	m_vMovieCount.push_back( szBuf );
	kLoader.LoadString_e( "Count_Cut", "", szBuf, MAX_PATH );
	m_vMovieCount.push_back( szBuf );

	kLoader.LoadString_e( "KeyOutline", "", szBuf, MAX_PATH );
	m_pKeyOutline = g_GUIMgr.CreateFrame( szBuf  );

	SAFEDELETE( m_pScreenShotIcon );
	kLoader.LoadString_e( "ScreenShotIcon", "", szBuf, MAX_PATH );
	m_pScreenShotIcon  = g_UIImageSetMgr.CreateImageByFullName( szBuf );
	

	RegMovieLibrary();
}

const ioHashString &ioMovieMaker::GetMovieEmoticonName( int iCount )
{
	static ioHashString szReturn;
	if( !COMPARE( iCount, 0, (int)m_vMovieCount.size() ) )
		return szReturn;
	return m_vMovieCount[iCount];
}

const ioHashString &ioMovieMaker::GetMovieSoundName( int iCount )
{
	static ioHashString szReturn;
	if( !COMPARE( iCount, 0, (int)m_vMovieSoundCount.size() ) )
		return szReturn;
	return m_vMovieSoundCount[iCount];
}

int ioMovieMaker::GetMaxMovieOption( int iOptionType )
{
	switch( iOptionType )
	{
	case MO_SIZE:
		return MAX_SIZE - 1;
	case MO_FRAME:
		return MAX_FRAME - 1;
	case MO_QUALITY:
		return MAX_QUALITY - 1;
	case MO_SOUND:
		return MAX_SOUND - 1;
	case MO_CODEC:
		return MAX_CODEC - 1;
	}
	return 0;
}

const ioHashString &ioMovieMaker::GetSizeTypeText( int iSizeType )
{
	static ioHashString szReturnString = STR(1);
	switch( iSizeType )
	{
	case SIZE_TYPE_1:
		szReturnString = STR(2);
		break;
	case SIZE_TYPE_2:
		szReturnString = STR(3);
		break;
	}
	return szReturnString;
}

const ioHashString &ioMovieMaker::GetFrameTypeText( int iFrameType )
{
	static ioHashString szReturnString = "15 fps";
	switch( iFrameType )
	{
	case FRAME_30:
		szReturnString = "30 fps";
		break;
	case FRAME_25:
		szReturnString = "25 fps";
		break;
	case FRAME_20:
		szReturnString = "20 fps";
		break;
	case FRAME_15:
		szReturnString = "15 fps";
		break;
	}
	return szReturnString;
}

const ioHashString &ioMovieMaker::GetQualityTypeText( int iQualityType )
{
	static ioHashString szReturnString = STR(1);
	switch( iQualityType )
	{
	case QUALITY_100:
		szReturnString = STR(2);
		break;
	case QUALITY_90:
		szReturnString = STR(3);
		break;
	case QUALITY_80:
		szReturnString = STR(4);
		break;
	case QUALITY_70:
		szReturnString = STR(5);
		break;
	case QUALITY_60:
		szReturnString = STR(6);
		break;
	}
	return szReturnString;
}

const ioHashString &ioMovieMaker::GetSoundTypeText( int iSoundType )
{
	static ioHashString szReturnString = STR(1);
	switch( iSoundType )
	{
	case SOUND_1:
		szReturnString = STR(2);
		break;
	case SOUND_2:
		szReturnString = STR(3);
		break;
	case SOUND_3:
		szReturnString = STR(4);
		break;
	}
	return szReturnString;
}

const ioHashString &ioMovieMaker::GetCodecTypeText( int iCodecType )
{
	static ioHashString szReturnString = "MPEG1";
	switch( iCodecType )
	{
	case CODEC_MPEG1:
		szReturnString = "MPEG1";
		break;
	case CODEC_MJPEG:
		szReturnString = "MJPEG";
		break;
	}
	return szReturnString;
}

bool ioMovieMaker::RegMovieLibrary()
{
	// 인증
	if( m_CapBandiLibrary.IsCreated() == FALSE )
	{
		if( FAILED( m_CapBandiLibrary.Create( BANDICAP_RELEASE_DLL_FILE_NAME ) ) )
		{
			LOG.PrintTimeAndLog( 0, "RegMovieLibrary : Create Failed!" );
			return false;
		}
		
		if( FAILED( m_CapBandiLibrary.Verify( "IOENTER_LOSTSAGA_20081111", "19638e1c" ) ) )
		{
			LOG.PrintTimeAndLog( 0, "RegMovieLibrary : Verify Failed!" );
			return false;
		}
	}
	return true;
}

BCAP_CONFIG &ioMovieMaker::GetConfig()
{
	// 비디오 코덱
	switch( Setting::MovieCodec() )
	{
	case CODEC_MPEG1:
		BCapConfigPreset( &m_CapConfig, BCAP_PRESET_MPEG1 ); // MPEG1 포멧
		break;
	default:
		BCapConfigPreset( &m_CapConfig, BCAP_PRESET_MJPEG ); // MJPEG 포멧
		break;
	}	
	
	// 비디오 사이즈
	switch( Setting::MovieSizeType() )
	{
	case SIZE_TYPE_1:            //현재
		m_CapConfig.VideoSizeW = Setting::Width();
		m_CapConfig.VideoSizeH = Setting::Height();
		break;
	default:					 //절반
		m_CapConfig.VideoSizeW = Setting::Width() / 2;
		m_CapConfig.VideoSizeH = Setting::Height() / 2;
		break;
	}	

	// 비디오 프레임
	switch( Setting::MovieFrameType() )
	{
	case FRAME_30:
		m_CapConfig.VideoFPS = 30.0f;
		break;
	case FRAME_25:
		m_CapConfig.VideoFPS = 25.0f;
		break;
	case FRAME_20:
		m_CapConfig.VideoFPS = 20.0f;
		break;
	default:
		m_CapConfig.VideoFPS = 15.0f;
		break;
	}

	// 비디오 화질
	switch( Setting::MovieQualityType() )
	{
	case QUALITY_100:
		m_CapConfig.VideoQuality = 100;
		break;
	case QUALITY_90:
		m_CapConfig.VideoQuality = 90;
		break;
	case QUALITY_80:
		m_CapConfig.VideoQuality = 80;
		break;
	case QUALITY_70:
		m_CapConfig.VideoQuality = 70;
		break;
	default:
		m_CapConfig.VideoQuality = 60;
		break;
	}

	// 사운드
	switch( Setting::MovieSoundType() )
	{
	case SOUND_1:
		m_CapConfig.AudioChannels   = 2;
		m_CapConfig.AudioSampleRate = 44000;
		break;
	case SOUND_2:
		m_CapConfig.AudioChannels   = 1;
		m_CapConfig.AudioSampleRate = 24000;
		break;
	default:
		m_CapConfig.AudioChannels   = 1;
		m_CapConfig.AudioSampleRate = 12000;
		break;	
	};
	m_CapBandiLibrary.CheckConfig( &m_CapConfig );	
	m_CapBandiLibrary.SetMinMaxFPS( 50, 100 );
	return m_CapConfig;
}

TCHAR *ioMovieMaker::GetSaveFileName()
{
	g_App.CreateScreenShotPath();

	char szFullSavePath[MAX_PATH*2]="";
	_fullpath( szFullSavePath, Setting::GetScreenShotSavePath().c_str(), sizeof( szFullSavePath ) );
	
	char szTestPath[MAX_PATH*2];
	StringCbPrintf( szTestPath, sizeof(szTestPath), "%s\\*.avi", szFullSavePath );	
	
	/*
	WIN32_FIND_DATA wfd;
	ZeroMemory( &wfd, sizeof(WIN32_FIND_DATA) );
	int iNameCount = 1;
	char szFileName[MAX_PATH];
	BOOL bResult = TRUE;
	HANDLE hSrch = FindFirstFile( szTestPath, &wfd );
	while( bResult )
	{
		if( !( wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			const char *pStr = strstr( wfd.cFileName, "LostSagaMovie" );
			if( pStr )
			{
				_splitpath( pStr, NULL, NULL, szFileName, NULL );

				int iNumber = atoi( szFileName + 21 );       // 21 == strlen( "LostSagaMovie_000000_" )
				iNameCount = max( iNameCount, iNumber + 1 );
			}
		}
		bResult = FindNextFile( hSrch, &wfd );			
	}
	FindClose( hSrch );
	*/

	SYSTEMTIME st;
	GetLocalTime( &st );
	StringCbPrintf( m_szPathName, sizeof(m_szPathName),	"%s\\LostSagaMovie_%02d%02d%02d_%02d%02d%02d.avi",
														szFullSavePath,
														st.wYear - 2000,
														st.wMonth,
														st.wDay,
														st.wHour,
														st.wMinute,
														st.wSecond );
	return m_szPathName;
}

void ioMovieMaker::GetMovieFileSize( char *szFileSize )
{ 
	if( m_hMovieFile != NULL && m_hMovieFile != INVALID_HANDLE_VALUE )
	{
		__int64 iKByte = 0;	
		__int64 iMByte = 0;
		__int64 iGByte = 0;

		if ( GetFileSizeEx( m_hMovieFile, &m_lFileSize ) )
		{
			iKByte = m_lFileSize.QuadPart / 1024;
			iMByte = iKByte / 1024;
			iGByte = iMByte / 1024;
		}

		if( iGByte == 0 )
			sprintf( szFileSize, " / %.1fMB", (double)iKByte / 1024 );
		else
			sprintf( szFileSize, " / %.1fGB", (double)iMByte / 1024 );
	}
	else
		sprintf( szFileSize, " / 실패" );

	/*
	struct _stati64 statbuf; 
		
	if( _stati64( m_szPathName, &statbuf ) ) 
		return;

	__int64 iKByte = statbuf.st_size / 1024;
	__int64 iMByte = iKByte / 1024;
	__int64 iGByte = iMByte / 1024;

	if( iGByte == 0 )
	{
		sprintf( szFileSize, " / %.1fMB", (double)iKByte / 1024 );
	}
	else
	{
		sprintf( szFileSize, " / %.1fGB", (double)iMByte / 1024 );
	}
	*/	
}

int ioMovieMaker::GetMovieKeySize()
{	
	return g_FontMgr.GetTextWidth( g_KeyManager.GetKeyText( g_KeyManager.GetCurrentMovieCaptureKey() ), TS_NORMAL, FONT_SIZE_13 ) + 14;
}

void ioMovieMaker::TextLeftCut( char szSrc[2048], int iCutWidth, float fScale, char szReturn[2048] )
{
	int iLen = strlen( szSrc );
	char szDst[2048];
	memset( szDst, 0, sizeof( szDst ) );
	int iBuf = 0;
	while( iBuf < iLen )
	{
		if( g_FontMgr.GetTextWidth( &szSrc[iBuf], TS_OUTLINE_FULL, fScale ) < iCutWidth )
		{
			sprintf( szDst, "%s", &szSrc[iBuf] );
			break;		
		}

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
		if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szSrc[iBuf] ) )
#else
		if(false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szSrc[iBuf] ) )
#endif

#else
		if(IsDBCSLeadByte( (BYTE)szSrc[iBuf] ) )
#endif

			iBuf+=2;
		else
			iBuf+=1;
	}	

	sprintf( szReturn, " . . %s", szDst );
}

void ioMovieMaker::SetCapture()
{
	if( !m_bMovieCapture ) return;

	// 캡쳐 설정
	if( RegMovieLibrary() )
	{
		ioRenderSystem *pRenderSystem = &RenderSystem();
		if( !pRenderSystem ) return;

		if( m_CapBandiLibrary.IsCreated() )
		{
			BCAP_CONFIG cfg = GetConfig();
			m_CapBandiLibrary.SetConfig( &cfg );	

			// 캡처를 시작합니다.
			if( FAILED( m_CapBandiLibrary.Start( GetSaveFileName(), NULL, BCAP_MODE_D3D9_SCALE, (LONG_PTR)pRenderSystem->GetDevice() ) ) )
			{
				LOG.PrintTimeAndLog( 0, "ToggleVideoCapture : Start Failed!" );
				return;
			}
			m_dwMoviePlayTime = FRAMEGETTIME();			
		}
	}
}

void ioMovieMaker::SetScreenShot()
{
	m_dwScreenShotCurTime = FRAMEGETTIME();
	m_szScreenShotPath.Clear();
	g_QuestMgr.QuestCompleteTerm( QC_SCREEN_SHOT );
}

void ioMovieMaker::SetScreenShotComplete( const ioHashString &szScreenShotPath )
{
	char szSrc[2048];
	char szDrive[100] = "";
	char szPath[2048] = "";
	_splitpath( szScreenShotPath.c_str(), szDrive, szPath, NULL, NULL );
	sprintf( szSrc, "%s%s", szDrive, szPath );
	char szReturn[2048];
	memset( szReturn, 0, sizeof( szReturn ) );

	char szTitle[MAX_PATH] = "";
	SafeSprintf( szTitle, sizeof( szTitle ), STR(1) );
	int iTitleSize = g_FontMgr.GetTextWidth( szTitle, TS_OUTLINE_FULL_3X, FONT_SIZE_13 );
	if( g_FontMgr.GetTextWidth( szSrc, TS_OUTLINE_FULL_3X, FONT_SIZE_13 ) < Setting::Width() - (87+iTitleSize) )
		m_szScreenShotPath = szSrc;
	else
	{
		TextLeftCut( szSrc, Setting::Width() - ( iTitleSize + 87 + g_FontMgr.GetTextWidth( ". . ", TS_OUTLINE_FULL_3X, FONT_SIZE_13 ) ) , FONT_SIZE_13, szReturn );
		m_szScreenShotPath = szReturn;
	}
}

void ioMovieMaker::ClearScreenShotComplete()
{
	m_dwScreenShotCurTime = 0;
	m_szScreenShotPath.Clear();
}

bool ioMovieMaker::ToggleMovieCapture()
{
	if( TRUE == m_CapBandiLibrary.IsCapturing() || m_bMovieCapture )      
	{		
		const ioHashString &szSoundName = GetMovieSoundName( COUNT_CUT );
		if( !szSoundName.IsEmpty() )
			g_SoundMgr.PlaySound( szSoundName );

		SAFEDELETE( m_pMovieIcon );
		m_pMovieIcon = g_UIImageSetMgr.CreateImageByFullName( m_vMovieCount[COUNT_CUT] );			

		//캡쳐중이면 중단
		m_CapBandiLibrary.Stop();
		m_bMovieCapture = false;

		CloseFileHandle();

		if( m_dwMovieStartCurTime != 0 )
		{
			m_dwMovieCutDelayCurTime = FRAMEGETTIME();
		}
		m_dwMovieStartCurTime = 0;
		m_dwMovieCutCurTime = FRAMEGETTIME();

		// 유저에게 전송...
		SP2Packet kPacket( CTPK_MOVIE_CONTROL );
		kPacket << MOVIE_CAPTURE_CUT;
		TCPNetwork::SendToServer( kPacket );		
	}
	else if( !m_bMovieCapture )
	{
		if( Setting::IsWin98Series() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			return false;
		}

		if( FRAMEGETTIME() - m_dwMovieCutDelayCurTime > m_dwMovieCutDelayTime )
		{
			m_bMovieCapture		  = true;
			m_dwMovieStartCurTime = FRAMEGETTIME();
			m_iMovieStartSec      = -1;		
			memset( m_szSaveFileInfo, 0, sizeof( m_szSaveFileInfo ) );

			// 유저에게 전송...
			SP2Packet kPacket( CTPK_MOVIE_CONTROL );
			kPacket << MOVIE_CAPTURE_START;
			TCPNetwork::SendToServer( kPacket );

			m_dwMovieCutDelayCurTime = 0;
		}

		g_QuestMgr.QuestCompleteTerm( QC_MAKE_MOVIE );
	}
	return m_bMovieCapture;
}

void ioMovieMaker::MovieCapturing( ioUIRenderer *pUIRender )
{
	ProcessMovieCapturing();

	if( m_bMovieCapture )
	{
		if( m_CapBandiLibrary.IsCapturing() )
		{
			ioRenderSystem *pRenderSystem = &RenderSystem();
			if( !pRenderSystem ) return;

			m_CapBandiLibrary.Work( (LONG_PTR)pRenderSystem->GetDevice() );			

			GetFileHandle();
		}	
		else if( m_dwMovieStartCurTime == 0 )
		{
			// 예외 ... 캡처 라이브러리가 내부적으로 촬영을 중단했다.
			ToggleMovieCapture();
		}
	}
	
	RenderMovieCapturing( pUIRender );
}

void ioMovieMaker::ProcessMovieCapturing()
{
	if( m_dwMovieStartCurTime != 0 )
	{
		int iPrevSec = m_iMovieStartSec;
		DWORD dwCurGapTime = (FRAMEGETTIME() - m_dwMovieStartCurTime);

		if( dwCurGapTime >= m_dwMovieStartTime )
		{
			m_dwMovieStartCurTime = 0;
			m_iMovieStartSec = -1;
			SetCapture();			
		}
		else
		{
			m_iMovieStartSec = dwCurGapTime / 1000;
			if( iPrevSec != m_iMovieStartSec )
			{
				if( COMPARE( m_iMovieStartSec, 0, (int)m_vMovieCount.size() ) )
				{
					SAFEDELETE( m_pMovieIcon );
					m_pMovieIcon = g_UIImageSetMgr.CreateImageByFullName( m_vMovieCount[m_iMovieStartSec] );			

					const ioHashString &szSoundName = GetMovieSoundName( m_iMovieStartSec );
					if( !szSoundName.IsEmpty() )
						g_SoundMgr.PlaySound( szSoundName );
				}
			}
		}
	}
	else if( m_dwMovieCutCurTime != 0 )
	{
		if( FRAMEGETTIME() - m_dwMovieCutCurTime >= m_dwCompleteTime )
			m_dwMovieCutCurTime = 0;
	}	

	if( m_dwScreenShotCurTime != 0 )
	{
		if( FRAMEGETTIME() - m_dwScreenShotCurTime >= 2000 + m_dwCompleteTime )
			m_dwScreenShotCurTime = 0;
	}
	
	if( FRAMEGETTIME() - m_dwRecDotShowTime > 500 )
	{
		m_dwRecDotShowTime = FRAMEGETTIME();
		m_bRecDotShow = !m_bRecDotShow;
	}
}

void ioMovieMaker::RenderMovieCapturing( ioUIRenderer *pUIRender )
{
	if( RenderScreenShot( pUIRender ) ) return;
	if( !m_pMovieIcon ) return;
	
	ioRenderSystem *pRenderSystem = &RenderSystem();
	if( !pRenderSystem ) return;

	if( m_bMovieCapture )
	{
		if( m_pBackground )
		{
			m_pBackground->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pBackground->SetSize( Setting::Width(), m_pBackground->GetHeight() );
			m_pBackground->Render( 0, 0 );
		}

		if( m_iMovieStartSec == -1 )
		{
			RenderMovieCapturingStart();
		}
		else
		{
			RenderMovieCapturingDelay();
		}	
	}	
	else if( m_dwMovieCutCurTime != 0 )
	{	
		if( m_pBackground )
		{
			m_pBackground->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pBackground->SetSize( Setting::Width(), m_pBackground->GetHeight() );
			m_pBackground->Render( 0, 0 );
		}

		RenderMovieCapturingCut();
	}
	else
		return;

	//

	if( pUIRender )
		pUIRender->Render( pRenderSystem );
}

void ioMovieMaker::RenderMovieCapturingDelay()
{
	int iXPos = Setting::Width() / 2;
	int iYPos = 12;

	int iRealXPos, iFirstSize, iSecondSize;
	iRealXPos = iFirstSize = iSecondSize = 0;

	char szFirst[MAX_PATH] = "";
	char szSecond[MAX_PATH] = "";

	int iMaxSec = ( (int)m_dwMovieStartTime / 1000 ) - 1;
	if( m_iMovieStartSec >= iMaxSec ) 
	{
		SafeSprintf( szSecond, sizeof( szSecond ), STR(1) );
		iSecondSize = g_FontMgr.GetTextWidth( szSecond, TS_OUTLINE_FULL_3X, FONT_SIZE_13 );
		iRealXPos = ( iFirstSize + 20 ) / 2;    		
	}
	else
	{
		//int iCurSec = max( 0, iMaxSec - m_iMovieStartSec );
		SafeSprintf( szFirst, sizeof( szFirst ), STR(2) );
		SafeSprintf( szSecond, sizeof( szSecond ), STR(3) );				

		iFirstSize = g_FontMgr.GetTextWidth( szFirst, TS_OUTLINE_FULL_3X, FONT_SIZE_13 );
		iSecondSize= g_FontMgr.GetTextWidth( szSecond, TS_OUTLINE_FULL_3X, FONT_SIZE_13 );
		iRealXPos = ( iFirstSize + iSecondSize + GetMovieKeySize() + 20 ) / 2;    		
		RenderPrintKeyText( (iXPos - iRealXPos) + 20 + iFirstSize + iSecondSize, iYPos );	
	}

	if( m_pMovieIcon )
	{
		m_pMovieIcon->SetScale( 0.50f );
		m_pMovieIcon->Render( iXPos - iRealXPos, iYPos + 8, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	g_FontMgr.SetAlignType( TAT_LEFT );	
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 167, 167, 167 );
	g_FontMgr.PrintText( (iXPos - iRealXPos) + 20, iYPos, FONT_SIZE_13, szFirst );
	g_FontMgr.SetTextColor( 230, 230, 230 );
	g_FontMgr.PrintText( (iXPos - iRealXPos) + 20 + iFirstSize, iYPos, FONT_SIZE_13, szSecond );
}

void ioMovieMaker::RenderMovieCapturingStart()
{
	int iXPos = Setting::Width() / 2;
	int iYPos = 12;

	int iRealXPos, iFirstSize, iSecondSize, iFileInfoSize;
	iRealXPos = iFirstSize = iSecondSize = iFileInfoSize = 0;
	char szFirst[MAX_PATH] = "";
	char szSecond[MAX_PATH] = "";
	
	DWORD dwPlayTime = ( FRAMEGETTIME() - m_dwMoviePlayTime ) / 1000;		
	DWORD dwPlaySec  = dwPlayTime % 60;
	DWORD dwPlayMin  = ( dwPlayTime % 3600 ) / 60;
	DWORD dwPlayHour = dwPlayTime / 3600;

	StringCbPrintf( m_szSaveFileInfo, sizeof( m_szSaveFileInfo ), "%02d:%02d:%02d ", dwPlayHour, dwPlayMin, dwPlaySec );
	iFileInfoSize = g_FontMgr.GetTextWidth( m_szSaveFileInfo, TS_OUTLINE_FULL_3X, FONT_SIZE_13 );

	char szFileSize[MAX_PATH] = "";
	GetMovieFileSize( szFileSize );
	SafeSprintf( szFirst, sizeof( szFirst ), STR(1), szFileSize );
	iFirstSize = g_FontMgr.GetTextWidth( szFirst, TS_OUTLINE_FULL_3X, FONT_SIZE_13 );

	SafeSprintf( szSecond, sizeof( szSecond ), STR(2) );
	iSecondSize = g_FontMgr.GetTextWidth( szSecond, TS_OUTLINE_FULL_3X, FONT_SIZE_13 );

	iRealXPos = ( iFileInfoSize + iFirstSize + iSecondSize + GetMovieKeySize() + 11 ) / 2;    

	if( m_pRecDot && m_bRecDotShow )
		m_pRecDot->Render( iXPos - iRealXPos, iYPos + 4, UI_RENDER_NORMAL, TFO_BILINEAR );

	g_FontMgr.SetAlignType( TAT_LEFT );	
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 255, 51, 0 );
	g_FontMgr.PrintText( (iXPos - iRealXPos) + 11, iYPos, FONT_SIZE_13, m_szSaveFileInfo );
	g_FontMgr.SetTextColor( 167, 167, 167 );
	g_FontMgr.PrintText( (iXPos - iRealXPos) + 11 + iFileInfoSize, iYPos, FONT_SIZE_13, szFirst );
	g_FontMgr.SetTextColor( 230, 230, 230 );
	g_FontMgr.PrintText( (iXPos - iRealXPos) + 11 + iFileInfoSize + iFirstSize, iYPos, FONT_SIZE_13, szSecond );

	RenderPrintKeyText( (iXPos - iRealXPos) + 11 + iFileInfoSize + iFirstSize + iSecondSize, iYPos );	
}

void ioMovieMaker::RenderMovieCapturingCut()
{
	int iXPos = Setting::Width() / 2;
	int iYPos = 12;

	int iRealXPos, iFirstSize, iSecondSize;
	iRealXPos = iFirstSize = iSecondSize = 0;

	char szFirst[MAX_PATH] = "";
	char szSecond[MAX_PATH] = "";
	if( strcmp( m_szSaveFileInfo, "" ) == 0 )
	{
		SafeSprintf( szSecond, sizeof( szSecond ), STR(1) );
		iSecondSize = g_FontMgr.GetTextWidth( szSecond, TS_OUTLINE_FULL_3X, FONT_SIZE_13 );
		iRealXPos = ( iSecondSize + 20) / 2;    		
	}
	else
	{
		SafeSprintf( szFirst, sizeof( szFirst ), STR(2) );
		iFirstSize = g_FontMgr.GetTextWidth( szFirst, TS_OUTLINE_FULL_3X, FONT_SIZE_13 );

		char szSrc[2048];
		char szDrive[100] = "";
		char szPath[2048] = "";
		_splitpath( m_szPathName, szDrive, szPath, NULL, NULL );
		StringCbPrintf( szSrc, sizeof( szSrc ), "%s%s", szDrive, szPath );

		memset( szSecond, 0, sizeof( szSecond ) );
		if( g_FontMgr.GetTextWidth( szSrc, TS_OUTLINE_FULL, FONT_SIZE_13 ) < Setting::Width() - (87+iFirstSize) )
			StringCbPrintf( szSecond, sizeof( szSecond ), "%s", szSrc );
		else
			TextLeftCut( szSrc, Setting::Width() - ( (87+iFirstSize) + g_FontMgr.GetTextWidth( ". . ", TS_OUTLINE_FULL, FONT_SIZE_13 ) ) , FONT_SIZE_13, szSecond );

		SafeSprintf( szSecond, sizeof( szSecond ), STR(3), szSrc );
		iSecondSize = g_FontMgr.GetTextWidth( szSecond, TS_OUTLINE_FULL_3X, FONT_SIZE_13 );
		iRealXPos = ( iFirstSize + iSecondSize + 20) / 2;    		
	}

	m_pMovieIcon->SetScale( 0.50f );
	m_pMovieIcon->Render( iXPos - iRealXPos, iYPos + 8, UI_RENDER_NORMAL, TFO_BILINEAR );

	g_FontMgr.SetAlignType( TAT_LEFT );	
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 167, 167, 167 );
	g_FontMgr.PrintText( (iXPos - iRealXPos) + 20, iYPos, FONT_SIZE_13, szFirst );
	g_FontMgr.SetTextColor( 230, 230, 230 );
	g_FontMgr.PrintText( (iXPos - iRealXPos) + 20 + iFirstSize, iYPos, FONT_SIZE_13, szSecond );
}

bool ioMovieMaker::RenderScreenShot( ioUIRenderer *pUIRender )
{
	if( !m_pScreenShotIcon ) return false;
	if( m_dwScreenShotCurTime == 0 ) return false;

	ioRenderSystem *pRenderSystem = &RenderSystem();
	if( !pRenderSystem ) return false;

	if( m_pBackground )
	{
		m_pBackground->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pBackground->SetSize( Setting::Width(), m_pBackground->GetHeight() );
		m_pBackground->Render( 0, 0 );
	}

	int iXPos = Setting::Width() / 2;
	int iYPos = 12;

	char szTitle[MAX_PATH] = "";
	char szPrintText[MAX_PATH] = "";
	if( m_szScreenShotPath.IsEmpty() )
	{
		SafeSprintf( szTitle, sizeof( szTitle ),  STR(1) );
	}
	else
	{
		SafeSprintf( szTitle, sizeof( szTitle ), STR(2) );
		StringCbPrintf( szPrintText, sizeof( szPrintText ), "%s", m_szScreenShotPath.c_str() );
	}
	int iRealXPos = (g_FontMgr.GetTextWidth( szTitle, TS_OUTLINE_FULL_3X, FONT_SIZE_13 ) + g_FontMgr.GetTextWidth( szPrintText, TS_OUTLINE_FULL_3X, FONT_SIZE_13 ) + 20) / 2;    

	m_pScreenShotIcon->SetScale( 0.50f );
	m_pScreenShotIcon->Render( iXPos - iRealXPos, iYPos + 8, UI_RENDER_NORMAL, TFO_BILINEAR );

	g_FontMgr.SetAlignType( TAT_LEFT );	
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_3X );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( 230, 230, 230 );
	g_FontMgr.PrintText( (iXPos - iRealXPos) + 20, iYPos, FONT_SIZE_13, szTitle );
	g_FontMgr.SetTextColor( 167, 167, 167 );
	g_FontMgr.PrintText( (iXPos - iRealXPos) + 20 + g_FontMgr.GetTextWidth( szTitle, TS_OUTLINE_FULL_3X, FONT_SIZE_13 ), iYPos, FONT_SIZE_13, szPrintText );

	if( pUIRender )
		pUIRender->Render( pRenderSystem );

	return true;
}

void ioMovieMaker::RenderPrintKeyText( int iXPos, int iYPos )
{
	if( m_pKeyOutline )
	{
		m_pKeyOutline->SetSize( GetMovieKeySize() , 18 );
		m_pKeyOutline->Render( iXPos, iYPos );

		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );	
		g_FontMgr.PrintText( iXPos + 7, iYPos + 1, FONT_SIZE_13, g_KeyManager.GetKeyText( g_KeyManager.GetCurrentMovieCaptureKey() ) );
	}	
}

void ioMovieMaker::GetFileHandle()
{
	if ( m_bCreateFile )
		return;

	//생성한 파일 핸들 얻기 (맨 처음 동작시)
	m_hMovieFile = CreateFile( m_szPathName, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, 
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	m_bCreateFile = true;	
}

void ioMovieMaker::CloseFileHandle()
{
	if ( !m_bCreateFile )
		return;

	//핸들 값이 널값이 아니면서 잘못된 핸들이 아닐때 종료
	if( m_hMovieFile != NULL && m_hMovieFile != INVALID_HANDLE_VALUE )
	{
		CloseHandle( m_hMovieFile );
	}

	m_bCreateFile = false;
}