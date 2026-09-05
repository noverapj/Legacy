#pragma once

#include "ioEtcItem.h"
#include "ioUserEtcItem.h"

#include "..\Util\IORandom.h"

typedef struct tagPrizeData
{
	int	m_iPresentType;
	int	m_iPresentMent;
	int	m_iPresentPeriod;

	int m_iPresentValue1;
	int m_iPresentValue2;
	int m_iPresentValue3;
	int m_iPresentValue4;

	tagPrizeData()
	{
		Init();
	}

	void Init()
	{
		m_iPresentType = 0;		// 선물 타입
		m_iPresentMent = 0;		// 선물 받았을때 맨트
		m_iPresentPeriod = 0;	// 선물 기간

		m_iPresentValue1 = 0;
		m_iPresentValue2 = 0;
		m_iPresentValue3 = 0;
		m_iPresentValue4 = 0;
	}

} PrizeData;

typedef std::vector< PrizeData > PrizeDataVec;

class User;
class ioEtcItem
{
public:
	enum 
	{
		// AABBBBBB // AA: 1:횟수용, 2:시간용, 3:즉시사용, 4:영구사용, 5:날자용 // BBBBBB: 종류 1~99999 : 권한 , 100000 ~ 199999 : 클래스별 권한 아이템
		// BBBBBB ( 십만 자리는 겹치면 안됨 : 1000001(기존) , 1000002(추가정상) , 2000001(추가불가) )
		EIT_NONE                   = 0,

