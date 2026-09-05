
//----------------------------------------------------------------------------------
/*
Filename	: ioFlashPlayer.cpp
Author		: megamegy
Date		: 2013.10.08
Desc		: '.swf' file player
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "ioFlashPlayer.h"
#include "ioRenderSystem.h"
#include "ioUIRenderer.h"
#include "ioXMLElement.h"
#include "ioStringConverter.h"
#include "ioMouse.h"

#ifdef _DEBUG
	//#pragma comment(lib, "FlashDXD.lib")
	#pragma comment(lib, "FlashDX.lib")
#else
	#pragma comment(lib, "FlashDX.lib")
#endif

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
FlashPlayer
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
FlashPlayer::FlashPlayer()
: m_flashDX( 0 )
, m_player( 0 )
, m_as( 0 )
, m_state( IFlashDXPlayer::STATE_IDLE )
, m_curtexindex( 0 )
, m_transparencymode( IFlashDXPlayer::TMODE_FULL_ALPHA )
, m_loop( false )
, m_posx( 0.0f )
, m_posy( 0.0f )
, m_width( 0.0f )
, m_height( 0.0f )
, m_filename( "" )
, m_torandertarget( false )
{
	for ( int i = 0; i < NUM_TEX_IN_ROT; ++i ) {
		m_textures[ i ] = 0;
	}		
}

FlashPlayer::~FlashPlayer()
{
	Destroy();
}

//----------------------------------------------------------------------------------
/*
Clone
*/
FlashPlayer*	FlashPlayer::Clone( IDirect3DDevice9* device )
{
	if ( !device ) {
		return 0;
	}

	FlashPlayer* ret = new FlashPlayer();
		
	ret->Init( device, GetWidth(), GetHeight(), m_torandertarget );
	ret->Load( GetFileName() );
	ret->SetPos( GetPosX(), GetPosY() );
	ret->SetLoop( IsLoop() );
	ret->SetTransparencyMode( IsTransparencyMode() );
	ret->SetBackgroundColor( GetBKColor() );

	return ret;
}

//----------------------------------------------------------------------------------
/*
Init / Resize / Destroy
*/
bool	FlashPlayer::Init( IDirect3DDevice9* device, unsigned int width, unsigned int height, bool torandertarget )
{	
	return Init( device, width, height, width, height, torandertarget );
}

bool	FlashPlayer::Init( IDirect3DDevice9* device, unsigned int flashwidth, unsigned int flashheight, unsigned int width, unsigned int height, bool torandertarget )
{	
	m_torandertarget = torandertarget;

	DestroyFlash();

	m_flashDX = GetFlashToDirectXInstance();
	if ( !m_flashDX ) {
		return false;
	}

	m_player = m_flashDX->CreatePlayer( width, height );
	if ( !m_player ) {
		return false;
	}

	m_as = new ASInterface( m_player );
	
	return Resize( device, flashwidth, flashheight, width, height );
}

void	FlashPlayer::Destroy()
{
	DestroyFlash();

	ReleaseTextures();
}

bool	FlashPlayer::Resize( IDirect3DDevice9* device, unsigned int width, unsigned int height )
{
	return Resize( device, width, height, width, height );
}

bool	FlashPlayer::Resize( IDirect3DDevice9* device, unsigned int flashwidth, unsigned int flashheight, unsigned int width, unsigned int height )
{
	m_width = static_cast<float>( width );
	m_height = static_cast<float>( height );

	return CreateRenderTarget( device, flashwidth, flashheight, width, height );
}

//----------------------------------------------------------------------------------
/*
.swf 파일 로드
*/
bool	FlashPlayer::Load( const char* filename )
{
	if ( !m_player || !filename ) {
		return false;
	}
		
	if ( !m_player->LoadMovie( _bstr_t( filename ) ) ) {
		return false;
	}

	m_filename = filename;

	Play();

	//m_player->StopPlaying();
	
	//RenderToCurTexture( false );

	return true;
}

bool	FlashPlayer::Reload()
{
	return Load( m_filename.c_str() );
}

