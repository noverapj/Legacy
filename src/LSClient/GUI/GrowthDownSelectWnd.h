#pragma once

#include "../io3DEngine/ioButton.h"
#include "../ioComplexStringPrinter.h"

#include "NewShopBuyWnd.h"

class ManualMgr;
class ioUIRenderImage;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct tagClassTypeInfo
{
	int m_iClassType;
	int m_iCurLevel;

	tagClassTypeInfo()
	{
		m_iClassType = 0;
		m_iCurLevel = 0;
	};
}ClassTypeInfo;

typedef std::vector< ClassTypeInfo > ClassTypeInfoList;

class ClassTypeInfoSort : public std::binary_function< const ClassTypeInfo&, const ClassTypeInfo&, bool >
{
public:
	bool operator()( const ClassTypeInfo &lhs , const ClassTypeInfo &rhs ) const
	{
		if( lhs.m_iCurLevel > rhs.m_iCurLevel )
		{
			return true;
		}				
		else if( lhs.m_iCurLevel == rhs.m_iCurLevel )
		{
			if( lhs.m_iClassType < rhs.m_iClassType )
				return true;
		}
		return false;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SelectListBtn : public ioButton
{
protected:
	int m_iCurMagicCode;

	ioHashString m_szDefaultText;
	ioHashString m_szSelectText;

public:
	void InitData();
	void SetListBtnInfo( ioHashString szTitle, int iMagicCode );

	inline int GetListMagicCode() const { return m_iCurMagicCode; }

protected:
	virtual void OnRender();

public:
	virtual void iwm_show();
	virtual void ParseExtraInfo( ioXMLElement &xElement );

public:
	SelectListBtn();
	virtual ~SelectListBtn();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GrowthDownSelectWnd : public ioWnd
{
public: 
	enum 
	{
		ID_ITEM_ICON			= 1,
		ID_CHANGE_CLASS_BTN		= 2,
		ID_CHANGE_GROWTH_BTN	= 3,
		ID_SELECT_BTN			= 4,

		ID_CLASS_LIST_WND		= 101,
		ID_GROWTH_LIST_WND		= 102,
	};

protected:
	int m_iClassType;
	int m_iReUseTargetClass;

	IntVec m_vExtraClassVec;
	ioHashString m_TitleText[8];
	ioUIRenderImage* m_pPImg;

	ioComplexStringPrinter m_EtcInfo;
	ioComplexStringPrinter m_Desc[2];

	ManualMgr* m_pManual;
	
protected:
	void UpdateData();
	void CheckSelectBtn();

	void SendGrowthDown();

protected:
	void GetClassList( vNewShopPullDownItem &rkList );
	void GetGrowthList( vNewShopPullDownItem &rkList );

	void SetClassSelect( ioWnd *pOpenBtn, int iListArray );	
	void SetGrowthSelect( ioWnd *pOpenBtn, int iListArray );

	void SetReUseTargetClassSelect( int iClassType );

	void PrintManual( int a_iXPos, int a_iYPos , float fScale );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	GrowthDownSelectWnd(void);
	virtual ~GrowthDownSelectWnd(void);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class GrowthAllDownSelectWnd : public ioWnd
{
public: 
	enum 
	{
		ID_ITEM_ICON			= 1,
		ID_CHANGE_CLASS_BTN		= 2,
		ID_SELECT_BTN			= 3,

		ID_CLASS_LIST_WND		= 101,

		ID_SOLDIER_WND          = 1000,
	};

	enum
	{
		MAX_DESC  = 2,
		MAX_TITLE = 8,
	};

protected:
	int m_iClassType;
	IntVec m_vExtraClassVec;

	ioComplexStringPrinter m_Desc[MAX_DESC];
	ioComplexStringPrinter m_sTitles[MAX_TITLE];

	CHARACTER m_CharInfo;

protected:
	void UpdateData();
	void CheckSelectBtn();

	void SendGrowthDown();

protected:
	void GetClassList( vNewShopPullDownItem &rkList );
	void GetGrowthList( vNewShopPullDownItem &rkList );

	void SetClassSelect( ioWnd *pOpenBtn, int iListArray );
	void SetGrowthSelect( ioWnd *pOpenBtn, int iListArray );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void OnRenderAfterChild();

public:
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

public:
	GrowthAllDownSelectWnd(void);
	virtual ~GrowthAllDownSelectWnd(void);
};