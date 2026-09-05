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

class ItemCustomIconBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pIconBack;

	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;
	ioUIRenderImage *m_pCostumeMark;

	ioUIRenderFrame *m_pStateOrangeFrm;
	ioUIRenderFrame *m_pStateGreenFrm;
	ioUIRenderFrame *m_pStateBlueFrm;

	int m_iMarkType; //
	bool m_bEquip;
	int m_iNum;
	float m_fScale;
	float m_fBackScale;
	bool m_bIsCostume;
	int m_nGradeType;

protected:
	void RenderIcon(int iXPos, int iYPos, UIRenderType eType);
	void RenderBack(int iXPos, int iYPos );
	void RenderOtherInfo( int iXPos, int iYPos );

public:
	bool SetIcon( ioUIRenderImage *pIcon, float fScale, float fBackScale, int iNum=0 );
	void SetMarkType( int iMarkType ) { m_iMarkType = iMarkType; } 
	void SetEquip( bool bEquip ){ m_bEquip = bEquip; }
	int  GetMarkType(){ return m_iMarkType; }
	void SetCostume( bool bIsCostume ) { m_bIsCostume = bIsCostume; }
	void SetGradeType( int nGradeType ){ m_nGradeType = nGradeType; }

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );	
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

protected:
	virtual void OnDrawNormal( int iXPos, int iYPos );
	virtual void OnDrawOvered( int iXPos, int iYPos );
	virtual void OnDrawPushed( int iXPos, int iYPos );
	virtual void OnDrawDisabled( int iXPos, int iYPos );

protected:
	virtual void OnProcess( float fTimePerSec );

public:
	ItemCustomIconBtn();
	virtual ~ItemCustomIconBtn();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemCustomInfoWnd : public ioButton
{
public:
	enum 
	{
		ID_ICON_BTN			= 1,
		ID_MALE1_BTN		= 2,
		ID_MALE2_BTN		= 3,
		ID_FEMALE_BTN		= 4,
		ID_CUSTOM_INFO_BTN  = 5,
	};

	enum
	{
		MAX_DESC	= 3,
	};

protected:
	bool m_bWidthCutDescArray[MAX_DESC];
	ioComplexStringPrinter m_szDescArray[MAX_DESC];

	InventoryItemInfo m_ItemInfo;
	ioUIRenderImage *m_pEmptyImg;

	int m_iExtraItemIndex;
	
protected:
	void SetBlank();
	void ClearDesc();
	void RenderDesc( int iXPos, int iYPos );

	void SetIsWidthCutDesc( int iArray , bool bWidthCut );
	bool IsWidthCutDesc( int iArray );

	bool SetExtraItemInfo();

public:
	void SetInfo( InventoryItemInfo &rkInfo );

	InventoryItemInfo& GetInfo();
	int GetMarkType();

	inline int GetExtraItemIndex() const { return m_iExtraItemIndex; }
	inline int GetMagicCode() const { return m_ItemInfo.m_iMagicCode; }
	bool IsCustomToolTip();
	bool IsIconOver();

protected:
	virtual void OnRender();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	ItemCustomInfoWnd();
	virtual ~ItemCustomInfoWnd();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ItemCustomSelectWnd : public ioMovingWnd
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
	vInventoryItemInfoList m_vInventoryItemInfoList;

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
	void ProcessCustomBtnTooltip();

protected:
	void ChangeTab( DWORD dwID );
	void UpdatePage( int iNewPage );

	void SetExtraItemInfoVec( DWORD dwTabID );
	bool IsRightTab( DWORD dwTabID, int iValue1 );
	int  GetTabType( DWORD dwTabID );
	void OnItemCustomInfoPush( DWORD dwID );

public:
	void Update();

public:
	ItemCustomSelectWnd();
	virtual ~ItemCustomSelectWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemCustomMakeUploadWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT		= 1,
		ID_UPLOAD	= 2,
		ID_CANCEL   = 3,

		ID_FILE_FIND1 = 10,
		ID_FILE_FIND2 = 11,
		ID_FILE_FIND3 = 12,
		ID_FILE_FIND4 = 13,
		ID_FILE_FIND5 = 14,
		ID_FILE_FIND6 = 15,
		ID_FILE_FIND7 = 16,
		ID_FILE_FIND8 = 17,
		ID_FILE_FIND9 = 18,
		ID_FILE_FIND10= 19,
	};

	enum
	{
		MAX_FILE = 10,
		DEFAULT_HEIGHT = 170,
		LINE_ADD_HEIGHT= 24,
	};

protected:
	ioUIRenderFrame *m_pBottomFrm;
	ioUIRenderFrame *m_pTextLineFrm;