//----------------------------------------------------------------------------------
/*
셋팅
*/
void	FlashPlayer::SetTransparencyMode( bool alpha )
{
	if ( !m_player ) {
		return;
	}

	if ( alpha ) {		
		m_transparencymode = IFlashDXPlayer::TMODE_FULL_ALPHA;
	} else {
		m_transparencymode = IFlashDXPlayer::TMODE_OPAQUE;
	}

	m_player->SetTransparencyMode( m_transparencymode );
}

void	FlashPlayer::SetBackgroundColor( unsigned char r, unsigned char g, unsigned char b )
{
	if ( !m_player ) {
		return;
	}

	m_player->SetBackgroundColor( RGB( r, g, b ) );
}

void	FlashPlayer::SetBackgroundColor( DWORD color )
{
	if ( !m_player ) {
		return;
	}

	m_player->SetBackgroundColor( color );
}

void	FlashPlayer::SetPos( float x, float y )
{
	m_posx = x;
	m_posy = y;
}

void	FlashPlayer::SetLoop( bool loop )
{
	if ( !m_player ) {
		return;
	}

	m_loop = loop;

	m_player->SetLoop( m_loop );
}

//----------------------------------------------------------------------------------
/*
Get
*/
const char*		FlashPlayer::GetFileName() const
{
	if ( !m_player ) {
		return 0;
	}

	return m_filename.c_str();
}

bool	FlashPlayer::IsTransparencyMode() const
{
	if ( !m_player ) {
		return false;
	}

	return ( m_player->GetTransparencyMode() == IFlashDXPlayer::TMODE_FULL_ALPHA );
}

DWORD	FlashPlayer::GetBKColor() const
{
	if ( !m_player ) {
		return 0xFFFFFFFF;
	}

	return m_player->GetBackgroundColor();
}

bool	FlashPlayer::IsPlaying() const
{
	if ( !m_player ) {
		return false;
	}

	return ( m_state == IFlashDXPlayer::STATE_PLAYING );
	//return ( m_player->GetState() == IFlashDXPlayer::STATE_PLAYING );
}

//----------------------------------------------------------------------------------
/*
RenderToTarget : 특정 Texture에 Render할 경우 사용 ( init함수 마지막 인자를 true로 셋팅해야함 )
*/
void	FlashPlayer::RenderToTarget( IDirect3DDevice9* device, IDirect3DTexture9* rendertarget )
{
	if ( !m_torandertarget ) {
		return;
	}

	if ( !m_player || !device || !rendertarget ) {
		return;
	}
		
	unsigned int numDirtyRects;
	const RECT* dirtyRects;
	if ( m_player->IsNeedUpdate( NULL, &dirtyRects, &numDirtyRects ) )
	{
		IDirect3DTexture9* texture = m_textures[ m_curtexindex ];
		if ( ++m_curtexindex == NUM_TEX_IN_ROT ) {
			m_curtexindex = 0;
		}

		IDirect3DSurface9* srcsurface;
		if ( FAILED( texture->GetSurfaceLevel( 0, &srcsurface ) ) ) {
			assert( false );
		}

		HDC surfaceDC;
		if ( FAILED( srcsurface->GetDC( &surfaceDC ) ) ) {
			assert( false );
		}

		m_player->DrawFrame( surfaceDC );

		if ( FAILED( srcsurface->ReleaseDC( surfaceDC ) ) ) {
			assert( false );
		}

		IDirect3DSurface9* destsurface;
		if ( FAILED( rendertarget->GetSurfaceLevel( 0, &destsurface ) ) ) {
			assert( false );
		}

		for ( unsigned int i = 0; i < numDirtyRects; ++i )
		{
			POINT destPoint = { static_cast<LONG>( dirtyRects[ i ].left + m_posx ), static_cast<LONG>( dirtyRects[ i ].top + m_posy ) };
			if ( FAILED( device->UpdateSurface( srcsurface, dirtyRects + i, destsurface, &destPoint ) ) ) {
				assert ( false );
			}
		}

		destsurface->Release();
		srcsurface->Release();
	}
}

