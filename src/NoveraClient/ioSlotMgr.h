#pragma once

class ioSlotItem;

class ioSlotMgr : public Singleton< ioSlotMgr >
{

public:
	enum 
	{
		SLOT_1 = 0,
		SLOT_2,
		SLOT_3,
		SLOT_4,
		MAX_SLOT,
	};

	enum
	{
		TYPE_BUFF = 0,
		TYPE_REVIVE, 
	};

protected:
	ioSlotItem*		m_pSlotItem[MAX_SLOT];

	DWORD			m_dwSlot;

public:
	ioSlotMgr();
	virtual ~ioSlotMgr();

public:
	void InitSlot();
	void SetSlotToSlot(int nSrcSlot, int nDstSlot);
	void SetInvetToSlot(const int nDstSlot, const DWORD dwType);
	void SetSlotItem(DWORD dwSlot) { m_dwSlot = dwSlot; }
	

	ioSlotItem* GetSlot(const int nSlot);
	ioSlotItem* GetSlot(const DWORD dwType);
	const DWORD GetSlotItem() {return m_dwSlot;}
	const DWORD GetSlotItemType(const int nSlot);
	const WORD GetReviveKey();
	const int GetEmptySlot();

	bool IsCoolTime(const int nSlot);

	int UseItem(const int nSlot);
	int UseItem(const DWORD dwType);
	void UpdateItem(const int nSlot);
	void UpdateItem(const DWORD dwType);
	void UpdateAllItem();

	void InitAllItem();

public:
	static ioSlotMgr& GetSingleton();
};

#define g_SlotMgr ioSlotMgr::GetSingleton()

class ioSlotItem
{
protected:
	ioHashString	m_strItemImg;
	ioHashString	m_strItemBackImg;
	ioHashString	m_strBuff;

	int				m_nMySlot;
	DWORD			m_dwType;

	int				m_nCount;
	WORD			m_wKey;
	DWORD			m_dwCoolTime;

	DWORD			m_dwSetCT;

public:
	void ResetItem();

	void SetType(DWORD dwType){m_dwType = dwType;}
	void SetCoolTime();
	void SetCoolTime(const DWORD dwCoolTime) {m_dwCoolTime = dwCoolTime;}
	void SetItemNum(const int nItem) {m_nCount = nItem;}
	void SetKeyName(const WORD wKey) {m_wKey = wKey;}
	void SetImg(const ioHashString& strImg) {m_strItemImg = strImg;}
	void SetBackImg(const ioHashString& strBackImg) {m_strItemBackImg = strBackImg;}
	void SetBuffName(const ioHashString& strBuff) {m_strBuff = strBuff;}
	
	bool IsCoolTime(); 
	const int GetMySlot(){return m_nMySlot;}
	const DWORD GetType(){return m_dwType;}
	const DWORD GetCTFrame() {return m_dwSetCT;}
	const int GetCountItem() {return m_nCount;}
	const WORD GetKeyName() {return m_wKey;}

	const ioHashString& GetImg() {return m_strItemImg;}
	const ioHashString& GetBackImg() {return m_strItemBackImg;}
	const ioHashString& GetBuffName() {return m_strBuff;}

public:
	ioSlotItem(const int nNum);
	virtual ~ioSlotItem();
};

