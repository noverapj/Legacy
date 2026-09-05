// DBClient.h: interface for the DBClient class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBCLIENT_H__8BBCE6AF_B8A6_4D7C_A9FA_B2B4B32491CA__INCLUDED_)
#define AFX_DBCLIENT_H__8BBCE6AF_B8A6_4D7C_A9FA_B2B4B32491CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../QueryData/QueryData.h"
#include "../Nodeinfo/NodeHelpStructDefine.h"
#include "../util/cSerialize.h"
//DB AGENT MSG TYPE
// GET : Select , SET : Insert , DEL : Delete , UPD : Update
#define DBAGENT_USER_LOGIN_INFO_GET_UPD		0x0002
#define DBAGENT_USER_DATA_GET		        0x0003
#define DBAGENT_USER_DATA_UPD               0x0004
#define DBAGENT_USER_LADDER_POINT_UPD       0x0005

#define DBAGENT_CHAR_DATA_SET				0x0006
#define DBAGENT_CHAR_INDEX_GET              0x0007
#define DBAGENT_CHAR_DATA_GET				0x0008
#define DBAGENT_CHAR_DATA_UPD               0x0009

#define DBAGENT_INVEN_DATA_SET				0x0010
#define DBAGENT_INVEN_DATA_CREATE_INDEX    	0x0011
#define DBAGENT_INVEN_DATA_GET				0x0012
#define DBAGENT_INVEN_DATA_UPD				0x0013

#define DBAGENT_GUILD_ETC_ITEM_DELETE_UPD   0x0014

#define DBAGENT_CHAR_DATE_LIMIT_DATE_DEL    0x0016

#define DBAGENT_CLASS_EXPERT_DATA_GET       0x0017
#define DBAGENT_CLASS_EXPERT_DATA_UPD       0x0018
#define DBAGENT_CLASS_EXPERT_DATA_SET       0x0019
#define DBAGENT_CLASS_EXPERT_DATA_NEW_INDEX	0x0020

#define DBAGENT_CHAR_DATA_DEL               0x0021

#define DBAGENT_HEADQUARTERS_DATA_COUNT     0x0022
#define DBAGENT_HEADQUARTERS_DATA_SET       0x0023
#define DBAGENT_HEADQUARTERS_DATA_GET       0x0024
#define DBAGENT_HEADQUARTERS_DATA_UPD       0x0025

#define DBAGENT_CHAR_DATA_ALL_GET           0x0030

#define DBAGENT_USER_LOGOUT_UPD             0x0031
#define DBAGENT_USER_ALL_LOGOUT_UPD         0x0032
#define DBAGENT_USER_LOGIN_INFO_GET         0x0033

#define DBAGENT_SERVER_INFO_DEL          	0x0034
#define DBAGENT_SERVER_SECOND_KEY_GET       0x0035

#define DBAGENT_ITEM_BUYCNT_SET             0x0036

#define DBAGENT_AWARD_EXPERT_GET			0x0040
#define DBAGENT_AWARD_DATA_SET				0x0041
#define DBAGENT_AWARD_DATA_CREATE_INDEX    	0x0042
#define DBAGENT_AWARD_DATA_GET				0x0043
#define DBAGENT_AWARD_DATA_UPD				0x0044
#define DBAGENT_AWARD_EXPERT_UPD            0x0045

#define DBAGENT_USER_COUNT_UPD           	0x0048
#define DBAGENT_USER_MOVE_SERVER_ID_UPD     0x0049
#define DBAGENT_SERVER_ON_UPD               0x0050

#define DBAGENT_FRIEND_LIST_GET             0x0052
#define DBAGENT_FRIEND_USERID_CHECK_GET     0x0053
#define DBAGENT_FRIEND_INSERT_GET           0x0054
#define DBAGENT_FRIEND_DELETE_DEL           0x0055

#define DBAGENT_EVENT_DATA_ALL_GET          0x0056
#define DBAGENT_EVENT_DATA_UPD              0x0057

#define DBAGENT_USER_RECORD_GET             0x0059
#define DBAGENT_USER_RECORD_UPD             0x0060

#define DBAGENT_CREATE_GUILD_GET				0x0061
#define DBAGENT_CREATE_GUILD_REG_GET			0x0062 
#define DBAGENT_CREATE_GUILD_INFO_GET			0x0063
#define DBAGENT_CREATE_GUILD_FAIL_PESO_UPD		0x0064
#define DBAGENT_USER_GUILD_INFO_GET				0x0065  
#define DBAGENT_ENTRY_DELAY_GUILD_LIST_GET		0x0066
#define DBAGENT_GUILD_MEMBER_DELETE_DEL			0x0067
#define DBAGENT_GUILD_MEMBER_EVENT_UPD			0x0068
#define DBAGENT_GUILD_ENTRY_DELAY_MEMBER_GET	0x0069
#define DBAGENT_GUILD_MEMBER_LIST_GET			0x0070
#define DBAGENT_GUILD_TITLE_UPD					0x0071
#define DBAGENT_GUILD_NAME_CHANGE_GET           0x0072
#define DBAGENT_GUILD_DELAY_GUILD_LIST_DEL      0x0073
#define DBAGENT_GUILD_ENTRY_APP_GET             0x0073
#define DBAGENT_GUILD_ENTRY_CANCEL_DEL          0x0074
#define DBAGENT_GUILD_ENTRY_AGREE_GET           0x0075
#define DBAGENT_GUILD_ENTRY_REFUSE_DEL          0x0076
#define DBAGENT_GUILD_ENTRY_AGREE_USER_INFO_GET 0x0077
#define DBAGENT_GUILD_LEAVE_USER_DEL            0x0078
#define DBAGENT_GUILD_MASTER_CHANGE_UPD         0x0079
#define DBAGENT_GUILD_POSITION_CHANGE_UPD       0x0080
#define DBAGENT_GUILD_KICK_OUT_DEL              0x0081
#define DBAGENT_GUILD_SIMPLE_DATA_GET           0x0082
#define DBAGENT_GUILD_MARK_CHANGE_UPD           0x0083
#define DBAGENT_GUILD_MEMBER_LIST_EX_GET		0x0084

#define DBAGENT_FRIEND_REQUEST_LIST_GET			0x0085
#define DBAGENT_FRIEND_APPLICATION_GET			0x0086
#define DBAGENT_FRIEND_REQUEST_DELETE_DEL       0x0087
#define DBAGENT_FRIEND_DEVELOPER_INSERT_GET     0x0088

#define DBAGENT_USER_ENTRY_GET                  0x0089
#define DBAGENT_USER_EXIST_GET                  0x0090

#define DBAGENT_GUILD_MARK_CHANGE_KEY_VALUE_SET 0x0091
#define DBAGENT_GUILD_MARK_CHANGE_KEY_VALUE_GET 0x0092
#define DBAGENT_GUILD_MARK_CHANGE_KEY_VALUE_UPD 0x0093

#define DBAGENT_CHAR_REG_DATE_UPD               0x0094

#define DBAGENT_GUILD_MARK_BLOCK_INFO    		0x0095

#define DBAGENT_ETC_ITEM_DATA_SET               0x0096
#define DBAGENT_ETC_ITEM_DATA_CREATE_INDEX      0x0097
#define DBAGENT_ETC_ITEM_DATA_GET               0x0098
#define DBAGENT_ETC_ITEM_DATA_UPD               0x0099

#define DBAGENT_PRESENT_DATA_SET                0x0100
#define DBAGENT_PRESENT_DATA_GET                0x0101

#define DBAGENT_PRESENT_DATA_LOG_SET            0x0102
#define DBAGENT_PRESENT_DATA_LOG_GET            0x0103

#define DBAGENT_EVENT_DATA_SET                  0x0104
#define DBAGENT_EVENT_INDEX_GET                 0x0105