		// 권한 아이템 
		EIT_ETC_WHOLE_CHAT          = 1000001,  
		EIT_ETC_FRIEND_SLOT_EXTEND  = 4000002,
		EIT_ETC_PESO_EXP_BONUS      = 2000003,
		EIT_ETC_CHANGE_ID           = 3000004,
		EIT_ETC_CUSTOM_SOUND        = 1000005,
		EIT_ETC_BUY_MORTMAIN_CHAR   = 1000006,
		EIT_ETC_GUILD_CREATE        = 3000007,
		EIT_ETC_GUILD_MARK_CHANGE   = 3000008,
		EIT_ETC_GROWTH_DOWN		    = 1000009,
		EIT_ETC_CHAR_SLOT_EXTEND    = 4000010,
		EIT_ETC_PESO_BONUS          = 5000011,
		EIT_ETC_EXP_BONUS           = 5000012,
		EIT_ETC_FISHING_BAIT	    = 1000013,
		EIT_ETC_FISHING_ROD		    = 5000014,
		EIT_ETC_FISHING_SLOT_EXTEND = 4000015,
		EIT_ETC_FISHING_MOON_BAIT   = 1000016,
		EIT_ETC_FISHING_MOON_ROD    = 5000017,
		EIT_ETC_GUILD_NAME_CHANGE   = 3000018,
		EIT_ETC_SOLDIER_PACKAGE     = 3000019,
		EIT_ETC_DECORATION_PACKAGE  = 3000020,
		EIT_ETC_GOLDMONSTER_COIN 	= 1000021,
		EIT_ETC_GASHAPON            = 1000022,
		EIT_ETC_ITEM_COMPOUND	    = 1000023,
		EIT_ETC_FISHING_PACKAGE     = 3000024,
		EIT_ETC_PESO_EXP_PACKAGE    = 3000025,
		EIT_ETC_MONSTER_COIN 	    = 1000026,
		EIT_ETC_DECO_UNDERWEAR_PACKAGE = 3000027, 
		EIT_ETC_RANDOM_DECO_M	 	= 1000028,		// 치장 랜덤(남)
		EIT_ETC_RANDOM_DECO_W	 	= 1000029,		// 치장 랜덤(여)
		EIT_ETC_MULTIPLE_COMPOUND   = 1000030,
		EIT_ETC_MULTIPLE_EQUAL_COMPOUND = 1000031,
		EIT_ETC_EXCAVATING_KIT      = 2000032,
		EIT_ETC_GOLD_EXCAVATING_KIT = 2000033,
		EIT_ETC_NEW_EXCAVATING_KIT =	4000001,			// 2018-01-15 by bckim, 탐사 확장	
		EIT_ETC_NEW_EXCAVATING_SHOVEL	=	1002831,		// 2018-01-15 by bckim, 탐사 확장	
		EIT_ETC_GASHAPON2           = 1000034,
		EIT_ETC_GASHAPON3           = 1000035,
		EIT_ETC_GASHAPON4           = 1000036,
		EIT_ETC_GASHAPON5           = 1000037,
		EIT_ETC_GASHAPON6           = 1000038,
		EIT_ETC_GASHAPON7           = 1000039,
		EIT_ETC_GASHAPON8           = 1000040,
		EIT_ETC_GASHAPON9           = 1000041,
		EIT_ETC_GASHAPON10          = 1000042,
		EIT_ETC_GASHAPON11          = 1000043,
		EIT_ETC_GASHAPON12          = 1000044,
		EIT_ETC_GASHAPON13          = 1000045,		
		EIT_ETC_TRADE_STATE_CHANGE  = 1000046,
		EIT_ETC_SPECIAL_PACKAGE1    = 3000047,
		EIT_ETC_QUEST_EVENT         = 1000048,		
		EIT_ETC_SILVER_COIN 	    = 1000049,  
		EIT_ETC_MILEAGE 			= 1000050,
		EIT_ETC_BATTLE_RECORD_INIT  = 3000050,
		EIT_ETC_LADDER_RECORD_INIT  = 3000051,
		EIT_ETC_HERO_RECORD_INIT    = 3000052,
		EIT_ETC_GASHAPON14          = 1000053, // 북미 트레저 캡슐 	
		EIT_ETC_GASHAPON15          = 1000054,
		EIT_ETC_GASHAPON16          = 1000055,
		EIT_ETC_GASHAPON17          = 1000056,
		EIT_ETC_GASHAPON18          = 1000057,
		EIT_ETC_GASHAPON19          = 1000058,
		EIT_ETC_GASHAPON20          = 1000059,
		EIT_ETC_GASHAPON21          = 1000060,
		EIT_ETC_GASHAPON22          = 1000061,
		EIT_ETC_GASHAPON23          = 1000062,
		EIT_ETC_GASHAPON24          = 1000063,
		EIT_ETC_GASHAPON25          = 1000064,
		EIT_ETC_GASHAPON26          = 1000065,
		EIT_ETC_GASHAPON27          = 1000066,
		EIT_ETC_GASHAPON28          = 1000067,
		EIT_ETC_GASHAPON29          = 1000068,
		EIT_ETC_GASHAPON30          = 1000069,
		EIT_ETC_GASHAPON31          = 1000070,
		EIT_ETC_GASHAPON32          = 1000071,
		EIT_ETC_GASHAPON33          = 1000072,
		EIT_ETC_GASHAPON34          = 1000073,
		EIT_ETC_GASHAPON35          = 1000074,
		EIT_ETC_GASHAPON36          = 1000075,
		EIT_ETC_GASHAPON37          = 1000076,
		EIT_ETC_GASHAPON38          = 1000077,
		EIT_ETC_GASHAPON39          = 1000078,
		EIT_ETC_GASHAPON40          = 1000079,
		EIT_ETC_GASHAPON41          = 1000080,
		EIT_ETC_GASHAPON42          = 1000081,
		EIT_ETC_GASHAPON43          = 1000082,
		EIT_ETC_GASHAPON44          = 1000083,
		EIT_ETC_GASHAPON45          = 1000084,
		EIT_ETC_GASHAPON46          = 1000085,
		EIT_ETC_GASHAPON47          = 1000086,
		EIT_ETC_GASHAPON48          = 1000087,
		EIT_ETC_GASHAPON49          = 1000088,
		EIT_ETC_GASHAPON50          = 1000089,
		EIT_ETC_GASHAPON51          = 1000090,
		EIT_ETC_GASHAPON52          = 1000091,
		EIT_ETC_GASHAPON53          = 1000092,	
		EIT_ETC_SKELETON_BIG        = 2000093,	
		EIT_ETC_SKELETON_BIGHEAD    = 2000094,
		EIT_ETC_SKELETON_SMALL      = 2000095,
		EIT_ETC_ITEM_COMPOUND2	    = 1000096,
		EIT_ETC_ITEM_COMPOUND3	    = 1000097,
		EIT_ETC_MULTIPLE_COMPOUND2	= 1000098,
		EIT_ETC_MULTIPLE_COMPOUND3	= 1000099,
		EIT_ETC_MULTIPLE_EQUAL_COMPOUND2 = 1000100,
		EIT_ETC_MULTIPLE_EQUAL_COMPOUND3 = 1000101,
		EIT_ETC_MULTIPLE_COMPOUND4	= 1000423,
		EIT_ETC_MULTIPLE_COMPOUND5	= 1000424,
		EIT_ETC_MOTION1             = 4000102,
		EIT_ETC_MOTION2             = 4000103,
		EIT_ETC_MOTION3             = 4000104,
		EIT_ETC_MOTION4             = 4000105,
		EIT_ETC_MOTION5             = 4000106,
		EIT_ETC_MOTION6             = 4000107,
		EIT_ETC_MOTION7             = 4000108,
		EIT_ETC_MOTION8             = 4000109,
		EIT_ETC_MOTION9             = 4000110,
		EIT_ETC_MOTION10            = 4000111,
		EIT_ETC_MOTION11            = 4000112,
		EIT_ETC_MOTION12            = 4000113,
		EIT_ETC_MOTION13            = 4000114,
		EIT_ETC_MOTION14            = 4000115,
		EIT_ETC_MOTION15            = 4000116,
		EIT_ETC_MOTION16            = 4000117,
		EIT_ETC_MOTION17            = 4000118,
		EIT_ETC_MOTION18            = 4000119,
		EIT_ETC_MOTION19            = 4000120,
		EIT_ETC_MOTION20            = 4000121,
		EIT_ETC_MOTION21            = 4000122,
		EIT_ETC_MOTION22            = 4000123,
		EIT_ETC_MOTION23            = 4000124,
		EIT_ETC_MOTION24            = 4000125,
		EIT_ETC_MOTION25            = 4000126,
		EIT_ETC_MOTION26            = 4000127,
		EIT_ETC_MOTION27            = 4000128,
		EIT_ETC_MOTION28            = 4000129,
		EIT_ETC_MOTION29            = 4000130,
		EIT_ETC_MOTION30            = 4000131,
		EIT_ETC_MOTION31            = 4000132,
		EIT_ETC_MOTION32            = 4000133,
		EIT_ETC_MOTION33            = 4000134,
		EIT_ETC_MOTION34            = 4000135,
		EIT_ETC_MOTION35            = 4000136,
		EIT_ETC_MOTION36            = 4000137,
		EIT_ETC_MOTION37            = 4000138,
		EIT_ETC_MOTION38            = 4000139,
		EIT_ETC_MOTION39            = 4000140,
		EIT_ETC_MOTION40            = 4000141,
		EIT_ETC_MOTION41            = 4000142,
		EIT_ETC_MOTION42            = 4000143,
		EIT_ETC_MOTION43            = 4000144,
		EIT_ETC_MOTION44            = 4000145,
		EIT_ETC_MOTION45            = 4000146,
		EIT_ETC_MOTION46            = 4000147,
		EIT_ETC_MOTION47            = 4000148,
		EIT_ETC_MOTION48            = 4000149,
		EIT_ETC_MOTION49            = 4000150,
		EIT_ETC_MOTION50            = 4000151,
		EIT_ETC_MOTION51            = 4000152,
		EIT_ETC_MOTION52            = 4000153,
		EIT_ETC_MOTION53            = 4000154,
		EIT_ETC_MOTION54            = 4000155,
		EIT_ETC_MOTION55            = 4000156,
		EIT_ETC_MOTION56            = 4000157,
		EIT_ETC_MOTION57            = 4000158,
		EIT_ETC_MOTION58            = 4000159,
		EIT_ETC_MOTION59            = 4000160,
		EIT_ETC_MOTION60            = 4000161,
		EIT_ETC_MOTION61            = 4000162,
		EIT_ETC_MOTION62            = 4000163,
		EIT_ETC_MOTION63            = 4000164,
		EIT_ETC_MOTION64            = 4000165,
		EIT_ETC_MOTION65            = 4000166,
		EIT_ETC_MOTION66            = 4000167,
		EIT_ETC_MOTION67            = 4000168,
		EIT_ETC_MOTION68            = 4000169,
		EIT_ETC_MOTION69            = 4000170,
		EIT_ETC_MOTION70            = 4000171,
		EIT_ETC_MOTION71            = 4000172,
		EIT_ETC_MOTION72            = 4000173,
		EIT_ETC_MOTION73            = 4000174,
		EIT_ETC_MOTION74            = 4000175,
		EIT_ETC_MOTION75            = 4000176,
		EIT_ETC_MOTION76            = 4000177,
		EIT_ETC_MOTION77            = 4000178,
		EIT_ETC_MOTION78            = 4000179,
		EIT_ETC_MOTION79            = 4000180,
		EIT_ETC_MOTION80            = 4000181,
		EIT_ETC_MOTION81            = 4000182,
		EIT_ETC_MOTION82            = 4000183,
		EIT_ETC_MOTION83            = 4000184,
		EIT_ETC_MOTION84            = 4000185,
		EIT_ETC_MOTION85            = 4000186,
		EIT_ETC_MOTION86            = 4000187,
		EIT_ETC_MOTION87            = 4000188,
		EIT_ETC_MOTION88            = 4000189,
		EIT_ETC_MOTION89            = 4000190,
		EIT_ETC_MOTION90            = 4000191,
		EIT_ETC_MOTION91            = 4000192,
		EIT_ETC_MOTION92            = 4000193,
		EIT_ETC_MOTION93            = 4000194,
		EIT_ETC_MOTION94            = 4000195,
		EIT_ETC_MOTION95            = 4000196,
		EIT_ETC_MOTION96            = 4000197,
		EIT_ETC_MOTION97            = 4000198,
		EIT_ETC_MOTION98            = 4000199,
		EIT_ETC_MOTION99            = 4000200,
		EIT_ETC_MOTION100           = 4000201,
		EIT_ETC_CUSTOM_ITEM_SKIN    = 1000202,
		EIT_ETC_COSTUM_ITEM_SKIN    = 1000203,
		EIT_ETC_PACKAGE1            = 3000203,
		EIT_ETC_PACKAGE2            = 3000204,
		EIT_ETC_PACKAGE3            = 3000205,
		EIT_ETC_PACKAGE4            = 3000206,
		EIT_ETC_PACKAGE5            = 3000207,
		EIT_ETC_PACKAGE6            = 3000208,
		EIT_ETC_PACKAGE7            = 3000209,
		EIT_ETC_PACKAGE8            = 3000210,
		EIT_ETC_PACKAGE9            = 3000211,
		EIT_ETC_PACKAGE10           = 3000212,
		EIT_ETC_PACKAGE11           = 3000213,
		EIT_ETC_PACKAGE12           = 3000214,
		EIT_ETC_PACKAGE13           = 3000215,
		EIT_ETC_PACKAGE14           = 3000216,
		EIT_ETC_PACKAGE15           = 3000217,
		EIT_ETC_PACKAGE16           = 3000218,
		EIT_ETC_PACKAGE17           = 3000219,
		EIT_ETC_PACKAGE18           = 3000220,
		EIT_ETC_PACKAGE19           = 3000221,
		EIT_ETC_PACKAGE20           = 3000222,
		EIT_ETC_PACKAGE21           = 3000223,
		EIT_ETC_PACKAGE22           = 3000224,
		EIT_ETC_PACKAGE23           = 3000225,
		EIT_ETC_PACKAGE24           = 3000226,
		EIT_ETC_PACKAGE25           = 3000227,
		EIT_ETC_PACKAGE26           = 3000228,
		EIT_ETC_PACKAGE27           = 3000229,
		EIT_ETC_PACKAGE28           = 3000230,
		EIT_ETC_PACKAGE29           = 3000231,
		EIT_ETC_PACKAGE30           = 3000232,
		EIT_ETC_PACKAGE31           = 3000233,
		EIT_ETC_PACKAGE32           = 3000234,
		EIT_ETC_PACKAGE33           = 3000235,
		EIT_ETC_PACKAGE34           = 3000236,
		EIT_ETC_PACKAGE35           = 3000237,
		EIT_ETC_PACKAGE36           = 3000238,
		EIT_ETC_PACKAGE37           = 3000239,
		EIT_ETC_PACKAGE38           = 3000240,
		EIT_ETC_PACKAGE39           = 3000241,
		EIT_ETC_PACKAGE40           = 3000242,
		EIT_ETC_PACKAGE41           = 3000243,
		EIT_ETC_PACKAGE42           = 3000244,
		EIT_ETC_PACKAGE43           = 3000245,
		EIT_ETC_PACKAGE44           = 3000246,
		EIT_ETC_PACKAGE45           = 3000247,
		EIT_ETC_PACKAGE46           = 3000248,
		EIT_ETC_PACKAGE47           = 3000249,
		EIT_ETC_PACKAGE48           = 3000250,
		EIT_ETC_PACKAGE49           = 3000251,
		EIT_ETC_PACKAGE50           = 3000252,
		EIT_ETC_PACKAGE51           = 3000253,
		EIT_ETC_PACKAGE52           = 3000254,
		EIT_ETC_PACKAGE53           = 3000255,
		EIT_ETC_PACKAGE54           = 3000256,
		EIT_ETC_PACKAGE55           = 3000257,
		EIT_ETC_PACKAGE56           = 3000258,
		EIT_ETC_PACKAGE57           = 3000259,
		EIT_ETC_PACKAGE58           = 3000260,
		EIT_ETC_PACKAGE59           = 3000261,
		EIT_ETC_PACKAGE60           = 3000262,
		EIT_ETC_PACKAGE61           = 3000263,
		EIT_ETC_PACKAGE62           = 3000264,
		EIT_ETC_PACKAGE63           = 3000265,
		EIT_ETC_PACKAGE64           = 3000266,
		EIT_ETC_PACKAGE65           = 3000267,
		EIT_ETC_PACKAGE66           = 3000268,
		EIT_ETC_PACKAGE67           = 3000269,
		EIT_ETC_PACKAGE68           = 3000270,
		EIT_ETC_PACKAGE69           = 3000271,
		EIT_ETC_PACKAGE70           = 3000272,
		EIT_ETC_PACKAGE71           = 3000273,
		EIT_ETC_PACKAGE72           = 3000274,
		EIT_ETC_PACKAGE73           = 3000275,
		EIT_ETC_PACKAGE74           = 3000276,
		EIT_ETC_PACKAGE75           = 3000277,
		EIT_ETC_PACKAGE76           = 3000278,
		EIT_ETC_PACKAGE77           = 3000279,
		EIT_ETC_PACKAGE78           = 3000280,
		EIT_ETC_PACKAGE79           = 3000281,
		EIT_ETC_PACKAGE80           = 3000282,
		EIT_ETC_PACKAGE81           = 3000283,
		EIT_ETC_PACKAGE82           = 3000284,
		EIT_ETC_PACKAGE83           = 3000285,
		EIT_ETC_PACKAGE84           = 3000286,
		EIT_ETC_PACKAGE85           = 3000287,
		EIT_ETC_PACKAGE86           = 3000288,
		EIT_ETC_PACKAGE87           = 3000289,
		EIT_ETC_PACKAGE88           = 3000290,
		EIT_ETC_PACKAGE89           = 3000291,
		EIT_ETC_PACKAGE90           = 3000292,
		EIT_ETC_PACKAGE91           = 3000293,
		EIT_ETC_PACKAGE92           = 3000294,
		EIT_ETC_PACKAGE93           = 3000295,
		EIT_ETC_PACKAGE94           = 3000296,
		EIT_ETC_PACKAGE95           = 3000297,
		EIT_ETC_PACKAGE96           = 3000298,
		EIT_ETC_PACKAGE97           = 3000299,
		EIT_ETC_PACKAGE98           = 3000300,
		EIT_ETC_PACKAGE99           = 3000301,
		EIT_ETC_PACKAGE100          = 3000302,
		EIT_ETC_CUSTOM_ITEM_SKIN_TEST = 4000303,
		EIT_ETC_BLOCK1			    = 5000304,
		EIT_ETC_BLOCK2			    = 5000305,
		EIT_ETC_BLOCK3			    = 5000306,
		EIT_ETC_BLOCK4			    = 5000307,
		EIT_ETC_BLOCK5			    = 5000308,
		EIT_ETC_BLOCK6			    = 5000309,
		EIT_ETC_BLOCK7			    = 5000310,
		EIT_ETC_BLOCK8			    = 5000311,
		EIT_ETC_BLOCK9			    = 5000312,
		EIT_ETC_BLOCK10			    = 5000313,
		EIT_ETC_PREMIUM_SOLDIER_PACKAGE= 3000314,
		EIT_ETC_EVENT_CHECK1        = 1000315,
		EIT_ETC_EVENT_CHECK2        = 1000316,
		EIT_ETC_EVENT_CHECK3        = 1000317,
		EIT_ETC_EVENT_CHECK4        = 1000318,
		EIT_ETC_EVENT_CHECK5        = 1000319,
		EIT_ETC_EVENT_CHECK6        = 1000320,
		EIT_ETC_EVENT_CHECK7        = 1000321,
		EIT_ETC_EVENT_CHECK8        = 1000322,
		EIT_ETC_EVENT_CHECK9        = 1000323,
		EIT_ETC_EVENT_CHECK10       = 1000324,
		EIT_ETC_EVENT_CHECK11       = 1000325,
		EIT_ETC_EVENT_CHECK12       = 1000326,
		EIT_ETC_EVENT_CHECK13       = 1000327,
		EIT_ETC_EVENT_CHECK14       = 1000328,
		EIT_ETC_EVENT_CHECK15       = 1000329,
		EIT_ETC_EVENT_CHECK16       = 1000330,
		EIT_ETC_EVENT_CHECK17       = 1000331,
		EIT_ETC_EVENT_CHECK18       = 1000332,
		EIT_ETC_EVENT_CHECK19       = 1000333,
		EIT_ETC_EVENT_CHECK20       = 1000334,
		EIT_ETC_EVENT_CHECK21       = 1000335,
		EIT_ETC_EVENT_CHECK22       = 1000336,
		EIT_ETC_EVENT_CHECK23       = 1000337,
		EIT_ETC_EVENT_CHECK24       = 1000338,
		EIT_ETC_EVENT_CHECK25       = 1000339,
		EIT_ETC_EVENT_CHECK26       = 1000340,
		EIT_ETC_EVENT_CHECK27       = 1000341,
		EIT_ETC_EVENT_CHECK28       = 1000342,
		EIT_ETC_EVENT_CHECK29       = 1000343,
		EIT_ETC_EVENT_CHECK30       = 1000344,
		EIT_ETC_EVENT_CHECK31       = 1000345,
		EIT_ETC_EVENT_CHECK32       = 1000346,
		EIT_ETC_EVENT_CHECK33       = 1000347,
		EIT_ETC_EVENT_CHECK34       = 1000348,
		EIT_ETC_EVENT_CHECK35       = 1000349,
		EIT_ETC_EVENT_CHECK36       = 1000350,
		EIT_ETC_EVENT_CHECK37       = 1000351,
		EIT_ETC_EVENT_CHECK38       = 1000352,
		EIT_ETC_EVENT_CHECK39       = 1000353,
		EIT_ETC_EVENT_CHECK40       = 1000354,
		EIT_ETC_EVENT_CHECK41       = 1000355,
		EIT_ETC_EVENT_CHECK42       = 1000356,
		EIT_ETC_EVENT_CHECK43       = 1000357,
		EIT_ETC_EVENT_CHECK44       = 1000358,
		EIT_ETC_EVENT_CHECK45       = 1000359,
		EIT_ETC_EVENT_CHECK46       = 1000360,
		EIT_ETC_EVENT_CHECK47       = 1000361,
		EIT_ETC_EVENT_CHECK48       = 1000362,
		EIT_ETC_EVENT_CHECK49       = 1000363,
		EIT_ETC_EVENT_CHECK50       = 1000364,
		EIT_ETC_EVENT_CHECK51       = 1000365,
		EIT_ETC_EVENT_CHECK52       = 1000366,
		EIT_ETC_EVENT_CHECK53       = 1000367,
		EIT_ETC_EVENT_CHECK54       = 1000368,
		EIT_ETC_EVENT_CHECK55       = 1000369,
		EIT_ETC_EVENT_CHECK56       = 1000370,
		EIT_ETC_EVENT_CHECK57       = 1000371,
		EIT_ETC_EVENT_CHECK58       = 1000372,
		EIT_ETC_EVENT_CHECK59       = 1000373,
		EIT_ETC_EVENT_CHECK60       = 1000374,
		EIT_ETC_EVENT_CHECK61       = 1000375,
		EIT_ETC_EVENT_CHECK62       = 1000376,
		EIT_ETC_EVENT_CHECK63       = 1000377,
		EIT_ETC_EVENT_CHECK64       = 1000378,
		EIT_ETC_EVENT_CHECK65       = 1000379,
		EIT_ETC_EVENT_CHECK66       = 1000380,
		EIT_ETC_EVENT_CHECK67       = 1000381,
		EIT_ETC_EVENT_CHECK68       = 1000382,
		EIT_ETC_EVENT_CHECK69       = 1000383,
		EIT_ETC_EVENT_CHECK70       = 1000384,
		EIT_ETC_EVENT_CHECK71       = 1000385,
		EIT_ETC_EVENT_CHECK72       = 1000386,
		EIT_ETC_EVENT_CHECK73       = 1000387,
		EIT_ETC_EVENT_CHECK74       = 1000388,
		EIT_ETC_EVENT_CHECK75       = 1000389,
		EIT_ETC_EVENT_CHECK76       = 1000390,
		EIT_ETC_EVENT_CHECK77       = 1000391,
		EIT_ETC_EVENT_CHECK78       = 1000392,
		EIT_ETC_EVENT_CHECK79       = 1000393,
		EIT_ETC_EVENT_CHECK80       = 1000394,
		EIT_ETC_EVENT_CHECK81       = 1000395,
		EIT_ETC_EVENT_CHECK82       = 1000396,
		EIT_ETC_EVENT_CHECK83       = 1000397,
		EIT_ETC_EVENT_CHECK84       = 1000398,
		EIT_ETC_EVENT_CHECK85       = 1000399,
		EIT_ETC_EVENT_CHECK86       = 1000400,
		EIT_ETC_EVENT_CHECK87       = 1000401,
		EIT_ETC_EVENT_CHECK88       = 1000402,
		EIT_ETC_EVENT_CHECK89       = 1000403,
		EIT_ETC_EVENT_CHECK90       = 1000404,
		EIT_ETC_EVENT_CHECK91       = 1000405,
		EIT_ETC_EVENT_CHECK92       = 1000406,
		EIT_ETC_EVENT_CHECK93       = 1000407,
		EIT_ETC_EVENT_CHECK94       = 1000408,
		EIT_ETC_EVENT_CHECK95       = 1000409,
		EIT_ETC_EVENT_CHECK96       = 1000410,
		EIT_ETC_EVENT_CHECK97       = 1000411,
		EIT_ETC_EVENT_CHECK98       = 1000412,
		EIT_ETC_EVENT_CHECK99       = 1000413,
		EIT_ETC_EVENT_CHECK100      = 1000414,
		EIT_ETC_ITEM_GROWTH_CATALYST = 1000415,
		EIT_ETC_ITEM_LUCKY_COIN_1    = 1000416,
		EIT_ETC_ITEM_LUCKY_COIN_2    = 1000417,
		EIT_ETC_ITEM_LUCKY_COIN_3    = 1000418,
		EIT_ETC_ITEM_LUCKY_COIN_4    = 1000419,
		EIT_ETC_ITEM_COMPOUNDEX_1    = 1000420,
		EIT_ETC_ITEM_COMPOUNDEX_2    = 1000421,
		EIT_ETC_ITEM_COMPOUNDEX_3    = 1000422,
		EIT_ETC_ITEM_RAINBOW_MIXER   = 4000423,
		EIT_ETC_ITEM_LOSTSAGA_MIXER  = 4000424,

