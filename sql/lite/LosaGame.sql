-- Schema-only dump (no data). Generated from live MSSQL conversion.
-- Restore: sqlite3 <db> < this file
PRAGMA foreign_keys=OFF;
BEGIN TRANSACTION;
CREATE TABLE [define_award] ([idx] INTEGER NOT NULL, [awardHanName] TEXT NOT NULL, [awardEngName] TEXT NOT NULL, [limitrankingPoint] INTEGER NOT NULL);
CREATE TABLE [define_blockIP] ([idx] INTEGER NOT NULL, [userID] TEXT, [userIP] TEXT NOT NULL, [endDate] TEXT, [note] TEXT, [regDate] TEXT NOT NULL);
CREATE TABLE [define_class] ([idx] INTEGER NOT NULL, [classHanName] TEXT NOT NULL, [classEngName] TEXT NOT NULL, [movieorderby] INTEGER NOT NULL, [limitrankingLevel] INTEGER NOT NULL, [limituseLevel] INTEGER NOT NULL, [premiumType] INTEGER NOT NULL);
CREATE TABLE [define_class_price] ([idx] INTEGER NOT NULL, [class1_price] INTEGER NOT NULL, [class2_price] INTEGER NOT NULL, [class3_price] INTEGER NOT NULL, [class4_price] INTEGER NOT NULL, [class5_price] INTEGER NOT NULL, [class6_price] INTEGER NOT NULL, [class7_price] INTEGER NOT NULL, [class8_price] INTEGER NOT NULL, [class9_price] INTEGER NOT NULL, [class10_price] INTEGER NOT NULL, [class11_price] INTEGER NOT NULL, [class12_price] INTEGER NOT NULL, [class13_price] INTEGER NOT NULL, [class14_price] INTEGER NOT NULL, [class15_price] INTEGER NOT NULL, [class16_price] INTEGER NOT NULL, [class17_price] INTEGER NOT NULL, [class18_price] INTEGER NOT NULL, [class19_price] INTEGER NOT NULL, [class20_price] INTEGER NOT NULL, [class21_price] INTEGER NOT NULL, [class22_price] INTEGER NOT NULL, [class23_price] INTEGER NOT NULL, [class24_price] INTEGER NOT NULL, [class25_price] INTEGER NOT NULL, [class26_price] INTEGER NOT NULL, [class27_price] INTEGER NOT NULL, [class28_price] INTEGER NOT NULL, [class29_price] INTEGER NOT NULL, [class30_price] INTEGER NOT NULL, [class31_price] INTEGER NOT NULL, [class32_price] INTEGER NOT NULL, [class33_price] INTEGER NOT NULL, [class34_price] INTEGER NOT NULL, [class35_price] INTEGER NOT NULL, [class36_price] INTEGER NOT NULL, [class37_price] INTEGER NOT NULL, [class38_price] INTEGER NOT NULL, [class39_price] INTEGER NOT NULL, [class40_price] INTEGER NOT NULL, [class41_price] INTEGER NOT NULL, [class42_price] INTEGER NOT NULL, [class43_price] INTEGER NOT NULL, [class44_price] INTEGER NOT NULL, [class45_price] INTEGER NOT NULL, [class46_price] INTEGER NOT NULL, [class47_price] INTEGER NOT NULL, [class48_price] INTEGER NOT NULL, [class49_price] INTEGER NOT NULL, [class50_price] INTEGER NOT NULL, [class51_price] INTEGER NOT NULL, [class52_price] INTEGER NOT NULL, [class53_price] INTEGER NOT NULL, [class54_price] INTEGER NOT NULL, [class55_price] INTEGER NOT NULL, [class56_price] INTEGER NOT NULL, [class57_price] INTEGER NOT NULL, [class58_price] INTEGER NOT NULL, [class59_price] INTEGER NOT NULL, [class60_price] INTEGER NOT NULL, [class61_price] INTEGER NOT NULL, [class62_price] INTEGER NOT NULL, [class63_price] INTEGER NOT NULL, [class64_price] INTEGER NOT NULL, [class65_price] INTEGER NOT NULL, [class66_price] INTEGER NOT NULL, [class67_price] INTEGER NOT NULL, [class68_price] INTEGER NOT NULL, [class69_price] INTEGER NOT NULL, [class70_price] INTEGER NOT NULL, [class71_price] INTEGER NOT NULL, [class72_price] INTEGER NOT NULL, [class73_price] INTEGER NOT NULL, [class74_price] INTEGER NOT NULL, [class75_price] INTEGER NOT NULL, [class76_price] INTEGER NOT NULL, [class77_price] INTEGER NOT NULL, [class78_price] INTEGER NOT NULL, [class79_price] INTEGER NOT NULL, [class80_price] INTEGER NOT NULL, [class81_price] INTEGER NOT NULL, [class82_price] INTEGER NOT NULL, [class83_price] INTEGER NOT NULL, [class84_price] INTEGER NOT NULL, [class85_price] INTEGER NOT NULL, [class86_price] INTEGER NOT NULL, [class87_price] INTEGER NOT NULL, [class88_price] INTEGER NOT NULL, [class89_price] INTEGER NOT NULL, [class90_price] INTEGER NOT NULL, [class91_price] INTEGER NOT NULL, [class92_price] INTEGER NOT NULL, [class93_price] INTEGER NOT NULL, [class94_price] INTEGER NOT NULL, [class95_price] INTEGER NOT NULL, [class96_price] INTEGER NOT NULL, [class97_price] INTEGER NOT NULL, [class98_price] INTEGER NOT NULL, [class99_price] INTEGER NOT NULL, [class100_price] INTEGER NOT NULL, [class101_price] INTEGER NOT NULL, [class102_price] INTEGER NOT NULL, [class103_price] INTEGER NOT NULL, [class104_price] INTEGER NOT NULL, [class105_price] INTEGER NOT NULL, [class106_price] INTEGER NOT NULL, [class107_price] INTEGER NOT NULL, [class108_price] INTEGER NOT NULL, [class109_price] INTEGER NOT NULL, [class110_price] INTEGER NOT NULL, [class111_price] INTEGER NOT NULL, [class112_price] INTEGER NOT NULL, [class113_price] INTEGER NOT NULL, [class114_price] INTEGER NOT NULL, [class115_price] INTEGER NOT NULL, [class116_price] INTEGER NOT NULL, [class117_price] INTEGER NOT NULL, [class118_price] INTEGER NOT NULL, [class119_price] INTEGER NOT NULL, [class120_price] INTEGER NOT NULL, [class121_price] INTEGER NOT NULL, [class122_price] INTEGER NOT NULL, [class123_price] INTEGER NOT NULL, [class124_price] INTEGER NOT NULL, [class125_price] INTEGER NOT NULL, [class126_price] INTEGER NOT NULL, [class127_price] INTEGER NOT NULL, [class128_price] INTEGER NOT NULL, [class129_price] INTEGER NOT NULL, [class130_price] INTEGER NOT NULL, [class131_price] INTEGER NOT NULL, [class132_price] INTEGER NOT NULL, [class133_price] INTEGER NOT NULL, [class134_price] INTEGER NOT NULL, [class135_price] INTEGER NOT NULL, [class136_price] INTEGER NOT NULL, [class137_price] INTEGER NOT NULL, [class138_price] INTEGER NOT NULL, [class139_price] INTEGER NOT NULL, [class140_price] INTEGER NOT NULL, [class141_price] INTEGER NOT NULL, [class142_price] INTEGER NOT NULL, [class143_price] INTEGER NOT NULL, [class144_price] INTEGER NOT NULL, [class145_price] INTEGER NOT NULL, [class146_price] INTEGER NOT NULL, [class147_price] INTEGER NOT NULL, [class148_price] INTEGER NOT NULL, [class149_price] INTEGER NOT NULL, [class150_price] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [define_config] ([idx] INTEGER NOT NULL, [objName] TEXT NOT NULL, [value1] TEXT NOT NULL, [value2] TEXT, [value3] TEXT, [value4] TEXT, [value5] TEXT, [note] TEXT, [regDate] TEXT NOT NULL);
CREATE TABLE [define_encode_key] ([idx] INTEGER NOT NULL, [encodeKey] TEXT NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [define_game_server] ([idx] INTEGER NOT NULL, [serverID] INTEGER NOT NULL, [serverIP] TEXT NOT NULL, [serverPort] INTEGER NOT NULL, [serverName] TEXT, [clientPort] INTEGER NOT NULL, [connNumber] INTEGER NOT NULL, [status] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([serverID]));
CREATE TABLE [define_gear_customidx] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL);
CREATE TABLE [define_geoip] ([sip] TEXT NOT NULL, [eip] TEXT NOT NULL, [sbwv] INTEGER NOT NULL, [ebwv] INTEGER NOT NULL, [country] TEXT NOT NULL, [countryname] TEXT NOT NULL);
CREATE TABLE [define_hero_price] ([HeroCode] INTEGER NOT NULL, [HeroPrice] INTEGER NOT NULL, PRIMARY KEY ([HeroCode]));
CREATE TABLE [define_manage] ([idx] INTEGER NOT NULL, [objName] TEXT NOT NULL, [code] INTEGER, [itemName] TEXT);
CREATE TABLE [define_medal] ([idx] INTEGER NOT NULL, [medalCode] INTEGER NOT NULL, [eng_name] TEXT NOT NULL, [point] INTEGER NOT NULL, [title] TEXT NOT NULL, [content1] TEXT NOT NULL, [content2] TEXT NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([medalCode]));
CREATE TABLE [define_quest] ([idx] INTEGER NOT NULL, [questIDX] INTEGER NOT NULL, [title] TEXT NOT NULL, [progress] TEXT NOT NULL, [content1] TEXT, [content2] TEXT, [content3] TEXT, [content4] TEXT);
CREATE TABLE [define_site_config] ([idx] INTEGER NOT NULL, [objName] TEXT NOT NULL, [content] TEXT NOT NULL, [note] TEXT, [regDate] TEXT NOT NULL);
CREATE TABLE [define_zipcode] ([zipcode] TEXT, [sido] TEXT, [gugun] TEXT, [dong] TEXT, [ri] TEXT, [bldg] TEXT, [bunji] TEXT, [seq] TEXT);
CREATE TABLE [event_chuchun] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [friendIDX] INTEGER NOT NULL, [flag] INTEGER NOT NULL, [userIP] TEXT NOT NULL, [userType] INTEGER NOT NULL, [itemFlag] INTEGER NOT NULL, [cpID] TEXT, [cpType] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [event_chuchun_conndate] ([accountIDX] INTEGER NOT NULL, [connDate] TEXT NOT NULL);
CREATE TABLE [event_item_limited] ([AccountIDX] INTEGER NOT NULL, [EventType] INTEGER NOT NULL, [GoodsIndex] INTEGER NOT NULL, [GoodsCount] INTEGER NOT NULL, PRIMARY KEY ([AccountIDX], [EventType], [GoodsIndex]));
CREATE TABLE [itemCode] ([idx] INTEGER NOT NULL, [itemType] REAL, [itemCode] REAL, [itemValue] REAL);
CREATE TABLE [LOSA_PRIVATE_UserInfo] ([userID] TEXT, [UserName] TEXT, [email] TEXT);
CREATE TABLE [mannerUserID] ([accountIDX] INTEGER, [bonus] INTEGER, [cnt] INTEGER);
CREATE TABLE [TB_LTS_WEEKTIME] ([userID] TEXT NOT NULL, [GmtCodeNo] INTEGER NOT NULL, [Wday] INTEGER NOT NULL, [Wtime] INTEGER NOT NULL, PRIMARY KEY ([userID], [GmtCodeNo], [Wday], [Wtime]));
CREATE TABLE [TB_LTS_WEEKTIME_PARENT_CHILD_INFO] ([PARENT_ID] TEXT NOT NULL, [CHILD_ID] TEXT NOT NULL, [Agree_Type] TEXT, [AgreeDate] TEXT, [RegIP] TEXT, [RegDate] TEXT, PRIMARY KEY ([PARENT_ID], [CHILD_ID]));
CREATE TABLE [TB_WEEKTIME] ([wday] INTEGER NOT NULL, [wtime] INTEGER NOT NULL);
CREATE TABLE [TEMP_MONTHLY_MANNER_USER] ([AccountIDX] INTEGER);
CREATE TABLE [userAbilityDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [class1_type] INTEGER NOT NULL, [class1_basic01] INTEGER NOT NULL, [class1_basic02] INTEGER NOT NULL, [class1_basic03] INTEGER NOT NULL, [class1_basic04] INTEGER NOT NULL, [class1_skill1_sd] INTEGER NOT NULL, [class1_skill1_asd] INTEGER NOT NULL, [class1_skill1_ad] INTEGER NOT NULL, [class1_skill1_as] INTEGER NOT NULL, [class1_timeslot] INTEGER NOT NULL, [class1_limitdate] TEXT NOT NULL, [class2_type] INTEGER NOT NULL, [class2_basic01] INTEGER NOT NULL, [class2_basic02] INTEGER NOT NULL, [class2_basic03] INTEGER NOT NULL, [class2_basic04] INTEGER NOT NULL, [class2_skill1_sd] INTEGER NOT NULL, [class2_skill1_asd] INTEGER NOT NULL, [class2_skill1_ad] INTEGER NOT NULL, [class2_skill1_as] INTEGER NOT NULL, [class2_timeslot] INTEGER NOT NULL, [class2_limitdate] TEXT NOT NULL, [class3_type] INTEGER NOT NULL, [class3_basic01] INTEGER NOT NULL, [class3_basic02] INTEGER NOT NULL, [class3_basic03] INTEGER NOT NULL, [class3_basic04] INTEGER NOT NULL, [class3_skill1_sd] INTEGER NOT NULL, [class3_skill1_asd] INTEGER NOT NULL, [class3_skill1_ad] INTEGER NOT NULL, [class3_skill1_as] INTEGER NOT NULL, [class3_timeslot] INTEGER NOT NULL, [class3_limitdate] TEXT NOT NULL, [class4_type] INTEGER NOT NULL, [class4_basic01] INTEGER NOT NULL, [class4_basic02] INTEGER NOT NULL, [class4_basic03] INTEGER NOT NULL, [class4_basic04] INTEGER NOT NULL, [class4_skill1_sd] INTEGER NOT NULL, [class4_skill1_asd] INTEGER NOT NULL, [class4_skill1_ad] INTEGER NOT NULL, [class4_skill1_as] INTEGER NOT NULL, [class4_timeslot] INTEGER NOT NULL, [class4_limitdate] TEXT NOT NULL, [class5_type] INTEGER NOT NULL, [class5_basic01] INTEGER NOT NULL, [class5_basic02] INTEGER NOT NULL, [class5_basic03] INTEGER NOT NULL, [class5_basic04] INTEGER NOT NULL, [class5_skill1_sd] INTEGER NOT NULL, [class5_skill1_asd] INTEGER NOT NULL, [class5_skill1_ad] INTEGER NOT NULL, [class5_skill1_as] INTEGER NOT NULL, [class5_timeslot] INTEGER NOT NULL, [class5_limitdate] TEXT NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userAccessoryDB] ([idx] INTEGER NOT NULL, [AccountIDX] INTEGER NOT NULL, [ItemCode] INTEGER NOT NULL, [PeriodType] INTEGER NOT NULL, [PeriodDate] TEXT NOT NULL, [StatValue] INTEGER NOT NULL, [ClassType] INTEGER NOT NULL, [RegDate] TEXT NOT NULL, [ComposeCode] INTEGER NOT NULL, [ComposeValue] INTEGER NOT NULL, PRIMARY KEY ([AccountIDX], [idx]));
CREATE TABLE [userAttendanceDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [checkDate] TEXT NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userAttendDB] ([AccountIDX] INTEGER NOT NULL, [AttendCount] INTEGER NOT NULL, [RewardTable] INTEGER NOT NULL, [ConnectDate] TEXT NOT NULL, [RegDate] TEXT NOT NULL, PRIMARY KEY ([AccountIDX]));
CREATE TABLE [userAwardDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [award1_type] INTEGER NOT NULL, [award1_number] INTEGER NOT NULL, [award1_point] INTEGER NOT NULL, [award2_type] INTEGER NOT NULL, [award2_number] INTEGER NOT NULL, [award2_point] INTEGER NOT NULL, [award3_type] INTEGER NOT NULL, [award3_number] INTEGER NOT NULL, [award3_point] INTEGER NOT NULL, [award4_type] INTEGER NOT NULL, [award4_number] INTEGER NOT NULL, [award4_point] INTEGER NOT NULL, [award5_type] INTEGER NOT NULL, [award5_number] INTEGER NOT NULL, [award5_point] INTEGER NOT NULL, [award6_type] INTEGER NOT NULL, [award6_number] INTEGER NOT NULL, [award6_point] INTEGER NOT NULL, [award7_type] INTEGER NOT NULL, [award7_number] INTEGER NOT NULL, [award7_point] INTEGER NOT NULL, [award8_type] INTEGER NOT NULL, [award8_number] INTEGER NOT NULL, [award8_point] INTEGER NOT NULL, [award9_type] INTEGER NOT NULL, [award9_number] INTEGER NOT NULL, [award9_point] INTEGER NOT NULL, [award10_type] INTEGER NOT NULL, [award10_number] INTEGER NOT NULL, [award10_point] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userCardMatchingDB] ([AccountIDX] INTEGER NOT NULL, [MissionCount] INTEGER NOT NULL, [MissionType] INTEGER NOT NULL, [MissionMark1] INTEGER NOT NULL, [MissionMark2] INTEGER NOT NULL, [Mark1] INTEGER NOT NULL, [Mark2] INTEGER NOT NULL, [UpdateTime] TEXT NOT NULL, [RegDate] TEXT NOT NULL);
CREATE TABLE [userCashDB] ([accountIDX] INTEGER NOT NULL, [amtCash] INTEGER NOT NULL, [amtBonus] INTEGER NOT NULL, [amtLimit] INTEGER NOT NULL, [amtSum] INTEGER NOT NULL, [chgDate] TEXT NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountIDX]));
CREATE TABLE [userCharacterDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [ClassType] INTEGER NOT NULL, [CI_type] INTEGER NOT NULL, [CI_gender] INTEGER NOT NULL, [CI_beard] INTEGER NOT NULL, [CI_face] INTEGER NOT NULL, [CI_hair] INTEGER NOT NULL, [CI_skinColor] INTEGER NOT NULL, [CI_hairColor] INTEGER NOT NULL, [CI_trinket] INTEGER NOT NULL, [CI_position] INTEGER NOT NULL, [CI_underwear] INTEGER NOT NULL, [EI1_type] INTEGER NOT NULL, [EI2_type] INTEGER NOT NULL, [EI3_type] INTEGER NOT NULL, [EI4_type] INTEGER NOT NULL, [limitTime] INTEGER NOT NULL, [limitType] INTEGER NOT NULL, [leaderType] INTEGER NOT NULL, [rentalType] INTEGER NOT NULL, [rentalTime] INTEGER NOT NULL, [awakeType] INTEGER NOT NULL, [awakeTime] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userClassDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [class1_type] INTEGER NOT NULL, [class1_level] INTEGER NOT NULL, [class1_expert] INTEGER NOT NULL, [class1_stat] INTEGER NOT NULL, [class2_type] INTEGER NOT NULL, [class2_level] INTEGER NOT NULL, [class2_expert] INTEGER NOT NULL, [class2_stat] INTEGER NOT NULL, [class3_type] INTEGER NOT NULL, [class3_level] INTEGER NOT NULL, [class3_expert] INTEGER NOT NULL, [class3_stat] INTEGER NOT NULL, [class4_type] INTEGER NOT NULL, [class4_level] INTEGER NOT NULL, [class4_expert] INTEGER NOT NULL, [class4_stat] INTEGER NOT NULL, [class5_type] INTEGER NOT NULL, [class5_level] INTEGER NOT NULL, [class5_expert] INTEGER NOT NULL, [class5_stat] INTEGER NOT NULL, [class6_type] INTEGER NOT NULL, [class6_level] INTEGER NOT NULL, [class6_expert] INTEGER NOT NULL, [class6_stat] INTEGER NOT NULL, [class7_type] INTEGER NOT NULL, [class7_level] INTEGER NOT NULL, [class7_expert] INTEGER NOT NULL, [class7_stat] INTEGER NOT NULL, [class8_type] INTEGER NOT NULL, [class8_level] INTEGER NOT NULL, [class8_expert] INTEGER NOT NULL, [class8_stat] INTEGER NOT NULL, [class9_type] INTEGER NOT NULL, [class9_level] INTEGER NOT NULL, [class9_expert] INTEGER NOT NULL, [class9_stat] INTEGER NOT NULL, [class10_type] INTEGER NOT NULL, [class10_level] INTEGER NOT NULL, [class10_expert] INTEGER NOT NULL, [class10_stat] INTEGER NOT NULL);
CREATE TABLE [userCloverDB] ([accountIDX] INTEGER NOT NULL, [CloverCnt] INTEGER NOT NULL, [LastChargeDate] INTEGER NOT NULL, [RemainTime] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountIDX]));
CREATE TABLE [userCoinDB] ([AccountIDX] INTEGER NOT NULL, [UpdateDate] TEXT NOT NULL, [CoinType] INTEGER NOT NULL, [RegDate] TEXT NOT NULL, PRIMARY KEY ([AccountIDX], [CoinType]));
CREATE TABLE [userConfigDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [keyvalue] TEXT NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userConfigDB_TEST] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [keyvalue] TEXT NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userCooperationDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [userID] TEXT NOT NULL, [cpIDX] INTEGER, [cpID] TEXT NOT NULL, [cpType] INTEGER NOT NULL, [note] TEXT, [regDate] TEXT NOT NULL);
CREATE TABLE [userCostumeDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [itemCode] INTEGER NOT NULL, [mcustom] INTEGER NOT NULL, [fcustom] INTEGER NOT NULL, [periodType] INTEGER NOT NULL, [periodDate] TEXT NOT NULL, [classType] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([idx], [accountIDX]));
CREATE TABLE [userCustomMedalDB] ([idx] INTEGER NOT NULL, [AccountIDX] INTEGER NOT NULL, [MedalCode] INTEGER NOT NULL, [BasicGrowth1] INTEGER NOT NULL, [BasicGrowth2] INTEGER NOT NULL, [BasicGrowth3] INTEGER NOT NULL, [BasicGrowth4] INTEGER NOT NULL, [SkillGrowth1] INTEGER NOT NULL, [SkillGrowth2] INTEGER NOT NULL, [SkillGrowth3] INTEGER NOT NULL, [SkillGrowth4] INTEGER NOT NULL, [SetCharaterNum] INTEGER NOT NULL, [LimitType] INTEGER NOT NULL, [LimitDate] TEXT NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userDiceGameDB] ([AccountIDX] INTEGER NOT NULL, [Position] INTEGER NOT NULL, [Trace01] INTEGER NOT NULL, [Trace02] INTEGER NOT NULL, [Trace03] INTEGER NOT NULL, [Trace04] INTEGER NOT NULL, [Trace05] INTEGER NOT NULL, [Trace06] INTEGER NOT NULL, [Board] INTEGER NOT NULL, [RewardStep01] INTEGER NOT NULL, [RewardStep02] INTEGER NOT NULL, [RewardStep03] INTEGER NOT NULL, [RewardStep04] INTEGER NOT NULL, [RewardStep05] INTEGER NOT NULL, [RewardStep06] INTEGER NOT NULL, [RewardStep07] INTEGER NOT NULL, [RewardStep08] INTEGER NOT NULL, [RewardStep09] INTEGER NOT NULL, [RewardStep10] INTEGER NOT NULL, [UpdateTime] TEXT, [RegDate] TEXT);
CREATE TABLE [userEventBingo_Number] ([accountidx] INTEGER NOT NULL, [number1] INTEGER NOT NULL, [number2] INTEGER NOT NULL, [number3] INTEGER NOT NULL, [number4] INTEGER NOT NULL, [number5] INTEGER NOT NULL, [number6] INTEGER NOT NULL, [number7] INTEGER NOT NULL, [number8] INTEGER NOT NULL, [number9] INTEGER NOT NULL, [number10] INTEGER NOT NULL, [number11] INTEGER NOT NULL, [number12] INTEGER NOT NULL, [number13] INTEGER NOT NULL, [number14] INTEGER NOT NULL, [number15] INTEGER NOT NULL, [number16] INTEGER NOT NULL, [number17] INTEGER NOT NULL, [number18] INTEGER NOT NULL, [number19] INTEGER NOT NULL, [number20] INTEGER NOT NULL, [number21] INTEGER NOT NULL, [number22] INTEGER NOT NULL, [number23] INTEGER NOT NULL, [number24] INTEGER NOT NULL, [number25] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountidx]));
CREATE TABLE [userEventBingo_Present] ([accountidx] INTEGER NOT NULL, [item1] INTEGER NOT NULL, [item2] INTEGER NOT NULL, [item3] INTEGER NOT NULL, [item4] INTEGER NOT NULL, [item5] INTEGER NOT NULL, [item6] INTEGER NOT NULL, [item7] INTEGER NOT NULL, [item8] INTEGER NOT NULL, [item9] INTEGER NOT NULL, [item10] INTEGER NOT NULL, [item11] INTEGER NOT NULL, [item12] INTEGER NOT NULL, [item13] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountidx]));
CREATE TABLE [userEventDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [value1] INTEGER NOT NULL, [value2] INTEGER NOT NULL, [eventType] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userEventPirateRoulette_Number] ([accountidx] INTEGER NOT NULL, [HP] INTEGER NOT NULL, [slot1] INTEGER NOT NULL, [slot2] INTEGER NOT NULL, [slot3] INTEGER NOT NULL, [slot4] INTEGER NOT NULL, [slot5] INTEGER NOT NULL, [slot6] INTEGER NOT NULL, [slot7] INTEGER NOT NULL, [slot8] INTEGER NOT NULL, [slot9] INTEGER NOT NULL, [slot10] INTEGER NOT NULL, [slot11] INTEGER NOT NULL, [slot12] INTEGER NOT NULL, [slot13] INTEGER NOT NULL, [slot14] INTEGER NOT NULL, [slot15] INTEGER NOT NULL, [slot16] INTEGER NOT NULL, [slot17] INTEGER NOT NULL, [slot18] INTEGER NOT NULL, [slot19] INTEGER NOT NULL, [slot20] INTEGER NOT NULL, [slot21] INTEGER NOT NULL, [slot22] INTEGER NOT NULL, [slot23] INTEGER NOT NULL, [slot24] INTEGER NOT NULL, [slot25] INTEGER NOT NULL, [slot26] INTEGER NOT NULL, [slot27] INTEGER NOT NULL, [slot28] INTEGER NOT NULL, [slot29] INTEGER NOT NULL, [slot30] INTEGER NOT NULL, [slot31] INTEGER NOT NULL, [slot32] INTEGER NOT NULL, [slot33] INTEGER NOT NULL, [slot34] INTEGER NOT NULL, [slot35] INTEGER NOT NULL, [slot36] INTEGER NOT NULL, [slot37] INTEGER NOT NULL, [slot38] INTEGER NOT NULL, [slot39] INTEGER NOT NULL, [slot40] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountidx]));
CREATE TABLE [userEventPirateRoulette_Present] ([accountidx] INTEGER NOT NULL, [reward1] INTEGER NOT NULL, [reward2] INTEGER NOT NULL, [reward3] INTEGER NOT NULL, [reward4] INTEGER NOT NULL, [reward5] INTEGER NOT NULL, [reward6] INTEGER NOT NULL, [reward7] INTEGER NOT NULL, [reward8] INTEGER NOT NULL, [reward9] INTEGER NOT NULL, [reward10] INTEGER NOT NULL, [reward11] INTEGER NOT NULL, [reward12] INTEGER NOT NULL, [reward13] INTEGER NOT NULL, [reward14] INTEGER NOT NULL, [reward15] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountidx]));
CREATE TABLE [userFishDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [fish1_type] INTEGER NOT NULL, [fish1_an] INTEGER NOT NULL, [fish2_type] INTEGER NOT NULL, [fish2_an] INTEGER NOT NULL, [fish3_type] INTEGER NOT NULL, [fish3_an] INTEGER NOT NULL, [fish4_type] INTEGER NOT NULL, [fish4_an] INTEGER NOT NULL, [fish5_type] INTEGER NOT NULL, [fish5_an] INTEGER NOT NULL, [fish6_type] INTEGER NOT NULL, [fish6_an] INTEGER NOT NULL, [fish7_type] INTEGER NOT NULL, [fish7_an] INTEGER NOT NULL, [fish8_type] INTEGER NOT NULL, [fish8_an] INTEGER NOT NULL, [fish9_type] INTEGER NOT NULL, [fish9_an] INTEGER NOT NULL, [fish10_type] INTEGER NOT NULL, [fish10_an] INTEGER NOT NULL, [fish11_type] INTEGER NOT NULL, [fish11_an] INTEGER NOT NULL, [fish12_type] INTEGER NOT NULL, [fish12_an] INTEGER NOT NULL, [fish13_type] INTEGER NOT NULL, [fish13_an] INTEGER NOT NULL, [fish14_type] INTEGER NOT NULL, [fish14_an] INTEGER NOT NULL, [fish15_type] INTEGER NOT NULL, [fish15_an] INTEGER NOT NULL, [fish16_type] INTEGER NOT NULL, [fish16_an] INTEGER NOT NULL, [fish17_type] INTEGER NOT NULL, [fish17_an] INTEGER NOT NULL, [fish18_type] INTEGER NOT NULL, [fish18_an] INTEGER NOT NULL, [fish19_type] INTEGER NOT NULL, [fish19_an] INTEGER NOT NULL, [fish20_type] INTEGER NOT NULL, [fish20_an] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userFriendBestDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [friendIDX] INTEGER NOT NULL, [flag] INTEGER NOT NULL, [flagTime] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userFriendDB] ([idx] INTEGER NOT NULL, [userAccIDX] INTEGER NOT NULL, [friendAccIDX] INTEGER NOT NULL, [sendCloverCnt] INTEGER NOT NULL, [sendCloverDate] INTEGER NOT NULL, [receiveCloverCnt] INTEGER NOT NULL, [receiveCloverDate] INTEGER NOT NULL, [receiveBCloverCnt] INTEGER NOT NULL, [flag] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userGameDB] ([accountIDX] INTEGER NOT NULL, [userState] INTEGER NOT NULL, [gameMoney] INTEGER NOT NULL, [playTime] INTEGER NOT NULL, [conn_count] INTEGER NOT NULL, [userLevel] INTEGER NOT NULL, [userEXP] INTEGER NOT NULL, [userFishingLevel] INTEGER NOT NULL, [userFishingEXP] INTEGER NOT NULL, [userExcavationLevel] INTEGER NOT NULL, [userExcavationEXP] INTEGER NOT NULL, [userHerocEXP] INTEGER NOT NULL, [userHerosEXP] INTEGER NOT NULL, [rencpoint] INTEGER NOT NULL, [renspoint] INTEGER NOT NULL, [relateLevel] INTEGER NOT NULL, [regionType] INTEGER NOT NULL, [refillData] INTEGER NOT NULL, [connDate] TEXT NOT NULL, [regDate] TEXT NOT NULL, [Practice] INTEGER NOT NULL, PRIMARY KEY ([accountIDX]));
CREATE TABLE [userGoldDB] ([idx] INTEGER NOT NULL, [AccountIDX] INTEGER NOT NULL, [OriginalAmount] INTEGER NOT NULL, [Amount] INTEGER NOT NULL, [GoldType] INTEGER NOT NULL, [GoldStatus] INTEGER NOT NULL, [ExpirationDate] TEXT NOT NULL, [RegDate] TEXT NOT NULL, PRIMARY KEY ([AccountIDX], [GoldType], [ExpirationDate], [idx]));
CREATE TABLE [userGuildAttendDB] ([InitDate] TEXT NOT NULL, [AccountIDX] INTEGER NOT NULL, [GuildIDX] INTEGER NOT NULL, [RegDate] TEXT NOT NULL, PRIMARY KEY ([InitDate], [AccountIDX]));
CREATE TABLE [userGuildDB] ([idx] INTEGER NOT NULL, [guildName] TEXT NOT NULL, [aboutguild] TEXT NOT NULL, [guildmark] INTEGER NOT NULL, [ranking] INTEGER NOT NULL, [point] INTEGER NOT NULL, [todaypoint] INTEGER NOT NULL, [guildLevel] INTEGER NOT NULL, [maxcount] INTEGER NOT NULL, [membercount] INTEGER NOT NULL, [today_vc] INTEGER NOT NULL, [total_vc] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([guildName]));
CREATE TABLE [userGuildHQDB] ([GuildIDX] INTEGER NOT NULL, [ItemSerial] INTEGER NOT NULL, [ItemCode] INTEGER NOT NULL, [ItemXZ] INTEGER NOT NULL, [ItemY] INTEGER NOT NULL, [ItemRotate] INTEGER NOT NULL, [ItemDecoScore] INTEGER NOT NULL, [ItemStatus] INTEGER NOT NULL, PRIMARY KEY ([GuildIDX], [ItemSerial]));
CREATE TABLE [userGuildHQVerDB] ([GuildIDX] INTEGER NOT NULL, [VerStatus] INTEGER NOT NULL, [UptDate] TEXT NOT NULL, [RegDate] TEXT NOT NULL, PRIMARY KEY ([GuildIDX]));
CREATE TABLE [userGuildInfoDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [guildIDX] INTEGER NOT NULL, [guildPosition] TEXT, [guildJoinType] INTEGER NOT NULL, [msg] INTEGER NOT NULL, [AttendDate] TEXT NOT NULL, [GuildRankDate] TEXT NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userHqDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [class1_type] INTEGER NOT NULL, [class1_posx] INTEGER NOT NULL, [class1_posy] INTEGER NOT NULL, [class2_type] INTEGER NOT NULL, [class2_posx] INTEGER NOT NULL, [class2_posy] INTEGER NOT NULL, [class3_type] INTEGER NOT NULL, [class3_posx] INTEGER NOT NULL, [class3_posy] INTEGER NOT NULL, [class4_type] INTEGER NOT NULL, [class4_posx] INTEGER NOT NULL, [class4_posy] INTEGER NOT NULL, [class5_type] INTEGER NOT NULL, [class5_posx] INTEGER NOT NULL, [class5_posy] INTEGER NOT NULL, [class6_type] INTEGER NOT NULL, [class6_posx] INTEGER NOT NULL, [class6_posy] INTEGER NOT NULL, [class7_type] INTEGER NOT NULL, [class7_posx] INTEGER NOT NULL, [class7_posy] INTEGER NOT NULL, [class8_type] INTEGER NOT NULL, [class8_posx] INTEGER NOT NULL, [class8_posy] INTEGER NOT NULL, [class9_type] INTEGER NOT NULL, [class9_posx] INTEGER NOT NULL, [class9_posy] INTEGER NOT NULL, [class10_type] INTEGER NOT NULL, [class10_posx] INTEGER NOT NULL, [class10_posy] INTEGER NOT NULL, [lockType] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountIDX]));
CREATE TABLE [userInfoBDB] ([accountIDX] INTEGER NOT NULL, [tel] TEXT, [mobile] TEXT, [zipcode] TEXT NOT NULL, [address1] TEXT NOT NULL, [address2] TEXT NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountIDX]));
CREATE TABLE [userInfoDB] ([accountIDX] INTEGER NOT NULL, [visit_count] INTEGER NOT NULL, [checkDate] TEXT NOT NULL, [rec_index] INTEGER NOT NULL, [rec_inc] INTEGER NOT NULL, [rec_dec] INTEGER NOT NULL, [cnnDate] TEXT NOT NULL, [clickDate] TEXT, [userIP] TEXT, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountIDX]));
CREATE TABLE [userItemDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [item1_type] INTEGER NOT NULL, [item1_code] INTEGER NOT NULL, [item2_type] INTEGER NOT NULL, [item2_code] INTEGER NOT NULL, [item3_type] INTEGER NOT NULL, [item3_code] INTEGER NOT NULL, [item4_type] INTEGER NOT NULL, [item4_code] INTEGER NOT NULL, [item5_type] INTEGER NOT NULL, [item5_code] INTEGER NOT NULL, [item6_type] INTEGER NOT NULL, [item6_code] INTEGER NOT NULL, [item7_type] INTEGER NOT NULL, [item7_code] INTEGER NOT NULL, [item8_type] INTEGER NOT NULL, [item8_code] INTEGER NOT NULL, [item9_type] INTEGER NOT NULL, [item9_code] INTEGER NOT NULL, [item10_type] INTEGER NOT NULL, [item10_code] INTEGER NOT NULL, [item11_type] INTEGER NOT NULL, [item11_code] INTEGER NOT NULL, [item12_type] INTEGER NOT NULL, [item12_code] INTEGER NOT NULL, [item13_type] INTEGER NOT NULL, [item13_code] INTEGER NOT NULL, [item14_type] INTEGER NOT NULL, [item14_code] INTEGER NOT NULL, [item15_type] INTEGER NOT NULL, [item15_code] INTEGER NOT NULL, [item16_type] INTEGER NOT NULL, [item16_code] INTEGER NOT NULL, [item17_type] INTEGER NOT NULL, [item17_code] INTEGER NOT NULL, [item18_type] INTEGER NOT NULL, [item18_code] INTEGER NOT NULL, [item19_type] INTEGER NOT NULL, [item19_code] INTEGER NOT NULL, [item20_type] INTEGER NOT NULL, [item20_code] INTEGER NOT NULL);
CREATE TABLE [userItemEtcDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [item1_type] INTEGER NOT NULL, [item1_value1] INTEGER NOT NULL, [item1_value2] INTEGER NOT NULL, [item2_type] INTEGER NOT NULL, [item2_value1] INTEGER NOT NULL, [item2_value2] INTEGER NOT NULL, [item3_type] INTEGER NOT NULL, [item3_value1] INTEGER NOT NULL, [item3_value2] INTEGER NOT NULL, [item4_type] INTEGER NOT NULL, [item4_value1] INTEGER NOT NULL, [item4_value2] INTEGER NOT NULL, [item5_type] INTEGER NOT NULL, [item5_value1] INTEGER NOT NULL, [item5_value2] INTEGER NOT NULL, [item6_type] INTEGER NOT NULL, [item6_value1] INTEGER NOT NULL, [item6_value2] INTEGER NOT NULL, [item7_type] INTEGER NOT NULL, [item7_value1] INTEGER NOT NULL, [item7_value2] INTEGER NOT NULL, [item8_type] INTEGER NOT NULL, [item8_value1] INTEGER NOT NULL, [item8_value2] INTEGER NOT NULL, [item9_type] INTEGER NOT NULL, [item9_value1] INTEGER NOT NULL, [item9_value2] INTEGER NOT NULL, [item10_type] INTEGER NOT NULL, [item10_value1] INTEGER NOT NULL, [item10_value2] INTEGER NOT NULL, [item11_type] INTEGER NOT NULL, [item11_value1] INTEGER NOT NULL, [item11_value2] INTEGER NOT NULL, [item12_type] INTEGER NOT NULL, [item12_value1] INTEGER NOT NULL, [item12_value2] INTEGER NOT NULL, [item13_type] INTEGER NOT NULL, [item13_value1] INTEGER NOT NULL, [item13_value2] INTEGER NOT NULL, [item14_type] INTEGER NOT NULL, [item14_value1] INTEGER NOT NULL, [item14_value2] INTEGER NOT NULL, [item15_type] INTEGER NOT NULL, [item15_value1] INTEGER NOT NULL, [item15_value2] INTEGER NOT NULL, [item16_type] INTEGER NOT NULL, [item16_value1] INTEGER NOT NULL, [item16_value2] INTEGER NOT NULL, [item17_type] INTEGER NOT NULL, [item17_value1] INTEGER NOT NULL, [item17_value2] INTEGER NOT NULL, [item18_type] INTEGER NOT NULL, [item18_value1] INTEGER NOT NULL, [item18_value2] INTEGER NOT NULL, [item19_type] INTEGER NOT NULL, [item19_value1] INTEGER NOT NULL, [item19_value2] INTEGER NOT NULL, [item20_type] INTEGER NOT NULL, [item20_value1] INTEGER NOT NULL, [item20_value2] INTEGER NOT NULL);
CREATE TABLE [userItemExtraDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [item1_type] INTEGER NOT NULL, [item1_reinforce] INTEGER NOT NULL, [item1_slotindex] INTEGER NOT NULL, [item1_tradetype] INTEGER NOT NULL, [item1_mcustom] INTEGER NOT NULL, [item1_fcustom] INTEGER NOT NULL, [item1_failexp] INTEGER NOT NULL, [item1_limitType] INTEGER NOT NULL, [item1_limitDate] TEXT NOT NULL, [item2_type] INTEGER NOT NULL, [item2_reinforce] INTEGER NOT NULL, [item2_slotindex] INTEGER NOT NULL, [item2_tradetype] INTEGER NOT NULL, [item2_mcustom] INTEGER NOT NULL, [item2_fcustom] INTEGER NOT NULL, [item2_failexp] INTEGER NOT NULL, [item2_limitType] INTEGER NOT NULL, [item2_limitDate] TEXT NOT NULL, [item3_type] INTEGER NOT NULL, [item3_reinforce] INTEGER NOT NULL, [item3_slotindex] INTEGER NOT NULL, [item3_tradetype] INTEGER NOT NULL, [item3_mcustom] INTEGER NOT NULL, [item3_fcustom] INTEGER NOT NULL, [item3_failexp] INTEGER NOT NULL, [item3_limitType] INTEGER NOT NULL, [item3_limitDate] TEXT NOT NULL, [item4_type] INTEGER NOT NULL, [item4_reinforce] INTEGER NOT NULL, [item4_slotindex] INTEGER NOT NULL, [item4_tradetype] INTEGER NOT NULL, [item4_mcustom] INTEGER NOT NULL, [item4_fcustom] INTEGER NOT NULL, [item4_failexp] INTEGER NOT NULL, [item4_limitType] INTEGER NOT NULL, [item4_limitDate] TEXT NOT NULL, [item5_type] INTEGER NOT NULL, [item5_reinforce] INTEGER NOT NULL, [item5_slotindex] INTEGER NOT NULL, [item5_tradetype] INTEGER NOT NULL, [item5_mcustom] INTEGER NOT NULL, [item5_fcustom] INTEGER NOT NULL, [item5_failexp] INTEGER NOT NULL, [item5_limitType] INTEGER NOT NULL, [item5_limitDate] TEXT NOT NULL, [item6_type] INTEGER NOT NULL, [item6_reinforce] INTEGER NOT NULL, [item6_slotindex] INTEGER NOT NULL, [item6_tradetype] INTEGER NOT NULL, [item6_mcustom] INTEGER NOT NULL, [item6_fcustom] INTEGER NOT NULL, [item6_failexp] INTEGER NOT NULL, [item6_limitType] INTEGER NOT NULL, [item6_limitDate] TEXT NOT NULL, [item7_type] INTEGER NOT NULL, [item7_reinforce] INTEGER NOT NULL, [item7_slotindex] INTEGER NOT NULL, [item7_tradetype] INTEGER NOT NULL, [item7_mcustom] INTEGER NOT NULL, [item7_fcustom] INTEGER NOT NULL, [item7_failexp] INTEGER NOT NULL, [item7_limitType] INTEGER NOT NULL, [item7_limitDate] TEXT NOT NULL, [item8_type] INTEGER NOT NULL, [item8_reinforce] INTEGER NOT NULL, [item8_slotindex] INTEGER NOT NULL, [item8_tradetype] INTEGER NOT NULL, [item8_mcustom] INTEGER NOT NULL, [item8_fcustom] INTEGER NOT NULL, [item8_failexp] INTEGER NOT NULL, [item8_limitType] INTEGER NOT NULL, [item8_limitDate] TEXT NOT NULL, [item9_type] INTEGER NOT NULL, [item9_reinforce] INTEGER NOT NULL, [item9_slotindex] INTEGER NOT NULL, [item9_tradetype] INTEGER NOT NULL, [item9_mcustom] INTEGER NOT NULL, [item9_fcustom] INTEGER NOT NULL, [item9_failexp] INTEGER NOT NULL, [item9_limitType] INTEGER NOT NULL, [item9_limitDate] TEXT NOT NULL, [item10_type] INTEGER NOT NULL, [item10_reinforce] INTEGER NOT NULL, [item10_slotindex] INTEGER NOT NULL, [item10_tradetype] INTEGER NOT NULL, [item10_mcustom] INTEGER NOT NULL, [item10_fcustom] INTEGER NOT NULL, [item10_failexp] INTEGER NOT NULL, [item10_limitType] INTEGER NOT NULL, [item10_limitDate] TEXT NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userItemMedalDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [item1_type] INTEGER NOT NULL, [item1_class] INTEGER NOT NULL, [item1_limitType] INTEGER NOT NULL, [item1_limitDate] TEXT NOT NULL, [item2_type] INTEGER NOT NULL, [item2_class] INTEGER NOT NULL, [item2_limitType] INTEGER NOT NULL, [item2_limitDate] TEXT NOT NULL, [item3_type] INTEGER NOT NULL, [item3_class] INTEGER NOT NULL, [item3_limitType] INTEGER NOT NULL, [item3_limitDate] TEXT NOT NULL, [item4_type] INTEGER NOT NULL, [item4_class] INTEGER NOT NULL, [item4_limitType] INTEGER NOT NULL, [item4_limitDate] TEXT NOT NULL, [item5_type] INTEGER NOT NULL, [item5_class] INTEGER NOT NULL, [item5_limitType] INTEGER NOT NULL, [item5_limitDate] TEXT NOT NULL, [item6_type] INTEGER NOT NULL, [item6_class] INTEGER NOT NULL, [item6_limitType] INTEGER NOT NULL, [item6_limitDate] TEXT NOT NULL, [item7_type] INTEGER NOT NULL, [item7_class] INTEGER NOT NULL, [item7_limitType] INTEGER NOT NULL, [item7_limitDate] TEXT NOT NULL, [item8_type] INTEGER NOT NULL, [item8_class] INTEGER NOT NULL, [item8_limitType] INTEGER NOT NULL, [item8_limitDate] TEXT NOT NULL, [item9_type] INTEGER NOT NULL, [item9_class] INTEGER NOT NULL, [item9_limitType] INTEGER NOT NULL, [item9_limitDate] TEXT NOT NULL, [item10_type] INTEGER NOT NULL, [item10_class] INTEGER NOT NULL, [item10_limitType] INTEGER NOT NULL, [item10_limitDate] TEXT NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userItemMedalExtendDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [medal1_class] INTEGER NOT NULL, [medal1_slot] INTEGER NOT NULL, [medal1_period] INTEGER NOT NULL, [medal2_class] INTEGER NOT NULL, [medal2_slot] INTEGER NOT NULL, [medal2_period] INTEGER NOT NULL, [medal3_class] INTEGER NOT NULL, [medal3_slot] INTEGER NOT NULL, [medal3_period] INTEGER NOT NULL, [medal4_class] INTEGER NOT NULL, [medal4_slot] INTEGER NOT NULL, [medal4_period] INTEGER NOT NULL, [medal5_class] INTEGER NOT NULL, [medal5_slot] INTEGER NOT NULL, [medal5_period] INTEGER NOT NULL, [medal6_class] INTEGER NOT NULL, [medal6_slot] INTEGER NOT NULL, [medal6_period] INTEGER NOT NULL, [medal7_class] INTEGER NOT NULL, [medal7_slot] INTEGER NOT NULL, [medal7_period] INTEGER NOT NULL, [medal8_class] INTEGER NOT NULL, [medal8_slot] INTEGER NOT NULL, [medal8_period] INTEGER NOT NULL, [medal9_class] INTEGER NOT NULL, [medal9_slot] INTEGER NOT NULL, [medal9_period] INTEGER NOT NULL, [medal10_class] INTEGER NOT NULL, [medal10_slot] INTEGER NOT NULL, [medal10_period] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userLeagueAuthDB] ([idx] INTEGER NOT NULL, [leagueIDX] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userLeagueCheerDB] ([idx] INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, [accountIDX] INTEGER NOT NULL, [leagueIDX] INTEGER NOT NULL, [teamIDX] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userLeagueInfoDB] ([idx] INTEGER NOT NULL, [leagueIDX] INTEGER NOT NULL, [leagueName] TEXT NOT NULL, [maxRound] INTEGER NOT NULL, [bannerA] INTEGER NOT NULL, [bannerB] INTEGER NOT NULL, [modeType] INTEGER NOT NULL, [maxcount] INTEGER NOT NULL, [autoType] INTEGER NOT NULL, [announce] TEXT NOT NULL, [recruitDate] INTEGER NOT NULL, [waitDate] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userLeagueListDB] ([idx] INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, [accountIDX] INTEGER NOT NULL, [startDate] INTEGER NOT NULL, [endDate] INTEGER NOT NULL, [leagueType] INTEGER NOT NULL, [leagueState] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userLeagueRewardCheerDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [rewardPeso] INTEGER NOT NULL, [startDate] INTEGER NOT NULL, [flag] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userLeagueRewardDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [startDate] INTEGER NOT NULL, [currentRound] INTEGER NOT NULL, [userFactionType] INTEGER NOT NULL, [winnerFactionType] INTEGER NOT NULL, [rewardPeso] INTEGER NOT NULL, [factionRank] INTEGER NOT NULL, [factionPoint] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userLeagueRewardUserDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [leagueIDX] INTEGER NOT NULL, [sendNickname] TEXT NOT NULL, [leagueName] TEXT NOT NULL, [roundnum] INTEGER NOT NULL, [maxRound] INTEGER NOT NULL, [presentCode1] INTEGER NOT NULL, [presentCode2] INTEGER NOT NULL, [presentCode3] INTEGER NOT NULL, [presentCode4] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userLeagueRoundDB] ([idx] INTEGER NOT NULL, [infoIDX] INTEGER NOT NULL, [roundType1_playDate] INTEGER NOT NULL, [roundType1_presentCode1] INTEGER NOT NULL, [roundType1_presentCode2] INTEGER NOT NULL, [roundType1_presentCode3] INTEGER NOT NULL, [roundType1_presentCode4] INTEGER NOT NULL, [roundType2_playDate] INTEGER NOT NULL, [roundType2_presentCode1] INTEGER NOT NULL, [roundType2_presentCode2] INTEGER NOT NULL, [roundType2_presentCode3] INTEGER NOT NULL, [roundType2_presentCode4] INTEGER NOT NULL, [roundType3_playDate] INTEGER NOT NULL, [roundType3_presentCode1] INTEGER NOT NULL, [roundType3_presentCode2] INTEGER NOT NULL, [roundType3_presentCode3] INTEGER NOT NULL, [roundType3_presentCode4] INTEGER NOT NULL, [roundType4_playDate] INTEGER NOT NULL, [roundType4_presentCode1] INTEGER NOT NULL, [roundType4_presentCode2] INTEGER NOT NULL, [roundType4_presentCode3] INTEGER NOT NULL, [roundType4_presentCode4] INTEGER NOT NULL, [roundType5_playDate] INTEGER NOT NULL, [roundType5_presentCode1] INTEGER NOT NULL, [roundType5_presentCode2] INTEGER NOT NULL, [roundType5_presentCode3] INTEGER NOT NULL, [roundType5_presentCode4] INTEGER NOT NULL, [roundType6_playDate] INTEGER NOT NULL, [roundType6_presentCode1] INTEGER NOT NULL, [roundType6_presentCode2] INTEGER NOT NULL, [roundType6_presentCode3] INTEGER NOT NULL, [roundType6_presentCode4] INTEGER NOT NULL, [roundType7_playDate] INTEGER NOT NULL, [roundType7_presentCode1] INTEGER NOT NULL, [roundType7_presentCode2] INTEGER NOT NULL, [roundType7_presentCode3] INTEGER NOT NULL, [roundType7_presentCode4] INTEGER NOT NULL, [roundType8_playDate] INTEGER NOT NULL, [roundType8_presentCode1] INTEGER NOT NULL, [roundType8_presentCode2] INTEGER NOT NULL, [roundType8_presentCode3] INTEGER NOT NULL, [roundType8_presentCode4] INTEGER NOT NULL, [roundType9_playDate] INTEGER NOT NULL, [roundType9_presentCode1] INTEGER NOT NULL, [roundType9_presentCode2] INTEGER NOT NULL, [roundType9_presentCode3] INTEGER NOT NULL, [roundType9_presentCode4] INTEGER NOT NULL, [roundType10_playDate] INTEGER NOT NULL, [roundType10_presentCode1] INTEGER NOT NULL, [roundType10_presentCode2] INTEGER NOT NULL, [roundType10_presentCode3] INTEGER NOT NULL, [roundType10_presentCode4] INTEGER NOT NULL, [roundType11_playDate] INTEGER NOT NULL, [roundType11_presentCode1] INTEGER NOT NULL, [roundType11_presentCode2] INTEGER NOT NULL, [roundType11_presentCode3] INTEGER NOT NULL, [roundType11_presentCode4] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userLeagueTeamDB] ([idx] INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, [leagueIDX] INTEGER NOT NULL, [teamName] TEXT NOT NULL, [leaderIDX] INTEGER NOT NULL, [leagueStartPosition] INTEGER NOT NULL, [leaguePosition] INTEGER NOT NULL, [maxcount] INTEGER NOT NULL, [cheerPoint] INTEGER NOT NULL, [currentRound] INTEGER NOT NULL, [factionPoint] INTEGER NOT NULL, [factionType] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userLeagueTeamMemberDB] ([idx] INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, [accountIDX] INTEGER NOT NULL, [leagueIDX] INTEGER NOT NULL, [teamIDX] INTEGER NOT NULL, [joinType] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userLeagueWinnerHistoryDB] ([idx] INTEGER NOT NULL, [leagueTitle] TEXT NOT NULL, [startDate] INTEGER NOT NULL, [endDate] INTEGER NOT NULL, [teamIDX] INTEGER NOT NULL, [teamName] TEXT, [factionName] TEXT, [factionType] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userLeagueWinnerInfoDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [nickName] TEXT NOT NULL, [userLevel] INTEGER NOT NULL, [historyIDX] INTEGER NOT NULL, [teamIDX] INTEGER NOT NULL, [classType] INTEGER NOT NULL, [classGender] INTEGER NOT NULL, [classFace] INTEGER NOT NULL, [classHair] INTEGER NOT NULL, [classSkinColor] INTEGER NOT NULL, [classHairColor] INTEGER NOT NULL, [classUnderwear] INTEGER NOT NULL, [classGear1] INTEGER NOT NULL, [classGear2] INTEGER NOT NULL, [classGear3] INTEGER NOT NULL, [classGear4] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userLoginDB] ([accountIDX] INTEGER NOT NULL, [encodeKey] TEXT NOT NULL, [gameServerID] INTEGER NOT NULL, [userIP] TEXT, [connDate] TEXT NOT NULL, PRIMARY KEY ([accountIDX]));
CREATE TABLE [userMatchModeDailyRankingDB] ([Ranking] INTEGER NOT NULL, [AccountIDX] INTEGER NOT NULL, [MatchPoint] INTEGER NOT NULL, PRIMARY KEY ([AccountIDX], [Ranking]));
CREATE TABLE [userMatchModeDB] ([AccountIDX] INTEGER NOT NULL, [MatchPoint] INTEGER NOT NULL, [WinCount] INTEGER NOT NULL, [LoseCount] INTEGER NOT NULL, [MaxWinCount] INTEGER NOT NULL, [PlayCount] INTEGER NOT NULL, [RankMMR] INTEGER NOT NULL, [RegDate] TEXT NOT NULL, PRIMARY KEY ([AccountIDX]));
CREATE TABLE [userMatchModeHistoryDB] ([idx] INTEGER NOT NULL, [AccountIDX] INTEGER NOT NULL, [WinLoseType] INTEGER NOT NULL, [TeamWinCount] INTEGER NOT NULL, [OpposingTeamWinCount] INTEGER NOT NULL, [PlayDate] TEXT NOT NULL, PRIMARY KEY ([AccountIDX], [PlayDate], [idx]));
CREATE TABLE [userMatchModeSeasonRankingDB] ([AccountIDX] INTEGER NOT NULL, [S1_Ranking] INTEGER NOT NULL, [S1_MatchPoint] INTEGER NOT NULL, [S1_SeasonDate] TEXT NOT NULL, [S2_Ranking] INTEGER NOT NULL, [S2_MatchPoint] INTEGER NOT NULL, [S2_SeasonDate] TEXT NOT NULL, [S3_Ranking] INTEGER NOT NULL, [S3_MatchPoint] INTEGER NOT NULL, [S3_SeasonDate] TEXT NOT NULL, [S4_Ranking] INTEGER NOT NULL, [S4_MatchPoint] INTEGER NOT NULL, [S4_SeasonDate] TEXT NOT NULL, [S5_Ranking] INTEGER NOT NULL, [S5_MatchPoint] INTEGER NOT NULL, [S5_SeasonDate] TEXT NOT NULL, [S6_Ranking] INTEGER NOT NULL, [S6_MatchPoint] INTEGER NOT NULL, [S6_SeasonDate] TEXT NOT NULL, [S7_Ranking] INTEGER NOT NULL, [S7_MatchPoint] INTEGER NOT NULL, [S7_SeasonDate] TEXT NOT NULL, PRIMARY KEY ([AccountIDX]));
CREATE TABLE [userMedalDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [medal1_type] INTEGER NOT NULL, [medal2_type] INTEGER NOT NULL, [medal3_type] INTEGER NOT NULL, [medal4_type] INTEGER NOT NULL, [medal5_type] INTEGER NOT NULL, [medal6_type] INTEGER NOT NULL, [medal7_type] INTEGER NOT NULL, [medal8_type] INTEGER NOT NULL, [medal9_type] INTEGER NOT NULL, [medal10_type] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userMedalDB_receive] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [medalIDX] INTEGER NOT NULL, [flag] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userMemberDB] ([accountIDX] INTEGER NOT NULL, [userID] TEXT NOT NULL, [nickName] TEXT NOT NULL, [userPWD] TEXT NOT NULL, [email] TEXT NOT NULL, [mailling] INTEGER NOT NULL, [userType] INTEGER NOT NULL, [joinType] INTEGER NOT NULL, [eventType] INTEGER NOT NULL, [cpType] INTEGER NOT NULL, [limitType] INTEGER NOT NULL, [limitDate] TEXT NOT NULL, [trackingcode] INTEGER NOT NULL, [regDate] TEXT NOT NULL, [makeType] INTEGER NOT NULL, PRIMARY KEY ([userID]));
CREATE TABLE [userMissionDB] ([AccountIDX] INTEGER NOT NULL, [MissionType] INTEGER NOT NULL, [MissionCode] INTEGER NOT NULL, [MissionValue] INTEGER NOT NULL, [MissionStatus] INTEGER NOT NULL, [RegDate] TEXT NOT NULL, PRIMARY KEY ([AccountIDX], [MissionType], [MissionCode]));
CREATE TABLE [userNameDB] ([accountIDX] INTEGER NOT NULL, [userName] TEXT NOT NULL, [userBirthday] TEXT NOT NULL, [userJumin1] TEXT, [userJumin2] TEXT, [virtualJumin] TEXT, [ipinCI] TEXT, [ipinDI] TEXT, [userEnCode] TEXT NOT NULL, [userNumber] INTEGER NOT NULL, [userGender] INTEGER NOT NULL, [returnValue] INTEGER NOT NULL, [userIP] TEXT, [realType] INTEGER NOT NULL, [certType] TEXT NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountIDX]));
CREATE TABLE [userOakBarrelDB] ([AccountIDX] INTEGER NOT NULL, [Step] INTEGER NOT NULL, [Slot1] INTEGER NOT NULL, [Slot2] INTEGER NOT NULL, [Slot3] INTEGER NOT NULL, [Slot4] INTEGER NOT NULL, [Slot5] INTEGER NOT NULL, [Slot6] INTEGER NOT NULL, [Slot7] INTEGER NOT NULL, [Slot8] INTEGER NOT NULL, [Slot9] INTEGER NOT NULL, [Slot10] INTEGER NOT NULL, [Slot11] INTEGER NOT NULL, [Slot12] INTEGER NOT NULL, [UpdateTime] TEXT NOT NULL, [RegDate] TEXT NOT NULL, [LimitSword] INTEGER NOT NULL);
CREATE TABLE [userOAuthDB] ([idx] INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, [accountIDX] INTEGER NOT NULL, [provider] TEXT NOT NULL, [providerUserID] TEXT NOT NULL, [email] TEXT, [regDate] TEXT NOT NULL);
CREATE TABLE [userParentDB] ([accountIDX] INTEGER NOT NULL, [parentName] TEXT NOT NULL, [parentBirth] TEXT, [parentJumin1] TEXT, [parentJumin2] TEXT, [parentEncode] TEXT, [parentNumber] INTEGER, [email] TEXT, [agreeSDate] TEXT, [agreeEDate] TEXT, [agreeType] INTEGER NOT NULL, [logIDX] INTEGER NOT NULL, [regDate] TEXT NOT NULL, [Certify_Type] TEXT, [PIpinCI] TEXT, [PIpinDI] TEXT, PRIMARY KEY ([accountIDX]));
CREATE TABLE [userPCBangPlayTimeDB] ([PCBangDate] TEXT NOT NULL, [PCBangIndex] INTEGER NOT NULL, [AccountIDX] INTEGER NOT NULL, [PCBangPlayTime] INTEGER NOT NULL, PRIMARY KEY ([PCBangDate], [PCBangIndex], [AccountIDX]));
CREATE TABLE [userPCBangPregentDB] ([PCBangDate] TEXT NOT NULL, [AccountIDX] INTEGER NOT NULL, [PCBangCount] INTEGER NOT NULL, [PCBangBonusCash] INTEGER NOT NULL, PRIMARY KEY ([PCBangDate], [AccountIDX]));
CREATE TABLE [userPersonalHQDB] ([AccountIDX] INTEGER NOT NULL, [ItemSerial] INTEGER NOT NULL, [ItemCode] INTEGER NOT NULL, [ItemXZ] INTEGER NOT NULL, [ItemY] INTEGER NOT NULL, [ItemRotate] INTEGER NOT NULL, [ItemDecoScore] INTEGER NOT NULL, [ItemStatus] INTEGER NOT NULL, PRIMARY KEY ([AccountIDX], [ItemSerial]));
CREATE TABLE [userPetDB] ([accountIDX] INTEGER NOT NULL, [petIDX] INTEGER NOT NULL, [petCode] INTEGER NOT NULL, [petRank] INTEGER NOT NULL, [petLevel] INTEGER NOT NULL, [petExp] INTEGER NOT NULL, [petEquip] INTEGER NOT NULL, [petStatus] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountIDX], [petIDX]));
CREATE TABLE [userPieceDB] ([idx] INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, [accountIDX] INTEGER NOT NULL, [piece1_type] INTEGER NOT NULL, [piece1_value1] INTEGER NOT NULL, [piece1_value2] INTEGER NOT NULL, [piece1_cnt] INTEGER NOT NULL, [piece2_type] INTEGER NOT NULL, [piece2_value1] INTEGER NOT NULL, [piece2_value2] INTEGER NOT NULL, [piece2_cnt] INTEGER NOT NULL, [piece3_type] INTEGER NOT NULL, [piece3_value1] INTEGER NOT NULL, [piece3_value2] INTEGER NOT NULL, [piece3_cnt] INTEGER NOT NULL, [piece4_type] INTEGER NOT NULL, [piece4_value1] INTEGER NOT NULL, [piece4_value2] INTEGER NOT NULL, [piece4_cnt] INTEGER NOT NULL, [piece5_type] INTEGER NOT NULL, [piece5_value1] INTEGER NOT NULL, [piece5_value2] INTEGER NOT NULL, [piece5_cnt] INTEGER NOT NULL, [piece6_type] INTEGER NOT NULL, [piece6_value1] INTEGER NOT NULL, [piece6_value2] INTEGER NOT NULL, [piece6_cnt] INTEGER NOT NULL, [piece7_type] INTEGER NOT NULL, [piece7_value1] INTEGER NOT NULL, [piece7_value2] INTEGER NOT NULL, [piece7_cnt] INTEGER NOT NULL, [piece8_type] INTEGER NOT NULL, [piece8_value1] INTEGER NOT NULL, [piece8_value2] INTEGER NOT NULL, [piece8_cnt] INTEGER NOT NULL, [piece9_type] INTEGER NOT NULL, [piece9_value1] INTEGER NOT NULL, [piece9_value2] INTEGER NOT NULL, [piece9_cnt] INTEGER NOT NULL, [piece10_type] INTEGER NOT NULL, [piece10_value1] INTEGER NOT NULL, [piece10_value2] INTEGER NOT NULL, [piece10_cnt] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userPopStoreDB] ([AccountIDX] INTEGER NOT NULL, [PopupSlot] INTEGER NOT NULL, [RegDate] TEXT NOT NULL, PRIMARY KEY ([AccountIDX], [PopupSlot]));
CREATE TABLE [userPracticeDB] ([AccountIDX] INTEGER NOT NULL, [PracticeIDX] INTEGER NOT NULL, [PracticeCount] INTEGER NOT NULL, [PracticeGrade] INTEGER NOT NULL, [PracticeTime] INTEGER NOT NULL, [PracticeRank] INTEGER NOT NULL, [UpdateDate] TEXT NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([AccountIDX], [PracticeIDX]));
CREATE TABLE [userPracticeDB_Present] ([PresentValue] INTEGER NOT NULL, [PresentType1] INTEGER NOT NULL, [Code1] INTEGER NOT NULL, [Value1] INTEGER NOT NULL, [PresentType2] INTEGER NOT NULL, [Code2] INTEGER NOT NULL, [Value2] INTEGER NOT NULL, [PresentType3] INTEGER NOT NULL, [Code3] INTEGER NOT NULL, [Value3] INTEGER NOT NULL, [PresentType4] INTEGER NOT NULL, [Code4] INTEGER NOT NULL, [Value4] INTEGER NOT NULL, [PresentType5] INTEGER NOT NULL, [Code5] INTEGER NOT NULL, [Value5] INTEGER NOT NULL, PRIMARY KEY ([PresentValue]));
CREATE TABLE [userPracticeDB_Ranking] ([StartDate] TEXT NOT NULL, [EndDate] TEXT NOT NULL, [AccountIDX] INTEGER NOT NULL, [PracticeIDX] INTEGER NOT NULL, [PracticeRank] INTEGER NOT NULL, [sendIDX] INTEGER NOT NULL, [value] INTEGER NOT NULL, [msgType] INTEGER, [limitDate] TEXT);
CREATE TABLE [userPresentDB] ([idx] INTEGER NOT NULL, [sendIDX] INTEGER NOT NULL, [receiveIDX] INTEGER NOT NULL, [presentType] INTEGER NOT NULL, [value1] INTEGER NOT NULL, [value2] INTEGER NOT NULL, [value3] INTEGER NOT NULL, [value4] INTEGER NOT NULL, [msgType] INTEGER NOT NULL, [flag] INTEGER NOT NULL, [limitDate] TEXT NOT NULL, [regDate] TEXT NOT NULL, [TranIDX] INTEGER NOT NULL);
CREATE TABLE [userQuestCompleteDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [quest1_type] INTEGER NOT NULL, [quest1_time] INTEGER NOT NULL, [quest2_type] INTEGER NOT NULL, [quest2_time] INTEGER NOT NULL, [quest3_type] INTEGER NOT NULL, [quest3_time] INTEGER NOT NULL, [quest4_type] INTEGER NOT NULL, [quest4_time] INTEGER NOT NULL, [quest5_type] INTEGER NOT NULL, [quest5_time] INTEGER NOT NULL, [quest6_type] INTEGER NOT NULL, [quest6_time] INTEGER NOT NULL, [quest7_type] INTEGER NOT NULL, [quest7_time] INTEGER NOT NULL, [quest8_type] INTEGER NOT NULL, [quest8_time] INTEGER NOT NULL, [quest9_type] INTEGER NOT NULL, [quest9_time] INTEGER NOT NULL, [quest10_type] INTEGER NOT NULL, [quest10_time] INTEGER NOT NULL, [quest11_type] INTEGER NOT NULL, [quest11_time] INTEGER NOT NULL, [quest12_type] INTEGER NOT NULL, [quest12_time] INTEGER NOT NULL, [quest13_type] INTEGER NOT NULL, [quest13_time] INTEGER NOT NULL, [quest14_type] INTEGER NOT NULL, [quest14_time] INTEGER NOT NULL, [quest15_type] INTEGER NOT NULL, [quest15_time] INTEGER NOT NULL, [quest16_type] INTEGER NOT NULL, [quest16_time] INTEGER NOT NULL, [quest17_type] INTEGER NOT NULL, [quest17_time] INTEGER NOT NULL, [quest18_type] INTEGER NOT NULL, [quest18_time] INTEGER NOT NULL, [quest19_type] INTEGER NOT NULL, [quest19_time] INTEGER NOT NULL, [quest20_type] INTEGER NOT NULL, [quest20_time] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userQuestDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [quest1_type] INTEGER NOT NULL, [quest1_value1] INTEGER NOT NULL, [quest1_value2] INTEGER NOT NULL, [quest1_time] INTEGER NOT NULL, [quest2_type] INTEGER NOT NULL, [quest2_value1] INTEGER NOT NULL, [quest2_value2] INTEGER NOT NULL, [quest2_time] INTEGER NOT NULL, [quest3_type] INTEGER NOT NULL, [quest3_value1] INTEGER NOT NULL, [quest3_value2] INTEGER NOT NULL, [quest3_time] INTEGER NOT NULL, [quest4_type] INTEGER NOT NULL, [quest4_value1] INTEGER NOT NULL, [quest4_value2] INTEGER NOT NULL, [quest4_time] INTEGER NOT NULL, [quest5_type] INTEGER NOT NULL, [quest5_value1] INTEGER NOT NULL, [quest5_value2] INTEGER NOT NULL, [quest5_time] INTEGER NOT NULL, [quest6_type] INTEGER NOT NULL, [quest6_value1] INTEGER NOT NULL, [quest6_value2] INTEGER NOT NULL, [quest6_time] INTEGER NOT NULL, [quest7_type] INTEGER NOT NULL, [quest7_value1] INTEGER NOT NULL, [quest7_value2] INTEGER NOT NULL, [quest7_time] INTEGER NOT NULL, [quest8_type] INTEGER NOT NULL, [quest8_value1] INTEGER NOT NULL, [quest8_value2] INTEGER NOT NULL, [quest8_time] INTEGER NOT NULL, [quest9_type] INTEGER NOT NULL, [quest9_value1] INTEGER NOT NULL, [quest9_value2] INTEGER NOT NULL, [quest9_time] INTEGER NOT NULL, [quest10_type] INTEGER NOT NULL, [quest10_value1] INTEGER NOT NULL, [quest10_value2] INTEGER NOT NULL, [quest10_time] INTEGER NOT NULL, [quest11_type] INTEGER NOT NULL, [quest11_value1] INTEGER NOT NULL, [quest11_value2] INTEGER NOT NULL, [quest11_time] INTEGER NOT NULL, [quest12_type] INTEGER NOT NULL, [quest12_value1] INTEGER NOT NULL, [quest12_value2] INTEGER NOT NULL, [quest12_time] INTEGER NOT NULL, [quest13_type] INTEGER NOT NULL, [quest13_value1] INTEGER NOT NULL, [quest13_value2] INTEGER NOT NULL, [quest13_time] INTEGER NOT NULL, [quest14_type] INTEGER NOT NULL, [quest14_value1] INTEGER NOT NULL, [quest14_value2] INTEGER NOT NULL, [quest14_time] INTEGER NOT NULL, [quest15_type] INTEGER NOT NULL, [quest15_value1] INTEGER NOT NULL, [quest15_value2] INTEGER NOT NULL, [quest15_time] INTEGER NOT NULL, [quest16_type] INTEGER NOT NULL, [quest16_value1] INTEGER NOT NULL, [quest16_value2] INTEGER NOT NULL, [quest16_time] INTEGER NOT NULL, [quest17_type] INTEGER NOT NULL, [quest17_value1] INTEGER NOT NULL, [quest17_value2] INTEGER NOT NULL, [quest17_time] INTEGER NOT NULL, [quest18_type] INTEGER NOT NULL, [quest18_value1] INTEGER NOT NULL, [quest18_value2] INTEGER NOT NULL, [quest18_time] INTEGER NOT NULL, [quest19_type] INTEGER NOT NULL, [quest19_value1] INTEGER NOT NULL, [quest19_value2] INTEGER NOT NULL, [quest19_time] INTEGER NOT NULL, [quest20_type] INTEGER NOT NULL, [quest20_value1] INTEGER NOT NULL, [quest20_value2] INTEGER NOT NULL, [quest20_time] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userRankingDB] ([accountIDX] INTEGER NOT NULL, [class] INTEGER NOT NULL, [battle] INTEGER NOT NULL, [training] INTEGER NOT NULL, [award] INTEGER NOT NULL, [medal] INTEGER NOT NULL, [tester] INTEGER NOT NULL, [region] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountIDX]));
CREATE TABLE [userRankingDB_award] ([accountIDX] INTEGER NOT NULL, [awardLevel] INTEGER NOT NULL, [expert] INTEGER NOT NULL, [award1_prev] INTEGER NOT NULL, [award1_now] INTEGER NOT NULL, [award1_point] INTEGER NOT NULL, [award2_prev] INTEGER NOT NULL, [award2_now] INTEGER NOT NULL, [award2_point] INTEGER NOT NULL, [award3_prev] INTEGER NOT NULL, [award3_now] INTEGER NOT NULL, [award3_point] INTEGER NOT NULL, [award4_prev] INTEGER NOT NULL, [award4_now] INTEGER NOT NULL, [award4_point] INTEGER NOT NULL, [award5_prev] INTEGER NOT NULL, [award5_now] INTEGER NOT NULL, [award5_point] INTEGER NOT NULL, [award6_prev] INTEGER NOT NULL, [award6_now] INTEGER NOT NULL, [award6_point] INTEGER NOT NULL, [award7_prev] INTEGER NOT NULL, [award7_now] INTEGER NOT NULL, [award7_point] INTEGER NOT NULL, [award8_prev] INTEGER NOT NULL, [award8_now] INTEGER NOT NULL, [award8_point] INTEGER NOT NULL, [award9_prev] INTEGER NOT NULL, [award9_now] INTEGER NOT NULL, [award9_point] INTEGER NOT NULL, [award10_prev] INTEGER NOT NULL, [award10_now] INTEGER NOT NULL, [award10_point] INTEGER NOT NULL, [award11_prev] INTEGER NOT NULL, [award11_now] INTEGER NOT NULL, [award11_point] INTEGER NOT NULL, [award12_prev] INTEGER NOT NULL, [award12_now] INTEGER NOT NULL, [award12_point] INTEGER NOT NULL, [award13_prev] INTEGER NOT NULL, [award13_now] INTEGER NOT NULL, [award13_point] INTEGER NOT NULL, [award14_prev] INTEGER NOT NULL, [award14_now] INTEGER NOT NULL, [award14_point] INTEGER NOT NULL, [award15_prev] INTEGER NOT NULL, [award15_now] INTEGER NOT NULL, [award15_point] INTEGER NOT NULL, [award16_prev] INTEGER NOT NULL, [award16_now] INTEGER NOT NULL, [award16_point] INTEGER NOT NULL, [award17_prev] INTEGER NOT NULL, [award17_now] INTEGER NOT NULL, [award17_point] INTEGER NOT NULL, [award18_prev] INTEGER NOT NULL, [award18_now] INTEGER NOT NULL, [award18_point] INTEGER NOT NULL, [award19_prev] INTEGER NOT NULL, [award19_now] INTEGER NOT NULL, [award19_point] INTEGER NOT NULL, [award20_prev] INTEGER NOT NULL, [award20_now] INTEGER NOT NULL, [award20_point] INTEGER NOT NULL, [award21_prev] INTEGER NOT NULL, [award21_now] INTEGER NOT NULL, [award21_point] INTEGER NOT NULL, [award22_prev] INTEGER NOT NULL, [award22_now] INTEGER NOT NULL, [award22_point] INTEGER NOT NULL, [award23_prev] INTEGER NOT NULL, [award23_now] INTEGER NOT NULL, [award23_point] INTEGER NOT NULL, [award24_prev] INTEGER NOT NULL, [award24_now] INTEGER NOT NULL, [award24_point] INTEGER NOT NULL, [award25_prev] INTEGER NOT NULL, [award25_now] INTEGER NOT NULL, [award25_point] INTEGER NOT NULL, [award26_prev] INTEGER NOT NULL, [award26_now] INTEGER NOT NULL, [award26_point] INTEGER NOT NULL, [award27_prev] INTEGER NOT NULL, [award27_now] INTEGER NOT NULL, [award27_point] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userRankingDB_award_top100] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [nickName] TEXT, [userLevel] INTEGER, [guildIDX] INTEGER, [awardType] INTEGER NOT NULL, [rank_prev] INTEGER NOT NULL, [rank_now] INTEGER NOT NULL, [awardEXP] INTEGER NOT NULL);
CREATE TABLE [userRankingDB_award_top3] ([idx] INTEGER NOT NULL, [awardType] INTEGER NOT NULL, [rank1_accountIDX] INTEGER NOT NULL, [rank1_userLevel] INTEGER NOT NULL, [rank1_nickName] TEXT NOT NULL, [rank2_accountIDX] INTEGER NOT NULL, [rank2_userLevel] INTEGER NOT NULL, [rank2_nickName] TEXT NOT NULL, [rank3_accountIDX] INTEGER NOT NULL, [rank3_userLevel] INTEGER NOT NULL, [rank3_nickName] TEXT NOT NULL, [dateIDX] TEXT NOT NULL);
CREATE TABLE [userRankingDB_battle] ([accountIDX] INTEGER NOT NULL, [win_fix] INTEGER NOT NULL, [tie_fix] INTEGER NOT NULL, [lose_fix] INTEGER NOT NULL, [win_prev] INTEGER NOT NULL, [tie_prev] INTEGER NOT NULL, [lose_prev] INTEGER NOT NULL, [rank_prev] INTEGER NOT NULL, [rank_now] INTEGER NOT NULL, [rank_total] INTEGER NOT NULL, [rank_battle] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountIDX]));
CREATE TABLE [userRankingDB_class] ([accountIDX] INTEGER, [classType] INTEGER, [prevRanking] INTEGER, [nowRanking] INTEGER, [userLevel] INTEGER, [expert] INTEGER);
CREATE TABLE [userRankingDB_class_top100] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [nickName] TEXT, [userLevel] INTEGER, [guildIDX] INTEGER, [classType] INTEGER NOT NULL, [rank_prev] INTEGER NOT NULL, [rank_now] INTEGER NOT NULL, [classLevel] INTEGER NOT NULL, [classEXP] INTEGER NOT NULL);
CREATE TABLE [userRankingDB_class_top3] ([idx] INTEGER NOT NULL, [classType] INTEGER NOT NULL, [rank1_accountIDX] INTEGER NOT NULL, [rank1_userLevel] INTEGER NOT NULL, [rank1_nickName] TEXT NOT NULL, [rank2_accountIDX] INTEGER NOT NULL, [rank2_userLevel] INTEGER NOT NULL, [rank2_nickName] TEXT NOT NULL, [rank3_accountIDX] INTEGER NOT NULL, [rank3_userLevel] INTEGER NOT NULL, [rank3_nickName] TEXT NOT NULL, [dateIDX] TEXT NOT NULL);
CREATE TABLE [userRankingDB_classgroup] ([accountIDX] INTEGER NOT NULL, [nickName] TEXT, [userLevel] INTEGER NOT NULL, [expert] INTEGER NOT NULL, [ranking_8day] INTEGER NOT NULL, [ranking_7day] INTEGER NOT NULL, [ranking_6day] INTEGER NOT NULL, [ranking_5day] INTEGER NOT NULL, [ranking_4day] INTEGER NOT NULL, [ranking_3day] INTEGER NOT NULL, [ranking_2day] INTEGER NOT NULL, [ranking_1day] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountIDX]));
CREATE TABLE [userRankingDB_classgroup_top10000] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [nickName] TEXT, [userLevel] INTEGER NOT NULL, [expert] INTEGER NOT NULL, [rank_now] INTEGER NOT NULL, [rank_prev] INTEGER NOT NULL, [guildIDX] INTEGER, [guildName] TEXT, [guildMark] INTEGER, [regDate] TEXT NOT NULL);
CREATE TABLE [userRankingDB_faction] ([accountIDX] INTEGER NOT NULL, [win_fix] INTEGER NOT NULL, [tie_fix] INTEGER NOT NULL, [lose_fix] INTEGER NOT NULL, [win_prev] INTEGER NOT NULL, [tie_prev] INTEGER NOT NULL, [lose_prev] INTEGER NOT NULL, [rank_prev] INTEGER NOT NULL, [rank_now] INTEGER NOT NULL, [rank_total] INTEGER NOT NULL, [rank_faction] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountIDX]));
CREATE TABLE [userRankingDB_hero] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [title] INTEGER NOT NULL, [daily] INTEGER NOT NULL, [dailyB] INTEGER NOT NULL, [season1] INTEGER NOT NULL, [season2] INTEGER NOT NULL, [season3] INTEGER NOT NULL, [season4] INTEGER NOT NULL, [season5] INTEGER NOT NULL, [season6] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userRankingDB_medal] ([accountIDX] INTEGER NOT NULL, [medalLevel] INTEGER NOT NULL, [expert] INTEGER NOT NULL, [ranking_prev] INTEGER NOT NULL, [ranking_now] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userRankingDB_temp] ([accountIDX] INTEGER NOT NULL, [rank] INTEGER);
CREATE TABLE [userRankingDB_temp_expert] ([accountIDX] INTEGER NOT NULL, [expert] INTEGER NOT NULL);
CREATE TABLE [userRankingDB_temp_level] ([accountIDX] INTEGER NOT NULL, [level] INTEGER NOT NULL, [expert] INTEGER NOT NULL);
CREATE TABLE [userRankingDB_temp_ranking] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL);
CREATE TABLE [userRecordBattleDB] ([accountIDX] INTEGER NOT NULL, [type1_win] INTEGER NOT NULL, [type1_lose] INTEGER NOT NULL, [type1_kill] INTEGER NOT NULL, [type1_death] INTEGER NOT NULL, [type2_win] INTEGER NOT NULL, [type2_lose] INTEGER NOT NULL, [type2_kill] INTEGER NOT NULL, [type2_death] INTEGER NOT NULL, [type3_win] INTEGER NOT NULL, [type3_lose] INTEGER NOT NULL, [type3_kill] INTEGER NOT NULL, [type3_death] INTEGER NOT NULL, [type4_win] INTEGER NOT NULL, [type4_lose] INTEGER NOT NULL, [type4_kill] INTEGER NOT NULL, [type4_death] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountIDX]));
CREATE TABLE [userRecordGuildDB] ([guildIDX] INTEGER NOT NULL, [type1_win] INTEGER NOT NULL, [type1_lose] INTEGER NOT NULL, [type1_kill] INTEGER NOT NULL, [type1_death] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([guildIDX]));
CREATE TABLE [userRegionDB] ([idx] INTEGER NOT NULL, [rt1point] INTEGER NOT NULL, [rt1point_bonus] INTEGER NOT NULL, [rt1point_today] INTEGER NOT NULL, [rt2point] INTEGER NOT NULL, [rt2point_bonus] INTEGER NOT NULL, [rt2point_today] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userRegionDB_compen] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [rt1point] INTEGER NOT NULL, [rt1point_bonus] INTEGER NOT NULL, [rt1count] INTEGER NOT NULL, [rt2point] INTEGER NOT NULL, [rt2point_bonus] INTEGER NOT NULL, [rt2count] INTEGER NOT NULL, [regionType] INTEGER NOT NULL, [renspoint] INTEGER NOT NULL, [regionRanking] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userRegionDB_history] ([idx] INTEGER NOT NULL, [point] INTEGER NOT NULL, [point_bonus] INTEGER NOT NULL, [membercount] INTEGER NOT NULL, [regionType] INTEGER NOT NULL, [season] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userRelativeLevelDB] ([accountIDX] INTEGER NOT NULL, [weekend_BackupLevel] INTEGER NOT NULL, [weekend_BackupExp] INTEGER NOT NULL, [init_time] INTEGER NOT NULL, [reward_State] INTEGER NOT NULL, [regDate] TEXT NOT NULL, PRIMARY KEY ([accountIDX]));
CREATE TABLE [userSBoxDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [presentType] INTEGER NOT NULL, [value1] INTEGER NOT NULL, [value2] INTEGER NOT NULL, [amount] INTEGER NOT NULL, [bonusAmount] INTEGER NOT NULL, [flag] INTEGER NOT NULL, [billingID] TEXT NOT NULL, [limitDate] TEXT NOT NULL, [regDate] TEXT NOT NULL);
CREATE TABLE [userSpentGoldDB] ([AccountIDX] INTEGER NOT NULL, [UserSpentGold] INTEGER NOT NULL, [RegDate] TEXT NOT NULL, PRIMARY KEY ([AccountIDX]));
CREATE TABLE [userSpentGoldLogDB] ([RegDate] TEXT NOT NULL, [AccountIDX] INTEGER NOT NULL, [UserSpentGold] INTEGER NOT NULL, PRIMARY KEY ([RegDate], [AccountIDX]));
CREATE TABLE [userSpiritDB] ([idx] INTEGER NOT NULL, [AccountIDX] INTEGER NOT NULL, [SpiritType] INTEGER NOT NULL, [SpiritCount] INTEGER NOT NULL, [RegDate] TEXT NOT NULL, PRIMARY KEY ([AccountIDX], [SpiritType]));
CREATE TABLE [userTimeCashDB] ([AccountIDX] INTEGER NOT NULL, [ItemCode] INTEGER NOT NULL, [ItemCount] INTEGER NOT NULL, [ItemStatus] INTEGER NOT NULL, [RegDate] TEXT NOT NULL, [StartDate] TEXT NOT NULL, [OverDate] TEXT NOT NULL, [UpdDate] TEXT NOT NULL, PRIMARY KEY ([AccountIDX], [ItemCode]));
CREATE TABLE [userTimeGateDB] ([AccountIDX] INTEGER NOT NULL, [GetTime] TEXT NOT NULL, PRIMARY KEY ([AccountIDX]));
CREATE TABLE [userTitleDB] ([AccountIDX] INTEGER NOT NULL, [TitleCode] INTEGER NOT NULL, [TitleValue] INTEGER NOT NULL, [TitleLevel] INTEGER NOT NULL, [TitlePremium] INTEGER NOT NULL, [TitleEquip] INTEGER NOT NULL, [TitleStatus] INTEGER NOT NULL, [UpdDate] TEXT NOT NULL, [RegDate] TEXT NOT NULL, PRIMARY KEY ([AccountIDX], [TitleCode]));
CREATE TABLE [userTradeDB] ([idx] INTEGER NOT NULL, [accountIDX] INTEGER NOT NULL, [itemType] INTEGER NOT NULL, [value1] INTEGER NOT NULL, [value2] INTEGER NOT NULL, [price] INTEGER NOT NULL, [mcustom] INTEGER NOT NULL, [fcustom] INTEGER NOT NULL, [userIP] TEXT, [periodDate] INTEGER NOT NULL, [regDate] TEXT NOT NULL);
CREATE VIEW [classPriceView]