//----------------------------------------------------------------------------------
/*
Render : init함수 마지막 인자를 false로 셋팅해야함
*/
void	FlashPlayer::Render( IDirect3DDevice9* device )
{
	if ( m_torandertarget ) {
		return;
	}

	if ( !m_player || !device ) {
		return;
	}
		
	bool advancenext = false;
	unsigned int numDirtyRects;
	const RECT* dirtyRects;
	if ( m_player->IsNeedUpdate( NULL, &dirtyRects, &numDirtyRects ) )
	{
		IDirect3DSurface9* srcsurface;
		if ( FAILED( m_textures[ m_curtexindex ]->GetSurfaceLevel( 0, &srcsurface ) ) ) {
			assert( false );
		}

		HDC surfaceDC;
		if ( FAILED( srcsurface->GetDC( &surfaceDC ) ) ) {
			assert( false );
		}

		m_player->DrawFrame( surfaceDC );

		if ( FAILED( srcsurface->ReleaseDC( surfaceDC ) ) ) {
			assert( false );
		}

		srcsurface->Release();

		advancenext = true;
	}

	m_vertices[ 0 ].Set(	m_posx,				m_posy,				0.0f, 1.0f, 0xFFFFFFFF, 0.0f, 0.0f );
	m_vertices[ 1 ].Set(	m_posx + m_width,	m_posy,				0.0f, 1.0f, 0xFFFFFFFF, 1.0f, 0.0f );
	m_vertices[ 2 ].Set(	m_posx,				m_posy + m_height,	0.0f, 1.0f, 0xFFFFFFFF, 0.0f, 1.0f );
	m_vertices[ 3 ].Set(	m_posx + m_width,	m_posy + m_height,	0.0f, 1.0f, 0xFFFFFFFF, 1.0f, 1.0f );

	device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	//device->SetRenderState( D3DRS_FOGENABLE, false );
	device->SetRenderState( D3DRS_ALPHABLENDENABLE, true );

	device->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	device->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );

	device->SetTexture( 0, m_textures[ m_curtexindex ] );

	device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	device->SetFVF( TLVERTEX::FVF );
	device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_vertices, sizeof( TLVERTEX ) );

	if ( advancenext ) 
	{
		if ( ++m_curtexindex == NUM_TEX_IN_ROT ) {
			m_curtexindex = 0;
		}
	}
}

//----------------------------------------------------------------------------------
/*
Render : init함수 마지막 인자를 false로 셋팅해야함
*/
IDirect3DTexture9*	FlashPlayer::Render()
{
	if ( m_torandertarget ) {
		return 0;
	}

	if ( !m_player ) {
		return 0;
	}
		
	bool advancenext = false;
	//if ( m_player->GetState() == IFlashDXPlayer::STATE_PLAYING )
	if ( m_state == IFlashDXPlayer::STATE_PLAYING )
	{
		RenderToCurTexture( false );

		advancenext = true;
	}

	IDirect3DTexture9* ret = m_textures[ m_curtexindex ];

	if ( advancenext ) 
	{
		if ( ++m_curtexindex == NUM_TEX_IN_ROT ) {
			m_curtexindex = 0;
		}
	}

	return ret;
}

void	FlashPlayer::RenderToTexture( IDirect3DTexture9* texture )
{
	if ( !texture ) {
		return;
	}

	if ( !m_player ) {
		return;
	}

	IDirect3DSurface9* srcsurface;
	if ( FAILED( texture->GetSurfaceLevel( 0, &srcsurface ) ) ) {
		assert( false );
	}

	HDC surfaceDC;
	if ( FAILED( srcsurface->GetDC( &surfaceDC ) ) ) {
		assert( false );
	}

	m_player->DrawFrameEX( surfaceDC );

	if ( FAILED( srcsurface->ReleaseDC( surfaceDC ) ) ) {
		assert( false );
	}

	srcsurface->Release();
}

void	FlashPlayer::RenderToCurTexture( bool advance )
{
	RenderToTexture( m_textures[ m_curtexindex ] );

	if ( advance )
	{
		if ( ++m_curtexindex == NUM_TEX_IN_ROT ) {
			m_curtexindex = 0;
		}
	}
}

