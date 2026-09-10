#include "StdAfx.h"
#include "ioSlotMgr.h"

#include "gui/TowerDefWnd.h"

template<> ioSlotMgr* Singleton< ioSlotMgr >::ms_Singleton = 0;


ioSlotMgr::ioSlotMgr()
{
	for(int i = 0; i < MAX_SLOT; i++)
		m_pSlotItem[i] = new ioSlotItem(i);

	m_dwSlot = Setting::GetSlotItem();

	DWORD dwSlot = m_dwSlot >>  24;
	DWORD dwEtcItem = ioEtcItem::EIT_NONE;

	if(dwSlot == 0)
		dwEtcItem = ioEtcItem::EIT_NONE;
	else
		dwEtcItem = dwSlot - 1 + ioEtcItem::EIT_ETC_CONSUMPTION_BUFF01;

	m_pSlotItem[SLOT_1]->SetType(dwEtcItem);


	dwSlot =  (m_dwSlot >>  16) & 0x000000ff;

	if(dwSlot == 0)
		dwEtcItem = ioEtcItem::EIT_NONE;
	else
		dwEtcItem = dwSlot - 1 + ioEtcItem::EIT_ETC_CONSUMPTION_BUFF01;

	m_pSlotItem[SLOT_2]->SetType(dwEtcItem);


	dwSlot =  (m_dwSlot >>  8) & 0x000000ff;

	if(dwSlot == 0)
		dwEtcItem = ioEtcItem::EIT_NONE;
	else
		dwEtcItem = dwSlot - 1 + ioEtcItem::EIT_ETC_CONSUMPTION_BUFF01;

	m_pSlotItem[SLOT_3]->SetType(dwEtcItem);


	dwSlot =  m_dwSlot & 0x000000ff;

	if(dwSlot == 0)
		dwEtcItem = ioEtcItem::EIT_NONE;
	else
		dwEtcItem = dwSlot - 1 + ioEtcItem::EIT_ETC_CONSUMPTION_BUFF01;

	m_pSlotItem[SLOT_4]->SetType(dwEtcItem);
}


ioSlotMgr::~ioSlotMgr()
{
	DWORD dwSlot = 0;

	for(int i = 0; i < MAX_SLOT; i++)
	{
		if(m_pSlotItem[i])
		{
			if(m_pSlotItem[i]->GetType() != ioEtcItem::EIT_NONE )
				dwSlot = dwSlot | ( (m_pSlotItem[i]->GetType() - ioEtcItem::EIT_ETC_CONSUMPTION_BUFF01 + 1) << (24 - (i * 8)));
		}

		SAFEDELETE(m_pSlotItem[i]);
	}

	if( dwSlot != Setting::GetSlotItem())
	{
		Setting::SetSlotItem(dwSlot);
		Setting::SaveSetting(g_MyInfo.GetPublicID());
	}
}


void ioSlotMgr::SetSlotToSlot(int nSrcSlot, int nDstSlot)
{
	ioSlotItem* pSrcSlot = m_pSlotItem[nSrcSlot];
	ioSlotItem* pDstSlot = m_pSlotItem[nDstSlot];

	DWORD dwTmp = pDstSlot->GetType();

	pDstSlot->SetType(pSrcSlot->GetType());
	pSrcSlot->SetType(dwTmp);
}

void ioSlotMgr::SetInvetToSlot(const int nDstSlot, const DWORD dwType)
{
	m_pSlotItem[nDstSlot]->SetType(dwType);
}



void ioSlotMgr::InitSlot()
{
	for(int i = 0; i < MAX_SLOT; i++)
	{
		DWORD dwType = m_pSlotItem[i]->GetType();

		if(COMPARE(dwType, ioEtcItem::EIT_ETC_CONSUMPTION_BUFF01, ioEtcItem::EIT_ETC_CONSUMPTION_BUFF64+1))
		{
			ioEtcItemConsumption *pItem = dynamic_cast< ioEtcItemConsumption * >( g_EtcItemMgr.FindEtcItem(dwType) );
			if(pItem)
			{
				m_pSlotItem[i]->SetCoolTime(pItem->GetCoolTime());
				m_pSlotItem[i]->SetImg(pItem->GetIconImg());
				m_pSlotItem[i]->SetBackImg(pItem->GetIconBackImg());
				m_pSlotItem[i]->SetBuffName(pItem->GetSlotBuff());
				m_pSlotItem[i]->SetKeyName(g_KeyManager.GetCurrentConsumptionKey(i));
				m_pSlotItem[i]->SetItemNum(0);
			}

			ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
			ioUserEtcItem::ETCITEMSLOT kSlot;

			if(pUserEtcItem->GetEtcItem( dwType, kSlot ) )
				m_pSlotItem[i]->SetItemNum(kSlot.m_iValue1);
		}
		else
		{
			if(dwType == ioEtcItem::EIT_ETC_CONSUMPTION_REVIVE)
			{
				ioEtcItemRevive *pItem = dynamic_cast< ioEtcItemRevive * >( g_EtcItemMgr.FindEtcItem(dwType) );
				if(pItem)
				{
					m_pSlotItem[i]->SetCoolTime(pItem->GetCoolTime());
					m_pSlotItem[i]->SetImg(pItem->GetIconImg());
					m_pSlotItem[i]->SetBackImg(pItem->GetIconBackImg());
					m_pSlotItem[i]->SetKeyName(g_KeyManager.GetCurrentConsumptionKey(i));
					m_pSlotItem[i]->SetItemNum(0);
				}

				ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
				ioUserEtcItem::ETCITEMSLOT kSlot;

				if(pUserEtcItem->GetEtcItem( dwType, kSlot ) )
					m_pSlotItem[i]->SetItemNum(kSlot.m_iValue1);
			}
			else if( dwType == ioEtcItem::EIT_NONE)
			{
				m_pSlotItem[i]->ResetItem();
			}
		}
	}
}