		EIT_ETC_ITEM_TIME_GASHAPON1   = 5000425,   // 5000425 ~ 5000524 총 100개
		EIT_ETC_ITEM_TIME_GASHAPON100 = 5000524,

		EIT_ETC_GASHAPON54			  = 1000525,   // 1000525 ~ 1000724 총 200개
		EIT_ETC_GASHAPON253			  = 1000724,

		EIT_ETC_PACKAGE101			  = 3000725,   // 3000725 ~ 3000924 총 200개
		EIT_ETC_PACKAGE300			  = 3000924,

		EIT_ETC_GOLD_BOX              = 1000925, 

		EIT_ETC_ITEM_COMPOUNDEX_4	  = 1000926,   // 1000926 ~ 1000932 총 7개
		EIT_ETC_ITEM_COMPOUNDEX_10	  = 1000932,   // 

		EIT_ETC_SOLDIER_PACKAGE2	  = 3000933,

		EIT_ETC_SOLDIER_SELECTOR1     = 3000934,
		EIT_ETC_SOLDIER_SELECTOR101   = 3001034,

		EIT_ETC_FOUR_EXTRA_COMPOUND1  = 1001035,
		EIT_ETC_FOUR_EXTRA_COMPOUND51 = 1001085,

		EIT_ETC_MOTION101             = 4001086,      // 모션 아이템 300개 더 추가 1086 ~ 1385
		EIT_ETC_MOTION400             = 4001385,

		EIT_ETC_EXPAND_MEDAL_SLOT01			= 1001386,      // 메달슬롯 오픈 아이템. 기간제 5개, 영구 5개.
		EIT_ETC_EXPAND_MEDAL_SLOT20			= 1001405,

		EIT_ETC_SOLDIER_EXP_BONUS			= 5001406,

		EIT_ETC_CONSUMPTION_BUFF01			= 1001407,		// 64 개 예약		
		EIT_ETC_CONSUMPTION_BUFF64			= 1001470,		

		EIT_ETC_CONSUMPTION_REVIVE			= 1001471,		// 즉시 부활	
		EIT_ETC_CONSUMPTION_COIN			= 1001472,		// 던전 코인

	    EIT_ETC_SELECT_EXTRA_GASHAPON		= 3001473,

		EIT_ETC_PRESET_PACKAGE1				= 3001601,		// preset package 일단 100개만...
		EIT_ETC_PRESET_PACKAGE100			= 3001700,

		EIT_ETC_SOLDIER_PACKAGE3			= 3001701,      // 영구 용병 패키지
		EIT_ETC_SOLDIER_PACKAGE10			= 3001708,      

		EIT_ETC_GROWTH_ALL_DOWN				= 1001709,

		EIT_ETC_PRIZE_ITEM1					= 1001711,		// 대회 모드 상품 200개
		EIT_ETC_PRIZE_ITEM200				= 1001910,

		EIT_ETC_TOURNAMENT_CREATE			= 3001911,      // 대회 생성권
		EIT_ETC_TOURNAMENT_PREMIUM_CREATE	= 3001912,      // 프리미엄 대회 생성권 - (배너 변경권)

		EIT_ETC_CLOVER 	                    = 1001913,

		EIT_ETC_GOLD_BOX01				    = 1002001,
		EIT_ETC_GOLD_BOX32			        = 1002032,

		EIT_ETC_ADD_CASH001                 = 5002033,         
		EIT_ETC_ADD_CASH100                 = 5002132,         

		EIT_ETC_TOURNAMENT_COIN				= 1002133,

		EIT_ETC_ROULETTE_COIN 	            = 1002134,