#define DBAGENT_EVENT_LOG_SET                   0x0108
#define DBAGENT_GUILD_USER_LADDER_POINT_ADD_GET 0x0109
#define DBAGENT_GUILD_ENTRY_APP_MASTER_GET      0x0110
#define DBAGENT_PUBLIC_ID_EXIST_GET             0x0111
#define DBAGENT_PUBLIC_ID_ETC_ITEM_UPD          0x0112

#define DBAGENT_GROWTH_DATA_GET					0x0113
#define DBAGENT_GROWTH_DATA_UPD					0x0114
#define DBAGENT_GROWTH_DATA_SET					0x0115
#define DBAGENT_GROWTH_DATA_NEW_INDEX			0x0116

#define DBAGENT_CHANGED_PUBLIC_ID_GET			0x0117

#define DBAGENT_CAMP_SEASON_BONUS_GET           0x0118    
#define DBAGENT_CAMP_SEASON_BONUS_DEL           0x0119

#define DBAGENT_BLOCK_TYPE_GET                  0x0120
#define DBAGENT_TRIAL_SET                       0x0121

#define DBAGENT_FISH_DATA_GET					0x0122
#define DBAGENT_FISH_DATA_UPD					0x0123
#define DBAGENT_FISH_DATA_SET					0x0124
#define DBAGENT_FISH_DATA_NEW_INDEX				0x0125

#define DBAGENT_PRESENT_DATA_UPD                0x0126

#define DBAGENT_EXTRAITEM_DATA_SET				0x0126
#define DBAGENT_EXTRAITEM_DATA_CREATE_INDEX    	0x0127
#define DBAGENT_EXTRAITEM_DATA_GET				0x0128
#define DBAGENT_EXTRAITEM_DATA_UPD				0x0129

#define DBAGENT_MEMBER_COUNT_GET                0x0130         
#define DBAGENT_MEMBER_SET                      0x0131

#define DBAGENT_FIRST_PUBLIC_ID_EXIST_GET       0x0132
#define DBAGENT_FIRST_PUBLIC_ID_UPD             0x0133
#define DBAGENT_FIRST_PUBLIC_ID_CHANGED_ID_GET  0x0134

#define DBAGENT_USER_INDEX_AND_PRESENT_CNT_GET  0x0135
#define DBAGENT_PRESENT_DATA_BY_USERINDEX_SET   0x0136

#define DBAGENT_CONTROL_KEYS_GET                0x0137
#define DBAGENT_CONTROL_KEYS_UPD                0x0138

#define DBAGENT_QUEST_DATA_SET					 0x0139
#define DBAGENT_QUEST_DATA_CREATE_INDEX    		 0x0140
#define DBAGENT_QUEST_DATA_GET					 0x0141
#define DBAGENT_QUEST_DATA_UPD					 0x0142
#define DBAGENT_QUEST_DATA_DEL                   0x0143
#define DBAGENT_QUEST_COMPLETE_DATA_SET          0x0144
#define DBAGENT_QUEST_COMPLETE_DATA_CREATE_INDEX 0x0145
#define DBAGENT_QUEST_COMPLETE_DATA_GET          0x0146
#define DBAGENT_QUEST_COMPLETE_DATA_UPD          0x0147

#define DBAGENT_MEDALITEM_DATA_SET				0x0148
#define DBAGENT_MEDALITEM_DATA_CREATE_INDEX    	0x0149
#define DBAGENT_MEDALITEM_DATA_GET				0x0150
#define DBAGENT_MEDALITEM_DATA_UPD				0x0151

#define DBAGENT_QUEST_WEB_ALARM_SET             0x0152
#define DBAGENT_PRESENT_ALL_DELETE_DEL          0x0153

#define DBAGENT_HERO_DATA_GET                   0x0154     
#define DBAGENT_HERO_TOP100_GET                 0x0155
#define DBAGENT_HERO_EXPERT_UPD                 0x0156

#define DBAGENT_TRADE_CREATE					0x0157
#define DBAGENT_TRADE_CREATE_INDEX				0x0158
#define DBAGENT_TRADE_COMPLETE					0x0159
#define DBAGENT_TRADE_CANCEL					0x0160

#define DBAGENT_SOLDIER_PRICE_COLLECTED_BUYTIME  0x0162
#define DBAGENT_SOLDIER_PRICE_COLLECTED_PLAYTIME 0x0163

#define DBAGENT_HERO_EXPERT_INIT                 0x0164
#define DBAGENT_ITEM_CUSTOM_UNIQUE_INDEX         0x0165

#define DBAGENT_TEST_SERVER_DELAY                0x0166

#define DBAGENT_BEST_FRIEND_LIST_GET             0x0167
#define DBAGENT_BEST_FRIEND_DEL					 0x0168
#define DBAGENT_BEST_FRIEND_LIST_UPD			 0x0169
#define DBAGENT_BEST_FRIEND_ADD_SET				 0x0170
#define DBAGENT_BEST_FRIEND_ADD_RESULT_GET		 0x0171
#define DBAGENT_CHAR_RENTAL_TIME_UPD             0x0172
#define DBAGENT_CHAR_RENTAL_HISTORY_SET          0x0173
#define DBAGENT_CHAR_RENTAL_HISTORY_GET          0x0174

#define DBAGENT_USER_BIRTH_DATE_GET              0x0175

#define DBAGNET_FRIEND_RECOMMEND_GET             0x0176
#define DBAGNET_FRIEND_RECOMMEND_UPD             0x0177

#define DBAGENT_DISCONNECT_CHECK_GET             0x0178

#define DBAGENT_EXPAND_MEDAL_SLOT_DATA_SET				0x0179
#define DBAGENT_EXPAND_MEDAL_SLOT_DATA_CREATE_INDEX    	0x0180
#define DBAGENT_EXPAND_MEDAL_SLOT_DATA_GET				0x0181
#define DBAGENT_EXPAND_MEDAL_SLOT_DATA_UPD				0x0182

#define DBAGENT_USER_SELECT_SHUT_DOWN_GET               0x0183

#define DBAGENT_ALCHEMIC_DATA_SET				0x0184
#define DBAGENT_ALCHEMIC_DATA_CREATE_INDEX    	0x0185
#define DBAGENT_ALCHEMIC_DATA_GET				0x0186
#define DBAGENT_ALCHEMIC_DATA_UPD				0x0187

#define DBAGENT_TOURNAMENT_TEAM_CREATE_GET              0x0188
#define DBAGENT_TOURNAMENT_TEAM_INDEX_GET               0x0189
#define DBAGENT_TOURNAMENT_TEAM_LIST_GET                0x0190
#define DBAGENT_TOURNAMENT_CREATE_TEAM_DATA_GET         0x0191
#define DBAGENT_TOURNAMENT_TEAM_MEMBER_GET              0x0192
#define DBAGENT_TOURNAMENT_TEAM_APP_LIST_GET            0x0193
#define DBAGENT_TOURNAMENT_TEAM_APP_ADD_SET             0x0194
#define DBAGENT_TOURNAMENT_TEAM_APP_DEL                 0x0195
#define DBAGENT_TOURNAMENT_TEAM_APP_REG_UPD             0x0196
#define DBAGENT_TOURNAMENT_TEAM_APP_LIST_DEL            0x0197
#define DBAGENT_TOURNAMENT_TEAM_MEMBER_DEL              0x0198
#define DBAGENT_TOURNAMENT_TEAM_APP_AGREE_MEMBER        0x0199       
#define DBAGENT_TOURNAMENT_TEAM_APP_AGREE_TEAM          0x0200		 
#define DBAGENT_USER_CAMP_POSITION_UPD                  0x0201
#define DBAGENT_TOURNAMENT_HISTORY_LIST                 0x0202
#define DBAGENT_TOURNAMENT_HISTORY_USER_LIST            0x0203
#define DBAGENT_TOURNAMENT_REWARD_DATA                  0x0204
#define DBAGENT_TOURNAMENT_REWARD_DATA_DELETE           0x0205 
#define DBAGENT_TOURNAMENT_CUSTOM_DATA_ADD              0x0206
#define DBAGENT_TOURNAMENT_CUSTOM_REWARD_GET            0x0207
#define DBAGENT_TOURNAMENT_CUSTOM_REWARD_DEL            0x0208

