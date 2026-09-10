#pragma once

#include "NewShopIconBtn.h"

class GashaponPresentListBtn : public NewShopIconBtn
{
protected:
	StringVector m_szNameList;
public:
	void AddName( const ioHashString &rName);
    void ClearName();

protected:
	virtual void OnRender();

public:
	GashaponPresentListBtn();
	virtual ~GashaponPresentListBtn();
};

///////////////////////////////////////////////////////////////////
// EIT_ETC_GASHAPON 에서만 사용
class GashaponPresentListWnd : public ioWnd
{
public:
	enum
	{
		ID_PRE_BTN	= 1,
		ID_NEXT_BTN	= 2,
		ID_X		= 3,

		ID_ALL_BTN		= 10,

		ID_ICON_BTN1	= 100,
		ID_ICON_BTN2	= 101,
		ID_ICON_BTN3	= 102,
		ID_ICON_BTN4	= 103,
		ID_ICON_BTN5	= 104,
		ID_ICON_BTN6	= 105,
		ID_ICON_BTN7	= 106,
		ID_ICON_BTN8	= 107,
		ID_ICON_BTN9	= 108,
		ID_ICON_BTN10	= 109,
		ID_ICON_BTN11	= 110,
		ID_ICON_BTN12	= 111,

		ICON_BTN_MAX	= 12,
	};

protected:
	struct GashaponPresentInfo
	{
		ioHashString m_szIconName;
		ioHashString m_szSubIconName;
		ioHashString m_szPresentName1;
		ioHashString m_szPresentName2;
	};

	typedef std::vector<GashaponPresentInfo> vGashponPresentInfo;

protected:
	int m_iMaxPage;
	int m_iCurPage;

	ioHashString m_szPageText;

	vGashponPresentInfo m_vGashponPresentInfoVec;

	static IntVec m_vPresentTypeList;
	static IntVec m_vPresentValue1List;
	static IntVec m_vPresentValue2List;
	static IntVec m_vPresentValue3List;
	static IntVec m_vPresentValue4List;

protected:
	void UpdatePage( int iPage );
	void UpdateIconBtn();
	void SetMaxPage();

	void SetGashaponList( IntVec &rPresentTypeList, IntVec &rPresentValue1List, IntVec &rPresentValue2List, IntVec &rPresentValue3List, IntVec &rPresentValue4List );

public:
	static void SetPresentList( IntVec &rPresentTypeList, IntVec &rPresentValue1List, IntVec &rPresentValue2List, IntVec &rPresentValue3List, IntVec &rPresentValue4List );
	static IntVec &GetPresentTypeList()   { return m_vPresentTypeList; }
	static IntVec &GetPresentValue1List() { return m_vPresentValue1List; }
	static IntVec &GetPresentValue2List() { return m_vPresentValue2List; }
	static IntVec &GetPresentValue3List() { return m_vPresentValue3List; }
	static IntVec &GetPresentValue4List() { return m_vPresentValue4List; }

public:
	virtual void iwm_show();
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );

	virtual void ParseExtraInfo( ioXMLElement &xElement );

protected:
	virtual void OnRender();

public:
	GashaponPresentListWnd();
	virtual ~GashaponPresentListWnd();
};

