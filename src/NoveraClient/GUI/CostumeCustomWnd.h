#pragma once

#include "../io3DEngine/ioStringConverter.h"
#include "../ioComplexStringPrinter.h"

#include "GlobalWnd.h"

class ioUI3DRender;
class ioUIRenderFrame;
class ioCreateChar;

//////////////////////////////////////////////////////////////////////////
class CostumeCustomInfoWnd : public ioButton
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

	int m_nCostumeIndex;

protected:
	void SetBlank();
	void ClearDesc();
	void RenderDesc( int iXPos, int iYPos );

	void SetIsWidthCutDesc( int iArray , bool bWidthCut );
	bool IsWidthCutDesc( int iArray );

	bool SetCostumeInfo();

public:
	void SetInfo( InventoryItemInfo &rkInfo );

	InventoryItemInfo& GetInfo();
	int GetMarkType();

	inline int GetCostumeIndex() const { return m_nCostumeIndex; }
	inline int GetMagicCode() const { return m_ItemInfo.m_iMagicCode; }
	bool IsCustomToolTip();
	bool IsIconOver();

protected:
	virtual void OnRender();

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	CostumeCustomInfoWnd();
	virtual ~CostumeCustomInfoWnd();
};

//////////////////////////////////////////////////////////////////////////
class CostumeCustomSelectWnd : public ioMovingWnd
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

	void SetCostumeInfoVec( DWORD dwTabID );
	bool IsRightTab( DWORD dwTabID, int iValue1 );
	int  GetTabType( DWORD dwTabID );
	void OnCostumeCustomInfoPush( DWORD dwID );

public:
	void Update();

public:
	CostumeCustomSelectWnd();
	virtual ~CostumeCustomSelectWnd();
};

//////////////////////////////////////////////////////////////////////////
class CostumeCustomMakeWnd : public ioWnd
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
	virtual void iwm_show();
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
	void SetCostumeInfo( InventoryItemInfo &rkItem, bool bMale );

public:
	CostumeCustomMakeWnd();
	virtual ~CostumeCustomMakeWnd();
};

//////////////////////////////////////////////////////////////////////////
class CostumeCustomMake3DViewWnd : public ioWnd
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
	void CreateCharacter( const CHARACTER &rkInfo, const CostumeEquipInfo &sCostumeEquipInfo, CustomTextureList &rkReturn );
	void ChangeLSCTexture( CustomTextureList &rkOriTexList, CustomTextureList &rkAftTexList );
	
protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRectRender( int iXPos, int iYPos );

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	CostumeCustomMake3DViewWnd();
	virtual ~CostumeCustomMake3DViewWnd();
};

//////////////////////////////////////////////////////////////////////////
class CostumeCustomMakeDetailViewWnd : public ioWnd
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
	void CreateCharacter( const CHARACTER &rkInfo, const CostumeEquipInfo &sCostumeEquipInfo, CustomTextureList &rkTexList );

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
	CostumeCustomMakeDetailViewWnd();
	virtual ~CostumeCustomMakeDetailViewWnd();
};

//////////////////////////////////////////////////////////////////////////
class CostumeCustomMakeConfirmWnd : public ioWnd
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

	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	bool SetCostumeCustomMakeInfo( InventoryItemInfo &rkItem, bool bMale );

public:
	CostumeCustomMakeConfirmWnd();
	virtual ~CostumeCustomMakeConfirmWnd();
};

//////////////////////////////////////////////////////////////////////////
class CostumeCustomMakeCompleteWnd : public ioMovingWnd
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

protected:
	InventoryItemInfo m_ItemInfo;
	bool              m_bMale;
	ioHashString      m_szName;

public:
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	bool SetCostumeCustomMakeInfo( InventoryItemInfo &rkItem, bool bMale );

public:
	CostumeCustomMakeCompleteWnd();
	virtual ~CostumeCustomMakeCompleteWnd();
};

//////////////////////////////////////////////////////////////////////////
class CostumeCustomChangeConfirmWnd : public ioWnd
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
	bool SetCostumeCustomChangeInfo( InventoryItemInfo &rkItem, bool bMale );

public:
	CostumeCustomChangeConfirmWnd();
	virtual ~CostumeCustomChangeConfirmWnd();
};

//////////////////////////////////////////////////////////////////////////
class CostumeCustomDeleteWnd : public ioWnd
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
	CostumeCustomDeleteWnd();
	virtual ~CostumeCustomDeleteWnd();
};

//////////////////////////////////////////////////////////////////////////
class CostumeCustomGlobalDetailViewWnd : public ioWnd
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
	void CreateCharacter( const CostumeEquipInfo &sCostumeEquipInfo );

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
	CostumeCustomGlobalDetailViewWnd();
	virtual ~CostumeCustomGlobalDetailViewWnd();
};

//////////////////////////////////////////////////////////////////////////
class CostumeCustomThumbnailWnd : public ioWnd
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
	void CreateCharacter( const CostumeEquipInfo &sCostumeEquipInfo, bool bMale );

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();
	void OnRectRender( int iXPos, int iYPos );

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	CostumeCustomThumbnailWnd();
	virtual ~CostumeCustomThumbnailWnd();
};

//////////////////////////////////////////////////////////////////////////
class CostumeCustomThumbnailTooltip : public ioWnd
{
public:
	enum
	{
		ID_CUSTOM_1 = 1,
		ID_CUSTOM_2 = 2,
	};

	enum
	{
		MIN_HEIGHT		= 26,
		TEXT_LINE_GAP   = 18,
		SIDE_TEXT_GAP	= 14,
		THUMBNAIL_WIDTH = 190,
		THUMBNAIL_HEIGHT= 193,

	};

protected:
	ioComplexStringPrinter m_Printer[GLOBAL_TEXT_TOOLTIP_LINE];
	CostumeEquipInfo m_CostumeEquipInfo;

protected:
	void ProcessPosition();
	void ClearList();

public:
	virtual void iwm_hide();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	void SetTooltipText( ioComplexStringPrinter *rkPrinter, const CostumeEquipInfo &sCostumeEquipInfo );

public:
	CostumeCustomThumbnailTooltip();
	virtual ~CostumeCustomThumbnailTooltip();
};