#define DBAGENT_EVENT_MARBLE_SET                        0x0226

#define DBAGENT_CLOVER_INFO_GET							0x0230
#define DBAGENT_CLOVER_INFO_SET							0x0231
#define DBAGENT_FRIEND_CLOVER_INFO_SET					0x0232
#define DBAGENT_FRIEND_RECEIVE_CLOVER_SET				0x0233

#define DBAGENT_BINGO_NUMBER_SET						0x0234
#define DBAGENT_BINGO_PRESENT_SET						0x0235
#define DBAGENT_BINGO_NUMBER_GET						0x0236
#define DBAGENT_BINGO_PRESENT_GET						0x0237
#define DBAGENT_BINGO_NUMBER_UPD						0x0238
#define DBAGENT_BINGO_PRESENT_UPD						0x0239

#define DBAGENT_RELATIVE_GRADE_INFO_GET					0x0240
#define DBAGENT_RELATIVE_GRADE_REWARD_UPD				0x0241
#define DBAGENT_RELATIVE_GRADE_TABLE_SET				0x0242

#define DBAGENT_RELATIVE_GRADE_UPDATE					0x0243
#define DBAGENT_TOURNAMENT_CHEER_DECISION				0x0244
#define DBAGENT_TOURNAMENT_CHEER_LIST_GET               0x0245

#define DBAGENT_TOURNAMENT_CHEER_REWARD_DATA            0x0246
#define DBAGENT_TOURNAMENT_CHEER_REWARD_DATA_DELETE     0x0247

// 청약
#define DBAGENT_SUBSCRIPTION_DATA_SET					0x0248
#define DBAGENT_SUBSCRIPTION_DATA_GET					0x0249
#define DBAGENT_SUBSCRIPTION_DATA_UPDATE				0x0250
#define DBAGENT_SUBSCRIPTION_DATA_DEL					0x0251

//출석체크
#define DBAGENT_ATTENDANCE_ADD							0x0252
#define DBAGENT_ATTENDANCE_LIST_GET						0x0253
#define DBAGENT_ATTENDANCE_DELETE						0x0254

//펫
#define DBAGENT_PET_DATA_GET							0x0255
#define DBAGENT_PET_DATA_SET							0x0256
#define DBAGENT_PET_DATA_UPD							0x0257
//#define DBAGENT_PET_DATA_CREATE_INDEX					0x0258

//코스튬
#define DBAGENT_COSTUME_DATA_GET					    0x0258
#define DBAGENT_COSTUME_DATA_INSERT						0x0259
#define DBAGENT_COSTUME_DATA_UPDATE						0x0260
#define DBAGENT_COSTUME_DATA_DELETE						0x0261

//미션
#define DBAGENT_MISSION_DATA_GET						0x0262
#define DBAGENT_MISSION_DATA_UPDATE						0x0263	
#define DBAGENT_MISSION_DATA_INIT						0x0264

//출석부
#define DBAGENT_ROLLBOOK_DATA_GET						0x0265
#define DBAGENT_ROLLBOOK_DATA_UPDATE					0x0266

//선물함 삭제
#define DBAGNET_PRESENT_DEL								0x0267

//길드 개편
#define DBAGENT_GUILD_ATTENDANCE_MEMBER_GET				0x0268
#define DBAGENT_GUILD_ATTEND_REWARD_UPDATE				0x0269
#define	DBAGENT_GUILD_RANK_REWARD_UPDATE				0x0270
#define DBAGENT_GUILD_USER_ATTENDANCE_INFO_INSERT		0x0271
#define DBAGENT_PRESENT_ADD_BY_PRIVATE					0x0272

#define DBAGENT_GAME_SPENT_MONEY_GET					0x0273
#define DBAGENT_GAME_SPENT_MONEY_SET					0x0274
#define DBAGENT_GAME_POPUP_INDEX_GET					0x0275
#define DBAGENT_GAME_POPUP_INDEX_SET					0x0276

//채널링 유저 고유 ID 겟
#define DBAGENT_USER_CHANNELING_KEY_VALUE_GET			0x0277		

//길드 본부 블럭 정보 select
#define DBAGENT_GUILD_BLOCK_INFOS_GET					0x0278
#define DBAGENT_GUILD_BLOCK_RETRIEVE_DELETE				0x0279
#define DBAGENT_GUILD_BLOCK_CONSTRUCT_MOVE				0x0280
#define DBAGENT_GUILD_INVEN_VERSION_GET					0x0281
#define DBAGENT_GUILD_INVEN_INFO_GET					0x0282
#define DBAGENT_GUILD_BLOCK_ADD							0x0283
#define DBAGENT_GUILD_BLOCK_DEFAULT_CONSTRUCT			0x0284

//개인 본부
#define DBAGENT_PERSONAL_HQ_INFOS_GET					0x0285
#define DBAGENT_PERSONAL_HQ_CONSTRUCT					0x0286
#define DBAGENT_PERSONAL_HQ_RETRIEVE					0x0287
#define DBAGENT_PERSONAL_HQ_IS_EXIST					0x0288
#define DBAGENT_PERSONAL_HQ_BLOCK_ADD					0x0289
#define DBAGENT_PERSONAL_HQ_INVEN_INFO_GET				0x0290
#define DBAGENT_PERSONAL_HQ_BLOCKS_INFO_GET				0x0291
#define DBAGENT_PERSONAL_HQ_DEFAULT_CONSTRUCT			0x0292

//기간 캐쉬 박스
#define DBAGENT_TIME_CASH_TABLE_GET						0x0293
#define DBAGENT_TIME_CASH_TABLE_INSERT					0x0294
#define DBAGENT_TIME_CASH_TABLE_UPDATE					0x0295

//칭호
#define DBAGENT_TITLE_UPDATE_STATUS						0x0296
#define DBAGENT_TITLE_INSERT_OR_UPDATE					0x0297
#define DBAGENT_TITLE_SELECT_TITLE						0x0298
//해적 룰렛
#define DBAGENT_PIRATEROULETTE_NUMBER_INSERT			0x0299
#define DBAGENT_PIRATEROULETTE_NUMBER_GET				0x0300
#define DBAGENT_PIRATEROULETTE_NUMBER_UPDATE			0x0301
#define DBAGENT_PIRATEROULETTE_PRESENT_INSERT			0x0302
#define DBAGENT_PIRATEROULETTE_PRESENT_GET				0x0303
#define DBAGENT_PIRATEROULETTE_PRESENT_UPDATE			0x0304

//보너스 캐쉬
#define DBAGENT_BONUS_CASH_INSERT						0x0305
#define DBAGENT_BONUS_CASH_UPDATE						0x0306
#define DBAGENT_BONUS_CASH_SELECT						0x0307
#define DBAGENT_BONUS_CASH_EXPIRATION_DELETE			0x0308
#define DBAGENT_EXPIRED_BONUS_CASH_SELECT				0x0309

//악세사리
#define DBAGENT_ACCESSORY_DATA_GET						0x0310
#define DBAGENT_ACCESSORY_DATA_UPDATE					0x0311
#define DBAGENT_ACCESSORY_DATA_DELETE					0x0312
#define DBAGENT_ACCESSORY_DATA_INSERT					0x0313


// 유저코인
#define DBAGENT_USERCOIN_SELECT							0x0314
#define DBAGENT_USERCOIN_INSERT							0x0315
#define DBAGENT_USERCOIN_UPDATE							0x0316

// 오크통
#define DBAGENT_OAK_BARREL_DATA_SELECT					0x0317
#define DBAGENT_OAK_BARREL_DATA_UPDATE					0x0318
#define DBAGENT_OAK_BARREL_TIME_UPDATE					0x0319
#define DBAGENT_OAK_BARREL_LIMIT_SWORD_INIT				0x0320

// 일대일모드
#define DBAGENT_USER_MATCH_DATA_SELECT					0x0321
#define DBAGENT_USER_MATCH_DATA_UPDATE					0x0322

