#pragma once


#include "SP2Packet.h"

class CustomSoundData
{
protected:
	typedef struct tagNetworkInfo
	{
		DWORD m_dwUserIndex;
		DWORD m_dwDataPos;

		tagNetworkInfo()
		{
			m_dwUserIndex = 0;
			m_dwDataPos   = 0;
		}
	}NetworkInfo;
	typedef std::vector< NetworkInfo > NetworkInfoList;

protected:
	BYTE *m_pData;
	DWORD m_dwSize;
	DWORD m_dwUnpacFileCRC;
	ioHashString    m_szKeyWord;
	ioHashString    m_szFilePath;
	NetworkInfoList m_vNetworkInfoList;

	DWORD  GetCurData( OUT CustomSoundPacket& rData , IN DWORD dwDataPos );

public:
	void   Clear();
	void   ClearNetworkInfo();

	bool   OpenAndPac( const char *szFileName , DWORD dwCRC );
	bool   UnpacAndSave( const char *szFileName );

	void   SendDataToAll( DWORD dwIndex );
	void   CheckAndDeleteNetworkInfo();

	DWORD  SetCurData( IN const CustomSoundPacket &rData, IN DWORD dwDataPos );
	void   NewData( DWORD dwSize );

	void   SetKeyWord( const char *szKeyWord ) { m_szKeyWord = szKeyWord; }

	const DWORD  GetSize() const { return m_dwSize; }
	const ioHashString &GetKeyWord() const { return m_szKeyWord; }
	const DWORD GetUnpacCRC() const { return m_dwUnpacFileCRC; }
	const ioHashString &GetFilePath() const { return m_szFilePath; }

	DWORD GetNetworkDataPos( DWORD dwUserIndex );
	void  SetNetworkDataPos( DWORD dwUserIndex, DWORD dwDataPos );
	void  AddNetworkInfo( DWORD dwUserIndex );
	bool  IsCompletedData( DWORD dwUserIndex );
	void  DeleteNetworkInfo( DWORD dwUserIndex );

public:
	CustomSoundData();
	virtual ~CustomSoundData();
};
//---------------------------------------------------------------------------------------------------------------------------------------------------


class ioCustomSoundMgr : public Singleton< ioCustomSoundMgr >
{
protected:
	enum { MAX_WAV_SIZE = 50000, };
	typedef std::map< DWORD , CustomSoundData* > CustomSoundDataMap;

	typedef struct tagCustomSoundInfo
	{
		DWORD              m_dwUserIndex;
		ioHashString       m_szPublicID;
		CustomSoundDataMap m_CustomSoundDataMap;
			
		tagCustomSoundInfo()
		{
			m_dwUserIndex = 0;
		}

	}CustomSoundInfo;
	typedef std::vector< CustomSoundInfo* > CustomSoundInfoList;

protected:
	CustomSoundInfoList     m_vCustomSoundInfoList;
	ioHashString            m_szDefaultChatIcon;
	int                     m_iMaxCustomData;
	DWORD                   m_dwDelayTimePlayMy;
	DWORD                   m_dwLastTimePlayMy;
	DWORD                   m_dwMaxPlayLength;
	
protected:
	enum { MAX_DISABLE_LIST = 100, };
	ioHashStringVec         m_vDisableList;
	int                     m_iCurDisableChangeArray;
	bool                    m_bSaveDisableList;
public:
	static ioCustomSoundMgr& GetSingleton();

protected:
	void Clear();
	void FilterChar( const char *src, char *dst, char *szFilterChar );

	CustomSoundInfo *GetCustomSoundInfoPtr( DWORD dwUserIndex );
	void LoadINIForDisable( ioINILoader &rkLoader );
	void SaveINIForDisable();

public:
	void CustomSoundLoop();
	void LoadINI( DWORD dwUserIndex, const ioHashString &rszPublicID );

	void GetCustomSoundInfo( IN const ioHashString & rszChat , OUT DWORD &rdwIndex, OUT DWORD &rdwCRC , OUT DWORD &rdwSize );
	void SendCustomSoundInfo( DWORD dwIndex , DWORD dwCRC, DWORD dwSize, bool bAll , TeamType eTeamType );
	void PlayMy( DWORD dwCustomSoundIndex , DWORD dwCRC );
	void PlayByPath( const char *szPath );

	const ioHashString &GetDefaultChatIcon() const { return m_szDefaultChatIcon; }

	void AddSendNetworkInfo( DWORD dwCustomSoundIndex, DWORD dwUserIndex );
	void AddCustomSoundData( DWORD dwUserIndex, const ioHashString &rszPublicID, DWORD dwCustomSoundIndex, DWORD dwCustomSoundSize );
	bool UpdateCustomSoundData( DWORD dwUserIndex, DWORD dwCustomSoundIndex, const CustomSoundPacket &rData );

	void ClearExceptMe();

	void GetMyKeywordAndPath( DWORD dwCustomSoundIndex, OUT ioHashString &rszKeyword, OUT ioHashString &rszPath );
	void DeleteMyCustomSoundData( DWORD dwCustomSoundIndex );
	bool AddMyCustomSoundData( DWORD dwCustomSoundIndex, const char *szKeyword, const char *szFileName );
	void UpdateMyKeyword( DWORD dwCustomSoundIndex, const char *szKeyword );
	void DeleteCustomSoundInfo( const ioHashString &rszPubicID );

	void SaveINIByFolder();
	void SaveINI( DWORD dwCustomSoundIndex );
	void SaveINIBlank( DWORD dwCustomSoundIndex );

	bool CheckWaveFile( const char *szFullPath );

	void InsertDisable( const ioHashString &rszPublicID );
	void RemoveDisable( const ioHashString &rszPublicID );
	bool IsDisable( const ioHashString &rszPublicID );

	void GetFilteredKeyword( IN const char *szKeyword, OUT ioHashString &rszFiltered );
	bool IsCanTimePlayMy() const;

public:
	ioCustomSoundMgr(void);
	virtual ~ioCustomSoundMgr(void);
};

#define g_CustomSoundMgr ioCustomSoundMgr::GetSingleton()