AS

SELECT * FROM (
		SELECT idx, class1_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class2_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class3_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class4_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class5_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class6_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class7_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class8_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class9_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class10_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class11_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class12_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class13_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class14_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class15_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class16_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class17_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class18_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class19_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class20_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class21_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class22_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class23_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class24_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class25_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class26_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class27_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class28_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class29_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class30_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class31_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class32_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class33_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class34_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class35_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class36_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class37_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class38_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class39_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class40_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class41_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class42_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class43_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class44_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class45_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class46_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class47_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class48_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class49_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class50_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class51_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class52_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class53_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class54_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class55_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class56_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class57_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class58_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class59_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class60_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class61_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class62_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class63_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class64_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class65_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class66_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class67_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class68_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class69_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class70_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class71_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class72_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class73_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class74_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class75_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class76_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class77_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class78_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class79_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class80_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class81_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class82_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class83_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class84_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class85_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class86_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class87_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class88_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class89_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class90_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class91_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class92_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class93_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class94_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class95_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class96_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class97_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class98_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class99_price AS class_price FROM define_class_price 
	UNION ALL 
		SELECT idx, class100_price AS class_price FROM define_class_price 
) AS A;
CREATE VIEW [userFriendListDB]  

AS

SELECT userAccIDX, friendAccIDX, guildIDX FROM userFriendListDB_table;
CREATE VIEW [userFriendListDB_table]  