protected:
	ioComplexStringPrinter m_ItemNameMent;
	ioComplexStringPrinter m_TextureCountMent;

protected:
	ioHashString m_szFileFindPath;

protected:
	struct TextureData
	{
		CustomTexture m_CustomTex;
		int           m_iWidth;
		int           m_iHeight;
		ioHashString  m_szSelectFile;

		TextureData()
		{
			m_iWidth = m_iHeight = 0;
		}
	};
	typedef std::vector< TextureData > TextureDataList;
	TextureDataList m_TextureList;

protected:
	DWORD m_dwCallWndID;

public:
	void SetUpload( DWORD dwCallWndID, ioComplexStringPrinter &rkItemNameMent, ioComplexStringPrinter &rkTextureCountMent, CustomTextureList &rkOriginalTexList );
	void SetFilePath( ioWnd *pReturnBtn, const ioHashString &rkFilePath );

protected:
	void CompleteUpload();

public:
	virtual void iwm_create();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

protected:
	virtual void OnRender();

public:
	ItemCustomMakeUploadWnd();
	virtual ~ItemCustomMakeUploadWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemCustomMakeDownloadWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
		ID_OK   = 2,
	};

protected:
	ioHashString m_szDownPath;
	ioHashString m_szDownFile;
	int          m_iFileCount;

public:
	void SetDownload( const ioHashString &rkDownPath, const ioHashString &rkDownFile, int iFileCount );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	ItemCustomMakeDownloadWnd();
	virtual ~ItemCustomMakeDownloadWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemCustomMakeWnd : public ioWnd
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
	enum FileUploadState
	{
		FUS_NONE                = 0,
		FUS_FILE_UPLOAD_DALAY   = 1,
		FUS_FILE_UPLOAD         = 2,
	};
	FileUploadState m_eFileUplaodState;
	DWORD           m_dwUploadDelayTime;
	DWORD           m_dwUploadUniqueIndex;
	struct FileUploadData
	{
		char m_szDstFilePath[1024];
		char m_szWriteFilePath[1024];
		int  m_iSrcWidth;
		int  m_iSrcHeight;

		FileUploadData()
		{
			m_iSrcWidth = m_iSrcHeight = 0;
			memset( m_szDstFilePath, 0, sizeof( m_szDstFilePath ) );
			memset( m_szWriteFilePath, 0, sizeof( m_szWriteFilePath ) ); 
		}
	};
	typedef std::vector< FileUploadData > FileUploadDataList;
	FileUploadDataList m_FileUploadDataList;
	FileUploadDataList m_FileUploadCompleteList;

protected:
	InventoryItemInfo m_ItemInfo;
	ioHashString      m_szItemName;
	bool			  m_bMale;

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
	void ProcessUploadDelay();

public:
	void StartCustomFileMake( DWORD dwUniqueIndex );
	void EndFileUpload( DWORD param );
	void CompleteCustomFileMake();

public:
	void SetExtraItemInfo( InventoryItemInfo &rkItem, bool bMale );

public:
	ItemCustomMakeWnd();
	virtual ~ItemCustomMakeWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemCustomMake3DViewWnd : public ioWnd
{
protected:
	ioUI3DRender	*m_pUI3DRender;
	ioCreateChar	*m_aMyChar;

protected:
	ioUIRenderFrame *m_pStateFrm;
	ioUIRenderImage *m_pRectSrc;
	ioUIRenderImage *m_pMouseLeft;
	ioUIRenderImage *m_pMouseRight;
	ioUIRenderImage *m_pMouseMiddle;

protected:
	enum
	{
		RECT_WIDTH_CNT = 18,
		RECT_HEIGHT_CNT= 16,
		RECT_GAP_SIZE  = 12,
	};

public:
	void CreateCharacter( const CHARACTER &rkInfo, const ITEM_DATA &rkItem, CustomTextureList &rkReturn );
	void ChangeLSCTexture( CustomTextureList &rkOriTexList, CustomTextureList &rkAftTexList );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRectRender( int iXPos, int iYPos );

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	ItemCustomMake3DViewWnd();
	virtual ~ItemCustomMake3DViewWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemCustomMakeDetailViewWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
	};

	enum
	{
		VIEW_WIDTH = 750,
		VIEW_HEIGHT= 465,
	};

protected:
	ioUI3DRender	*m_pUI3DRender;
	ioCreateChar	*m_aMyChar;

protected:
	ioUIRenderFrame *m_pTitleBar;

	ioUIRenderImage *m_pBackground;
	ioUIRenderImage *m_pMouseLeft;
	ioUIRenderImage *m_pMouseRight;
	ioUIRenderImage *m_pMouseMiddle;

