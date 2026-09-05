#pragma once

#include "../io3DEngine/ioMovingWnd.h"

#include "FirstHireWnd.h"
#include "ExtendedButton.h"
#include "SoldierExerciseWnd.h"

typedef struct tagShopItemInfo
{
	int  m_iSetItemIdx;
	bool m_bCanBuy;
	int  m_iPrice;
	int  m_iGradeLv;
	int  m_iBattleLv;
	int  m_iTrainingLv;
	int  m_iAwardLv;
	int  m_iMedalLv;
	int  m_iPriorityOrder; // 낮을 수록 순위가 높다.
	DWORD  m_dwSetCode;
	int  m_iShopMarkType; 

	tagShopItemInfo()
	{
		m_iSetItemIdx  = 0;
		m_bCanBuy      = false;
		m_iPrice       = 0;
		m_iGradeLv     = 0;
		m_iBattleLv    = 0;
		m_iTrainingLv  = 0;
		m_iAwardLv     = 0;
		m_iMedalLv     = 0;
		m_iPriorityOrder = 999999999; // ORDER_DEFAULT;
		m_dwSetCode      = 0;
		m_iShopMarkType  = 0; //SHOP_MARK_TYPE_NONE;
	}

}ShopItemInfo;
typedef std::vector<ShopItemInfo> vShopItemInfoList;

class ShopItemInfoSort : public std::binary_function< const ShopItemInfo& , const ShopItemInfo&, bool >
{
public:
	bool operator() ( const ShopItemInfo &lhs, const ShopItemInfo &rhs ) const
	{
		if( lhs.m_iShopMarkType != 0 /*SHOP_MARK_TYPE_NONE*/ || rhs.m_iShopMarkType != 0 /*SHOP_MARK_TYPE_NONE*/ )
		{
			if( lhs.m_iPriorityOrder < rhs.m_iPriorityOrder )
				return true;
		}
		else if( lhs.m_bCanBuy && rhs.m_bCanBuy )
		{
			if( lhs.m_iPrice < rhs.m_iPrice )
				return true;
			if( lhs.m_iPrice == rhs.m_iPrice )
			{
				if( lhs.m_iSetItemIdx < rhs.m_iSetItemIdx )
					return true;
			}
		}
		else if( !lhs.m_bCanBuy && !rhs.m_bCanBuy )
		{
			if( lhs.m_iPriorityOrder < rhs.m_iPriorityOrder )
				return true;
			if( lhs.m_iPriorityOrder == rhs.m_iPriorityOrder )
			{
				if( lhs.m_iGradeLv < rhs.m_iGradeLv )
					return true;
				if( lhs.m_iGradeLv == rhs.m_iGradeLv )
				{
					if( lhs.m_iBattleLv < rhs.m_iBattleLv )
						return true;
					if( lhs.m_iBattleLv == rhs.m_iBattleLv )
					{
						if( lhs.m_iTrainingLv < rhs.m_iTrainingLv )
							return true;
						if( lhs.m_iTrainingLv == rhs.m_iTrainingLv )
						{
							if( lhs.m_iAwardLv < rhs.m_iAwardLv )
								return true;
							if( lhs.m_iAwardLv == rhs.m_iAwardLv )
							{
								if( lhs.m_iMedalLv < rhs.m_iMedalLv )
									return true;
								if( lhs.m_iMedalLv == rhs.m_iMedalLv )
								{
									if( lhs.m_iPrice < rhs.m_iPrice )
										return true;
									if( lhs.m_iPrice == rhs.m_iPrice )
									{
										if( lhs.m_iSetItemIdx < rhs.m_iSetItemIdx )
											return true;
									}
								}
							}
						}
					}
				}
			}
		}
		else if( lhs.m_bCanBuy && !rhs.m_bCanBuy )
			return true;

		return false;
	}
};
//----------------------------------------------------------------------------------------------------------------------------------
// EVT_EXERCISESOLDIER
#define EVENT_ICON_SCALE 0.80f
class ExerciseSoldierEventWnd : public ioWnd
{
public:
	enum
	{
		ID_EXERCISE_SOLDIER_1_BTN  = 101,
		ID_EXERCISE_SOLDIER_2_BTN  = 102,
		ID_EXERCISE_SOLDIER_3_BTN  = 103,
		ID_EXERCISE_SOLDIER_4_BTN  = 104,
		ID_EXERCISE_SOLDIER_5_BTN  = 105,
		ID_EXERCISE_SOLDIER_6_BTN  = 106,
		ID_EXERCISE_SOLDIER_7_BTN  = 107,
		ID_EXERCISE_SOLDIER_8_BTN  = 108,
		ID_EXERCISE_SOLDIER_9_BTN  = 109,
		ID_EXERCISE_SOLDIER_10_BTN  = 110,
		ID_EXERCISE_SOLDIER_11_BTN  = 111,
		ID_EXERCISE_SOLDIER_12_BTN  = 112,
		ID_EXERCISE_SOLDIER_13_BTN  = 113,
		ID_EXERCISE_SOLDIER_14_BTN  = 114,
		ID_EXERCISE_SOLDIER_15_BTN  = 115,
		ID_EXERCISE_SOLDIER_16_BTN  = 116,
		ID_EXERCISE_SOLDIER_17_BTN  = 117,
		ID_EXERCISE_SOLDIER_18_BTN  = 118,
		ID_EXERCISE_SOLDIER_19_BTN  = 119,
		ID_EXERCISE_SOLDIER_20_BTN  = 120,
		ID_EXERCISE_SOLDIER_21_BTN  = 121,
		ID_EXERCISE_SOLDIER_22_BTN  = 122,
		ID_EXERCISE_SOLDIER_23_BTN  = 123,
		ID_EXERCISE_SOLDIER_24_BTN  = 124,
		ID_EXERCISE_SOLDIER_25_BTN  = 125,

		ID_VERT_SCROLL              = 1001,
	};

	enum 
	{
		MAX_EXERCISE_SOLDIER_BTN = 25,
	};

protected:
	ioPlayStage *m_pPlayStage;
	int          m_iCurPos;
	int          m_iMaxPage;
	int          m_iCurBtnSize;
	vShopItemInfoList m_vItemInfoList;
	IntVec       m_vExtraClassVec;
	ioUIRenderFrame *m_pLeftFrm;

	void _UpdateChildPos(int a_iArray ,int iWndID);
	bool IsCanExerciseChar( bool bSelectWait );

	void SetOwnerChangeWaitState();
	void SendChangeWait( int iArray );
	void SetBlankIcon( int a_iWndID );

	void SetMaxPage();

	bool ChangeBoughtChar( int iArray, bool bWait );

	void SortItemInfo();
	int  GetSetIdx( int iArray );

	void SetWndBtnSize();

public:
	void SetPlayStage( ioPlayStage *pPlayStage );
	void SetBoughtType( SoldierExerciseBtn::SoldierBoughtType eBoughtType , int iCharArray );
	void UpdateChildPos();

protected:
	virtual void OnProcess( float fTimePerSec );
	virtual void OnRender();

public:	
	virtual void iwm_command( ioWnd *pWnd, int cmd, DWORD param );
	virtual void iwm_show();	
	virtual void iwm_wheel( int zDelta );
	virtual void iwm_vscroll( DWORD dwID, int curPos );
	virtual void SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate = false );
	virtual void ParseExtraInfo( ioXMLElement &xElement );
	virtual void AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame );

public:
	ExerciseSoldierEventWnd();
	virtual ~ExerciseSoldierEventWnd();
};