//----------------------------------------------------------------------------------
/*
Control
*/
void	FlashPlayer::Play()
{
	if ( !m_player ) {
		return;
	}

	m_player->Rewind();
	m_player->StartPlaying();
	m_player->SetLoop( m_loop );

	m_state = IFlashDXPlayer::STATE_PLAYING;
}

void	FlashPlayer::Stop()
{
	if ( !m_player ) {
		return;
	}

	m_player->StopPlaying();
	m_player->Rewind();
	RenderToCurTexture( false );

	m_state = IFlashDXPlayer::STATE_STOPPED;
}

void	FlashPlayer::Pause()
{
	if ( !m_player ) {
		return;
	}

	m_player->StopPlaying();

	m_state = IFlashDXPlayer::STATE_STOPPED;
}

void	FlashPlayer::Resume()
{
	if ( !m_player ) {
		return;
	}

	m_player->StartPlaying();

	m_state = IFlashDXPlayer::STATE_PLAYING;
}

//----------------------------------------------------------------------------------
/*
Input
*/
void	FlashPlayer::MouseMove( int x, int y )
{
	if ( !m_player ) {
		return;
	}

	m_player->SetMousePos( x, y );
}

void	FlashPlayer::MouseLDown( int x, int y )
{
	if ( !m_player ) {
		return;
	}

	MouseMove( x, y );
	m_player->SetMouseButtonState( x, y, IFlashDXPlayer::eMouse1, true );
}

void	FlashPlayer::MouseLUp( int x, int y )
{
	if ( !m_player ) {
		return;
	}

	MouseMove( x, y );
	m_player->SetMouseButtonState( x, y, IFlashDXPlayer::eMouse1, false );
}

void	FlashPlayer::MouseRDown( int x, int y )
{
	if ( !m_player ) {
		return;
	}

	MouseMove( x, y );
	m_player->SetMouseButtonState( x, y, IFlashDXPlayer::eMouse2, true );
}

void	FlashPlayer::MouseRUp( int x, int y )
{
	if ( !m_player ) {
		return;
	}

	MouseMove( x, y );
	m_player->SetMouseButtonState( x, y, IFlashDXPlayer::eMouse2, false );
}

//----------------------------------------------------------------------------------
/*
파괴 관련
*/
void	FlashPlayer::DestroyFlash()
{
	if ( m_as )
	{
		delete m_as;
		m_as = 0;
	}

	if ( m_flashDX && m_player ) {
		m_flashDX->DestroyPlayer( m_player );
	}
	m_flashDX = 0;
	m_player = 0;
}

void	FlashPlayer::ReleaseTextures()
{
	for ( int i = 0; i < NUM_TEX_IN_ROT; ++i ) 
	{
		if ( m_textures[ i ] ) 
		{
			m_textures[ i ]->Release();
			m_textures[ i ] = 0;
		}
	}
}

//----------------------------------------------------------------------------------
/*
생성 관련
*/
bool	FlashPlayer::CreateRenderTarget( IDirect3DDevice9* device, unsigned int flashwidth, unsigned int flashheight, unsigned int width, unsigned int height )
{
	if ( m_player ) {
		m_player->ResizePlayer( flashwidth, flashheight, width, height );
	}

	ReleaseTextures();

	for ( int i = 0; i < NUM_TEX_IN_ROT; ++i )
	{
		if ( FAILED( device->CreateTexture( width, height, 1, 0, D3DFMT_A8R8G8B8, m_torandertarget ? D3DPOOL_SYSTEMMEM : D3DPOOL_MANAGED, &m_textures[ i ], NULL ) ) ) {
			return false;
		}
	}

	return true;
}



//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
/*
ioFlashPlayer
*/
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/*
생성 / 소멸
*/
ioFlashPlayer::ioFlashPlayer()
	: ioWndEX( IWEXT_FLASHPLAYER )
	, m_color( 0xFFFFFFFF )
	, m_ismouseover( false )
{	
	m_flashplayer = new FlashPlayer();
	m_flashplayer->Init( RenderSystem().GetDevice(), GetWidth(), GetHeight(), false );	
}

