//-------------------------------------------------------------------
//-------------------------------------------------------------------
// 작성자	: cha
// 날짜		: 2013.10.24
// 이름		: Visualizer2D
//-------------------------------------------------------------------
//-------------------------------------------------------------------

#ifndef __VISUALIZER2D_H__
#define __VISUALIZER2D_H__

#include <d3dx9.h>
#include "Vector.h"

//-------------------------------------------------------------
/*
Visualizer2D
*/
class ioWnd;
class ioRenderSystem;
class Visualizer2D
{
public:

	//-------------------------------------------------------------
	/*
	생성,파괴
	*/
	Visualizer2D();
	virtual ~Visualizer2D();

	//-------------------------------------------------------------
	/*
	Render Poly
	*/
	virtual void	RenderLine( const Vector2D& begin, const Vector2D& end, DWORD color );
	virtual void	RenderLine( const Vector2D& begin, const Vector2D& end, float weight, DWORD color );
	virtual void	RenderRectangle( const Vector2D& lt, const Vector2D& rt, const Vector2D& lb, const Vector2D& rb, float weight, DWORD color );
	virtual void	RenderSolidRectangle( const Vector2D& lt, const Vector2D& rt, const Vector2D& lb, const Vector2D& rb, DWORD color );

	//-------------------------------------------------------------
	/*
	Render
	*/
	virtual void	Render( IDirect3DDevice9* device );
	virtual void	Render( ioRenderSystem* rendersystem );
	virtual void	RenderRect( ioWnd* wnd, DWORD color, float weight, bool rendercross );

protected:

	//-------------------------------------------------------------
	/*
	Utils
	*/
	void			FillLineVertex( const Vector2D& v, DWORD color );
	void			FillRectangleVertex( const Vector2D& v, DWORD color );
		
protected:

	//-------------------------------------------------------------
	/*
	멤버들
	*/

	/*
	Line Vertex Struct
	*/
	struct LineVertex {
		enum { 
			FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE 
		};
		float		x, y, z, w;
		DWORD		color;
	};
	LineVertex		m_bufferline[ 10000 * 2 ];
	int				m_numdrawline;

	/*
	Rectangle Vertex Struct
	*/
	struct RectangleVertex {
		enum { 
			FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE 
		};
		float		x, y, z, w;
		DWORD		color;
	};
	RectangleVertex	m_bufferrectangle[ 10000 * 6 ];
	int				m_numdrawrectangle;
};

#endif //__VISUALIZER2D_H__