AS

SELECT userAccIDX, friendAccIDX, 0 AS 'guildIDX' FROM userFriendDB WHERE flag=1 and friendAccIDX not in (SELECT accountIDX FROM userMemberDB WHERE userType < 10)
UNION ALL
SELECT 0 AS 'userAccIDX', accountIDX AS 'friendAccIDX', guildIDX FROM userGuildInfoDB WHERE guildJoinType=1;
CREATE VIEW [userItemExtraView]

AS

SELECT * FROM (
	SELECT	idx, accountIDX, 1 AS pos, item1_type AS itemType, item1_reinforce AS reinforce, item1_limitType AS limitType, item1_limitDate AS limitDate
	FROM userItemExtraDB 

	UNION ALL 

	SELECT	idx, accountIDX, 2 AS pos, item2_type AS itemType, item2_reinforce AS reinforce, item2_limitType AS limitType, item2_limitDate AS limitDate
	FROM userItemExtraDB 

	UNION ALL 

	SELECT	idx, accountIDX, 3 AS pos, item3_type AS itemType, item3_reinforce AS reinforce, item3_limitType AS limitType, item3_limitDate AS limitDate
	FROM userItemExtraDB 

	UNION ALL 

	SELECT	idx, accountIDX, 4 AS pos, item4_type AS itemType, item4_reinforce AS reinforce, item4_limitType AS limitType, item4_limitDate AS limitDate
	FROM userItemExtraDB 

	UNION ALL 

	SELECT	idx, accountIDX, 5 AS pos, item5_type AS itemType, item5_reinforce AS reinforce, item5_limitType AS limitType, item5_limitDate AS limitDate
	FROM userItemExtraDB 

	UNION ALL 

	SELECT	idx, accountIDX, 6 AS pos, item6_type AS itemType, item6_reinforce AS reinforce, item6_limitType AS limitType, item6_limitDate AS limitDate
	FROM userItemExtraDB 

	UNION ALL 

	SELECT	idx, accountIDX, 7 AS pos, item7_type AS itemType, item7_reinforce AS reinforce, item7_limitType AS limitType, item7_limitDate AS limitDate
	FROM userItemExtraDB 

	UNION ALL 

	SELECT	idx, accountIDX, 8 AS pos, item8_type AS itemType, item8_reinforce AS reinforce, item8_limitType AS limitType, item8_limitDate AS limitDate
	FROM userItemExtraDB 

	UNION ALL 

	SELECT	idx, accountIDX, 9 AS pos, item9_type AS itemType, item9_reinforce AS reinforce, item9_limitType AS limitType, item9_limitDate AS limitDate
	FROM userItemExtraDB 

	UNION ALL 

	SELECT	idx, accountIDX, 10 AS pos, item10_type AS itemType, item10_reinforce AS reinforce, item10_limitType AS limitType, item10_limitDate AS limitDate
	FROM userItemExtraDB 
) AS A
WHERE itemType <> 0;
CREATE VIEW [userQuestCompleteView] AS

