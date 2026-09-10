#pragma once


class ioBonusCashManager : public Singleton< ioBonusCashManager >
{
public:
	struct BonusCashInfo
	{
		CTime ExpiredTime;
		int	  iAcquisitionCash;
		int	  iRemainingCash;

		BonusCashInfo::BonusCashInfo()
		{
			ExpiredTime = CTime(0);
			iAcquisitionCash = 0;
			iRemainingCash = 0;
		}

		bool empty() const
		{
			if ( iAcquisitionCash == 0 && iRemainingCash == 0 && ExpiredTime.GetTime() == 0 )
				return true;

			return false;
		}
	};
private:
	typedef std::map<int,BonusCashInfo> BonusCashInfoList;
	BonusCashInfoList m_AliveBonusCashInfoList;
	BonusCashInfoList m_ExpireBonusCashInfoList;
public:
	static ioBonusCashManager& GetSingeton();
public:
	void LoadBonusCashInfo();
	void ApplyBonusCashData( SP2Packet& rkPacket );
	void ApplyBonusCashAdd( SP2Packet& rkPacket );
	void ApplyBonusCashUpdate( SP2Packet& rkPacket );
	void ApplyBonusCashInfo( SP2Packet& rkPacket );
	void ApplyBonusCashAliveInfo( SP2Packet& rkPacket );;
	void ApplyBonusCashExpireInfo( SP2Packet& rkPacket );;
	void ApplyBonusCashExpired( SP2Packet& rkPacket );

	int GetAliveBonusCashInfoSize() { return m_AliveBonusCashInfoList.size(); }
	int GetExpireBonusCashInfoSize() { return m_ExpireBonusCashInfoList.size(); }
	const BonusCashInfo& GetConstAliveBonusCashInfoByIndex( int iIndex ) const ;
	const BonusCashInfo& GetConstAliveBonusCashInfoByArray( int iArray ) const ;

	const BonusCashInfo& GetConstExpireBonusCashInfoByIndex( int iIndex ) const ;
	const BonusCashInfo& GetConstExpireBonusCashInfoByArray( int iArray ) const ;

	bool CheckAlarmExpireBonusCash();
protected:
	int CalcTotalAliveBonusCash();
	void AddExpireInfo( int iIndex, BonusCashInfo& rkInfo, const ioHashString& szFuncName );
	bool GetAliveBonusCashInfoByIndex( int iIndex, BonusCashInfo& rkInfo );
public:
	ioBonusCashManager(void);
	virtual ~ioBonusCashManager(void);
};

#define g_BonusCashMgr ioBonusCashManager::GetSingleton()