ioFlashPlayer::ioFlashPlayer( IOWNDEX_TYPE type )
	: ioWndEX( type )
	, m_color( 0xFFFFFFFF )
	, m_ismouseover( false )
{	
	m_flashplayer = new FlashPlayer();
	m_flashplayer->Init( RenderSystem().GetDevice(), GetWidth(), GetHeight(), false );	
}

ioFlashPlayer::~ioFlashPlayer()
{
	if ( m_flashplayer ) {
		m_flashplayer->Destroy();
	}
	SAFEDELETE( m_flashplayer );
}

//----------------------------------------------------------------------------------
/*
ioWnd override
*/
void	ioFlashPlayer::SetSize( int width, int height )
{
	ioWndEX::SetSize( width, height );

	if ( m_flashplayer )
	{
		m_flashplayer->Resize( RenderSystem().GetDevice(), width, height );
		m_flashplayer->Reload();
	}
}

//----------------------------------------------------------------------------------
/*
ioWnd Event override
*/
void	ioFlashPlayer::iwm_lbuttondown( const ioMouse& mouse )
{
	ioWndEX::iwm_lbuttondown( mouse );

	if ( m_flashplayer ) {
		//if ( !m_flashplayer->IsPlaying() )
		//{
		//	m_flashplayer->SetLoop( true );
		//	m_flashplayer->Play();
		//}
		m_flashplayer->MouseLDown( mouse.GetMousePos().x - GetDerivedPosX(), mouse.GetMousePos().y - GetDerivedPosY() );		
	}
}

void	ioFlashPlayer::iwm_lbuttonup( const ioMouse& mouse )
{
	ioWndEX::iwm_lbuttonup( mouse );

	if ( m_flashplayer ) {
		m_flashplayer->MouseLUp( mouse.GetMousePos().x - GetDerivedPosX(), mouse.GetMousePos().y - GetDerivedPosY() );
	}
}

void	ioFlashPlayer::iwm_rbuttondown( const ioMouse& mouse )
{
	ioWndEX::iwm_rbuttondown( mouse );

	if ( m_flashplayer ) {		
		m_flashplayer->MouseRDown( mouse.GetMousePos().x - GetDerivedPosX(), mouse.GetMousePos().y - GetDerivedPosY() );
	}
}

void	ioFlashPlayer::iwm_rbuttonup( const ioMouse& mouse )
{
	ioWndEX::iwm_rbuttonup( mouse );

	if ( m_flashplayer ) {
		m_flashplayer->MouseRUp( mouse.GetMousePos().x - GetDerivedPosX(), mouse.GetMousePos().y - GetDerivedPosY() );
	}
}

void	ioFlashPlayer::iwm_mouseover( const ioMouse& mouse )
{
	ioWndEX::iwm_mouseover( mouse );

	m_ismouseover = true;

	if ( m_flashplayer ) {
		m_flashplayer->MouseMove( mouse.GetMousePos().x - GetDerivedPosX(), mouse.GetMousePos().y - GetDerivedPosY() );
	}
}

void	ioFlashPlayer::iwm_mousemove( const ioMouse& mouse )
{
	ioWndEX::iwm_mousemove( mouse );

	if ( m_flashplayer ) {
		m_flashplayer->MouseMove( mouse.GetMousePos().x - GetDerivedPosX(), mouse.GetMousePos().y - GetDerivedPosY() );
	}
}

void	ioFlashPlayer::iwm_mouseleave( const ioMouse& mouse )
{
	ioWndEX::iwm_mouseleave( mouse );

	m_ismouseover = false;

	if ( m_flashplayer ) {
		m_flashplayer->MouseMove( -100, -100 );
	}
}

//----------------------------------------------------------------------------------
/*
컨트롤 함수들
*/
bool	ioFlashPlayer::Load( const char* filename )
{
	if ( !filename || !m_flashplayer ) {
		return false;
	}

	return m_flashplayer->Load( filename );
}

void	ioFlashPlayer::Play()
{
	if ( m_flashplayer ) {
		m_flashplayer->Play();
	}
}

void	ioFlashPlayer::Stop()
{
	if ( m_flashplayer ) {
		m_flashplayer->Stop();
	}
}

