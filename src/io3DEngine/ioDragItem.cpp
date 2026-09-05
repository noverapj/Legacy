

#include "stdafx.h"

#include "ioWnd.h"
#include "ioDragItem.h"

#include "ioGUIManager.h"

ioGUIManager* ioDragItem::m_pGUIMgr = NULL;

ioDragItem::ioDragItem()
{
}

ioDragItem::~ioDragItem()
{
}

void ioDragItem::DoDragDrop( ioWnd *pStartWnd,
							 const RECT &rcDragCheck,
							 const POINT &ptOffset,
							 const RECT *pRcStartDrag )
{
	if( !m_pGUIMgr )	return;

	m_pGUIMgr->DoDragDrop( pStartWnd, this, rcDragCheck, ptOffset, pRcStartDrag );
}

void ioDragItem::SetDragImageName( const ioHashString &szDragImageName )
{
	m_DragImageName = szDragImageName;
	m_DragSubImageName.Clear();
}

void ioDragItem::SetDragSubImageName( const ioHashString &szDragSubImageName )
{
	m_DragSubImageName = szDragSubImageName;
}

const ioHashString& ioDragItem::GetDragImageName() const
{
	return m_DragImageName;
}

const ioHashString& ioDragItem::GetDragSubImageName() const
{
	return m_DragSubImageName;
}

int ioDragItem::GetTypeID() const
{
	return 0;
}

const ioHashString& ioDragItem::GetTypeName() const
{
	static ioHashString sName( "Unspecified" );

	return sName;
}