#pragma once

#include <Tchar.h>
#include "bandicap.h"

class ioRenderSystem;
class ioUIRenderer;
class ioUIRenderImage;
class ioUIRenderFrame;

class ioMovieMaker : public Singleton< ioMovieMaker >
{
public:
	// 옵션 타입
	enum
	{
		MO_SIZE = 1,
		MO_FRAME,
		MO_QUALITY,
		MO_SOUND,
		MO_CODEC,
	};

	// 비디오 사이즈
	enum
	{
		SIZE_TYPE_1 = 1,            //현재
		SIZE_TYPE_2,                //절반
		MAX_SIZE, 

		SIZE_DEF = SIZE_TYPE_2,
	};

	// 비디오 프레임
	enum
	{
		FRAME_30 = 1,
		FRAME_25,
		FRAME_20,
		FRAME_15,
		MAX_FRAME,

		FRAME_DEF = FRAME_30,
	};

	// 비디오 화질
	enum
	{
		QUALITY_100 = 1,
		QUALITY_90,
		QUALITY_80,
		QUALITY_70,
		QUALITY_60,
		MAX_QUALITY,

		QUALITY_DEF = QUALITY_80,
	};
	
	// 사운드
	enum
	{
		SOUND_1 = 1,            //44000k Stereo
		SOUND_2,				//24000k Mono
		SOUND_3,				//12000k Mono
		MAX_SOUND,

		SOUND_DEF = SOUND_2,
	};

	// 코덱
	enum
	{
		CODEC_MPEG1 = 1,
		CODEC_MJPEG = 2,
		MAX_CODEC, 

		CODEC_DEF = CODEC_MPEG1,
	};

	// 동영상 촬영 
	enum
	{
//		COUNT_5 = 0,
//		COUNT_4,
		COUNT_3 = 0,
		COUNT_2,
		COUNT_1,
		COUNT_START,
		COUNT_CUT,
	};
	
protected:
	CBandiCaptureLibrary m_CapBandiLibrary;
	BCAP_CONFIG          m_CapConfig;
	TCHAR				 m_szPathName[MAX_PATH];
	char				 m_szSaveFileInfo[MAX_PATH];

protected:	
	ioUIRenderImage *m_pBackground;
	ioUIRenderImage *m_pRecDot;
	ioUIRenderImage *m_pMovieIcon;
	ioUIRenderFrame *m_pKeyOutline;

	ioUIRenderImage *m_pScreenShotIcon;
	ioHashStringVec m_vMovieCount;
	ioHashStringVec m_vMovieSoundCount;
	
	bool  m_bMovieCapture;
	int   m_iMovieStartSec;
	DWORD m_dwMovieStartCurTime;
	DWORD m_dwMovieStartTime;

	//
	DWORD m_dwMovieCutDelayCurTime;
	DWORD m_dwMovieCutDelayTime;

	// 
	DWORD m_dwMovieCutCurTime;

	//
	DWORD m_dwMoviePlayTime;

	//
	DWORD m_dwScreenShotCurTime;
	ioHashString m_szScreenShotPath;

	//
	DWORD m_dwCompleteTime;

	//
	bool  m_bRecDotShow;
	DWORD m_dwRecDotShowTime;

	bool m_bCreateFile;

protected:
	HANDLE m_hMovieFile;
	LARGE_INTEGER m_lFileSize;

public:
	static ioMovieMaker& GetSingleton();

public:
	void Initialize(); 

public:
	const ioHashString &GetSizeTypeText( int iSizeType );
	const ioHashString &GetFrameTypeText( int iFrameType );
	const ioHashString &GetQualityTypeText( int iQualityType );
	const ioHashString &GetSoundTypeText( int iSoundType );
	const ioHashString &GetCodecTypeText( int iCodecType );
	int GetMaxMovieOption( int iOptionType );

protected:
	bool RegMovieLibrary();
	BCAP_CONFIG &GetConfig();
	void SetCapture();
	TCHAR *GetSaveFileName();
	void TextLeftCut( char szSrc[2048], int iCutWidth, float fScale, char szReturn[2048] );

protected:
	void ProcessMovieCapturing();
	void RenderMovieCapturing( ioUIRenderer *pUIRender );
	void RenderPrintKeyText( int iXPos, int iYPos );
	void RenderMovieCapturingDelay();
	void RenderMovieCapturingStart();
	void RenderMovieCapturingCut();
	bool RenderScreenShot( ioUIRenderer *pUIRender );
	void GetMovieFileSize( char *szFileSize );
	int  GetMovieKeySize();

public:
	bool ToggleMovieCapture();
	void MovieCapturing( ioUIRenderer *pUIRender );

public:
	const ioHashString &GetMovieEmoticonName( int iCount );
	const ioHashString &GetMovieSoundName( int iCount );
	DWORD GetMovieStartTime(){ return m_dwMovieStartTime; }
	bool  IsMovieCapturing(){ return m_bMovieCapture; }

public:    //스크린샷 이모티콘
	void SetScreenShot();
	void SetScreenShotComplete( const ioHashString &szScreenShotPath );
	void ClearScreenShotComplete();
	bool IsScreenShotComplete(){ return !m_szScreenShotPath.IsEmpty(); }

protected:
	void GetFileHandle();
	void CloseFileHandle();

public:
	ioMovieMaker();
	virtual ~ioMovieMaker();
};

#define g_MovieMaker ioMovieMaker::GetSingleton()

