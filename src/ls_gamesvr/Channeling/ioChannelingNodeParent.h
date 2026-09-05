#ifndef __ioChannelingNodeParent_h__
#define __ioChannelingNodeParent_h__

class User;
class UserBonusCash;

// 한국 채널링 서비스는 채널링 ID 무조건 사용함 ( private id를 채널링 ID로 사용하지 않음 )
class ioChannelingNodeParent
{
public: 
	enum 
	{
		MAX_ITEM_VALUE = 5,
	};
public:
	virtual bool AddRequestGetCashPacket( IN User *pUser, OUT SP2Packet &rkSendPacket, IN DWORD dwErrorPacketID, IN DWORD dwErrorPacketType ) = 0;
	virtual bool OnRecieveGetCash( User *pUser, SP2Packet &rkRecievePacket, DWORD dwErrorPacketID, DWORD dwErrorPacketType ) = 0;

	virtual bool AddRequestOutputCashPacket( IN User *pUser, OUT SP2Packet &rkSendPacket, IN DWORD dwErrorPacketID, IN DWORD dwErrorPacketType, IN const char *szRecvPrivateID, IN const char *szRecvPublicID, IN DWORD dwRecvUserIndex = 0) = 0;
	virtual bool CheckRequestOutputCash( User *pUser, int& iPayAmt, DWORD dwErrorPacketID, DWORD dwErrorPacketType, DWORD dwItemCode, IntOfTwoVec& vConsumeInfo) = 0;
	virtual bool CheckReciveOutputCash( User *pUser, int iReturnValue, int iPayAmt, bool bBillingError, const ioHashString &rsBillingError, DWORD dwErrorPacketID, DWORD dwErrorPacketType ) = 0;
	virtual bool UpdateOutputCash( User *pUser, SP2Packet &rkRecievePacket, int iPayAmt, DWORD dwErrorPacketID, DWORD dwErrorPacketType );

	virtual void SendCancelCash( IN SP2Packet &rkPacket ) {}
	virtual void SendAddCash( IN User *pUser, IN int iAddCash, IN DWORD dwEtcItemType ) {}
	virtual bool OnRecieveAddCash( User *pUser, SP2Packet &rkRecievePacket ) { return true; };

	static void GetItemValueList( IN SP2Packet &rkPacket, IN int iType, OUT int iItemValueList[MAX_ITEM_VALUE] );
	static void SetItemValueList( OUT SP2Packet &rkPacket, IN int iType, IN const int iItemValueList[MAX_ITEM_VALUE] );

	virtual bool AddRequestFillCashUrlPacket( IN User *pUser, OUT SP2Packet &rkSendPacket ){ return false; }

	virtual bool AddReuestSubscriptionRetractCashCheckPacket( IN User *pUser, OUT SP2Packet & rkSendPacket );
	virtual bool OnReceiveSubscriptionRetractCashCheck( IN User *pUser, OUT SP2Packet& rReceivePacket, IN DWORD dwErrorPacketID, DWORD dwErrorPacketType );

	virtual bool AddReuestSubscriptionRetractCashPacket( IN User *pUser, OUT SP2Packet & rkSendPacket );
	virtual bool OnReceiveSubscriptionRetractCash( IN User *pUser, OUT SP2Packet& rReceivePacket, IN DWORD dwErrorPacketID, DWORD dwErrorPacketType );

	virtual bool AddReuestSubscriptionRetractCashFailPacket( IN User *pUser, OUT SP2Packet & rkSendPacket );
	virtual bool OnReceiveSubscriptionRetractCashFail( IN User *pUser, OUT SP2Packet& rReceivePacket, IN DWORD dwErrorPacketID, DWORD dwErrorPacketType );

	virtual bool IsSubscriptionRetractCheck() = 0;
	virtual bool IsSubscrptionRetract() = 0;
	virtual bool IsPossibleToUseBonusCash(DWORD dwPacketID)
	{
		if( (dwPacketID == STPK_PRESENT_BUY) || (dwPacketID == STPK_CHAR_EXTEND) || (dwPacketID == STPK_CHAR_CHANGE_PERIOD) )
			return false;

		return true;
	}
	// 아이템 구매로 사용 할 보너스 캐시
	BOOL GetBonusCashInfoForOutputCash(User* pUser, const int iCurCash, int& iGoodsPrice, IntOfTwoVec& vConsumeInfo)

	{
		if( !pUser )
			return FALSE;

		int iBonusCash = pUser->GetAmountOfBonusCash();
		
		//HRYOON BONUS CASH 실캐시+보너스캐시 >= 아이템가격
		if( iCurCash + iBonusCash >= iGoodsPrice )
		{
			//HRYOON BONUS CASH 보너스 캐쉬 사용여부 
			if( iGoodsPrice > iCurCash )
			{
				//HRYOON BONUS CASH 사용할 보너스 캐시 계산
				iBonusCash	= iGoodsPrice - iCurCash;
				iGoodsPrice	= iCurCash;			// 빌링에서 차감할 실캐시

				UserBonusCash* pInfo = pUser->GetUserBonusCash();		//?
				if( pInfo )
				{
					pInfo->GetMoneyForConsume(vConsumeInfo, iBonusCash);		

					int iSize	= vConsumeInfo.size();
					int iVal	= 0;

					for( int i = 0; i < iSize; i++ )
						iVal += vConsumeInfo[i].value2;		//사용금액
				
					if( iVal == iBonusCash )				//사용금액 == 사용해야할 금액 같으면 OK
						return TRUE;

				}
			}
			else
				return TRUE;		//보너스 캐쉬 사용안해도 되는 경우
		}

		return FALSE;
	}

public:
	virtual ChannelingType GetType();

public:
	ioChannelingNodeParent(void);
	virtual ~ioChannelingNodeParent(void);
};

#endif // __ioChannelingNodeParent_h__