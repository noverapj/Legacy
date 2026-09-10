#pragma once


#include "../io3DEngine/ioMovingWnd.h"
#include "../ioComplexStringPrinter.h"

class ioUIRenderImage;
class ManualMgr 
{
public:
	enum 
	{
		MAX_ITEM = 30,
	};
protected:
	struct ColorText
	{
		int  m_iSpaceLine;
		DWORD m_dwColor;
		ioHashString m_szText;
		ioHashString m_szIconName;

		ColorText()
		{
			m_iSpaceLine = 0;
			m_dwColor = 0;
		}

		void Clear()
		{
			m_iSpaceLine = 0;
			m_dwColor = 0;

			m_szText.Clear();
			m_szIconName.Clear();
		}
	};
	typedef std::vector< ColorText > vColorText;
	typedef std::vector< vColorText > vLineText;
	typedef std::vector< vLineText > vManualList;

	// image
	struct ImgInfo
	{
		ioHashString m_szImageName;
		DWORD        m_dwDuration;

		ImgInfo()
		{
			m_szImageName.Clear();
			m_dwDuration = 0;
		}

		void Clear()
		{
			m_szImageName.Clear();
			m_dwDuration = 0;
		}
	};

	typedef std::vector< ImgInfo > ImgInfoVec;
	typedef std::vector< ImgInfoVec > ImgInfoVecOfVec;
protected:
	vManualList m_vManualList;
	// image
	ImgInfoVecOfVec m_ImginfoVecOfVec;

protected:
	virtual void ClearList();

public:
	virtual void LoadManual(const char* szININame);

#ifndef SHIPPING //UJ
	#define LoadManual_e	LoadManual
#else
	//UJ 130812, 스트링 상수 암호화 및 숨기기 위한 코드
	template <int N>
	__forceinline void LoadManual_e( const char (&szININame)[N] )
	{
		_ENCSTR(szININame, szEncININame);
		LoadManual(szEncININame);
	}
#endif

public:
	const int GetMaxManual();
	
	virtual const int GetMaxLine(const int iCurManual);
	virtual const int GetSpaceLine(const int iCurManual, const int iCurLine);
	virtual void SetPrinter(const int iCurManual, const int iCurLine, float fScale, OUT ioComplexStringPrinter &rkPrinter);

	const ioHashString* GetIconName(const int iCurManual, const int iCurLine);
	const ioHashString* GetTitleName(const int iCurManual);

	void GetStringAndColor( IN const int iCurManual, IN const int iCurLine, OUT char *szLineString , IN const int iLineStringArraySize ,
		                    OUT DWORD *pdwColor, IN const int iColorArraySize );

	// image
	const int GetMaxImage( const int iCurManual );
	const ioHashString* GetImageName( const int iCurManual, const int iCurArray );
	const DWORD GetImageDuration( const int iCurManual, const int iCurArray );

	bool IsEmpty() { return m_vManualList.empty(); }

public:
	ManualMgr();
	virtual ~ManualMgr();
};
//-------------------------------------------------------------------------
class ioItemManualMgr : public ManualMgr , public Singleton< ioItemManualMgr >
{
public:
	static ioItemManualMgr& GetSingleton();
	static ioItemManualMgr *GetSingletonPtr();
public:
	ioItemManualMgr();
	virtual ~ioItemManualMgr();
};
//-------------------------------------------------------------------------
class ioDecoManualMgr : public ManualMgr , public Singleton< ioDecoManualMgr >
{
public:
	static ioDecoManualMgr& GetSingleton();
	static ioDecoManualMgr *GetSingletonPtr();
public:
	ioDecoManualMgr();
	virtual ~ioDecoManualMgr();
};
//-------------------------------------------------------------------------
class ioEtcManualMgr : public ManualMgr , public Singleton< ioEtcManualMgr >
{
public:
	static ioEtcManualMgr& GetSingleton();
	static ioEtcManualMgr *GetSingletonPtr();
public:
	ioEtcManualMgr();
	virtual ~ioEtcManualMgr();
};
//-------------------------------------------------------------------------
class ioEventGoodsManualMgr : public ManualMgr , public Singleton< ioEventGoodsManualMgr >
{
public:
	static ioEventGoodsManualMgr& GetSingleton();
	static ioEventGoodsManualMgr *GetSingletonPtr();
public:
	ioEventGoodsManualMgr();
	virtual ~ioEventGoodsManualMgr();
};
//-------------------------------------------------------------------------
class ioEtcInvenManualMgr : public ManualMgr , public Singleton< ioEtcInvenManualMgr >
{
public:
	static ioEtcInvenManualMgr& GetSingleton();
	static ioEtcInvenManualMgr *GetSingletonPtr();
public:
	ioEtcInvenManualMgr();
	virtual ~ioEtcInvenManualMgr();
};
//-------------------------------------------------------------------------
class ioEtcInvenSubManualMgr : public ManualMgr , public Singleton< ioEtcInvenSubManualMgr >
{
public:
	static ioEtcInvenSubManualMgr& GetSingleton();
	static ioEtcInvenSubManualMgr *GetSingletonPtr();
public:
	ioEtcInvenSubManualMgr();
	virtual ~ioEtcInvenSubManualMgr();
};
//-------------------------------------------------------------------------
class ioPresentInvenManualMgr : public ManualMgr , public Singleton< ioPresentInvenManualMgr >
{
protected:
	typedef std::map< int, int > PresentMentMap;

