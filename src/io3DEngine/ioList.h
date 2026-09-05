

#ifndef _ioList_h_
#define _ioList_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioWnd;

class __EX ioListItem
{
public:
	virtual void Render( int iXPos, int iYPos ) = 0;

public:
	ioListItem();
	virtual ~ioListItem();
};

class __EX ioTextItem : public ioListItem
{
protected:
	ioHashString	m_szText;

public:
	void SetText( const char *szText );
	void SetText( const ioHashString &szText );

public:
	inline const ioHashString& GetText() const { return m_szText; }

public:
	virtual void Render( int iXPos, int iYPos );

public:
	ioTextItem();
	virtual ~ioTextItem();
};

class __EX ioList : public ioWnd
{
protected:
	typedef std::list< ioListItem* > ioItemList;
	ioItemList	m_ItemList;

public:
	void AddItem( ioListItem *pItem );
	void DeleteItem( int iIdx );
	void DeleteItem( ioListItem *pItem );
	void ClearAllItems();

public:
	ioListItem* GetItem( int iIdx );
	int GetItemCount() const;

protected:
	virtual void OnRender();

public:
	virtual DWORD GetWndType() const;

public:
	ioList();
	virtual ~ioList();
};

#endif