		EIT_ETC_BINGO_ITEM                = 1002135,
		EIT_ETC_BINGO_NUMBER_GASHAPON     = 1002136,
		EIT_ETC_BINGO_SHUFFLE_NUMBER      = 1002137,
		EIT_ETC_BINGO_SHUFFLE_REWARD_ITEM = 1002138,
		EIT_ETC_BINGO_RANDOM_NUMBER_CLEAR = 1002139,

		EIT_ETC_ITEM_COMPOUNDEX_11	  = 1002140,   // 강화카드 1002140 ~ 1002229 90개 추가
		EIT_ETC_ITEM_COMPOUNDEX_100	  = 1002229,   // 

		EIT_ETC_SUPER_GASHAPON1			= 1002230,
		EIT_ETC_SUPER_GASHAPON200		= 1002429,

		EIT_ETC_ITEM_LUCKY_COIN_5     = 1002430,
		EIT_ETC_ITEM_LUCKY_COIN_205   = 1002529,

		EIT_ETC_ITEM_SEND_PRESENT_1     = 1002530,
		EIT_ETC_ITEM_SEND_PRESENT_100   = 1002629,

		EIT_ETC_ITEM_RECHARGE_1         = 1002630,
		EIT_ETC_ITEM_RECHARGE_100       = 1002729,

		EIT_ETC_ACCESSORY_RECHARGE_1		= 1002830,     // 악세 충전기.
		EIT_ETC_ACCESSORY_RECHARGE_100		= 1002929,

		EIT_ETC_ITEM_LUCKY_COIN_206		 = 1005033, // 1005033 ~ 1005332 총 300개 추가
		EIT_ETC_ITEM_LUCKY_COIN_506		 = 1005332,

		EIT_ETC_SUPER_GASHAPON201		 = 1005333, // 1005333 ~ 1005632 총 300개 추가
		EIT_ETC_SUPER_GASHAPON500		 = 1005632,

		EIT_ETC_PACKAGE301				= 3002730,
		EIT_ETC_PACKAGE800				= 3003329,

		EIT_ETC_SOLDIER_EXP_ADD_001	    = 3003330,
		EIT_ETC_SOLDIER_EXP_ADD_200	    = 3003529,

		EIT_ETC_GASHAPON254			    = 1003530,  // 1003530 ~ 1003829 총 300개
		EIT_ETC_GASHAPON553			    = 1003829,
		
		EIT_ETC_ITEM_TIME_GASHAPON101   = 5003830,  // 5003830 ~ 5004129 총 200개
		EIT_ETC_ITEM_TIME_GASHAPON300   = 5004029,

		EIT_ETC_EVENT_CHECK101          = 1004030,	// 1004030 ~ 1004129 총 100개 추가.
		EIT_ETC_EVENT_CHECK200          = 1004129,

		EIT_ETC_SELECT_EXTRA_GASHAPON02 = 3004130,  // 3004130 ~ 3004178 총 50개 추가
		EIT_ETC_SELECT_EXTRA_GASHAPON51 = 3004179,

		EIT_ETC_SELECT_GASHAPON001      = 3004180,  // 3004180 ~ 3004479 총 300개 추가
		EIT_ETC_SELECT_GASHAPON300      = 3004479,

		EIT_ETC_RISING_GASHAPON_001      = 3005001,  // 3005001 ~ 3004479 총 100개 추가
		EIT_ETC_RISING_GASHAPON_MAX      = 3005100,

		EIT_ETC_FIXED_BINGO_NUMBER001   = 1004480,  // 1004480 ~ 1004879 총 400개 추가(빙고 더미용)

		//EIT_ETC_FIXED_BINGO_NUMBER400	= 1004879,  //	기존
		//EIT_ETC_FIXED_BINGO_NUMBER400	= 1004869,  // 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가  // 범위 조정
		EIT_ETC_FIXED_BINGO_NUMBER400	= 1004859,  // 2018-08-30 by bckim, 주사위 이벤트 추가			// 범위 조정

		EIT_ETC_ITEM_MATERIAL_COMPOUND001 = 1004880, // 조각 강화 장비
		EIT_ETC_ITEM_MATERIAL_COMPOUND050 = 1004929,

		EIT_ETC_PET_EGG_001				 = 1004930,
		EIT_ETC_PET_EGG_100				 = 1005029,

		EIT_ETC_RAINBOW_WHOLE_CHAT		 = 1005030,
		EIT_ETC_SOUL_STONE				 = 1005031,

#if defined ( SRC_ID )  || defined ( SRC_SEA )
		EIT_ETC_MILEAGE_COIN			 = 1000050,
#else
		EIT_ETC_MILEAGE_COIN			 = 1005633,
#endif
		EIT_ETC_EXTRAITEM_SLOT_EXTEND	 = 4005634,

		EIT_ETC_SUPER_GASHAPON501			= 1005635,						// 1005635 ~ 1005934 총 300개 추가
		EIT_ETC_SUPER_GASHAPON800			= 1005934,

		EIT_ETC_GASHAPON554					= 1005935,						// 1005935 ~ 1006234 총 300개 추가
		EIT_ETC_GASHAPON853					= 1006234,

		EIT_ETC_ITEM_TIME_GASHAPON301		= 5006235,						// 5006235 ~ 5006534 총 300개 추가
		EIT_ETC_ITEM_TIME_GASHAPON600		= 5006534,

		EIT_ETC_COMPOUND001					= 1006535,
		EIT_ETC_COMPOUND010					= 1006544,

		EIT_ETC_GUILD_HOUSING_BLOCK_0001	= 1006545,						// 1006545 ~ 1007544 길드 본부 하우징 시스템 아이템 1000개
		EIT_ETC_GUILD_HOUSING_BLOCK_1000	= 1007544,
		EIT_ETC_CREATE_GUILD_HQ				= 3007545,

		EIT_ETC_HOUSING_BLOCK_0001			= 1007546,						// 1006545 ~ 1007544 개인 본부 하우징 시스템 아이템 1000개
		EIT_ETC_HOUSING_BLOCK_1000			= 1008545,
		EIT_ETC_CREATE_HOME					= 5008546,

		EIT_ETC_SUPER_GASHAPON801			= 1008547,
		EIT_ETC_SUPER_GASHAPON1000			= 1008746,

		EIT_ETC_TITLE_0001					= 1008747, // 1008747 ~ 1009746 칭호 선물 아이템 1000개 추가
		EIT_ETC_TITLE_1000					= 1009746, 

		EIT_ETC_TITLE_PREMIUM				= 1009747,
		EIT_ETC_TIME_CASH1					= 5009748,
		EIT_ETC_TIME_CASH2					= 5009749,

		EIT_ETC_OAK_DRUM_ITEM				= 1009749,      // 공포의 오크통
		EIT_ETC_OAK_WOOD_SWORD				= 1009750,      // 공포의 오크통 꼽기 나무 칼
		EIT_ETC_OAK_SILVER_SWORD			= 1009751,      // 공포의 오크통 꼽기 실버 칼
		EIT_ETC_OAK_GOLD_SWORD				= 1009752,      // 공포의 오크통 꼽기 골드 칼

		EIT_ETC_PACKAGE801					= 3009753,
		EIT_ETC_PACKAGE1100					= 3010752,

		EIT_ETC_PCROOM_FISHING_ROD			= 4010753,		//pc방 전용 낚싯대
		EIT_ETC_PCROOM_FISHING_BAIT			= 4010754,

		EIT_ETC_COMPOUND011					= 1010755,
		EIT_ETC_COMPOUND061					= 1010804,

		// 레이드 모드 아이템
		EIT_ETC_RAID_TICKET					= 1010805,

		EIT_ETC_ACCESSORY_COMPOSE			= 1010806,		// 악세사리 합성기

		EIT_ETC_SUPER_GASHAPON1001			= 1011001,
		EIT_ETC_SUPER_GASHAPON2000			= 1012000,

		EIT_ETC_PET_FEED_01					= 1012001,	// 펫사료 1~10
		EIT_ETC_PET_FEED_10					= 1012010,
		EIT_ETC_ADDICTIVE_PIECE				= 1012011,	// 차원조각


		EIT_ETC_SOLDIER_EXP_ADD_201 = 3012012,
		EIT_ETC_SOLDIER_EXP_ADD_2000 = 3013811,

		// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가		EIT_ETC_CARD_MATCH_ITEM 3종 >> DEFINE
		EIT_ETC_CARD_MATCH_ITEM				= 1004870,			//매칭게임 판
		EIT_ETC_CARD_MATCH_TICKET			= 1004871,			//일반 매칭게임 티켓
		EIT_ETC_CARD_MATCH_PREMIUM_TICKET	= 1004872,			//프리미엄 매칭게임 티켓
		// End. 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가		

		// 2018-08-30 by bckim, 주사위 이벤트 추가	EIT_ETC_CARD_MATCH_ITEM 3종 >> DEFINE
		EIT_ETC_DICE_GAME_START_ITEM				= 1004860,			//뱀주사위 게임판
		EIT_ETC_DICE_GAME_DICE_ITEM					= 1004861,			//뱀주사위 게임 주사위
		EIT_ETC_DICE_GAME_REWARD_ITEM				= 1004862,			//뱀주사위 보상 변경권
		EIT_ETC_DICE_GAME_BORAD_ITEM				= 1004863,			//뱀주사위 판 변경권
		// End. 2018-08-30 by bckim, 주사위 이벤트 추가

		// 2020-02-17 by bckim, 낚시 시스템 확장
		// 1000101 ~ 1000200  : 미끼   		// 5000201 ~ 5000300  : 낚싯대		// 5000314 ~ 5000424  : 낚싯터
		EIT_ETC_PRIVATE_FISHING_BAIT_ITEM_1					= 1013812,		
		EIT_ETC_PRIVATE_FISHING_BAIT_ITEM_100				= 1013911,		
		EIT_ETC_PRIVATE_FISHING_ROD_ITEM_1					= 5013912,		
		EIT_ETC_PRIVATE_FISHING_ROD_ITEM_100				= 5014011,		
		EIT_ETC_PRIVATE_FISHING_PLACE_ITEM_1				= 5014012,		
		EIT_ETC_PRIVATE_FISHING_PLACE_ITEM_100				= 5014111,		
		// End. 2020-02-17 by bckim, 낚시 시스템 확장

		EIT_ETC_NEW_CASHAPON5000					= 1015000,
		EIT_ETC_NEW_CASHAPON7000					= 1017000,
	};

	enum
	{
		#ifdef SRC_TW
				MAX_COUNT          = 999999,
		#else
				MAX_COUNT          = 99999,
		#endif
		MAX_TIME           = 31536000, // SEC, 365일
		MAX_DAY            = 999,      // 999일 
		USE_TYPE_CUT_VALUE = 1000000,
	};