	PresentMentMap m_PresentMentMap;

protected:
	virtual void ClearList();

public:
	virtual void LoadManual(const char* szININame);

	virtual const int GetMaxLine(const int iCurManual);
	virtual const int GetSpaceLine(const int iCurManual, const int iCurLine);
	virtual void SetPrinter(const int iCurManual, const int iCurLine, float fScale, OUT ioComplexStringPrinter &rkPrinter);

public:
	static ioPresentInvenManualMgr& GetSingleton();
	static ioPresentInvenManualMgr *GetSingletonPtr();
public:
	ioPresentInvenManualMgr();
	virtual ~ioPresentInvenManualMgr();
};
//-------------------------------------------------------------------------
class ioExtraInvenManualMgr : public ManualMgr , public Singleton< ioExtraInvenManualMgr >
{
public:
	static ioExtraInvenManualMgr& GetSingleton();
	static ioExtraInvenManualMgr *GetSingletonPtr();
public:
	ioExtraInvenManualMgr();
	virtual ~ioExtraInvenManualMgr();
};
//-------------------------------------------------------------------------
class ioMedalInvenManualMgr : public ManualMgr , public Singleton< ioMedalInvenManualMgr >
{
public:
	static ioMedalInvenManualMgr& GetSingleton();
	static ioMedalInvenManualMgr *GetSingletonPtr();
public:
	ioMedalInvenManualMgr();
	virtual ~ioMedalInvenManualMgr();
};
//-------------------------------------------------------------------------
class ioDecoInvenManualMgr : public ManualMgr , public Singleton< ioDecoInvenManualMgr >
{
public:
	static ioDecoInvenManualMgr& GetSingleton();
	static ioDecoInvenManualMgr *GetSingletonPtr();
public:
	ioDecoInvenManualMgr();
	virtual ~ioDecoInvenManualMgr();
};
//-------------------------------------------------------------------------
class ioPetInvenManualMgr : public ManualMgr , public Singleton< ioPetInvenManualMgr >
{
public:
	static ioPetInvenManualMgr& GetSingleton();
	static ioPetInvenManualMgr *GetSingletonPtr();
public:
	ioPetInvenManualMgr();
	virtual ~ioPetInvenManualMgr();
};

class ioCostumeInvenManualMgr : public ManualMgr , public Singleton< ioCostumeInvenManualMgr >
{
public:
	static ioCostumeInvenManualMgr& GetSingleton();
	static ioCostumeInvenManualMgr *GetSingletonPtr();
public:
	ioCostumeInvenManualMgr();
	virtual ~ioCostumeInvenManualMgr();
};

class ioCostumeShopManualMgr : public ManualMgr , public Singleton< ioCostumeShopManualMgr >
{
public:
	static ioCostumeShopManualMgr& GetSingleton();
	static ioCostumeShopManualMgr *GetSingletonPtr();
public:
	ioCostumeShopManualMgr();
	virtual ~ioCostumeShopManualMgr();
};