SELECT * FROM (
		SELECT idx, accountIDX, 1 as pos, quest1_type as quest_type, quest1_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 2 as pos, quest2_type as quest_type, quest2_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 3 as pos, quest3_type as quest_type, quest3_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 4 as pos, quest4_type as quest_type, quest4_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 5 as pos, quest5_type as quest_type, quest5_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 6 as pos, quest6_type as quest_type, quest6_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 7 as pos, quest7_type as quest_type, quest7_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 8 as pos, quest8_type as quest_type, quest8_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 9 as pos, quest9_type as quest_type, quest9_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 10 as pos, quest10_type as quest_type, quest10_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 11 as pos, quest11_type as quest_type, quest11_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 12 as pos, quest12_type as quest_type, quest12_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 13 as pos, quest13_type as quest_type, quest13_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 14 as pos, quest14_type as quest_type, quest14_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 15 as pos, quest15_type as quest_type, quest15_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 16 as pos, quest16_type as quest_type, quest16_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 17 as pos, quest17_type as quest_type, quest17_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 18 as pos, quest18_type as quest_type, quest18_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 19 as pos, quest19_type as quest_type, quest19_time as quest_time FROM userQuestCompleteDB 
	UNION ALL 
		SELECT idx, accountIDX, 20 as pos, quest20_type as quest_type, quest20_time as quest_time FROM userQuestCompleteDB 
) as a
WHERE quest_type <> 0;
CREATE VIEW [userQuestView] AS

