

#ifndef _ioUIRenderFrame_h_
#define _ioUIRenderFrame_h_

#include "ioUIRenderElement.h"
#include "ioUIRenderImage.h"

class ioUIRenderImage;

class __EX ioUIRenderFrame : public ioUIRenderElement
{
public:
	enum FrameLocation
	{
		FL_TOP_LEFT,
		FL_TOP_EDGE,
		FL_TOP_RIGHT,
		FL_LEFT_EDGE,
		FL_CENTER,
		FL_RIGHT_EDGE,
		FL_BOTTOM_LEFT,
		FL_BOTTOM_EDGE,
		FL_BOTTOM_RIGHT,
		MAX_FRAME_LOCATION,
		FL_UNKNOWN_FRAME,
	};

	enum FrameLocationSize
	{
		FS_WIDTH,
		FS_HEIGHT,
		MAX_FRAME_SIZE,
	};

protected:
	struct FrameData
	{
		ioUIRenderImage *m_Frame;
		int m_FrameSize[MAX_FRAME_SIZE];
		
		FrameData()
		{
			Init();
		}

		FrameData& operator = ( ioUIRenderImage *pFrame )
		{
			m_Frame = pFrame;
			SetFramePos();
			return *this;
		}
		
		void Init()
		{
			m_Frame = NULL;
			for(int i = 0;i < MAX_FRAME_SIZE;i++)
				m_FrameSize[i] = 0;
		}

		void SetFramePos()
		{
			if( m_Frame )
			{
				m_FrameSize[FS_WIDTH] = m_Frame->GetWidth();
				m_FrameSize[FS_HEIGHT] = m_Frame->GetHeight();
			}
		}

		void Clear()
		{
			SAFEDELETE( m_Frame );
			Init();
		}
	};

protected:
	FrameData m_FrameData[MAX_FRAME_LOCATION];	
	D3DXVECTOR2 m_RenderedPos;

public:
	virtual void Render( float fOffX,
						 float fOffY,
						 UIRenderType eRenderType = UI_RENDER_NORMAL,
						 TextureFilterOption eOption = TFO_NEAREST,
						 bool bNoMagicAlign = false,
						 UIRenderType eGrayRenderType = UI_RENDER_NORMAL );

	virtual ioUIRenderElement* Clone() const;
	virtual UIElementType GetType() const;

	//-------------------------------------------------
	// Tool을 위한 인터페이스들
protected:	
	bool m_bAutoReSize;
	std::string m_name;

public:
	__forceinline void SetName( const std::string& name ) { m_name = name; }
	__forceinline const std::string& GetName() const { return m_name; }	
	__forceinline const RECT& GetRenderRect() const { return m_RenderRect; }
	//-------------------------------------------------

public:
	virtual void SetRenderRect( const RECT &rcRect );
	virtual void SetScale( float fRate );

	DWORD GetColor() { return m_dwColor; }
	virtual void SetColor( DWORD dwColor );
	virtual void SetColor( int iRed, int iGreen, int iBlue );
	virtual void SetAlpha( BYTE iAlpha );
	virtual void SetAlphaWithColor( BYTE iAlpha );

public:
	void SetImages( ioUIRenderImage *pTopLeft,
					ioUIRenderImage *pTopEdge,
					ioUIRenderImage *pTopRight,
					ioUIRenderImage *pLeftEdge,
					ioUIRenderImage *pCenter,
					ioUIRenderImage *pRightEdge,
					ioUIRenderImage *pBottomLeft,
					ioUIRenderImage *pBottomEdge,
					ioUIRenderImage *pBottomRight );

	bool SetImageForLocation( FrameLocation eLocation, ioUIRenderImage *pImage );
	bool SetImageForLocation( const ioHashString &szLocation, ioUIRenderImage *pImage );
	bool SetImageForSize( const ioHashString &szLocation, const ioHashString &szSize );

	ioUIRenderFrame::FrameData* GetDataForLocation( FrameLocation eLocation );

	inline void SetAutoReSize( bool bReSize ) { m_bAutoReSize = bReSize; }
	inline bool IsAutoReSize() const { return m_bAutoReSize; }
	inline const D3DXVECTOR2& GetRenderedPos(){ return m_RenderedPos; }

protected:
	void UpdateFrameImagesColor();
	void UpdateFrameImagesAlpha( int iAlpha );


	void AdjustFrameImages();
	void ClearFrameImages();

	float EdgeWidth( FrameLocation eLocation );
	float EdgeHeight( FrameLocation eLocation );

public:
	static FrameLocation ConvertFrame( const ioHashString &szType );

public:
	ioUIRenderFrame();
	ioUIRenderFrame( const ioUIRenderFrame &rhs );
	virtual ~ioUIRenderFrame();
};

inline ioUIRenderFrame* ToUIFrame( ioUIRenderElement *pElement )
{
	if( !pElement || pElement->GetType() != ioUIRenderElement::UIE_FRAME )
		return NULL;

	return dynamic_cast< ioUIRenderFrame* >( pElement );
}

#endif