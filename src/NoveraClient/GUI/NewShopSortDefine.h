#pragma once
#include "NewShopDataDefine.h"

enum NewShopFilter
{
	NSF_NONE					= 0,	//필터없음
	NSF_HIGH_GOLD_COST			= 1,	//가격높은순(골드)
	NSF_LOW_GOLD_COST			= 2,	//가격낮은순(골드)
	NSF_HIGH_PESO_COST			= 3,	//가격높은순(페소)
	NSF_LOW_PESO_COST			= 4,	//가격낮은순(페소)
	NSF_NO_LIMIT				= 5,	//계급제한해제상품
	NSF_NEED_HIGH_GRADE			= 6,	//요구계급 높은순
	NSF_NEED_LOW_GRADE			= 7,	//요구계급 낮은순
	NSF_NEW_AND_EVENT_MARK		= 8,	//이벤트상품(NEW/EVENT 마크)
	NSF_EVENT_ITEM_HIGH_COST	= 9,	//가격높은순(이벤트 코인)
	NSF_EVENT_ITEM_LOW_COST		= 10,	//가격낮은순(이벤트 코인)
	NSF_MAX						= 11,
};

class NewShopInfoDefaultSort : public std::binary_function< const NewShopItemInfo& , const NewShopItemInfo&, bool >
{
public:
	bool operator() ( const NewShopItemInfo &lhs, const NewShopItemInfo &rhs ) const
	{		
		if( lhs.m_iSortPoint < rhs.m_iSortPoint )
			return true;

		return false;
	}
};

class NewShopInfoHighGoldSort : public std::binary_function< const NewShopItemInfo& , const NewShopItemInfo&, bool >
{
public:
	bool operator() ( const NewShopItemInfo &lhs, const NewShopItemInfo &rhs ) const
	{
		int iLCash = 0;
		int iRCash = 0;

		for( int i = 0; i < (int)lhs.m_vCash.size(); ++i )
		{
			if( iLCash < lhs.m_vCash[i] )
				iLCash = lhs.m_vCash[i];
		}

		for( int i = 0; i < (int)rhs.m_vCash.size(); ++i )
		{
			if( iRCash < rhs.m_vCash[i] )
				iRCash = rhs.m_vCash[i];
		}

		if( iLCash > iRCash )
		{
			return true;
		}
		else if( iLCash == iRCash )
		{
			if( lhs.m_iShopMarkType == SHOP_MARK_TYPE_EVENT && rhs.m_iShopMarkType == SHOP_MARK_TYPE_HIT )
				return true;
		}
		
		return false;
	}
};

class NewShopInfoLowGoldSort : public std::binary_function< const NewShopItemInfo& , const NewShopItemInfo&, bool >
{
public:
	bool operator() ( const NewShopItemInfo &lhs, const NewShopItemInfo &rhs ) const
	{
		int iLCash = INT_MAX;
		int iRCash = INT_MAX;

		for( int i = 0; i < (int)lhs.m_vCash.size(); ++i )
		{
			if( lhs.m_vCash[i] > 0 && iLCash > lhs.m_vCash[i] )
				iLCash = lhs.m_vCash[i];
		}

		for( int i = 0; i < (int)rhs.m_vCash.size(); ++i )
		{
			if( rhs.m_vCash[i] > 0 && iRCash > rhs.m_vCash[i] )
				iRCash = rhs.m_vCash[i];
		}

		if( iLCash < iRCash )
		{
			return true;
		}
		else if( iLCash == iRCash )
		{
			if( lhs.m_iShopMarkType == SHOP_MARK_TYPE_EVENT && rhs.m_iShopMarkType == SHOP_MARK_TYPE_HIT )
				return true;
		}

		return false;
	}
};

class NewShopInfoHighPesoSort : public std::binary_function< const NewShopItemInfo& , const NewShopItemInfo&, bool >
{
public:
	bool operator() ( const NewShopItemInfo &lhs, const NewShopItemInfo &rhs ) const
	{
		int iLPeso = 0;
		int iRPeso = 0;

		for( int i = 0; i < (int)lhs.m_vPeso.size(); ++i )
		{
			if( iLPeso < lhs.m_vPeso[i] )
				iLPeso = lhs.m_vPeso[i];
		}

		for( int i = 0; i < (int)rhs.m_vPeso.size(); ++i )
		{
			if( iRPeso < rhs.m_vPeso[i] )
				iRPeso = rhs.m_vPeso[i];
		}

		if( iLPeso > iRPeso )
		{
			return true;
		}
		else if( iLPeso == iRPeso )
		{
			if( lhs.m_iShopMarkType == SHOP_MARK_TYPE_EVENT && rhs.m_iShopMarkType == SHOP_MARK_TYPE_HIT )
				return true;
		}

		return false;
	}
};

