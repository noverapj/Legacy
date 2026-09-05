//-------------------------------------------------------------------
//-------------------------------------------------------------------
// 작성자	: cha
// 날짜		: 2013.10.24
// 이름		: Visualizer2D
//-------------------------------------------------------------------
//-------------------------------------------------------------------

#include "stdafx.h"
#include "Visualizer2D.h"
#include "ioRenderSystem.h"
#include "ioWnd.h"

//-------------------------------------------------------------
/*
생성
*/
Visualizer2D::Visualizer2D()
: m_numdrawline( 0 )
, m_numdrawrectangle( 0 )
{
}

//-------------------------------------------------------------
/*
파괴
*/
Visualizer2D::~Visualizer2D()
{
}

//-------------------------------------------------------------
/*
Render Poly
*/
void	Visualizer2D::FillLineVertex( const Vector2D& v, DWORD color )
{
	m_bufferline[ m_numdrawline ].x = v.x;
	m_bufferline[ m_numdrawline ].y = v.y;
	m_bufferline[ m_numdrawline ].z = 0.0f;
	m_bufferline[ m_numdrawline ].w = 1.0f;
	m_bufferline[ m_numdrawline ].color = color;
	++m_numdrawline;
}
void	Visualizer2D::RenderLine( const Vector2D& begin, const Vector2D& end, DWORD color )
{
	FillLineVertex( begin, color );
	FillLineVertex( end, color );
}

void	Visualizer2D::RenderLine( const Vector2D& begin, const Vector2D& end, float weight, DWORD color )
{
	Vector2D dir = end - begin;
	dir.Rotate2( -D3DX_PI * 0.5f );
	Vector2D wnormal = dir.GetUnitVec() * weight;

	RenderSolidRectangle( begin + wnormal, end + wnormal, begin - wnormal, end - wnormal, color );
}

void	Visualizer2D::FillRectangleVertex( const Vector2D& v, DWORD color )
{
	m_bufferrectangle[ m_numdrawrectangle ].x = v.x;
	m_bufferrectangle[ m_numdrawrectangle ].y = v.y;
	m_bufferrectangle[ m_numdrawrectangle ].z = 0.0f;
	m_bufferrectangle[ m_numdrawrectangle ].w = 1.0f;
	m_bufferrectangle[ m_numdrawrectangle ].color = color;
	++m_numdrawrectangle;
}
void	Visualizer2D::RenderRectangle( const Vector2D& lt, const Vector2D& rt, const Vector2D& lb, const Vector2D& rb, float weight, DWORD color )
{
	RenderLine( lt, rt, weight, color );
	RenderLine( rt, rb, weight, color );
	RenderLine( lt, lb, weight, color );
	RenderLine( lb, rb, weight, color );
}

void	Visualizer2D::RenderSolidRectangle( const Vector2D& lt, const Vector2D& rt, const Vector2D& lb, const Vector2D& rb, DWORD color )
{
	FillRectangleVertex( lt, color );
	FillRectangleVertex( rt, color );
	FillRectangleVertex( lb, color );

	FillRectangleVertex( rt, color );
	FillRectangleVertex( rb, color );
	FillRectangleVertex( lb, color );
}

//-------------------------------------------------------------
/*
Render
*/
void	Visualizer2D::Render( IDirect3DDevice9* device )
{
	device->BeginScene();

	if ( m_numdrawline > 0 )
	{
		DWORD preFVF = 0;
		IDirect3DBaseTexture9* preTexture = 0;

		device->GetFVF( &preFVF );
		device->GetTexture( 0, &preTexture );

		device->SetTexture( 0, 0 );
		device->SetFVF( LineVertex::FVF );
		device->DrawPrimitiveUP( D3DPT_LINELIST, ( m_numdrawline + 1 ) / 2, (void*)m_bufferline, sizeof( LineVertex ) );

		device->SetTexture( 0, preTexture );
		device->SetFVF( preFVF );
	}

	if ( m_numdrawrectangle > 0 )
	{
		DWORD preFVF = 0;
		IDirect3DBaseTexture9* preTexture = 0;

		device->GetFVF( &preFVF );
		device->GetTexture( 0, &preTexture );
				
		device->SetTexture( 0, 0 );
		device->SetFVF( RectangleVertex::FVF );
		device->DrawPrimitiveUP( D3DPT_TRIANGLELIST, ( m_numdrawrectangle + 1 ) / 3, (void*)m_bufferrectangle, sizeof( RectangleVertex ) );

		device->SetTexture( 0, preTexture );
		device->SetFVF( preFVF );
	}

	device->EndScene();

	m_numdrawline = 0;
	m_numdrawrectangle = 0;
}

void	Visualizer2D::Render( ioRenderSystem* rendersystem )
{
	if ( !rendersystem ) {
		return;
	}
	
	if ( !rendersystem->BeginScene() ) {
		return;
	}
		
	rendersystem->DisableShader();
	rendersystem->SetDepthFunction( D3DCMP_LESSEQUAL, true, true );
	rendersystem->SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_DIFFUSE, D3DTA_DIFFUSE );
	rendersystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_DIFFUSE, D3DTA_DIFFUSE );
	rendersystem->DisableTextureUnitsFrom( 1 );
	rendersystem->ClearStreamDesc();

	IDirect3DDevice9* device = rendersystem->GetDevice();

	if ( m_numdrawline > 0 )
	{
		rendersystem->SetFixedFVF( LineVertex::FVF );
		device->DrawPrimitiveUP( D3DPT_LINELIST, ( m_numdrawline + 1 ) / 2, (void*)m_bufferline, sizeof( LineVertex ) );
	}

	if ( m_numdrawrectangle > 0 )
	{
		rendersystem->SetFixedFVF( RectangleVertex::FVF );
		device->DrawPrimitiveUP( D3DPT_TRIANGLELIST, ( m_numdrawrectangle + 1 ) / 3, (void*)m_bufferrectangle, sizeof( RectangleVertex ) );
	}

	rendersystem->EndScene();

	m_numdrawline = 0;
	m_numdrawrectangle = 0;	
}

void	Visualizer2D::RenderRect( ioWnd* wnd, DWORD color, float weight, bool rendercross )
{
	if ( !wnd ) {
		return;
	}

	int x = wnd->GetDerivedPosX();
	int y = wnd->GetDerivedPosY();
	int width = wnd->GetWidth();
	int height = wnd->GetHeight();

	RenderRectangle( Vector2D( x, y ), Vector2D( x + width, y ), Vector2D( x, y + height ), Vector2D( x + width, y + height ), weight, color );

	if ( rendercross )
	{
		RenderLine( Vector2D( x, y ), Vector2D( x + width, y + height ), weight, color );
		RenderLine( Vector2D( x + width, y ), Vector2D( x, y + height ), weight, color );
	}
}