

#ifndef _ioDragItem_h_
#define _ioDragItem_h_

#include "io3DCommon.h"

class ioWnd;
class ioGUIManager;

class __EX ioDragItem
{
protected:
	ioHashString m_DragImageName;
	ioHashString m_DragSubImageName;

public:
	static ioGUIManager *m_pGUIMgr;

public:
	virtual void DoDragDrop( ioWnd *pStartWnd,
							 const RECT &rcDragCheck,
							 const POINT &ptOffset,
							 const RECT *pRcStartDrag = NULL );

public:
	void SetDragImageName( const ioHashString &szDragImageName );
	void SetDragSubImageName( const ioHashString &szDragSubImageName );
	const ioHashString& GetDragImageName() const;
	const ioHashString& GetDragSubImageName() const;

public:
	virtual int GetTypeID() const;
	virtual const ioHashString& GetTypeName() const;

public:
	ioDragItem();
	virtual ~ioDragItem();
};

#endif