protected:
	char m_szSkinName[MAX_PATH];

public:
	void CreateCharacter( const CHARACTER &rkInfo, const ITEM_DATA &rkItem, CustomTextureList &rkTexList );

public:
	virtual void iwm_create();
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRenderBackground( int iXPos, int iYPos );

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	ItemCustomMakeDetailViewWnd();
	virtual ~ItemCustomMakeDetailViewWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemCustomMakeConfirmWnd : public ioWnd
{
public:
	enum
	{
		ID_ICON_BTN   = 1,
		ID_MAKE_BTN	  = 2,
		ID_CANCEL_BTN = 3,
		ID_EXIT_BTN	  = 4,
	};

protected:
	ioUIRenderImage *m_pExclamationMark;

protected:
	InventoryItemInfo m_ItemInfo;
	bool              m_bMale;
	ioHashString      m_szName;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();
	virtual void OnRenderAfterChild();

	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	bool SetItemCustomMakeInfo( InventoryItemInfo &rkItem, bool bMale );

public:
	ItemCustomMakeConfirmWnd();
	virtual ~ItemCustomMakeConfirmWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemCustomChangeConfirmWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT   = 1,
		ID_CHANGE = 2,
		ID_CANCEL = 3,
	};

protected:
	InventoryItemInfo m_ItemInfo;
	bool              m_bMale;
	ioHashString      m_szName;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnRender();

public:
	bool SetItemCustomChangeInfo( InventoryItemInfo &rkItem, bool bMale );

public:
	ItemCustomChangeConfirmWnd();
	virtual ~ItemCustomChangeConfirmWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemCustomMakeCompleteWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_EXIT  = 1,
		ID_VIEW  = 2,
		ID_CLOSE = 3,
	};

protected:
	ioUIRenderImage *m_pIconBack;
	ioUIRenderImage *m_pIcon;
	ioUIRenderImage *m_pCustomIcon;
	ioUIRenderImage *m_pNumText;
	ioUIRenderImage *m_pPlusText;

protected:
	InventoryItemInfo m_ItemInfo;
	int               m_iReinforce;
	bool              m_bMale;
	ioHashString      m_szName;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	bool SetItemCustomMakeInfo( InventoryItemInfo &rkItem, bool bMale );

public:
	ItemCustomMakeCompleteWnd();
	virtual ~ItemCustomMakeCompleteWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemCustomDeleteWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT          = 1,
		ID_MALE_DELETE   = 2,
		ID_FEMALE_DELETE = 3,
		ID_DELETE_EDIT   = 4,
	};

protected:
	ioWnd *m_pPreEdit;

protected:
	int m_iSlotIndex;

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

public:
	void SetSlotIndex( int iSlotIndex );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	
public:
	ItemCustomDeleteWnd();
	virtual ~ItemCustomDeleteWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemCustomThumbnailWnd : public ioWnd
{
protected:
	ioUI3DRender	*m_pUI3DRender;
	ioCreateChar	*m_aMyChar;
	DWORD            m_dwRotateCheckTime;

protected:
	ioUIRenderFrame *m_pBlueStateFrm;
	ioUIRenderFrame *m_pRedStateFrm;
	ioUIRenderImage *m_pRectSrc;

protected:
	enum
	{
		RECT_WIDTH_CNT = 15,
		RECT_HEIGHT_CNT= 16,
		RECT_GAP_SIZE  = 12,
	};

public:
	void CreateCharacter( EXTRAITEMSLOT &rkExtraItem, bool bMale );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRectRender( int iXPos, int iYPos );

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	ItemCustomThumbnailWnd();
	virtual ~ItemCustomThumbnailWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemCustomGlobalDetailViewWnd : public ioWnd
{
public:
	enum
	{
		ID_EXIT = 1,
	};

	enum
	{
		VIEW_WIDTH = 750,
		VIEW_HEIGHT= 465,
	};

protected:
	ioUI3DRender	*m_pUI3DRender;
	ioCreateChar	*m_CustomChar1;
	ioCreateChar	*m_CustomChar2;

protected:
	ioUIRenderFrame *m_pTitleBar;

	ioUIRenderImage *m_pBackground;
	ioUIRenderImage *m_pMouseLeft;
	ioUIRenderImage *m_pMouseRight;
	ioUIRenderImage *m_pMouseMiddle;

protected:
	char m_szSkinName[MAX_PATH];

public:
	void CreateCharacter( DWORD dwItemCode, DWORD dwMaleCustom, DWORD dwFemaleCustom );

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRenderBackground( int iXPos, int iYPos );

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	ItemCustomGlobalDetailViewWnd();
	virtual ~ItemCustomGlobalDetailViewWnd();
};
