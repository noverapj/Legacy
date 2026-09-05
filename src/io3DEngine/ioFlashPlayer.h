
//----------------------------------------------------------------------------------
/*
Filename	: ioFlashPlayer.h
Author		: megamegy
Date		: 2013.10.08
Desc		: '.swf' file player
*/
//----------------------------------------------------------------------------------

#ifndef _FLASHPLAYER_H_
#define _FLASHPLAYER_H_

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
FlashPlayer
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

#include "ioWndEX.h"
#include "comutil.h"
#include "../FlashPlayerToDirectX/Include/IFlashDX.h"
#include "../FlashPlayerToDirectX/Include/ASInterface.h"

class FlashPlayer
{
	//----------------------------------------------------------------------------------
	/*
	버퍼링에 사용될 surface 갯수
	*/
	enum { NUM_TEX_IN_ROT = 2 };

public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	FlashPlayer();
	~FlashPlayer();

	//----------------------------------------------------------------------------------
	/*
	Clone
	*/
	FlashPlayer*						Clone( IDirect3DDevice9* device );

	//----------------------------------------------------------------------------------
	/*
	Init / Resize / Destroy
	*/
	bool								Init( IDirect3DDevice9* device, unsigned int width, unsigned int height, bool torandertarget );
	bool								Init( IDirect3DDevice9* device, unsigned int flashwidth, unsigned int flashheight, unsigned int width, unsigned int height, bool torandertarget );
	bool								Resize( IDirect3DDevice9* device, unsigned int width, unsigned int height );
	bool								Resize( IDirect3DDevice9* device, unsigned int flashwidth, unsigned int flashheight, unsigned int width, unsigned int height );
	void								Destroy();

	//----------------------------------------------------------------------------------
	/*
	.swf 파일 로드
	*/
	bool								Load( const char* filename );
	bool								Reload();

	//----------------------------------------------------------------------------------
	/*
	셋팅
	*/
	void								SetTransparencyMode( bool alpha );
	void								SetBackgroundColor( unsigned char r, unsigned char g, unsigned char b );
	void								SetBackgroundColor( DWORD color );
	void								SetPos( float x, float y );
	void								SetLoop( bool loop );

	//----------------------------------------------------------------------------------
	/*
	Get
	*/
	const char*							GetFileName() const;
	bool								IsTransparencyMode() const;
	bool								IsLoop() const { return m_loop; }
	DWORD								GetBKColor() const;
	float								GetPosX() const { return m_posx; }
	float								GetPosY() const { return m_posy; }
	float								GetWidth() const { return m_width; }
	float								GetHeight() const { return m_height; }	
	bool								IsPlaying() const;

	//----------------------------------------------------------------------------------
	/*
	RenderToTarget : 특정 Texture에 Render할 경우 사용 ( init함수 마지막 인자를 true로 셋팅해야함 )
	*/
	void								RenderToTarget( IDirect3DDevice9* device, IDirect3DTexture9* rendertarget );

	//----------------------------------------------------------------------------------
	/*
	Render : init함수 마지막 인자를 false로 셋팅해야함
	*/
	void								Render( IDirect3DDevice9* device );

	//----------------------------------------------------------------------------------
	/*
	Render : init함수 마지막 인자를 false로 셋팅해야함
	*/
	IDirect3DTexture9*					Render();
	void								RenderToTexture( IDirect3DTexture9* texture );
	void								RenderToCurTexture( bool advance );

	//----------------------------------------------------------------------------------
	/*
	Control
	*/
	void								Play();
	void								Stop();
	void								Pause();
	void								Resume();

	//----------------------------------------------------------------------------------
	/*
	Input
	*/
	void								MouseMove( int x, int y );
	void								MouseLDown( int x, int y );
	void								MouseLUp( int x, int y );
	void								MouseRDown( int x, int y );
	void								MouseRUp( int x, int y );

protected:

	//----------------------------------------------------------------------------------
	/*
	파괴 관련
	*/
	void								DestroyFlash();
	void								ReleaseTextures();

	//----------------------------------------------------------------------------------
	/*
	생성 관련
	*/
	bool								CreateRenderTarget( IDirect3DDevice9* device, unsigned int flashwidth, unsigned int flashheight, unsigned int width, unsigned int height );

protected:

	IFlashDX*							m_flashDX;
	IFlashDXPlayer*						m_player;
	ASInterface*						m_as;

	//----------------------------------------------------------------------------------
	/*
	상태
	STATE_IDLE = 0,		///< no movie loaded
	STATE_PLAYING,
	STATE_STOPPED
	*/
	IFlashDXPlayer::EState				m_state;

	IDirect3DTexture9*					m_textures[ NUM_TEX_IN_ROT ];
	int									m_curtexindex;	