// 정기
#define DBAGENT_USER_SPIRIT_SELECT						0x0325
#define DBAGENT_USER_SPIRIT_INSERT						0x0326
#define DBAGENT_USER_SPIRIT_UPDATE						0x0327

// 커스텀 메달
#define DBAGENT_USER_CUSTOM_MEDAL_SELECT				0x0328
#define DBAGENT_USER_CUSTOM_MEDAL_INSERT				0x0329
#define DBAGENT_USER_CUSTOM_MEDAL_STATUS_UPDATE			0x0330
#define DBAGENT_USER_CUSTOM_MEDAL_DELETE				0x0331

// 타임게이트
#define DBAGENT_USER_TIME_GATE_SELECT					0x0332
#define DBAGENT_USER_TIME_GATE_UPDATE					0x0333

// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
#define DBAGENT_CARD_MATCHING_DATA_SELECT					0x0340
#define DBAGENT_CARD_MATCHING_DATA_UPDATE					0x0341
// End. 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가

// 2018-08-30 by bckim, 주사위 이벤트 추가		// 뱀주사위
#define DBAGENT_DICE_GAME_DATA_SELECT					0x0342
#define DBAGENT_DICE_GAME_DATA_UPDATE					0x0343
// End. 2018-08-30 by bckim, 주사위 이벤트 추가

#define DBAGENT_DISCONNECT_HDD_SERIAL_CHECK_GET			0x0344	// 2020-06-05 by bckim, 하드시리얼 유저 제재

enum
{
	DBAGENT_PRACTICE_LIST_GET = 0x7000,
	DBAGENT_PRACTICE_SET_ADMISSION = 0x7001,
	DBAGENT_PRACTICE_SET_TIME = 0x7002,
	DBAGENT_PRACTICE_INIT_DATA = 0x7003,
	DBAGENT_PRACTICE_RANK_LIST_GET = 0x7004,
};

#define DBAGENT_PCBANG_PLAY_TIME						0x7101

#define DBAGENT_GAME_PINGPONG							0x0999



//작업 방식
#define _INSERTDB       0
#define _DELETEDB       1
#define _SELECTDB       2
#define _UPDATEDB       3   
#define _SELECTEX1DB    4 

//결과 행동
#define _RESULT_CHECK   0
#define _RESULT_NAUGHT  1
#define _RESULT_DESTROY 2

//RESULT MSG TYPE
#define DBAGENT_RESULT_CHAR_CONFIRM              0
#define DBAGENT_RESULT_CHAR_RENAME               1

//
#define MAX_QUERY_SIZE              2048

class ioCharacter;
class ioQuestData;
class User;
class UserParent;
class DBClient;
class CConnectNode;

class DBAgentNode : public CConnectNode
{
	friend class DBClient;

protected:
	int	m_iDBAgentIndex;  
	int	m_iConnectPort;
	ioHashString  m_szConnectIP;

public:
	virtual void SessionClose( BOOL safely=TRUE );
	virtual void ReceivePacket( CPacket &packet );
	virtual void PacketParsing( CPacket &packet );

protected:
	void OnClose(SP2Packet &packet);

public:
	virtual void OnCreate();       //초기화
	virtual void OnDestroy();
	virtual bool CheckNS( CPacket &rkPacket );	
	virtual int  GetConnectType();

public:
	DWORD GetIndex()					{ return m_iDBAgentIndex; }
	DWORD GetConnectPort()				{ return m_iConnectPort; }
	const ioHashString &GetConnectIP()	{ return m_szConnectIP; }
	
public:
	void SetConnectInfo( const int iIndex, const ioHashString &szIP, int iPort );
	void Reconnect();

protected:
	DBAgentNode( SOCKET s, DWORD dwSendBufSize, DWORD dwRecvBufSize );
	virtual ~DBAgentNode();

	void Init();
	void Destroy();
};
//////////////////////////////////////////////////////////////////////////
class DBClient
{
private:
	static DBClient *sg_Instance;

protected:
	typedef std::map< DWORD, DBAgentNode * > DBAgentNodeMap;
	typedef std::list<std::pair<std::string,int>> DBAgentAddrs;
	DBAgentNodeMap m_DBAgentMap;		
	DBAgentAddrs m_DBAgentInfos;
	DWORD m_dwCurrentTime;

	int			m_iQueryID; //수련장
	cSerialize	m_FT; //수련장
	CQueryData	m_Query; //수련장


protected:
	void DestroyDBAgentMap();

protected:
	DWORD GameServerAgentID();
	DWORD GameServerThreadID();
	
public:
	static DBClient &GetInstance();
	static void ReleaseInstance();

protected:
	void OnPing();
	void OnClassPriceInfo();
	void OnTestZoneReconnect();
	void OnReconnect();

	ValueType GetValueType( VariableType nType, int len );
	bool SendMessage( DWORD dwAgentServerID, SP2Packet &rkPacket );

public:
	void Reset(const int iQueryID); //수련장
	bool ConnectTo();

	int  GetNodeSize()			{ return m_DBAgentMap.size(); }
	int  GetTotalNodeSize()		{ return m_DBAgentInfos.size(); }

	void AddDBAgentInfo(TCHAR* ipaddr, int port);
	bool IsDBAgentActive( int index );

public:
	void ProcessTime();
	void ProcessPing();

public:
	void OnCloseDBAgent( const int iIndex );
	void OnUpdateUserCount( const int64 iServerIndex, int iUserCount );
	void OnUpdateUserMoveServerID( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int64 serverId );
	void OnUpdateServerOn( const int64 iServerIndex );
	void OnDeleteGameServerInfo( const __int64 gameServerId );
	void OnSelectSecondEncryptKey();
	void OnSelectItemBuyCnt();

public:
	void OnSelectUserLoginInfo( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szPrivateID);
	void OnSelectUpdateUserLoginInfo( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szPrivateID , const int64 serverId);

	void OnUpdateUserLogout( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szPrivateID );
	void OnUpdateAllUserLogout( const __int64 gameServerId);
	
	void OnSelectUserData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szPrivateID );
	void OnUpdateUserData( const DWORD dwAgentID, const DWORD dwThreadID,  const USERDATA &user_data, const UserRelativeGradeData &user_relative_grade_data, DWORD dwConnectTime );
	void OnUpdateUserLadderPoint( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD &dwUserIndex, const int &iAccumulationLadderPoint, const int &iLadderPoint );
	void OnUpdateUserHeroExpert( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD &dwUserIndex,  const int &iHeroExpert );
	void OnInitUserHeroExpert( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD &dwUserIndex );