	enum UseType
	{
		UT_NONE      = 0,
		UT_COUNT     = 1,
		UT_TIME      = 2,
		UT_ONCE      = 3,
		UT_ETERNITY  = 4,
		UT_DATE      = 5,
	};

protected:
	ioHashString m_Name;

	DWORD        m_dwType; // AABBBBBB -> AA:Use Type , BBBBBB:item type
	UseType      m_eUseType;
	int          m_iUseValue;
	int          m_iMaxUse;
	int          m_iSellPeso;
	int          m_iMortmainSellPeso;
	bool         m_bCanSell;
	int			 m_iExtraType;
	bool		 m_bSpecialGoods;	//특별 상품 체크 ( 특정 시간에만 판매하는 상품 )
	bool		 m_bActive;
	bool		 m_bSQLUpdate;
	bool		 m_bNeedRealCash; // 리얼캐쉬로만 구매할 수 있는 아이템.
	IntVec       m_vSubscriptionList;
	IntVec       m_vValueList;
	IntVec       m_vPesoList;
	IntVec       m_vCashList;
	IntVec		 m_vMileageList;
	IntVec       m_vBonusPesoList;
	BoolVec      m_vActiveList;

	IntVec m_vCanMortmainList;

public:
	virtual void   LoadProperty( ioINILoader &rkLoader );

public:
	const ioHashString &GetName() const { return m_Name; }
	UseType GetUseType() const { return m_eUseType; }
	int     GetUseValue() const { return m_iUseValue; }
	int     GetMaxUse() const { return m_iMaxUse; }
	int     GetSellPeso() const { return m_iSellPeso; }
	int     GetMortmainSellPeso() const { return m_iMortmainSellPeso; }
	bool    IsCanSell() const { return m_bCanSell; }
	int		GetExtraType() const { return m_iExtraType; }
	int     GetValueListSize() { return m_vValueList.size(); }

	int     GetSubscriptionType( int iArray ) const;
	int     GetValue( int iArray ) const;
	int     GetPeso( int iArray ) const;
	int     GetCash( int iArray ) const;
	int		GetMileage( int iArray ) const;
	int     GetBonusPeso( int iArray ) const;
	bool    GetActive( int iArray ) const;

	void	SetActive( bool bActive )	{ m_bActive = bActive; }
	bool	IsActive()	{ return m_bActive; }

	void	SetSpecialGoods( bool bInfo )	{ m_bSpecialGoods = bInfo; }
	bool	IsSpecialGoods()	{ return m_bSpecialGoods; }

	void    SetName(ioHashString &rszName) { m_Name = rszName; }
	void    SetType(DWORD dwType) { m_dwType = dwType; }
	void    SetUseType( UseType eUseType ) { m_eUseType = eUseType; }
	void    SetUseValue(int UseValue) { m_iUseValue = UseValue; }
	void    SetMaxUse( int iMaxUse ) { m_iMaxUse = iMaxUse; }
	void    SetSellPeso( int iSellPeso ){ m_iSellPeso = iSellPeso; }
	void    SetMortmainSellPeso( int iSellPeso ){ m_iMortmainSellPeso = iSellPeso; }
	void    SetCanSell( bool bCanSell ){ m_bCanSell = bCanSell; }
	void	SetExtraType( int iExtraType ) { m_iExtraType = iExtraType; }

	void    AddSubscription( int iType );
	void    AddValue( int iValue );
	void    AddPeso( int iPeso );
	void    AddCash( int iCash );
	void	AddMileage( int iMileage );
	void    AddBonusPeso( int iBonusPeso );
	void	AddActive( bool bActive );

	void    SetSubscription( int iArray , int iType );
	void    SetValue( int iArray , int iValue );
	void    SetPeso( int iArray , int iPeso );
	void    SetCash( int iArray , int iCash );
	void    SetMileage( int iArray , int iMileage );
	void    SetBonusPeso( int iArray , int iBonusPeso );
	void	SetActive( int iArray, bool bActive );

	void	SetSQLUpdateInfo(bool bVal);

	void AddCanMortmain( int iCanMortmain );
	void SetCanMortmain( int iArray, int iCanMortmain );
	bool IsCanMortmain( int iArray ) const;

	void SetNeedRealCash( bool bNeed )	{ m_bNeedRealCash = bNeed; }
	bool IsNeedRealCash()				{ return m_bNeedRealCash; }

public:
	bool OnUse(  SP2Packet &rkPacket, User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );
	bool OnSell( SP2Packet &rkPacket, User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

	bool FindSuperGashaponPackageRandomPresent( DWORD dwEtcItemType, DWORD& dwPackageIndex);	// 2019-07-03 by bckim, 파밍모드 추가	

public:
	virtual DWORD GetType() const { return m_dwType; }
	
	virtual void  OnBuy(  User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );
	virtual bool  OnAction( int iType, User *pUser, User *pTargetUser = NULL ){ return false; }
	virtual bool  OnUseSwitch( SP2Packet &rkPacket, User *pUser ){ return false; }
	virtual void  OnAfterBuy( User *pUser, SP2Packet &rkPacket, int iArray, int iTransactionID, int bUserBonusCash ) {}
	
	virtual void  DeleteWork( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot ) {}
	
	virtual void  SendJoinUser( User *pExistUser, User *pNewUser ) {}

	virtual bool  IsUpdateTime( Room *pRoom , User *pUser ) { return false; }
	virtual bool  IsBuyCondition( int iUse ){ return true; }

	virtual bool LeaveRoomTimeItem( ioUserEtcItem::ETCITEMSLOT &rkSlot, User *pUser ){ return false; } // now UT_TIME용

	virtual bool IsAfterBuyAction() { return false; }

	virtual int	GetProperty() { return 0; }

	virtual bool IsSQLUpdateData() { return m_bSQLUpdate; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot ) { return false; }
	
public:
	ioEtcItem();
	virtual ~ioEtcItem();
};

/////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemWholeChat : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_WHOLE_CHAT; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemWholeChat();
	virtual ~ioEtcItemWholeChat();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemGrowthDown : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_GROWTH_DOWN; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemGrowthDown();
	virtual ~ioEtcItemGrowthDown();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemFishingBait : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_FISHING_BAIT; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemFishingBait();
	virtual ~ioEtcItemFishingBait();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemFishingMoonBait : public ioEtcItem
{
public:
#ifdef 	FISHING_SYSTEM_EX_BY_BCKIM	// 2020-02-17 by bckim, 낚시 시스템 확장
	virtual DWORD GetType() const {	return m_dwType; }
#else
	virtual DWORD GetType() const { return EIT_ETC_FISHING_MOON_BAIT; }
#endif	// FISHING_SYSTEM_EX_BY_BCKIM	// End. 2020-02-17 by bckim, 낚시 시스템 확장
	

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemFishingMoonBait();
	virtual ~ioEtcItemFishingMoonBait();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemFishingRod : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_FISHING_ROD; }

public:
	ioEtcItemFishingRod();
	virtual ~ioEtcItemFishingRod();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemFishingMoonRod : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }

public:
	ioEtcItemFishingMoonRod();
	virtual ~ioEtcItemFishingMoonRod();
};

// 2020-02-17 by bckim, 낚시 시스템 확장
class ioEtcItemPrivateFishing : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }

public:
	ioEtcItemPrivateFishing();
	virtual ~ioEtcItemPrivateFishing();
};
// End. 2020-02-17 by bckim, 낚시 시스템 확장


//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemFriendSlotExtend : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_FRIEND_SLOT_EXTEND; }

public:
	virtual bool IsBuyCondition( int iUse );

public:
	ioEtcItemFriendSlotExtend();
	virtual ~ioEtcItemFriendSlotExtend();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemCharSlotExtend : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_CHAR_SLOT_EXTEND; }

public:
	virtual bool IsBuyCondition( int iUse );

public:
	ioEtcItemCharSlotExtend();
	virtual ~ioEtcItemCharSlotExtend();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemFishingSlotExtend : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_FISHING_SLOT_EXTEND; }

public:
	virtual bool IsBuyCondition( int iUse );

public:
	ioEtcItemFishingSlotExtend();
	virtual ~ioEtcItemFishingSlotExtend();
};
//////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemPesoExpBonus : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_PESO_EXP_BONUS; }
	virtual bool IsUpdateTime( IN Room *pRoom , IN User *pUser );

public:
	ioEtcItemPesoExpBonus();
	virtual ~ioEtcItemPesoExpBonus();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemExtraItemCompound : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_ITEM_COMPOUND; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemExtraItemCompound();
	virtual ~ioEtcItemExtraItemCompound();
};

//////////////////////////////용병 조각, 첨가물을 사용하는 강화etcItem ////////////////////////////////////////////////
class ioEtcItemMaterialCompound : public ioEtcItem
{
protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemMaterialCompound() {}
	virtual ~ioEtcItemMaterialCompound() {}
};
/////////////////////////////////////////////////////////////
// 100% 성공 최고급 장비강화 도구
class ioEtcItemExtraItemCompound2 : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_ITEM_COMPOUND2; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemExtraItemCompound2();
	virtual ~ioEtcItemExtraItemCompound2();
};

///////////
class ioEtcItemExtraItemCompound3 : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_ITEM_COMPOUND3; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemExtraItemCompound3();
	virtual ~ioEtcItemExtraItemCompound3();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemMultipleCompound : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MULTIPLE_COMPOUND; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemMultipleCompound();
	virtual ~ioEtcItemMultipleCompound();
};

/////////
class ioEtcItemMultipleCompound2 : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MULTIPLE_COMPOUND2; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemMultipleCompound2();
	virtual ~ioEtcItemMultipleCompound2();
};

////////
class ioEtcItemMultipleCompound3 : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MULTIPLE_COMPOUND3; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemMultipleCompound3();
	virtual ~ioEtcItemMultipleCompound3();
};

class ioEtcItemMultipleCompound4 : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MULTIPLE_COMPOUND4; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemMultipleCompound4();
	virtual ~ioEtcItemMultipleCompound4();
};

class ioEtcItemMultipleCompound5 : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MULTIPLE_COMPOUND5; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemMultipleCompound5();
	virtual ~ioEtcItemMultipleCompound5();
};

//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemMultipleEqualCompound : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MULTIPLE_EQUAL_COMPOUND; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemMultipleEqualCompound();
	virtual ~ioEtcItemMultipleEqualCompound();
};

///////
class ioEtcItemMultipleEqualCompound2 : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MULTIPLE_EQUAL_COMPOUND2; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemMultipleEqualCompound2();
	virtual ~ioEtcItemMultipleEqualCompound2();
};