	IFlashDXPlayer::ETransparencyMode	m_transparencymode;

	bool								m_loop;
	float								m_posx, m_posy;
	float								m_width, m_height;

	std::string							m_filename;

	struct TLVERTEX
	{
		enum { FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1, };

		float		m_x;
		float		m_y;
		float		m_z;
		float		m_rhw;
		D3DCOLOR	m_color;
		float		m_u;
		float		m_v;

		void		Set( float x, float y, float z, float rhw, D3DCOLOR color, float u, float v )
		{
			m_x = x;
			m_y = y;
			m_z = z;
			m_rhw = rhw;
			m_color = color;
			m_u = u;
			m_v = v;
		}
	};
	TLVERTEX							m_vertices[ 4 ];

	bool								m_torandertarget;
};


//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioFlashPlayer
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
class __EX ioFlashPlayer : public ioWndEX
{
public:

	//----------------------------------------------------------------------------------
	/*
	생성 / 소멸
	*/
	ioFlashPlayer();
	ioFlashPlayer( IOWNDEX_TYPE type );
	virtual ~ioFlashPlayer();

public:

	//----------------------------------------------------------------------------------
	/*
	컨트롤 함수들
	*/
	bool					Load( const char* filename );
	void					Play();
	void					Stop();
	void					Pause();
	void					Resume();

	//----------------------------------------------------------------------------------
	/*
	기타 함수
	*/
	void					SetFrontColor( DWORD color ) { m_color = color; }
	DWORD					GetFrontColor() const { return m_color; }

	//----------------------------------------------------------------------------------
	/*
	ioWnd override
	*/
	virtual void			SetSize( int width, int height );
	//virtual void			SetWindowAlpha( int alpha );

	//----------------------------------------------------------------------------------
	/*
	ioWnd Event override
	*/
	virtual void			iwm_lbuttondown( const ioMouse& mouse );
	virtual void			iwm_lbuttonup( const ioMouse& mouse );
	virtual void			iwm_rbuttondown( const ioMouse& mouse );
	virtual void			iwm_rbuttonup( const ioMouse& mouse );
	virtual void			iwm_mouseover( const ioMouse& mouse );
	virtual void			iwm_mousemove( const ioMouse& mouse );
	virtual void			iwm_mouseleave( const ioMouse& mouse );
		
	//----------------------------------------------------------------------------------
	/*
	ioWnd 재정의 ParseExtraInfo
	*/
	virtual void			ParseExtraInfo( ioXMLElement &extxml );

	//----------------------------------------------------------------------------------
	/*
	Control
	*/
	void					SetFlashAlphaMode( bool alpha );
	void					SetFlashBKColor( DWORD color );
	void					SetFlashLoop( bool loop );

	//----------------------------------------------------------------------------------
	/*
	Get
	*/
	const char*				GetSWFFileName() const;
	bool					IsFlashAlphaMode() const;
	bool					IsFlashLoop() const;
	DWORD					GetFlashBKColor() const;
	bool					IsPlaying() const;

	//----------------------------------------------------------------------------------
	/*
	Tool을 위한 인터페이스들
	*/
	DECL_NAME_INTERFACE_FOR_TOOL( FlashPlayer, ioFlashPlayer )
	virtual ioWnd*			_Clone() { return new ioFlashPlayer(); }
	virtual void			Copy( ioWnd* wnd );
	virtual void			_SetSaveData( ioXMLElement& extxml );

protected:

	//----------------------------------------------------------------------------------
	/*
	ioWnd 재정의 인터페이스
	*/
	virtual void			OnRender();
	virtual void			CheckFrameReSize( int iWidth, int iHeight, int iPrevWidth, int iPrevHeight );

protected:
	
	FlashPlayer*			m_flashplayer;
	DWORD					m_color;

	bool					m_ismouseover;
};



////----------------------------------------------------------------------------------
////----------------------------------------------------------------------------------
///*
//ioFlashPlayerTestWnd
//*/
////----------------------------------------------------------------------------------
////----------------------------------------------------------------------------------
//class __EX ioFlashPlayerTestWnd : public ioWnd
//{
//public:
//
//	//----------------------------------------------------------------------------------
//	/*
//	생성 / 소멸
//	*/
//	ioFlashPlayerTestWnd();
//	virtual ~ioFlashPlayerTestWnd();
//
//	//----------------------------------------------------------------------------------
//	/*
//	ioWnd override
//	*/
//	virtual void	iwm_show();
//	virtual void	iwm_command( ioWnd *pWnd, int cmd, DWORD param );
//
//protected:
//
//	ioFlashPlayer*	m_flashplayer;
//};

#endif // _FLASHPLAYER_H_