//////////////////////////////////////////////////////////////////////////
// 로그인 후 유저 데이터를 한번에 가져오지 않고 순차적으로 가져온다.
// OnLoginSelectControlKeys 호출하면 OnLoginSelectAllAlchemicData까지 발생하므로 중간 함수를 호출하면 fail
public:   
	void OnLoginSelectControlKeys( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectAllAwardData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectAwardExpert( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectAllClassExpert( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectUserRecord( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectAllEtcItemData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectAllExtraItemData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );	
	void OnLoginSelectAllPetData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectAllQuestCompleteData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectAllQuestData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectAllCharData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectAllInvenData( const int iPrevDecoIndex, const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectAllGrowth( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectAllExMedalSlotData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectAllMedalItemData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectAllEventData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectAllAlchemicData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );	
	void OnLoginSelectCostumeData( const int iPrecCostumeIndex, const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectMissionData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnLoginSelectRollBookData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );

	void OnLoginSelectExtraItemData( const int iPrevExraIndex, const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIdx );

	void OnLoginSelectAccessoryData( const int iPrecAccessoryIndex, const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
//////////////////////////////////////////////////////////////////////////

public:
	void OnInsertCharData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const CHARACTER &kCharInfo );

	void OnSelectCharIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, int user_index, int iMsgResult, int iSelectCount = 1 , int iLogType = -1, int iBuyPrice = 0);
	void OnSelectCharData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, int char_index, int iMsgResult, int iLogType = -1, int iBuyPrice = 0);
	void OnUpdateCharData( const DWORD dwAgentID, const DWORD dwThreadID, ioCharacter &rkChar );
	void OnDeleteCharData( const DWORD dwAgentID, const DWORD dwThreadID, int char_index );
	void OnDeleteCharLimitDate( const DWORD dwAgentID, const DWORD dwThreadID, int user_index );
	void OnUpdateCharRegDate( const DWORD dwAgentID, const DWORD dwThreadID, int iCharIndex );
	void OnUpdateCharRentalTime( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwCharIndex, DWORD dwTime );
	void OnInsertCharRentalHistory( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwRentUserIdx, DWORD dwLoanUserIdx, int iClassType );
	void OnSelectCharRentalHistory( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, DWORD dwRentUserIdx, const ioHashString &rkRequestID, int iClassType );

public:
	void OnInsertClassExpert( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, std::vector<int>& contents );
	void OnSelectClassExpertIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnUpdateClassExpert( const DWORD dwAgentID, const DWORD dwThreadID, int user_index, cSerialize& v_FT );

public: 
	void OnInsertInvenData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, std::vector<int>& contents, bool bBuyCash, int iBuyPrice, int iLogType );
	void OnSelectInvenIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, bool bBuyCash, int iBuyPrice, int iLogType );
	void OnUpdateInvenData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, std::vector<int>& contents );

public:
	void OnInsertEtcItemData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, std::vector<int>& contents, bool bBuyCash, int iBuyPrice );
	void OnSelectEtcItemIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, bool bBuyCash = false, int iBuyPrice = 0 );
	void OnUpdateEtcItemData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, std::vector<int>& contents );

public:
	void OnInsertGrowth( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, cSerialize& v_FT );
	void OnSelectGrowthIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnUpdateGrowth( const DWORD dwAgentID, const DWORD dwThreadID, int user_index, DWORD dwClassInfoIdx, cSerialize& v_FT );

public:
	void OnInsertFishData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, cSerialize& v_FT );
	void OnSelectFishDataIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnSelectAllFishData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnUpdateFishData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIdx, DWORD dwClassInfoIdx, cSerialize& v_FT );

public: 
	void OnInsertExtraItemData( 
		const DWORD dwAgentID, 
		const DWORD dwThreadID, 
		const ioHashString &szUserGUID, 
		const ioHashString &szID, 
		DWORD dwUserIdx, 
		cSerialize& v_FT, 
		bool bBuyCash, int iBuyPrice, int iLogType, int iMachineCode, int iPeriodTime );
	void OnSelectExtraItemIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, bool bBuyCash, int iBuyPrice, int iLogType, int iMachineCode, int iPeriodTime);
	void OnUpdateExtraItemData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, cSerialize& v_FT );

public:
	void OnInsertAlchemicData( const DWORD dwAgentID, const DWORD dwThreadID,
							   const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx,
							   cSerialize& v_FT );

	void OnSelectAlchemicIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );

	void OnUpdateAlchemicData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, cSerialize& v_FT );

public:
	void OnUpdateAwardExpert( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIdx, int iAwardLevel, int iAwardExp );
	void OnInsertAwardData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, std::vector<int>& contents );
	void OnSelectAwardIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnUpdateAwardData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, std::vector<int>& contents );

		 ///////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	void OnSelectFriendList( const DWORD dwAgentID, const DWORD dwThreadID, int iFriendIDX, const DWORD dwUserIndex, const ioHashString &szPublicID, int iSelectCount );
	void OnSelectFriendRequestList( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwTableIndex, const DWORD dwUserIndex, int iSelectCount );
	void OnSelectFriendApplication( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &szFriendID );
	void OnDeleteFriendRequest( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwTableIndex );
	void OnSelectUserIDCheck( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szPublicID, const ioHashString &szUserPublicID ); 
	void OnSelectInsertFriend( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const ioHashString &szUserName, const DWORD dwTableIndex, const DWORD dwFriendIndex, const ioHashString &szFriendName );
	void OnDeleteFriend( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const ioHashString &szUserID, const ioHashString &szFriendPublicID );	
	void OnSelectFriendDeveloperInsert( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const ioHashString &szUserName );

	//////////////////////////////////////////////////////////////////////////
public:
	void OnSelectBestFriendList( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );	// 절친 목록
	void OnDeleteBestFirendTable( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwTableIndex );	//절친 테이블 삭제
	void OnUpdateBestFriendList( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwTableIndex, const DWORD dwState, const DWORD dwMagicDate ); //절친 정보 저장
	void OnInsertBestFriendAdd( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwFriendIndex );	// 절친 설정
	void OnSelectBestFriendAddResult( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwFriendIndex );	// 절친 설정 테이블 인덱스

public:
	void OnUpdateUserRecord( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, std::vector<int>& contents );

public: 
	void OnInsertQuestData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, std::vector<int>& contents );
	void OnSelectQuestIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnUpdateQuestData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, std::vector<int>& contents );

	void OnDeleteQuestData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwInvenIdex );	
	void OnInsertQuestCompleteData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, std::vector<int>& contents );
	void OnSelectQuestCompleteIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnUpdateQuestCompleteData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, std::vector<int>& contents );

	void OnInsertQuestWebAlarm( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwUserIdx, int iGradeLevel, DWORD dwQuestIndex, char *szPublicIP );
public:
	void OnInsertTrial( const DWORD dwAgentID, const DWORD dwThreadID, User *pReportUser, const ioHashString &szCriminalUserID , const ioHashString &rszChat, const ioHashString &rszChatType, const ioHashString &rszBattleRoomUserInfo, const ioHashString &rszCriminalIP , const ioHashString &rszReason, int iChatType , DWORD dwChannelIndex );

	////////////////////////////////////////////////////////////////////////////////////////////////////////
	void OnUpdateEventData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwIndex , int iValue1, int iValue2 );
	void OnInsertEventData( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex, int iValue1, int iValue2, int iEventType );
	void OnSelectEventIndex( const DWORD dwAgentID, const DWORD dwThreadID, const char *szUserGUID, DWORD dwUserIndex, int iEventType );
	void OnInsertEventLog( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, int iAddPeso );
	
public:			// GUILD
	void OnUpdateGuildEtcItemDelete( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, int iFieldNum, DWORD dwTableIndex );
	void OnSelectCreateGuild( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &szGuildName, const ioHashString &szGuildTitle,
		                      const int iGuildMark,  DWORD dwTableIndex, int iFieldNum, const int iGuildMaxEntry );
	void OnSelectCreateGuildReg( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex );
	void OnSelectCreateGuildInfo( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex );
	void OnUpdateCreateGuildFailPeso( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, int iAddPeso );
	void OnSelectUserGuildInfo( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnSelectEntryDelayGuildList( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex );
	void OnDeleteEntryDelayGuildList( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex );
	void OnDeleteGuildMemberDelete( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex );
	void OnUpdateGuildMemberEvent( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex );
	void OnSelectGuildEntryDelayMember( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex );
	void OnSelectGuildMemberList( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex, DWORD dwGuildJoinUser );
	void OnSelectGuildMemberListEx( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex, DWORD dwGuildJoinUser, bool bMemberUpdateCheck );
	void OnSelectGuildMarkBlockInfo( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szDeveloperID, DWORD dwGuildIndex, DWORD dwGuildMark );
	void OnUpdateGuildTitle( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwGuildIndex, const ioHashString &szGuildTitle );
	void OnSelectGuildNameChange( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex, const ioHashString &szGuildName, DWORD dwTableIndex, int iFieldNum );
	void OnSelectGuildEntryApp( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex, DWORD dwMasterIndex, DWORD dwSecondMasterIndex );
	void OnSelectGuildEntryAppMasterGet( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex );
	void OnDeleteGuildEntryCancel( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex );
	void OnSelectGuildEntryAgree( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwEntryUserIndex, DWORD dwGuildIndex );
	void OnDeleteGuildEntryRefuse( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwTableIndex );
	void OnSelectGuildEntryAgreeUserGuildInfo( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex );
	void OnDeleteGuildLeaveUser( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex );
	void OnUpdateGuildMasterChange( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwTargetIndex, DWORD dwGuildIndex );
	void OnUpdateGuildPositionChange( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwTargetIndex, DWORD dwGuildIndex, const ioHashString &szTargetID, const ioHashString &szPosition );
	void OnDeleteGuildKickOut( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwTableIndex, DWORD dwGuildIndex );
	void OnSelectGuildSimpleData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &szGuildUserID );
	void OnUpdateGuildMarkChange( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwGuildIndex, DWORD dwGuildMark );
	void OnInsertGuildMarkChangeKeyValue( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex, __int64 iMyMoney, int iChangeMoney, DWORD dwGuildMark, DWORD dwNewGuildMark );
	void OnSelectGuildMarkChangeKeyValue( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex );
	void OnUpdateGuildMarkChangeKeyValue( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwKeyIndex );	
	void OnSelectGuildUserLadderPointADD( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwGuildIndex, bool bPlus );

	void OnSelectCampSeasonBonus( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex );
	void OnDeleteCampSeasonBonus( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwBonusIndex, DWORD dwUserIndex, const ioHashString &rkUserNick, int iSeasonBonus );

