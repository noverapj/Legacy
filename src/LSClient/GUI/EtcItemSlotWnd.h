#pragma once
//////////////////////////////////////////////////////////////////////////

class ioInvenDragItem;

class EtcItemSlotWnd : public ioMovingWnd
{
public:
	enum
	{
		ID_BTN_EXIT		= 1,
		ID_BTN_SHORTCUT = 2,
		ID_BTN_CLOSE	= 3, 

		ID_SLOT_ITEM_BTN1		= 101,
		ID_SLOT_ITEM_BTN2		= 102,
		ID_SLOT_ITEM_BTN3		= 103,
		ID_SLOT_ITEM_BTN4		= 104,

		ID_SLOT_INVEN_START		= 201,
		ID_SLOT_INVEN_FINISH	= 212,
	};
	
protected:
	ioUIRenderImage* m_pBoxLeft;
	ioUIRenderImage* m_pBoxTop;

public:
	void ShowSlotWnd();
	void UpdateAllSlot();

protected:
	void SetSlotItemBtn();
	void SetSlotInvenWnd();
	
protected:
	DWORD CheckDropEnable( const ioMouse& mouse, ioDragItem *pItem );

public:
	virtual void iwm_show();
	virtual void iwm_hide();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual bool iwm_dropitem( ioDragItem *pItem, const ioMouse& mouse, ioWnd *pStartWnd );

protected:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void OnRender();

public:
	EtcItemSlotWnd();
	virtual ~EtcItemSlotWnd();
};
//////////////////////////////////////////////////////////////////////////
class ItemSlotBtn : public ioButton
{
protected:
	ioUIRenderImage *m_pEmptyImg;
	ioUIRenderImage *m_pItemIcon;
	ioUIRenderImage *m_pItemBackImg;

	ioUIRenderFrame *m_pEmptyFrm;
	ioUIRenderFrame *m_pPieceFrm;
	ioUIRenderFrame *m_pAdditiveFrm;
		
	ioInvenDragItem	*m_pDragItem;

	ioHashString	m_strKey;
	ioHashString	m_strIcon;

	int				m_nItem;	
	DWORD			m_dwItemCode;

public:
	void InitItem(const int nSlot);

protected:
	virtual void OnRender();

public:
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );

public:
	virtual void iwm_mouseleave( const ioMouse& mouse );

public:
	ItemSlotBtn();
	virtual ~ItemSlotBtn();
};
//////////////////////////////////////////////////////////////////////////
class SlotWnd : public ioButton
{
protected:
	ioUIRenderImage *m_pEmptyImg;
	ioUIRenderImage *m_pItemIcon;
	ioUIRenderImage *m_pSubIcon;
	ioUIRenderImage *m_pItemBackImg;

	ioUIRenderFrame *m_pEmptyFrm;
	ioUIRenderFrame *m_pPieceFrm;
	ioUIRenderFrame *m_pAdditiveFrm;

	ioInvenDragItem	*m_pDragItem;

	ioHashString	m_strIcon;
	ioHashString	m_sSubIcon;

	int				m_nItem;
	DWORD			m_dwItemCode;
	bool			m_bActive;

public:
	void InitItem(const ioEtcItem *pItem);

	DWORD GetItemCode() {return m_dwItemCode;}

	bool GetActive() {return m_bActive;}

protected:
	//virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:
	virtual void AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	virtual void iwm_mouseleave( const ioMouse& mouse );

public:
	SlotWnd();
	virtual ~SlotWnd();
};