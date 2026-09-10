#pragma once

#include "../CharacterInfo.h"
#include "../io3DEngine/ioStringConverter.h"
#include "../ioComplexStringPrinter.h"
#include "../ioUserExtraItem.h"
#include "MyInventoryWnd.h"

class ioUI3DRender;
class ioUIRenderFrame;
class ioCreateChar;
class ioItem;
class ioSetItemInfo;

class ItemCustomTrainIconBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;

	float m_fScale;
	float m_fBackScale;

	int m_nGradeType;

protected:
	void RenderIcon(int iXPos, int iYPos, UIRenderType eType);
	void RenderBack(int iXPos, int iYPos );

public:
	bool SetIcon( ioUIRenderImage *pIcon, float fScale, float fBackScale );
	void SetGradeType( int nGradeType ) { m_nGradeType = nGradeType; }
	
public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	

protected:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	ItemCustomTrainIconBtn();
	virtual ~ItemCustomTrainIconBtn();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemCustomTrainInfoWnd : public ioButton
{
public:
	enum 
	{
		ID_ICON_BTN		= 1,
		ID_MALE_BTN		= 2,
		ID_FEMALE_BTN	= 3,
	};

	enum
	{
		MAX_DESC	= 3,
	};

public:
	ioUIRenderImage *m_pEmptyImg;

protected:
	DWORD m_dwItemCode;
	ioHashString m_szItemName;

public:
	void SetInfo( DWORD dwItemCode );

public:
	DWORD GetItemCode(){ return m_dwItemCode; }

protected:
	virtual void OnRender();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	ItemCustomTrainInfoWnd();
	virtual ~ItemCustomTrainInfoWnd();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemCustomTrainSelectWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT_BTN			= 1,

		ID_TAB_1_BTN	= 101, // extra : all
		ID_TAB_2_BTN	= 102, // extra : weapon
		ID_TAB_3_BTN	= 103, // extra : armor
		ID_TAB_4_BTN	= 104, // extra : helmet
		ID_TAB_5_BTN	= 105, // extra : cloak

		ID_ITEM_INFO_WND_1_BTN	= 201,
		ID_ITEM_INFO_WND_2_BTN	= 202,
		ID_ITEM_INFO_WND_3_BTN	= 203,
		ID_ITEM_INFO_WND_4_BTN	= 204,
		ID_ITEM_INFO_WND_5_BTN	= 205,
		ID_ITEM_INFO_WND_6_BTN	= 206,
		ID_ITEM_INFO_WND_7_BTN	= 207,
		ID_ITEM_INFO_WND_8_BTN	= 208,

		ID_VERT_SCROLL			= 1001,
	};

	enum 
	{
		MAX_TAB_BTN				= 5,

		MAX_ITEM_INFO_WND_BTN	= 8,
		PAGE_PER_ITEM_NUM		= 4,
	};

protected:
	DWORDVec m_dwItemCodeList;

	int m_iCurPage;
	DWORD m_dwCurTabID;

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void iwm_vscroll(DWORD dwID, int curPos );
	virtual void iwm_wheel( int zDelta );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

protected:
	void ChangeTab( DWORD dwID );
	void UpdatePage( int iNewPage );

	void SetExtraItemInfoVec( DWORD dwTabID );
	bool IsRightTab( DWORD dwTabID, int iValue1 );
	int  GetTabType( DWORD dwTabID );

public:
	void Update();

public:
	ItemCustomTrainSelectWnd();
	virtual ~ItemCustomTrainSelectWnd();
};

//////////////////////////////////////////////////////////////////////////
class ItemCustomTrainMakeWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT_BTN		 = 1,
		ID_SELECT_ITEM   = 10,
		ID_UPLOAD_FILE	 = 11,
		ID_DOWNLOAD_FILE = 12,
		ID_DETAIL_VIEW   = 13,

		ID_CHANGE_BTN	 = 20,
		ID_HELP_BTN		 = 21,

		ID_BEFORE_VIEW   = 30,
		ID_AFTER_VIEW    = 31,
	};

protected:
	DWORD        m_dwItemCode;
	ioHashString m_szItemName;
	bool		 m_bMale;

protected:
	ioComplexStringPrinter m_ItemNameMent;
	ioComplexStringPrinter m_TextureCountMent;

protected:
	CustomTextureList m_OriginalTexList;
	CustomTextureList m_AfterTexList;
	int               m_TexSelectCount;	

protected:
	DWORD m_dwHelpID;

protected:
	void UploadTexture();
	void DownloadTexture();
	void ShowDetailView(); 

public:
	void AfterViewLSCTexture( int iSelectCount );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_esc();
	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();
	virtual void OnProcess( float fTimePerSec );

public:
	void SetExtraItemInfo( DWORD dwItemCode, bool bMale );

public:
	ItemCustomTrainMakeWnd();
	virtual ~ItemCustomTrainMakeWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemCustomWantEtcItemWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT		  = 1,
		ID_SHOP		  = 2,
		ID_INVENTORY  = 3,
		ID_CLOSE	  = 4,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIcon;

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	ItemCustomWantEtcItemWnd();
	virtual ~ItemCustomWantEtcItemWnd();
};