// 거래소
public:
	void OnSelectCreateTrade( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &rkUserNick, const char *szPublicIP,
							  DWORD dwItemType, DWORD dwItemMagicCode, DWORD dwItemValue, __int64 iItemPrice,
							  DWORD dwRegisterPeriod, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom );

	void OnSelectCreateTradeIndex( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &rkUserNick, const char *szPublicIP,
								   DWORD dwItemType, DWORD dwItemMagicCode, DWORD dwItemValue, __int64 iItemPrice,
								   DWORD dwRegisterPeriod, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom  );

	void OnTradeItemComplete( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwBuyUserIndex, DWORD dwRegisterUserIndex, const ioHashString &rkUserNick,
							  DWORD dwTradeIndex, DWORD dwItemType, DWORD dwItemMagicCode, DWORD dwItemValue, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, __int64 iItemPrice );

	void OnTradeItemCancel( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwRegisterUserIndex, const ioHashString &rkUserNick,
							DWORD dwTradeIndex, DWORD dwItemType, DWORD dwItemMagicCode, DWORD dwItemValue, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom, __int64 iItemPrice );


public:         //Present
	void OnInsertPresentData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szSendName, const ioHashString &szRecvName, short iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4,
							  short iPresentMent, CTime &rkLimitTime, short iPresentState );
	void OnInsertPresentDataLog( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szSendName, const ioHashString &szRecvName, short iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4,
							  short iPresentMent, CTime &rkLimitTime, short iPresentState );
	void OnInsertPresentDataByUserIndex( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwSendUserIndex, DWORD dwRecvUserIndex, short iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4, short iPresentMent, CTime &rkLimitTime, short iPresentState );
	void OnSelectPresentData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwPresentIndex, DWORD dwSelectCount );
	void OnUpdatePresentData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwPresentIndex, short iPresentState );
	void OnAllDeletePresentData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex );
	void OnDeletePresent( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwPresentIndex );

	void OnSelectUserIndexAndPresentCnt( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, DWORD dwUserIndex, const ioHashString &rszRecvPublicID, short iPresentType, int iBuyValue1, int iBuyValue2 );

	//private Present
	void OnPresentInsertByPrivateID( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szSendName, const ioHashString &szRecvName, short iPresentType, int iPresentValue1, int iPresentValue2, int iPresentValue3, int iPresentValue4,
		short iPresentMent, CTime &rkLimitTime, short iPresentState, int iIsPublicIDState );

public:		// Subscription
	void OnInsertSubscriptionData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex,
								   const ioHashString &szSubscriptionID, int iSubscriptionGold, int iBonusCash, short iPresentType, int iPresentValue1, int iPresentValue2,
								   short iSubscriptionState, CTime &rkLimitTime );
	void OnSubseriptionDataUpdate( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwDBIndex,
								   short iPresentType, int iValue1, int iValue2,
								   const ioHashString &szSubscriptionID, int iSubscriptionGold, int iBonusCash, short iSubscriptionState, CTime &rkLimitTime );

	void OnSelectSubscriptionData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, DWORD dwDBIndex, DWORD dwSelectCount );
	void OnSubscriptionDataDelete( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwDBIndex );


public:
	void OnSelectUserEntry( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, const DWORD dwUserIndex );
	void OnSelectUserExist( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &szFindPublicID );

	void OnSelectPublicIDExist( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIdx, const ioHashString &szNewPublicID );
	void OnUpdatePublicIDAndEtcItem( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &rszPublicID, const ioHashString &rszNewPublicID, DWORD dwEtcItemIndex, int iEtcItemFieldCnt, const ioHashString &rszPublicIP );
	void OnSelectChangedPublicID( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &rszNewPublicID );

public:
	void OnSelectBlockType( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const DWORD dwUserIndex );
	void OnSelectMemberConut( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szPrivateID );
	void OnInsertMember( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szPrivateID, const ioHashString &szPublicID );

	void OnSelectFirstPublicIDExist( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szUserGUID, const ioHashString &szNewPublicID );
	void OnUpdateFirstPublicID( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &szNewPublicID );
	void OnSelectChangedFirstPublicID( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &szPublicID, const ioHashString &szUserGUID, const ioHashString &szNewPublicID );

	void OnUpdateControlKeys( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ControlKeys &rkControlKeys );

public:
	void OnInsertMedalItemData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, cSerialize &v_FT, int iLogType, int iLimitTime );
	void OnSelectMedalItemIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, int iLogType, int iLimitTime);
	void OnUpdateMedalItemData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, cSerialize& v_FT );

public:
	void OnInsertExMedalSlotData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, cSerialize& v_FT, int iLogType );
	void OnSelectExMedalSlotIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, int iLogType );
	void OnUpdateExMedalSlotData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, cSerialize& v_FT );

public:
	void OnSelectHeroData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnSelectHeroTop100Data( int iMinNumber, int iMaxNumber );

public:
	void OnSelectItemCustomUniqueIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );

public:
	void OnDBServerTest();
	void OnDBServerTestLastQuery( const DWORD dwAgentID, const DWORD dwThreadID );

public:
	void OnInsertSoldierPriceCollectedBuyTime( int iClassType, __int64 iClassTime );
	void OnInsertSoldierPriceCollectedPlayTime( int iGradeLevel, __int64 iPlayTime );

public:
	void OnSelectHeadquartersDataCount( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnInsertHeadquartersData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIdx );
	void OnSelectHeadquartersData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnUpdateHeadquartersData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const UserHeadquartersOption &rkData );

public:
	void OnSelectUserBirthDate( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szPrivateID );
	void OnSelectUserSelectShutDown( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szPrivateID );

public:
	void OnSelectFriendRecommendData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx );
	void OnUpdateFriendRecommendData( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwTableIndex );

	void OnSelectDisconnectCheck( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szPrivateID );

	// 2020-06-05 by bckim, 하드시리얼 유저 제재
	void OnSelectUserRestrictionbyHddSerial( User *pUser, const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &sHddSerialString );
	// End. 2020-06-05 by bckim, 하드시리얼 유저 제재