void	ioFlashPlayer::Pause()
{
	if ( m_flashplayer ) {
		m_flashplayer->Pause();
	}
}

void	ioFlashPlayer::Resume()
{
	if ( m_flashplayer ) {
		m_flashplayer->Resume();
	}
}

//----------------------------------------------------------------------------------
/*
Control
*/
void	ioFlashPlayer::SetFlashAlphaMode( bool alpha )
{
	if ( m_flashplayer ) {
		m_flashplayer->SetTransparencyMode( alpha );
	}
}

void	ioFlashPlayer::SetFlashBKColor( DWORD color )
{
	if ( m_flashplayer ) {
		m_flashplayer->SetBackgroundColor( color );
	}
}

void	ioFlashPlayer::SetFlashLoop( bool loop )
{
	if ( m_flashplayer ) {
		m_flashplayer->SetLoop( loop );
	}
}

//----------------------------------------------------------------------------------
/*
Get
*/
const char*	ioFlashPlayer::GetSWFFileName() const
{
	if ( !m_flashplayer ) {
		return 0;
	}
	return m_flashplayer->GetFileName();
}

bool	ioFlashPlayer::IsFlashAlphaMode() const
{
	if ( !m_flashplayer ) {
		return false;
	}
	return m_flashplayer->IsTransparencyMode();
}

bool	ioFlashPlayer::IsFlashLoop() const
{
	if ( !m_flashplayer ) {
		return false;
	}
	return m_flashplayer->IsLoop();
}

DWORD	ioFlashPlayer::GetFlashBKColor() const
{
	if ( !m_flashplayer ) {
		return 0xFFFFFFFF;
	}
	return m_flashplayer->GetBKColor();
}

bool	ioFlashPlayer::IsPlaying() const
{
	if ( !m_flashplayer ) {
		return false;
	}

	return m_flashplayer->IsPlaying();
}

//----------------------------------------------------------------------------------
/*
ioWnd 재정의 인터페이스
*/
#include "../io3DEngine/ioMemoryPool.h"
class ioIQFlashPlayer : public IUIQuad, public ioPoolObject< ioIQFlashPlayer, 512 >
{
public:

	ioIQFlashPlayer() {}
	ioIQFlashPlayer( const D3DXVECTOR2& pos, const D3DXVECTOR2& size, IDirect3DTexture9* tex, DWORD color )
		: m_pos( pos )
		, m_size( size )
		, m_tex( tex )
		, m_color( color )
	{}
	virtual ~ioIQFlashPlayer() {}
		
	virtual int				GetQuadCount() const { return 1; }
	virtual void			AddInfo( ioQuadRenderArray *pQuadArray ) 
	{
		pQuadArray->AddRenderQuadInfo( m_pos, m_size, m_tex, D3DXVECTOR4( 0.0f, 0.0f, 1.0f, 1.0f ), UI_RENDER_NORMAL, TFO_NEAREST, m_color, 0 );
	}

public:

	D3DXVECTOR2				m_pos;
	D3DXVECTOR2				m_size;
	IDirect3DTexture9*		m_tex;
	DWORD					m_color;
};
void	ioFlashPlayer::OnRender()
{
	__super::OnRender();

	if ( !m_flashplayer ) {
		return;
	}

	g_UIRenderer.AddUIQuad(
		new ioIQFlashPlayer
			(
				D3DXVECTOR2( GetDerivedPosX(), GetDerivedPosY() ),
				D3DXVECTOR2( GetWidth(), GetHeight() ),
				m_flashplayer->Render(),
				m_color
			)
	);
}

void	ioFlashPlayer::CheckFrameReSize( int iWidth, int iHeight, int iPrevWidth, int iPrevHeight )
{
	__super::CheckFrameReSize( iWidth, iHeight, iPrevWidth, iPrevHeight );

	if ( m_flashplayer ) {
		m_flashplayer->Resize( RenderSystem().GetDevice(), iWidth, iHeight );
	}
}