SELECT * FROM (
		SELECT idx, accountIDX, 1 as pos, quest1_type as quest_type, quest1_value1 as quest_value1, quest1_value2 as quest_value2, quest1_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 2 as pos, quest2_type as quest_type, quest2_value1 as quest_value1, quest2_value2 as quest_value2, quest2_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 3 as pos, quest3_type as quest_type, quest3_value1 as quest_value1, quest3_value2 as quest_value2, quest3_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 4 as pos, quest4_type as quest_type, quest4_value1 as quest_value1, quest4_value2 as quest_value2, quest4_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 5 as pos, quest5_type as quest_type, quest5_value1 as quest_value1, quest5_value2 as quest_value2, quest5_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 6 as pos, quest6_type as quest_type, quest6_value1 as quest_value1, quest6_value2 as quest_value2, quest6_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 7 as pos, quest7_type as quest_type, quest7_value1 as quest_value1, quest7_value2 as quest_value2, quest7_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 8 as pos, quest8_type as quest_type, quest8_value1 as quest_value1, quest8_value2 as quest_value2, quest8_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 9 as pos, quest9_type as quest_type, quest9_value1 as quest_value1, quest9_value2 as quest_value2, quest9_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 10 as pos, quest10_type as quest_type, quest10_value1 as quest_value1, quest10_value2 as quest_value2, quest10_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 11 as pos, quest11_type as quest_type, quest11_value1 as quest_value1, quest11_value2 as quest_value2, quest11_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 12 as pos, quest12_type as quest_type, quest12_value1 as quest_value1, quest12_value2 as quest_value2, quest12_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 13 as pos, quest13_type as quest_type, quest13_value1 as quest_value1, quest13_value2 as quest_value2, quest13_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 14 as pos, quest14_type as quest_type, quest14_value1 as quest_value1, quest14_value2 as quest_value2, quest14_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 15 as pos, quest15_type as quest_type, quest15_value1 as quest_value1, quest15_value2 as quest_value2, quest15_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 16 as pos, quest16_type as quest_type, quest16_value1 as quest_value1, quest16_value2 as quest_value2, quest16_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 17 as pos, quest17_type as quest_type, quest17_value1 as quest_value1, quest17_value2 as quest_value2, quest17_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 18 as pos, quest18_type as quest_type, quest18_value1 as quest_value1, quest18_value2 as quest_value2, quest18_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 19 as pos, quest19_type as quest_type, quest19_value1 as quest_value1, quest19_value2 as quest_value2, quest19_time as quest_time FROM userQuestDB 
	UNION ALL 
		SELECT idx, accountIDX, 20 as pos, quest20_type as quest_type, quest20_value1 as quest_value1, quest20_value2 as quest_value2, quest20_time as quest_time FROM userQuestDB 
) as a
WHERE quest_type <> 0;
COMMIT;