//////////
class ioEtcItemMultipleEqualCompound3 : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MULTIPLE_EQUAL_COMPOUND3; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemMultipleEqualCompound3();
	virtual ~ioEtcItemMultipleEqualCompound3();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemChangeID : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_CHANGE_ID; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemChangeID();
	virtual ~ioEtcItemChangeID();
};
///////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemCustomSound : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_CUSTOM_SOUND; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemCustomSound();
	virtual ~ioEtcItemCustomSound();
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemBuyMortmainChar : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_BUY_MORTMAIN_CHAR; }

public:
	bool SetUsed( User *pUser, ioUserEtcItem *pUserEtcItem );
	bool AddEtcItem( User *pUser, ioUserEtcItem *pUserEtcItem );

public:
	ioEtcItemBuyMortmainChar();
	virtual ~ioEtcItemBuyMortmainChar();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemGuildCreate : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_GUILD_CREATE; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemGuildCreate();
	virtual ~ioEtcItemGuildCreate();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemGuildMarkChange : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_GUILD_MARK_CHANGE; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemGuildMarkChange();
	virtual ~ioEtcItemGuildMarkChange();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemGuildNameChange : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_GUILD_NAME_CHANGE; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemGuildNameChange();
	virtual ~ioEtcItemGuildNameChange();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemSoldierPackage : public ioEtcItem
{
public:
	enum ActiveFilter
	{
		AF_ALL				= 0, //Active값이 0과 1인 용병
		AF_ACTIVE			= 1, //Active값이 1인 용병
	};

protected:
	int m_iLimitClassTypeNum;
	int m_iActiveFilter;
	IntVec m_vSoldierCode;

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	inline int GetLimitClassTypeNum(){ return m_iLimitClassTypeNum; }
	inline int GetActiveFilter(){ return m_iActiveFilter; }

	bool IsRightSoldierCode( int iClassType );

public:
	ioEtcItemSoldierPackage();
	virtual ~ioEtcItemSoldierPackage();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemDecorationPackage : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_DECORATION_PACKAGE; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemDecorationPackage();
	virtual ~ioEtcItemDecorationPackage();
};
/////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemGoldMonsterCoin : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_GOLDMONSTER_COIN; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemGoldMonsterCoin();
	virtual ~ioEtcItemGoldMonsterCoin();
};
/////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemMonsterCoin : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MONSTER_COIN; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemMonsterCoin();
	virtual ~ioEtcItemMonsterCoin();
};
/////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemGashapon : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemGashapon();
	virtual ~ioEtcItemGashapon();
};
/////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemRandomDecoM : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_RANDOM_DECO_M; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemRandomDecoM();
	virtual ~ioEtcItemRandomDecoM();
};
/////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemRandomDecoW : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_RANDOM_DECO_W; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemRandomDecoW();
	virtual ~ioEtcItemRandomDecoW();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemPackage : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemPackage();
	virtual ~ioEtcItemPackage();
};
///////////////////////////////////////////////////////////////////////////
class ioEtcItemDecoUnderwearPackage : public ioEtcItem
{
public: 
	enum 
	{
		ALL_SELECT_DECO_CODE = 0,
	};
public:
	virtual DWORD GetType() const { return EIT_ETC_DECO_UNDERWEAR_PACKAGE; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemDecoUnderwearPackage();
	virtual ~ioEtcItemDecoUnderwearPackage();
};
///////////////////////////////////////////////////////////////////////////
class ioEtcItemExcavatingKit : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual bool IsUpdateTime( IN Room *pRoom , IN User *pUser );
	virtual void DeleteWork( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot );
	virtual bool LeaveRoomTimeItem( ioUserEtcItem::ETCITEMSLOT &rkSlot, User *pUser );
	virtual void OnBuy(  User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );
	virtual bool IsBuyCondition( int iUse );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemExcavatingKit();
	virtual ~ioEtcItemExcavatingKit();
};
///////////////////////////////////////////////////////////////////////////
class ioEtcItemTradeStateChange : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_TRADE_STATE_CHANGE; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemTradeStateChange();
	virtual ~ioEtcItemTradeStateChange();
};
///////////////////////////////////////////////////////////////////////////
class ioEtcItemQuestEvent : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_QUEST_EVENT; }

public:
	ioEtcItemQuestEvent();
	virtual ~ioEtcItemQuestEvent();
};
/////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemSilverCoin : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_SILVER_COIN; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemSilverCoin();
	virtual ~ioEtcItemSilverCoin();
};

/////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemMileage : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_MILEAGE_COIN; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemMileage();
	virtual ~ioEtcItemMileage();
};

//////////////////////////////////////////////////////////////////////////
class ioEtcItemBattleRecordInit : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_BATTLE_RECORD_INIT; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemBattleRecordInit();
	virtual ~ioEtcItemBattleRecordInit();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemLadderRecordInit : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_LADDER_RECORD_INIT; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemLadderRecordInit();
	virtual ~ioEtcItemLadderRecordInit();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemHeroRecordInit : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_HERO_RECORD_INIT; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemHeroRecordInit();
	virtual ~ioEtcItemHeroRecordInit();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemSkeleton : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnBuy(  User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );
	virtual bool  OnUseSwitch( SP2Packet &rkPacket, User *pUser );
	virtual bool  IsUpdateTime( IN Room *pRoom , IN User *pUser );
	virtual void  DeleteWork( User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot );
	virtual bool  LeaveRoomTimeItem( ioUserEtcItem::ETCITEMSLOT &rkSlot, User *pUser );
	virtual bool  IsBuyCondition( int iUse );