//----------------------------------------------------------------------------------
/*
Tool을 위한 인터페이스들
*/
void	ioFlashPlayer::ParseExtraInfo( ioXMLElement& extxml )
{
	ioWndEX::_ParseExtraInfo( extxml );
			
	if ( !m_flashplayer ) {
		return;
	}
		
	m_color = ioStringConverter::ParseColor( extxml.GetStringAttribute( "Color" ) );	
	m_flashplayer->SetLoop( false );
	m_flashplayer->SetTransparencyMode( extxml.GetBoolAttribute( "BackgroundTransparency" ) );
	m_flashplayer->SetBackgroundColor( ioStringConverter::ParseColor( extxml.GetStringAttribute( "BackColor" ) ) );

	int flashwidth = extxml.GetIntAttribute( "FlashWidth" ); 
	int flashheight = extxml.GetIntAttribute( "FlashHeight" );
	if ( flashwidth == 0 ) {
		flashwidth = GetWidth();
	}
	if ( flashheight == 0 ) {
		flashheight = GetHeight();
	}

	m_flashplayer->Resize( RenderSystem().GetDevice(), flashwidth, flashheight, GetWidth(), GetHeight() );
	m_flashplayer->Load( extxml.GetStringAttribute( "SWF" ) );	
}

void	ioFlashPlayer::Copy( ioWnd* wnd )
{
	ioWndEX::Copy( wnd );

	ioFlashPlayer* flashwnd = dynamic_cast<ioFlashPlayer*>(wnd);
	if ( !flashwnd ) {
		return;
	}

	flashwnd->m_color = m_color;
	if ( m_flashplayer )
	{
		SAFEDELETE( flashwnd->m_flashplayer );
		flashwnd->m_flashplayer = m_flashplayer->Clone( RenderSystem().GetDevice() );
	}
}

void	ioFlashPlayer::_SetSaveData( ioXMLElement& extxml )
{
	ioWndEX::_SetSaveData( extxml );

	if ( m_flashplayer )
	{
		extxml.SetBoolAttribute( "BackgroundTransparency", m_flashplayer->IsTransparencyMode() );
		extxml.SetStringAttribute( "Color", ioWnd::ConvertToColorString( m_color ).c_str() );
		extxml.SetStringAttribute( "BackColor", ioWnd::ConvertToColorString( m_flashplayer->GetBKColor() ).c_str() );
		extxml.SetIntAttribute( "FlashWidth", static_cast<int>( m_flashplayer->GetWidth() ) );
		extxml.SetIntAttribute( "FlashHeight", static_cast<int>( m_flashplayer->GetHeight() ) );
		extxml.SetStringAttribute( "SWF", m_flashplayer->GetFileName() );
	}
}



////----------------------------------------------------------------------------------
////----------------------------------------------------------------------------------
///*
//ioFlashPlayerTestWnd
//*/
////----------------------------------------------------------------------------------
////----------------------------------------------------------------------------------
//
////----------------------------------------------------------------------------------
///*
//생성 / 소멸
//*/
//ioFlashPlayerTestWnd::ioFlashPlayerTestWnd()
//	: ioWnd()
//	, m_flashplayer( 0 )
//{
//}
//
//ioFlashPlayerTestWnd::~ioFlashPlayerTestWnd()
//{
//}
//
////----------------------------------------------------------------------------------
///*
//ioWnd override
//*/
//void	ioFlashPlayerTestWnd::iwm_show()
//{
//	m_flashplayer = dynamic_cast<ioFlashPlayer*>( FindChildWnd( 1010 ) );
//
//	__super::iwm_show();
//}
//
//void	ioFlashPlayerTestWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
//{
//	__super::iwm_command( pWnd, cmd, param );
//
//	if ( !m_flashplayer ) {
//		return;
//	}
//
//	if ( cmd != IOBN_BTNUP ) {
//		return;
//	}
//
//	switch ( pWnd->GetID() )
//	{
//	case 10: // play
//		m_flashplayer->Play();
//		break;
//
//	case 20: // stop
//		m_flashplayer->Stop();
//		break;
//
//	case 30: // pause
//		m_flashplayer->Pause();
//		break;
//
//	case 40: // Resume
//		m_flashplayer->Resume();
//		break;
//	}
//}