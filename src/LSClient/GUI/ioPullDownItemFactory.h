#pragma once
#include "ioWnd.h"

class ioPullDownList;
class ioPullDownItem;
struct PullDownInnerImage;

enum PullDownItemType
{
	PDIT_SINGLE,
	PDIT_DOUBLE,
};

//풀다운 펼침 스타일
enum PullDownStyle
{
	PDS_DOWN,	//아래로(기본값)	
	PDS_UP,		//위로
};

//-----------------------------------------------------------------------------------------------------------------------------

//풀다운 내부에서 관리할 이미지 셋
struct PullDownInnerImage
{
	ioUIRenderImage*	m_pImage;
	DWORD				m_dwColor;
	DWORD				m_dwOverColor;
	bool				m_bManagedImageByItem;
	UIRenderType		m_eRenderType;

	int					m_iXPos;
	int					m_iYPos;

	PullDownInnerImage()
	{
		m_pImage				= NULL;
		m_dwColor				= 0;
		m_dwOverColor			= 0;
		m_bManagedImageByItem	= false;
		m_iXPos					= 0;
		m_iYPos					= 0;
		m_eRenderType			= UI_RENDER_NORMAL;
	}
};
typedef std::vector<PullDownInnerImage> vPullDownInnerImage;

//-----------------------------------------------------------------------------------------------------------------------------
//풀다운아이템 팩토리

class ioPullDownItemFactory : public Singleton< ioPullDownItemFactory >
{
public:
	static ioPullDownItemFactory& GetSingleton();

public:
	void Layout( ioPullDownList* pList, PullDownStyle eStyle );
	int PullDownItemLayout( ioPullDownList* pList, ioPullDownItem* pPullDown, PullDownStyle eStyle, int iCurrXPos, int iCurrYPos, int BlankWidth, int iBlankHeight );

	int SetSelectLayout( ioPullDownItem* pPullDown, PullDownStyle eStyle, int iXPos, int iYPos, int iWidth, int iHeight );

	__forceinline float GetFontTextPixel( float fScale ){ return 20.0f * fScale; }

public:
	ioPullDownItemFactory(void);
	virtual ~ioPullDownItemFactory(void);
};

#define g_PullDownFactory ioPullDownItemFactory::GetSingleton()