public:
	ioEtcItemSkeleton();
	virtual ~ioEtcItemSkeleton();
};
class ioEtcItemSkeletonBig : public ioEtcItemSkeleton
{
public:
	virtual DWORD GetType() const { return EIT_ETC_SKELETON_BIG; }

public:
	ioEtcItemSkeletonBig(){}
	virtual ~ioEtcItemSkeletonBig(){}
};
class ioEtcItemSkeletonBigHead : public ioEtcItemSkeleton
{
public:
	virtual DWORD GetType() const { return EIT_ETC_SKELETON_BIGHEAD; }

public:
	ioEtcItemSkeletonBigHead(){}
	virtual ~ioEtcItemSkeletonBigHead(){}
};
class ioEtcItemSkeletonSmall : public ioEtcItemSkeleton
{
public:
	virtual DWORD GetType() const { return EIT_ETC_SKELETON_SMALL; }

public:
	ioEtcItemSkeletonSmall(){}
	virtual ~ioEtcItemSkeletonSmall(){}
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemMotion : public ioEtcItem
{
public:
	enum
	{   // 1은 설정 안함이다.                                   
		MOTION_OPTION_1 = 2,            // 게임 승리 포즈
		MOTION_OPTION_2 = 3,			// 게임 패배 포즈
		MOTION_OPTION_3 = 4,			// 좋은 수상 포즈
		MOTION_OPTION_4 = 5,			// 나쁜 수상 포즈
		MOTION_OPTION_5 = 6,			// 전투대기방 포즈
		MOTION_OPTION_6 = 7,			// 진영전 대기방 포즈
		MOTION_OPTION_7 = 8,			// 래더 페이지 포즈
		MOTION_OPTION_8 = 9,			// 래더전 대기방 포즈
		MOTION_OPTION_9 = 10,           // 특수 동작 포즈
		MOTION_OPTION_10= 11,           // 대표 용병 포즈
		MOTION_OPTION_100 = 101,        // 진열 용병 포즈 예약 - 용병 200개
		MOTION_OPTION_300 = 300,        // 진열 용병 포즈 예약 - 용병 200개

		MAX_OPTION,
	};

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnBuy(  User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );
	virtual bool  IsBuyCondition( int iUse );

public:
	ioEtcItemMotion();
	virtual ~ioEtcItemMotion();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemCustomItemSkin : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_CUSTOM_ITEM_SKIN; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemCustomItemSkin();
	virtual ~ioEtcItemCustomItemSkin();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemCustomItemSkinTest : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_CUSTOM_ITEM_SKIN_TEST; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemCustomItemSkinTest();
	virtual ~ioEtcItemCustomItemSkinTest();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemCostumItemSkin : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_COSTUM_ITEM_SKIN; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemCostumItemSkin();
	virtual ~ioEtcItemCostumItemSkin();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemBlock : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void  OnBuy(  User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemBlock();
	virtual ~ioEtcItemBlock();
};
///////////////////////////////////////////////////////////////////////////
class ioEtcItemEventCheck : public ioEtcItem
{
public:
	ioEtcItemEventCheck(){}
	virtual ~ioEtcItemEventCheck(){}
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemExtraItemGrowthCatalyst : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_ITEM_GROWTH_CATALYST; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemExtraItemGrowthCatalyst();
	virtual ~ioEtcItemExtraItemGrowthCatalyst();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemLuckyCoin : public ioEtcItem
{
protected:
	struct ExchangePresent
	{
		int m_iIndex;

		int m_iNeedLuckyCoin;
		
		short m_iPresentType;
		short m_iPresentState;
		short m_iPresentMent;

		int   m_iPresentPeriod;
		int   m_iPresentValue1;
		int   m_iPresentValue2;
		int   m_iPresentValue3;
		int   m_iPresentValue4;

		ExchangePresent()
		{
			m_iIndex    = -1;
			m_iNeedLuckyCoin = 0;
			m_iPresentType = m_iPresentState = m_iPresentMent = 0;
			m_iPresentPeriod = m_iPresentValue1 = m_iPresentValue2 = m_iPresentValue3 = m_iPresentValue4 = 0;
		}
	};
	typedef std::vector< ExchangePresent > vExchangePresent;
	vExchangePresent m_ExchangePresent;

protected:
	virtual ioEtcItemLuckyCoin::ExchangePresent GetExchangePresent( int iIndex );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemLuckyCoin();
	virtual ~ioEtcItemLuckyCoin();
};
//////////////////////////////////////////////////////////////////////////////////////////////
// 강화 카드
class ioEtcItemCompoundEx : public ioEtcItem
{
protected:
	int m_iCompoundSuccessRand;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemCompoundEx();
	virtual ~ioEtcItemCompoundEx();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemRainbowMixer : public ioEtcItem 
{
protected:
	struct MixerPresent
	{
		short m_iPresentType;
		short m_iPresentState;
		short m_iPresentMent;

		int   m_iPresentPeriod;
		int   m_iPresentValue1;
		int   m_iPresentValue2;
		int   m_iPresentValue3;
		int   m_iPresentValue4;

		MixerPresent()
		{
			m_iPresentType = m_iPresentState = m_iPresentMent = 0;
			m_iPresentPeriod = m_iPresentValue1 = m_iPresentValue2 = m_iPresentValue3 = m_iPresentValue4 = 0;
		}
	};
	typedef std::vector< MixerPresent > vMixerPresent;
	vMixerPresent m_MixerPresent;


protected:
	ioEtcItemRainbowMixer::MixerPresent GetMixerPresent( int iIndex );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemRainbowMixer();
	virtual ~ioEtcItemRainbowMixer();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemLostSagaMixer : public ioEtcItemRainbowMixer
{
public:
	virtual DWORD GetType() const { return m_dwType; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemLostSagaMixer();
	virtual ~ioEtcItemLostSagaMixer();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemTimeGashapon : public ioEtcItem
{

public:

	//순차획득 가챠 전용
	enum
	{
		SEQUENCE_STATE_VALUE = 10000,	
	};	

protected:
	/************************************************************************/
	/* RepeatMinute마다 선물 획득 가능한 아이템								*/
	/************************************************************************/
	bool m_bRealTimeCheck;
	int  m_iRepeatMinute;
	
protected:
	/************************************************************************/
	/* ExtendData에 값이 세팅되면 유지한 시간에 따라 선물이 바뀌고 사용시   */
	/* 특별아이템에서 삭제된다                                              */
	/************************************************************************/
	struct ExtendData
	{
		int m_iUseMinute;
		int m_iMagicCode;

		ExtendData()
		{
			m_iUseMinute = m_iMagicCode = -1;
		}
	};
	typedef std::vector< ExtendData > ExtendDataVec;
	ExtendDataVec m_ExtendDataList;

protected:
	bool m_bSequenceOrder;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	void UseDefault( User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot, int iClientExcludeValue2 );
	void UseSequnceOrder( User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot, int iClientExcludeValue2 );

	void SendPresent( User *pUser, DWORD dwEtcItemType, ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:	

public:
	bool IsRealTimeCheck(){ return m_bRealTimeCheck; }
	bool IsSequenceOrder(){ return m_bSequenceOrder; }

public:
	ioEtcItemTimeGashapon();
	virtual ~ioEtcItemTimeGashapon();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemGoldBox : public ioEtcItemRainbowMixer
{
public:
	virtual DWORD GetType() const { return m_dwType; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemGoldBox();
	virtual ~ioEtcItemGoldBox();
};

//////////////////////////////////////////////////////////////////////////
class ioEtcItemSoldierSelector : public ioEtcItem
{
protected:
	struct RandomTime
	{
		int m_iRate;
		int m_iTime;

		RandomTime()
		{
			m_iRate = 0;
			m_iTime = 0;
		}
	};
	typedef std::vector< RandomTime > RandomTimeVector;

protected:
	RandomTimeVector m_vRandomTime;
    DWORD            m_dwTotalRate;
	IntVec           m_vSoldierCode;

	static bool      m_bRandomize;
	static IORandom  m_Random;

	int          m_iPresentPeriod;
	short        m_iPresentMent;

protected:
	bool IsRightSoldierCode( int iClassType );
	int GetSoldierTime();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemSoldierSelector();
	virtual ~ioEtcItemSoldierSelector();
};

//////////////////////////////////////////////////////////////////////////
class ioEtcItemFourExtraCompound : public ioEtcItem
{
public:
	enum 
	{
		MAX_EXTRA_ITEM = 4,
		MAX_SUCCESS    = 100,
	};
protected: 
	struct RandomTime
	{
		int m_iRate;
		int m_iTime;

		RandomTime()
		{
			m_iRate = 0;
			m_iTime = 0;
		}
	};
	typedef std::vector< RandomTime > RandomTimeVector;

protected:
	RandomTimeVector m_vRandomTime;
	DWORD            m_dwTotalRate;
	IntVec           m_vSoldierCode;
	DWORD            m_dwSuccessRate;

	static bool      m_bRandomize;
	static IORandom  m_Random;
	static IORandom  m_SucessRandom;

	int          m_iPresentPeriod;
	short        m_iPresentMent;

protected:
	bool IsRightSoldierCode( int iClassType );
	bool IsRightExtraItem( User *pUser, int iClassType, int iExtraItemSlotIdxArray[ioEtcItemFourExtraCompound::MAX_EXTRA_ITEM] );
	int  GetSoldierTime();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemFourExtraCompound();
	virtual ~ioEtcItemFourExtraCompound();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemExpandMedalSlot : public ioEtcItem
{
protected:
	enum
	{
		USE_LEVEL20  = 20,
		USE_LEVEL40  = 40,
		USE_LEVEL60  = 60,
		USE_LEVEL80  = 80,
		USE_LEVEL100 = 100,
	};
	enum
	{
		SLOT_NONE = 0,	// ExMedalSlot에서 1번은 쓰지 않는다.
		SLOT_NUM2 = 1,
		SLOT_NUM3 = 2,
		SLOT_NUM4 = 3,
		SLOT_NUM5 = 4,
		SLOT_NUM6 = 5,
	};
	int m_iSlotNumber;
	DWORD m_dwLimitTime;
	int m_iUseLevel;

public:
	virtual void   LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	inline const int GetLimitLevel() const { return m_iUseLevel; }
	inline const DWORD GetLimitTime() const { return m_dwLimitTime; }
	inline const int GetUseSlotNumber() const { return m_iSlotNumber; }

public:
	ioEtcItemExpandMedalSlot();
	virtual ~ioEtcItemExpandMedalSlot();
};

//////////////////////////////////////////////////////////////////////////
class ioEtcItemSoldierExpBonus : public ioEtcItem
{
	// 아이템 사용하기 - > 용병 선택 - > 해당 용병 경험치 보너스
protected:
	int m_iLimitLevel;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	inline const int GetLimitLevel() const { return m_iLimitLevel; }

public:
	ioEtcItemSoldierExpBonus();
	virtual ~ioEtcItemSoldierExpBonus();
};

/////////////////////////////////////////////////////////////////////////////////////////////
// 소모품 
//////////////////////////////////////////////////////////////////////////

class ioEtcItemConsumption : public ioEtcItem
{
protected:
	ioHashString m_strBuffName;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemConsumption();
	virtual ~ioEtcItemConsumption();
};


class ioEtcItemRevive : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemRevive();
	virtual ~ioEtcItemRevive();
};

///////////////////////////////////////////////////////////////////////////
class ioEtcItemSelectExtraGashapon : public ioEtcItem
{
public:
	enum 
	{
		MAX_SLOT       = 10,
		MAX_SUCCESS    = 100,
		DEFAULT_MACHINECODE = 32,
	};

	enum SelectType
	{
		ST_RANDOM_ADD = 1,
		ST_SELECT_ALL = 2,
	};

protected: 
	struct ExtraItemInfo
	{
		int  m_iCode;
		int  m_iRate;
		bool m_bActive;

		ExtraItemInfo()
		{
			m_iCode = 0;
			m_iRate = 0;
			m_bActive = true;
		}
	};
	typedef std::vector< ExtraItemInfo > ExtraItemInfoVector;

protected:
	int m_iSelectType;

	ExtraItemInfoVector m_vExtraItemInfo;

	IORandom  m_ExtraRandom;
	IORandom  m_SucessRandom;

	DWORD     m_dwExtraTotalRate;

	int m_iExtraRareDefaultRate;
	int m_iExtraNormalDefaultRate;
	int m_iSelectRate;
	int m_iExtraItemMachineCode;

protected:
	int GetExtraItemCode();

public:
	virtual void  LoadProperty( ioINILoader &rkLoader );

public:
	virtual bool IsAfterBuyAction() { return true; }
	virtual void OnAfterBuy( User *pUser, SP2Packet &rkPacket, int iArray, int iTransactionID, int bUserBonusCash );
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemSelectExtraGashapon();
	virtual ~ioEtcItemSelectExtraGashapon();
};

//////////////////////////////////////////////////////////////////////////
// preset package
//////////////////////////////////////////////////////////////////////////
class ioEtcItemPreSetPackage : public ioEtcItem
{
protected:
	int m_iClassType;
	int m_iLimitTime;
	ITEMSLOTVec m_vItemSlot;

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	bool SetPreSetPackage( User *pUser );

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	ioEtcItemPreSetPackage();
	virtual ~ioEtcItemPreSetPackage();
};

///////////////////////////////////////////////////////////////////////////
class ioEtcItemGrowthAllDown : public ioEtcItem
{
protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemGrowthAllDown();
	virtual ~ioEtcItemGrowthAllDown();
};

//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemPrizeItem : public ioEtcItem
{
protected:
	PrizeDataVec m_PrizeDataVec;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	bool _OnPrizeSend( User *pUser );

public:
	ioEtcItemPrizeItem();
	virtual ~ioEtcItemPrizeItem();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemTournamentCreate : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_TOURNAMENT_CREATE; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemTournamentCreate();
	virtual ~ioEtcItemTournamentCreate();
};
//////////////////////////////////////////////////////////////////////////
class ioEtcItemTournamentPremiumCreate : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_TOURNAMENT_PREMIUM_CREATE; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemTournamentPremiumCreate();
	virtual ~ioEtcItemTournamentPremiumCreate();
};

/////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemClover : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_CLOVER; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemClover();
	virtual ~ioEtcItemClover();
};

//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemAddCash : public ioEtcItem
{
protected:
	int m_iGetCashNum;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemAddCash();
	virtual ~ioEtcItemAddCash();
};
/////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemTournamentCoin : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_TOURNAMENT_COIN; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemTournamentCoin();
	virtual ~ioEtcItemTournamentCoin();
};

/////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemRouletteCoin : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_ROULETTE_COIN; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemRouletteCoin();
	virtual ~ioEtcItemRouletteCoin();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemBingoItem : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_BINGO_ITEM; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemBingoItem();
	virtual ~ioEtcItemBingoItem();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemBingoNumberGashapon : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_BINGO_NUMBER_GASHAPON; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemBingoNumberGashapon();
	virtual ~ioEtcItemBingoNumberGashapon();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemBingoShuffleNumber : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_BINGO_SHUFFLE_NUMBER; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemBingoShuffleNumber();
	virtual ~ioEtcItemBingoShuffleNumber();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemBingoShuffleRewardItem : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_BINGO_SHUFFLE_REWARD_ITEM; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemBingoShuffleRewardItem();
	virtual ~ioEtcItemBingoShuffleRewardItem();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemBingoRandomNumberClear: public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_BINGO_RANDOM_NUMBER_CLEAR; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemBingoRandomNumberClear();
	virtual ~ioEtcItemBingoRandomNumberClear();
};

/////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemSuperGashapon : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemSuperGashapon();
	virtual ~ioEtcItemSuperGashapon();
};

#ifdef OHTG_NEW_RANROM_BOX_20201109
class ioEtcItemRandomBox : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemRandomBox();
	virtual ~ioEtcItemRandomBox();

};
#endif //OHTG_NEW_RANROM_BOX_20201109

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemSendPresent : public ioEtcItem
{
public:
	typedef struct PresentData
	{
		int m_iPresentState;
		int	m_iPresentType;
		int	m_iPresentMent;
		int	m_iPresentPeriod;

		int m_iPresentValue1;
		int m_iPresentValue2;
		int m_iPresentValue3;
		int m_iPresentValue4;

		PresentData()
		{
			Init();
		}

		void Init()
		{
			m_iPresentState = 0;
			m_iPresentType = 0;		// 선물 타입
			m_iPresentMent = 0;		// 선물 받았을때 맨트
			m_iPresentPeriod = 0;	// 선물 기간

			m_iPresentValue1 = 0;
			m_iPresentValue2 = 0;
			m_iPresentValue3 = 0;
			m_iPresentValue4 = 0;
		}

	} PresentData;
	typedef std::vector<PresentData> vPresentData;

protected:
	vPresentData m_vPresentInfo;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemSendPresent();
	virtual ~ioEtcItemSendPresent();
};

//////////////////////////////////////////////////////////////////////////
class ioEtcItemSoldierExpAdd : public ioEtcItem 
{
protected:
	int m_iClassType;
	int m_iAddExp;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );
	
public:
	ioEtcItemSoldierExpAdd();
	virtual ~ioEtcItemSoldierExpAdd();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemRecharge : public ioEtcItem
{
public:
	struct RechargeInfo
	{
		int   m_iPeriod;
		float m_fMinRate;
		float m_fMaxRate;
		RechargeInfo()
		{
			m_iPeriod  = -1;
			m_fMinRate = 0.0f;
			m_fMaxRate = 0.0f;
		}
		RechargeInfo( int iPeriod, float fMinRate, float fMaxRate )
		{
			m_iPeriod  = iPeriod;
			m_fMinRate = fMinRate;
			m_fMaxRate = fMaxRate;
		}
	};
	typedef std::vector<RechargeInfo> vRechargeInfo;

protected:
	int           m_iMaxInfoCnt;
	vRechargeInfo m_RechargeInfoList;

	IORandom m_Random;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	int GetRechargeTime();

public:
	ioEtcItemRecharge();
	virtual ~ioEtcItemRecharge();
};



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcAccessoryRecharge : public ioEtcItem
{
public:
	struct RechargeInfo
	{
		int   m_iPeriod;
		float m_fMinRate;
		float m_fMaxRate;
		RechargeInfo()
		{
			m_iPeriod  = -1;
			m_fMinRate = 0.0f;
			m_fMaxRate = 0.0f;
		}
		RechargeInfo( int iPeriod, float fMinRate, float fMaxRate )
		{
			m_iPeriod  = iPeriod;
			m_fMinRate = fMinRate;
			m_fMaxRate = fMaxRate;
		}
	};
	typedef std::vector<RechargeInfo> vRechargeInfo;

protected:
	int           m_iMaxInfoCnt;
	vRechargeInfo m_RechargeInfoList;

	IORandom m_Random;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );

public:
	virtual DWORD GetType() const { return m_dwType; }
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	int GetRechargeTime();

public:
	ioEtcAccessoryRecharge();
	virtual ~ioEtcAccessoryRecharge();
};

///////////////////////////////////////////////////////////////////////////

class ioLocalParent;
class ioSelectGashapon : public ioEtcItem
{
public:
	virtual void  LoadProperty( ioINILoader &rkLoader );

public:
	virtual bool IsAfterBuyAction() { return true; }
	virtual void OnAfterBuy( User *pUser, SP2Packet &rkPacket, int iArray, int iTransactionID, int bUserBonusCash );
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:	
	bool OnAfterBuyProcessCash( User *pUser, ioLocalParent *pLocal, SP2Packet &rkPacket, int iArray, int iTransactionID, int iType, int iValue1, int iValue2, int bUseGoldCash );


public:
	ioSelectGashapon();
	virtual ~ioSelectGashapon();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemFixedBingoNumber : public ioEtcItem
{
protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemFixedBingoNumber();
	virtual ~ioEtcItemFixedBingoNumber();
};

//////////////////펫//////////////////////
class ioEtcItemPetEgg : public ioEtcItem
{
protected:
	bool m_bCashItem;

public:
	virtual void  LoadProperty( ioINILoader &rkLoader );

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemPetEgg();
	virtual ~ioEtcItemPetEgg();
};

///////////////특별한 확성기/////////////
class ioEtcItemRainbowWholeChat : public ioEtcItem
{
protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	virtual DWORD GetType() const { return EIT_ETC_RAINBOW_WHOLE_CHAT; }

public:
	ioEtcItemRainbowWholeChat();
	virtual ~ioEtcItemRainbowWholeChat();
};

/////////////////소울 스톤//////////////////////
class ioEtcItemSoulStone :  public ioEtcItem
{
public:
		virtual DWORD GetType() const { return EIT_ETC_SOUL_STONE; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemSoulStone();
	virtual ~ioEtcItemSoulStone();
};

////////////////////////////////////////////////확률 상승 가챠//////////////////////////////////////////

class ioRisingGashapon : public ioEtcItem
{
protected:
	int m_iFocusIndex;
	int m_iFocusCount;

public:
	virtual void  LoadProperty( ioINILoader &rkLoader );

public:
	virtual bool IsAfterBuyAction() { return true; }
	virtual void OnAfterBuy( User *pUser, SP2Packet &rkPacket, int iArray, int iTransactionID, int iBuyCash=0 );

	//virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:	
	bool OnAfterBuyProcessCash( User *pUser, ioLocalParent *pLocal, SP2Packet &rkPacket, int iArray, int iTransactionID, int iType, int iValue1, int iValue2 );

	int GetFocusIndex() const { return m_iFocusIndex; }
	int GetFocusCount() const { return m_iFocusCount; }


public:
	ioRisingGashapon();
	virtual ~ioRisingGashapon();
};

///////////////장비 확장 슬롯///////////////////
class ioEtcItemExtraSlotExtend : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_EXTRAITEM_SLOT_EXTEND; }

public:
	virtual bool IsBuyCondition( int iUse );

public:
	ioEtcItemExtraSlotExtend();
	virtual ~ioEtcItemExtraSlotExtend();
};

///////////////아이템 합성기/////////////////////
class ioEtcItemCompound : public ioEtcItem
{
public:
	ioEtcItemCompound();
	virtual ~ioEtcItemCompound();

public:
	bool IsUsedLimitedMaterial() { return m_bLimitedMaterial; }
	int GetResultGashaponCode() { return m_iResultGashaponCode; }
	int GetMaterialType() { return m_iMaterialType; }

protected:
	virtual void LoadProperty(ioINILoader &rkLoader);
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

protected:
	int m_iMaterialType;
	int m_iResultGashaponCode;
	bool m_bLimitedMaterial;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemHousingBlockItem : public ioEtcItem
{
protected:
	DWORD m_dwBlockItemCode;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DWORD GetType() const { return m_dwType; }
	virtual int	GetProperty() { return m_dwBlockItemCode; }
	
public:
	ioEtcItemHousingBlockItem();
	virtual ~ioEtcItemHousingBlockItem();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class ioEtcItemCreateGuildHQCreate : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }	

	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemCreateGuildHQCreate();
	virtual ~ioEtcItemCreateGuildHQCreate();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemCreateMyHomeCreate : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }	

	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );
	virtual void OnBuy( User *pUser,  ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	void ActiveMyHome(User *pUser, ioUserEtcItem::ETCITEMSLOT &rkSlot);

public:
	ioEtcItemCreateMyHomeCreate();
	virtual ~ioEtcItemCreateMyHomeCreate();
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemTimeCash : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }

public:
	ioEtcItemTimeCash();
	virtual ~ioEtcItemTimeCash();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemTitle : public ioEtcItem
{
protected:
	DWORD m_dwTitleCode;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DWORD GetType() const { return m_dwType; }
	virtual int	GetProperty() { return m_dwTitleCode; }

	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemTitle();
	virtual ~ioEtcItemTitle();
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemTitlePremium : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }	

	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemTitlePremium();
	virtual ~ioEtcItemTitlePremium();
};

class ioEtcItemOakBarrel : public ioEtcItem
{
public:
	virtual DWORD GetType() const {return m_dwType; }

public:
	ioEtcItemOakBarrel( DWORD dwType ) { m_dwType = dwType; }
	virtual ~ioEtcItemOakBarrel();

	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemPCROOMFishingRod : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }

public:
	ioEtcItemPCROOMFishingRod();
	virtual ~ioEtcItemPCROOMFishingRod();
};
//////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemPCROOMFishingBait : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return m_dwType; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemPCROOMFishingBait();
	virtual ~ioEtcItemPCROOMFishingBait();
};