public:
	void OnInsertTournamentTeamCreate( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, 
									   const DWORD dwTourIndex, const ioHashString &rkTeamName, BYTE MaxPlayer, int iLadderPoint, BYTE CampPos );
	void OnSelectTournamentTeamIndex( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwTourIndex );
	void OnSelectTournamentTeamList( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const DWORD dwUserIndex, const DWORD dwTeamIndex, const int iSelectCount );
	void OnSelectTournamentCreateTeamData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwTeamIndex );
	void OnSelectTournamentTeamMember( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const DWORD dwUserIndex, const DWORD dwTeamIndex );
	void OnSelectTournamentTeamAppList( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwTourIndex, const DWORD dwTeamIndex );
	void OnInsertTournamentTeamAppAdd( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwTourIndex, const DWORD dwTeamIndex, const DWORD dwMasterIndex );
	void OnDeleteTournamentTeamAppList( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, DWORD dwTourIndex );
	void OnDeleteTournamentTeamAppDel( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwTableIndex );
	void OnUpdateTournamentTeamAppReg( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwRegUserIndex, const DWORD dwTourIndex, const DWORD dwTeamIndex, const DWORD dwTableIndex, BYTE CampPos );
	void OnSelectTournamentTeamAppAgreeMember( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwTourIndex, const DWORD dwTeamIndex, const DWORD dwAppUserIndex );
	void OnSelectTournamentAppAgreeTeam( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwTeamIndex );
	void OnDeleteTournamentTeamMember( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwTourIndex, const DWORD dwTeamIndex, const DWORD dwTableIndex, const DWORD dwMemberIndex );
	void OnUpdateUserCampPosition( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, int iCampPosition );
	void OnSelectTournamentHistoryList( DWORD dwCount, DWORD dwStartIndex );
	void OnSelectTournamentHistoryUserList( DWORD dwHistoryIndex );
	void OnSelectTournamentReward( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const DWORD dwUserIndex );
	void OnDeleteTournamentReward( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwTableIndex );	
	void OnInsertTournamentCustomAdd( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const ioHashString &szPublicID, DWORD dwUseEtcItem,
									  DWORD dwStartDate, DWORD dwEndDate, uint8 Type, uint8 State, const ioHashString &rkTourName, uint16 MaxRound, DWORD dwBannerL, DWORD dwBannerS, int iModeBattleType,
									  uint8 MaxPlayer, uint8 RoundType, DWORD dwAppDate, DWORD dwDelayDate, DWORD dwRoundDate1,  DWORD dwRoundDate2,  DWORD dwRoundDate3, DWORD dwRoundDate4, DWORD dwRoundDate5,
									  DWORD dwRoundDate6, DWORD dwRoundDate7, DWORD dwRoundDate8, DWORD dwRoundDate9, DWORD dwRoundDate10 );
	void OnSelectTournamentCustomReward( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, DWORD dwUserIdx );
	void OnDeleteTournamentCustomReward( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwTableIndex );

	void OnInsertEventMarble( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, const ioHashString &szPublicID, int iChannelingType );
	void OnSelectCloverInfoRequest( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const ioHashString &szUserGUID );
	void OnUpdateCloverInfo( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iCloverCount, const int iLastChargeTime, const short sRemainTime );

	void OnUpdateFriendCloverInfo( const DWORD dwAgentID, const DWORD dwThreadID, const int iTableIndex, const int iSendCount, const int iSendDate, const int iReceiveCount, const int iReceiveDate, const int iBReceiveCount );
	void OnUpdateFriendReceiveCloverInfo( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwFriendIndex, const int iDate, const int iSendCount );

	// Bingo
	void OnInsertBingoNumber( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byNumber[][ ioBingo::MAX ] );
	void OnInsertBingoPresent( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byPresent[ ioBingo::PRESENT_COUNT ] );
	void OnSelectBingoNumber( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	void OnSelectBingoPresent( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	void OnUpdateBingoNumber( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byNumber[][ ioBingo::MAX ] );
	void OnUpdateBingoPresent( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byPresent[ ioBingo::PRESENT_COUNT ] );

	// RelativeGrade
	void OnSelectRelativeGradeInfo( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	void OnUpdateRelativeGradeReward( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, bool bReward );
	void OnInsertRelativeGradeTable( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );

	//Tournament Cheer
	void OnInsertTournamentCheerDecision( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwTourIndex, const DWORD dwTeamIndex );
	void OnSelectTournamentCheerList( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const DWORD dwUserIndex, const int iSelectCount, const DWORD dwTableIdx );
	void OnSelectTournamentCheerReward( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const DWORD dwUserIndex, DWORD dwCheerType );
	void OnDeleteTournamentCheerReward( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwTableIdx );

	//Attendacne Record
	void OnInsertAttendanceRecord( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const CTime& tConnectTime );
	void OnSelectAttendanceRecord( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	void OnDeleteAttendanceRecord( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	
	//Pet
	void OnUpdatePetData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, cSerialize& v_FT );
	void OnSelectPetIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIdx, int iLogType );
	bool OnDeletePetData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, DWORD dwInvenIdx, cSerialize& v_FT );
	bool OnInsertPetData( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szID, cSerialize& v_FT, CQueryData &query_data );

	//Costume
	void OnUpdateCostumeData(const DWORD dwAgentID, const DWORD  dwThreadID,const DWORD dwUserIndex, const DWORD dwCostumeIdx, const DWORD dwCode, const BYTE byPeriodType, const SYSTEMTIME& sysTime, const int iClassType);
	void OnInsertCostumeData(const DWORD dwAgentID, const DWORD  dwThreadID,const DWORD dwUserIndex, const DWORD dwCode, const BYTE byPeriodType, const SYSTEMTIME& sysTime, const BYTE byInsertType, int iValue1, int iValue2, int iVlaue3 = 0);
	void OnDeleteCostumeData(const DWORD dwAgentID, const DWORD  dwThreadID,const DWORD dwUserIndex, const DWORD dwCostumeIdx);

	//Accessory
	void OnUpdateAccessoryData(const DWORD dwAgentID, const DWORD  dwThreadID,const DWORD dwUserIndex, const DWORD dwAccessoryIdx, const DWORD dwCode, const BYTE byPeriodType, const SYSTEMTIME& sysTime, const int iClassType, const int iAccessoryValue, const int iComposeCode, const int iComposeValue);
	void OnDeleteAccessoryData(const DWORD dwAgentID, const DWORD  dwThreadID,const DWORD dwUserIndex, const DWORD dwAccessoryIdx);
	void OnInsertAccessoryData(const DWORD dwAgentID, const DWORD  dwThreadID,const DWORD dwUserIndex, const DWORD dwCode, const BYTE byPeriodType, const SYSTEMTIME& sysTime, const BYTE byInsertType, int iValue1, int iValue2, int iValue3 = 0);
	
	//Mission
	void OnUpdateMissionData(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, BYTE byMissionType, const DWORD dwMissionCode, const int MissionValue, const BYTE byState);
	void OnInitMissionData(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, BYTE byMissionType);

	//Roll book
	void OnUpdateRollBookData(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iTableIndex, const int iAttendCount);

	//Guild Reward
	void OnSelectGuildAttendanceInfo(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwGuildIndex, const DWORD dwUserIndex, SYSTEMTIME& sSelectDate, int iSelectType);
	void OnInsertUserGuildAttendanceInfo(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwGuildIndex, const DWORD dwUserIndex);
	void OnUpdateGuildAttendanceRewardDate(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex);
	void OnUpdateGuildRankRewardDate(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex);

	//popupstore
	void OnSelectSpentCash( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIndex );
	void OnUpdateSpentCash( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, int iCash );
	void OnSelectPopupIndex( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szUserGUID, const ioHashString &szID, DWORD dwUserIndex );
	void OnInsertPopupIndex( const DWORD dwAgentID, const DWORD dwThreadID, DWORD dwUserIndex, int iPopupIndex );

	//채널링 고유 키값
	void OnSelectUserChannelingKeyValue( const DWORD dwAgentID, const DWORD dwThreadID, const ioHashString &szPrivateID, const ioHashString &szUserGUID, const int iChannelingType );

	//길드 본부
	void OnSelectGuildBlocksInfos(const DWORD dwUserIndex, const DWORD dwGuildIndex, const DWORD dwRoomIndex, int iPage	= 1);
	void OnRetrieveOrDeleteBlock(const DWORD dwGuildIndex, const DWORD dwRoomIndex, const __int64 dwItemIndex, const BYTE byState, const ioHashString &szPublicID, const DWORD dwUserIndex, const DWORD dwItemCode);
	void OnConstructOrMoveBlock(const DWORD dwGuildIndex, const DWORD dwRoomIndex, const ioHashString &szPublicID, const __int64 dwItemIndex, const DWORD dwItemCode, const BYTE byState, const int iXZ, const int iY, const BYTE byDirection, const DWORD dwUserIndex );
	void OnSelectGuildInvenVersion(const DWORD dwUserIndex, const DWORD dwGuildIndex, const int iRequestType);
	void OnSelectGuildInvenInfo(const DWORD dwUserIndex, const DWORD dwGuildIndex, const int iRequestType, const __int64 iRequestVer);
	void OnAddGuildBlockItem(const DWORD dwUserIndex, const DWORD dwGuildIndex, const DWORD dwItemCode, const int iAddType);
	void OnDefaultConstructGuildBlock(const DWORD dwGuildIndex, const DWORD dwItemCode, const int iXZ, const int iY, BYTE byDirection);

	//개인 본부
	void OnPersonalHQConstructBlock(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwRoomIndex, const ioHashString &szPublicID, const __int64 iItemIndex, const DWORD dwItemCode, const int iXZ, const int iY, const BYTE byDirection, const DWORD dwUserIndex );
	void OnPersonalHQRetrieveBlock(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwRoomIndex, const __int64 dwItemIndex, const BYTE byState, const ioHashString &szPublicID, const DWORD dwUserIndex, const DWORD dwItemCode);
	void IsExistPersonalHQInfo(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, int iType, int iValue1, int iValue2);
	void OnAddPersonalHQBlockItem(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwItemCode, const int iCount, BOOL bEnd);
	void OnSelectPersonalInvenInfo(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex);
	void OnSelectPersonalBlocksInfo(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwRoomIndex, const DWORD dwUserIndex,  int iPage	= 1);
	void OnDefaultConstructPersonalBlock(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwItemCode, const int iXZ, const int iY, BYTE byDirection);

	//Time Cash
	void OnSelectTimeCashTable(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex);
	void OnInsertTimeCashTable(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const ioHashString& szGUID, const DWORD dwCode, const DWORD dwEndDate);
	void OnUpdateTimeCashTable(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const ioHashString& szGUID, const DWORD dwCode);

	//칭호
	void OnSelectTitlesInfo(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex);
	void OnUpdateTitleStatus(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex);
	void OnInsertOrUpdateTitleInfo(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwCode, const __int64 iValue, const int iLevel, const BYTE byPremium, 
								const BYTE byEquip, const BYTE byStatus, const BYTE byActionType);

	//Pirate Roulette
	  // Pirate Roulette Reset / Start
	void OnInsertPirateRouletteNumber( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	  // Pirate Roulette Reward Received Info.
	void OnInsertPirateRoulettePresent( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	void OnSelectPirateRouletteNumber( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	void OnSelectPirateRoulettePresent( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	void OnUpdatePirateRouletteNumber( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwHP, const BYTE byNumber[ioPirateRoulette::ROULETTE_BOARD_MAX] );
	void OnUpdatePirateRoulettePresent( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byPresent[ioPirateRoulette::ROULETTE_PRESENT_MAX] );

	//보너스 캐쉬
	void OnInsertBonusCash(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iAmount, CTime& cEndDate);
	void OnUpdateBonusCash(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwCashIndex, const int iAmount, BonusCashUpdateType eType, int iBuyItemType, int iBuyValue1, int iBuyValue2, const ioHashString& szGUID);
	void OnSelectBonusCash(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwSelectType);
	void OnDeleteExpirationBonusCash(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex);
	void OnSelectExpiredBonusCash(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex);

	//유저코인
	void OnLoginSelectUserCoin( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	void OnInsertUserCoin( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byCoinType );
	void OnUpdateUserCoin( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byCoinType );
	
	// 오크통
	// 로그인 시 받아오는 오크통 데이터
	void OnSelectOakBarrelData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	// 오크통 데이터 업데이트
	void OnUpdateOakBarrelData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byUpdateType, const BYTE byOakStep, const BYTE byHole[OAK_BARREL_HOLE], const int iLimitSword );
	// 오크통 칼 일일 사용 한도 수량 초기화 시각 업데이트
	void OnUpdateOakBarrelTimeData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	// 오크통 칼 일일 사용 한도 수량 초기화 업데이트
	void OnUpdateOakBarrelLimitSwordInit( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iLmitSwordMax );

	// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
	void OnSelectCardMatchingData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );// 로그인 시 받아오는 카드매칭 데이터		
	void OnUpdateCardMatchingData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byUpdateType, const BYTE byCardMissionType , const BYTE byCardMissionMark1, const BYTE byCardMissionMark2, const BYTE byCardMark1, const BYTE byCardMark2); // 카드매칭 데이터 업데이트 	
	// End. 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
	
	// 2018-08-30 by bckim, 주사위 이벤트 추가  	
	void DBClient::OnSelectDiceGameData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	//void DBClient::OnUpdateDiceGameData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const BYTE byOakStep, const BYTE byHole[OAK_BARREL_HOLE], const int iLimitSword );
	void DBClient::OnUpdateDiceGameData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iPosition,const int iTraceInfo[DICE_GAME_TRACE_DB],const BYTE byBoradIndex, const int iRewardIndex[DICE_GAME_REWARD_DB] );

	// End. 2018-08-30 by bckim, 주사위 이벤트 추가

	// 랭킹전
	void OnSelectUserMatch( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	void OnUpdateUserMatch( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iPlayCount, const int iMMR, const int iWinCnt, const int iMaxWinCnt, const int iMaxLoseCnt, const int iRankMMR );

	// 조각개편
	void OnSelectUserSpiritData( const DWORD dwAgentID, const DWORD dwThreadID, const int TableIndex, const DWORD dwUserIndex, const int iCount );
	void OnInsertUserSpiritData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iSpiritCode, const int iQuantity );
	void OnUpdateUserSpiritData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iSpiritCode, const int iQuantity );

	// 커스텀 메달
	void OnSelectCustomMedal( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	void OnDeleteCustomMedal( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD iItemIndex );
	void OnInsertCustomMedal(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iItemCode, const int iGrowth1
		, const int iGrowth2, const int iGrowth3, const int iGrowth4, const int iGrowth5, const int iGrowth6, const int iGrowth7, const int iGrowth8,
		const int iClass, const int iLimitType,SYSTEMTIME sysTime, int iPresentIndex, int iSlotIndex );

	void OnUpdateCustomMedalState( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const int iItemCode, const int iItemIndex
		, const int iClass, const bool bState );

	// 타임게이트
	void OnSelectTimeGate( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	void OnUpdateTimeGate( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );

	// 수련장
	void OnLoginSelectPracticeData( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	void OnPracticeRankList( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex );
	bool OnUpdatePracticeAdmission( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex
		, const DWORD dwPracticeID, const DWORD dwPracticeGrade, const DWORD dwPracticeCount, const DWORD dwPracticeTime, const DWORD dwPracticeRank, int iMoney );
	bool OnUpdatePracticeTime( const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex
		, const DWORD dwPracticeID, const DWORD dwPracticeGrade, const DWORD dwCurrentGrade, const DWORD dwPracticeCount, const DWORD dwPracticeTime, const DWORD dwPracticeRank );
	void OnInitPracticeData(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwPracticeID, const DWORD dwPracticeCount);

	void OnPCRoomPlaytime_Add(const DWORD dwAgentID, const DWORD dwThreadID, const DWORD dwUserIndex, const DWORD dwPCBangNumber);

private:			/* Singleton Class */
    DBClient();
    virtual ~DBClient();
};

#define g_DBClient DBClient::GetInstance()
#endif // !defined(AFX_DBCLIENT_H__8BBCE6AF_B8A6_4D7C_A9FA_B2B4B32491CA__INCLUDED_)
