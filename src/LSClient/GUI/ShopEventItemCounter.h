#pragma once

struct EventItemCounter
{	
	int	m_nSoldierItem;
	int	m_nDecoItem;
	int	m_nExtraItem;
	int	m_nEtcItem;
	int	m_nGoodsItem;
	int	m_nCostumeItem;

	EventItemCounter()
	{
		Clear();
	}

	void Clear()
	{
		m_nSoldierItem	= 0;
		m_nExtraItem	= 0;
		m_nDecoItem		= 0;
		m_nEtcItem		= 0;
		m_nCostumeItem	= 0;
		m_nGoodsItem	= 0;
	}

	void SetSoldierItem(int nSoldier)	{ m_nSoldierItem = nSoldier; }
	void SetDecoItem(int nDeco)			{ m_nDecoItem = nDeco; }
	void SetExtraItem(int nExtra)		{ m_nExtraItem = nExtra; }
	void SetEtcItem(int nEtc)			{ m_nEtcItem = nEtc; }
	void SetGoodsItem(int nGoods)		{ m_nGoodsItem = nGoods; }
	void SetCostumeItem(int nCostume)	{ m_nCostumeItem = nCostume; }

	int GetTotalItem()		{ return m_nSoldierItem + m_nExtraItem + m_nDecoItem + m_nCostumeItem + m_nEtcItem; }
	int GetSoldierItem()	{ return m_nSoldierItem; }	
	int GetExtraItem()		{ return m_nExtraItem; }
	int GetDecoItem()		{ return m_nDecoItem; }
	int GetEtcItem()		{ return m_nEtcItem; }
	int GetCostume()		{ return m_nCostumeItem; }
	int GetGoodsItem()		{ return m_nGoodsItem; }
};