/////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemRaidTicket : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_RAID_TICKET; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );
	virtual void OnAfterBuy( User *pUser, SP2Packet &rkPacket, int iArray, int iTransactionID, int bUserBonusCash );

public:
	ioEtcItemRaidTicket();
	virtual ~ioEtcItemRaidTicket();
};

class ioEtcItemAccessoryCompose : public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_ACCESSORY_COMPOSE; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemAccessoryCompose();
	virtual ~ioEtcItemAccessoryCompose();
};

/////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemAddictivePiece: public ioEtcItem
{
public:
	virtual DWORD GetType() const { return EIT_ETC_ADDICTIVE_PIECE; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemAddictivePiece();
	virtual ~ioEtcItemAddictivePiece();
};

/////////////////////////////////////////////////////////////////////////////////////////////
class ioEtcItemPetFeed : public ioEtcItem
{
private:
	int m_iAddExp;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual DWORD GetType() const { return m_dwType; }
	int GetAddExp() { return m_iAddExp; }

protected:
	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );

public:
	ioEtcItemPetFeed();
	virtual ~ioEtcItemPetFeed();
};
////////////////////////////////////////////////////////////////////////////////////////////
// 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
class ioEtcItemCardMatching : public ioEtcItem
{
public:
	virtual DWORD GetType() const {return m_dwType; }

public:
	ioEtcItemCardMatching( DWORD dwType ) { m_dwType = dwType; }
	virtual ~ioEtcItemCardMatching();

	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );
};
// End. 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
//////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////
// 2018-08-30 by bckim, 주사위 이벤트 추가
class ioEtcItemDiceGame : public ioEtcItem
{
public:
	virtual DWORD GetType() const {return m_dwType; }

public:
	ioEtcItemDiceGame( DWORD dwType ) { m_dwType = dwType; }
	virtual ~ioEtcItemDiceGame();

	virtual bool _OnUse( SP2Packet &rkPacket , User *pUser, ioUserEtcItem *pUserEtcItem, ioUserEtcItem::ETCITEMSLOT &rkSlot );
};
// End. 2018-08-30 by bckim, 주사위 이벤트 추가
//////////////////////////////////////////////////////////////////////////////////////////////