class NewShopInfoLowPesoSort : public std::binary_function< const NewShopItemInfo& , const NewShopItemInfo&, bool >
{
public:
	bool operator() ( const NewShopItemInfo &lhs, const NewShopItemInfo &rhs ) const
	{
		int iLPeso = INT_MAX;
		int iRPeso = INT_MAX;

		for( int i = 0; i < (int)lhs.m_vPeso.size(); ++i )
		{
			if( lhs.m_vPeso[i] > 0 && iLPeso > lhs.m_vPeso[i] )
				iLPeso = lhs.m_vPeso[i];
		}

		for( int i = 0; i < (int)rhs.m_vPeso.size(); ++i )
		{
			if( rhs.m_vPeso[i] > 0 && iRPeso > rhs.m_vPeso[i] )
				iRPeso = rhs.m_vPeso[i];
		}

		if( iLPeso < iRPeso )
		{
			return true;
		}
		else if( iLPeso == iRPeso )
		{
			if( lhs.m_iShopMarkType == SHOP_MARK_TYPE_EVENT && rhs.m_iShopMarkType == SHOP_MARK_TYPE_HIT )
				return true;
		}

		return false;
	}
};

class NewShopInfoNoLimitSort : public std::binary_function< const NewShopItemInfo& , const NewShopItemInfo&, bool >
{
public:
	bool operator() ( const NewShopItemInfo &lhs, const NewShopItemInfo &rhs ) const
	{		
		if( lhs.m_iShopMarkType == SHOP_MARK_TYPE_GRADE_LIMIT_CLEAR )
			return true;

		return false;
	}
};

class NewShopInfoSoliderNeedGradeHighSort : public std::binary_function< const NewShopItemInfo& , const NewShopItemInfo&, bool >
{
public:
	bool operator() ( const NewShopItemInfo &lhs, const NewShopItemInfo &rhs ) const
	{		
		if( lhs.m_iNeedLevel > rhs.m_iNeedLevel )
			return true;

		return false;
	}
};

class NewShopInfoSoliderNeedGradeLowSort : public std::binary_function< const NewShopItemInfo& , const NewShopItemInfo&, bool >
{
public:
	bool operator() ( const NewShopItemInfo &lhs, const NewShopItemInfo &rhs ) const
	{		
		if( lhs.m_iNeedLevel < rhs.m_iNeedLevel )
			return true;

		return false;
	}
};

class NewShopInfoNewAndEventSort : public std::binary_function< const NewShopItemInfo& , const NewShopItemInfo&, bool >
{
public:
	bool operator() ( const NewShopItemInfo &lhs, const NewShopItemInfo &rhs ) const
	{		
		if( lhs.m_iShopMarkType == SHOP_MARK_TYPE_NEW || lhs.m_iShopMarkType == SHOP_MARK_TYPE_HIT )
			return true;

		return false;
	}
};


class NewShopInfoEventStampHighSort : public std::binary_function< const NewShopItemInfo& , const NewShopItemInfo&, bool >
{
public:
	bool operator() ( const NewShopItemInfo &lhs, const NewShopItemInfo &rhs ) const
	{
		for( int i = 0; i < (int)lhs.m_vStamp.size(); ++i )
		{
			for( int j = 0; j < (int)rhs.m_vStamp.size(); ++j )
			{
				if( lhs.m_vStamp[i] > rhs.m_vStamp[j] )
					return true;
			}
		}

		return false;
	}
};

class NewShopInfoEventStampLowSort : public std::binary_function< const NewShopItemInfo& , const NewShopItemInfo&, bool >
{
public:
	bool operator() ( const NewShopItemInfo &lhs, const NewShopItemInfo &rhs ) const
	{
		for( int i = 0; i < (int)lhs.m_vStamp.size(); ++i )
		{
			for( int j = 0; j < (int)rhs.m_vStamp.size(); ++j )
			{
				if( lhs.m_vStamp[i] < rhs.m_vStamp[j] )
					return true;
			}
		}

		return false;
	}
};