int ioSlotMgr::UseItem(const int nSlot)
{
	return UseItem(m_pSlotItem[nSlot]->GetType());
}

int ioSlotMgr::UseItem(const DWORD dwType)
{
	ioSlotItem* pItem = GetSlot(dwType);

	if(!pItem) return 1;

	if(pItem->GetCountItem() < 1)
		return 2;

	if(pItem->IsCoolTime())
		return 3;

	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	ioUserEtcItem::ETCITEMSLOT kSlot;

	int nType = pItem->GetType();
	if( pUserEtcItem->GetEtcItem(nType, kSlot ) )   
	{
		if(pUserEtcItem->IsCanUse(nType) )
		{
			pUserEtcItem->SendUse(nType);
			pItem->SetCoolTime();
			return 0;
		}
		else
		{
			return 2;
		}
	}

	return 0;
}

void ioSlotMgr::UpdateItem(const DWORD dwType)
{
	ioSlotItem* pItem = GetSlot(dwType);

	if(!pItem) return;

	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	ioUserEtcItem::ETCITEMSLOT kSlot;

	if(pUserEtcItem->GetEtcItem(dwType, kSlot ))
	{
		pItem->SetItemNum(kSlot.m_iValue1);
		pItem->SetKeyName(g_KeyManager.GetCurrentConsumptionKey(pItem->GetMySlot()));
	}

	TowerDefWnd *pTowerWnd = dynamic_cast< TowerDefWnd * >( g_GUIMgr.FindWnd(TOWERDEF_USER_GAUGE_WND));
	if(pTowerWnd)
		pTowerWnd->UpdateSlotItem(pItem->GetMySlot());
}

void ioSlotMgr::UpdateAllItem()
{
	for(int i = 0; i < MAX_SLOT; i++)
		UpdateItem(m_pSlotItem[i]->GetType());
}

void ioSlotMgr::InitAllItem()
{
	TowerDefWnd *pTowerWnd = dynamic_cast< TowerDefWnd * >( g_GUIMgr.FindWnd(TOWERDEF_USER_GAUGE_WND));
	if(pTowerWnd)
		pTowerWnd->InitSlotItem();
}

ioSlotItem* ioSlotMgr::GetSlot(const int nSlot)
{
	return GetSlot(m_pSlotItem[nSlot]->GetType());
}

ioSlotItem* ioSlotMgr::GetSlot(const DWORD dwType)
{
	for(int i = 0; i < MAX_SLOT; i++)
	{
		if(m_pSlotItem[i]->GetType() == dwType)
			return m_pSlotItem[i];
	}

	return NULL;
}

bool ioSlotMgr::IsCoolTime(const int nSlot)
{
	return m_pSlotItem[nSlot]->IsCoolTime();
}


const DWORD ioSlotMgr::GetSlotItemType(const int nSlot)
{
	return m_pSlotItem[nSlot]->GetType();
}

const WORD ioSlotMgr::GetReviveKey()
{
	for(int i = 0; i < MAX_SLOT; i++)
	{
		if(m_pSlotItem[i]->GetType() == ioEtcItem::EIT_ETC_CONSUMPTION_REVIVE)
		{
			if(m_pSlotItem[i]->GetCountItem() > 0)
				return m_pSlotItem[i]->GetKeyName();
		}
	}

	return 0xff; 
}

const int ioSlotMgr::GetEmptySlot()
{
	for(int i = 0; i < MAX_SLOT; i++)
	{
		if(m_pSlotItem[i]->GetType() == ioEtcItem::EIT_NONE)
			return i;
	}

	return -1;
}

ioSlotMgr& ioSlotMgr::GetSingleton()
{
	return Singleton<ioSlotMgr>::GetSingleton();
}

//////////////////////////////////////////////////////////////////////////
ioSlotItem::ioSlotItem(const int nNum)
{
	m_nMySlot = nNum;

	m_strItemImg = "";
	m_strItemBackImg = "";
	m_strBuff = "";
	m_dwType = ioEtcItem::EIT_NONE;
	m_nCount = 0;
	m_wKey = 0;
	m_dwCoolTime = 0;
	m_dwSetCT = 0;
}

ioSlotItem::~ioSlotItem()
{

}

void ioSlotItem::ResetItem()
{
	m_nCount = 0;
	m_dwCoolTime = 0;
	m_dwSetCT = 0;
	m_strItemImg = "";
	m_strItemBackImg = "";
	m_strBuff = "";
}

void ioSlotItem::SetCoolTime()
{
	m_dwSetCT = FRAMEGETTIME() + m_dwCoolTime;
}

bool ioSlotItem::IsCoolTime()
{
	DWORD dwFrame = FRAMEGETTIME();

	if( dwFrame > m_dwSetCT)
		return false;

	return true;
}