class ioPopupStorePresentItemManualMgr : public ManualMgr , public Singleton< ioPopupStorePresentItemManualMgr >
{
protected:
	typedef std::map< int, int > PresentMentMap;

	PresentMentMap m_PresentMentMap;

protected:
	virtual void ClearList();

public:
	virtual void LoadManual(const char* szININame);

	virtual const int GetMaxLine(const int iCurManual);
	virtual const int GetSpaceLine(const int iCurManual, const int iCurLine);
	virtual void SetPrinter(const int iCurManual, const int iCurLine, float fScale, OUT ioComplexStringPrinter &rkPrinter);

public:
	static ioPopupStorePresentItemManualMgr& GetSingleton();
	static ioPopupStorePresentItemManualMgr *GetSingletonPtr();

public:
	ioPopupStorePresentItemManualMgr();
	virtual ~ioPopupStorePresentItemManualMgr();
};
//-------------------------------------------------------------------------
class ioAccessoryInvenManualMgr : public ManualMgr , public Singleton< ioAccessoryInvenManualMgr >
{
public:
	static ioAccessoryInvenManualMgr& GetSingleton();
	static ioAccessoryInvenManualMgr *GetSingletonPtr();
public:
	ioAccessoryInvenManualMgr();
	virtual ~ioAccessoryInvenManualMgr();
};
//-------------------------------------------------------------------------
// 위의 메뉴얼 클래스들과 아래의 윈도우는 전혀 상관없다.
class ManualWnd : public ioWnd
{
public:
	enum
	{
		ID_VERT_SCROLL      = 1000,
	};

	enum
	{
		MAX_PAGE = 20,
	};

protected:
	struct ManualHelp
	{
		int m_iOffSetX;
		int m_iOffSetY; 
		ioUIRenderImage *m_pImage;
		ioComplexStringPrinter m_szHelp;
		ManualHelp()
		{
			m_pImage = NULL;
			m_iOffSetX = m_iOffSetY = 0;
		}
	};
	typedef std::vector< ManualHelp* > vManualHelp;
	struct ManualHelpList
	{
		bool         m_bLoad;
		ioHashString m_szKey;

		ioHashString m_szTitle;
		vManualHelp  m_ManualHelp;
		ManualHelpList()
		{
			m_bLoad = false;
		}
	};
	typedef std::map< DWORD, ManualHelpList > ManualHelpListMap;
	ManualHelpListMap m_ManualHelpListMap;

protected:
	int             m_iCurPos;
	int             m_iCurIndex;
	ManualHelpList 	m_CurManualHelp;

protected:
	void RenderManual( int iXPos, int iYPos );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll(DWORD dwID, int curPos );
	
protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	
public:
	void ShowWndByCurManual( const int iCurManual );

public:
	ManualWnd();
	virtual ~ManualWnd();
};
//////////////////////////////////////////////////////////////////////////   6005
class ManualListWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT				= 1,
		ID_MANUAL_WND       = 2,
		ID_VERT_SCROLL      = 1000,
	};

	enum
	{
		MAX_PAGE = 20,
	};

protected:
	ioUIRenderImage *m_pDarkLine;
	ioUIRenderImage *m_pLightLine;
	ioUIRenderImage *m_pOverLine;
	ioUIRenderImage *m_pSelectIcon;
	ioUIRenderImage *m_pDotLine;

protected:
	struct ManualTitle
	{
		DWORD m_dwIndex;
		ioHashString m_szTitle;
		ManualTitle()
		{
			m_dwIndex = 0;
		}
	};
	typedef std::vector< ManualTitle > ManualTitleList;
	ManualTitleList  m_ManualTitleList;
	int				 m_iOverListIndex;
	int              m_iPushListIndex;
	int				 m_iCurPos;
	int              m_iSelectIndex;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll(DWORD dwID, int curPos );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void ShowWndByCurManual( const int iCurManual );

public:
	bool QuestGuide( DWORD dwIndex, float &rfXPos, float &rfYPos );

public:
	ManualListWnd();
	virtual ~ManualListWnd();
};

