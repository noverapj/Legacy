USE [master]
GO
/****** Object:  Database [LosaLogData]    Script Date: 06/09/2026 01:10:56 ******/
CREATE DATABASE [LosaLogData]
 CONTAINMENT = NONE
 ON  PRIMARY 
( NAME = N'Log_Data', FILENAME = N'/var/opt/mssql/data/LosaLogData.mdf' , SIZE = 1876992KB , MAXSIZE = UNLIMITED, FILEGROWTH = 1024KB )
 LOG ON 
( NAME = N'Log_Data_log', FILENAME = N'/var/opt/mssql/data/LosaLogData.ldf' , SIZE = 102144KB , MAXSIZE = 2048GB , FILEGROWTH = 10%)
 WITH CATALOG_COLLATION = DATABASE_DEFAULT, LEDGER = OFF
GO
ALTER DATABASE [LosaLogData] SET COMPATIBILITY_LEVEL = 160
GO
IF (1 = FULLTEXTSERVICEPROPERTY('IsFullTextInstalled'))
begin
EXEC [LosaLogData].[dbo].[sp_fulltext_database] @action = 'enable'
end
GO
ALTER DATABASE [LosaLogData] SET ANSI_NULL_DEFAULT OFF 
GO
ALTER DATABASE [LosaLogData] SET ANSI_NULLS OFF 
GO
ALTER DATABASE [LosaLogData] SET ANSI_PADDING OFF 
GO
ALTER DATABASE [LosaLogData] SET ANSI_WARNINGS OFF 
GO
ALTER DATABASE [LosaLogData] SET ARITHABORT OFF 
GO
ALTER DATABASE [LosaLogData] SET AUTO_CLOSE OFF 
GO
ALTER DATABASE [LosaLogData] SET AUTO_SHRINK OFF 
GO
ALTER DATABASE [LosaLogData] SET AUTO_UPDATE_STATISTICS ON 
GO
ALTER DATABASE [LosaLogData] SET CURSOR_CLOSE_ON_COMMIT OFF 
GO
ALTER DATABASE [LosaLogData] SET CURSOR_DEFAULT  GLOBAL 
GO
ALTER DATABASE [LosaLogData] SET CONCAT_NULL_YIELDS_NULL OFF 
GO
ALTER DATABASE [LosaLogData] SET NUMERIC_ROUNDABORT OFF 
GO
ALTER DATABASE [LosaLogData] SET QUOTED_IDENTIFIER OFF 
GO
ALTER DATABASE [LosaLogData] SET RECURSIVE_TRIGGERS OFF 
GO
ALTER DATABASE [LosaLogData] SET  DISABLE_BROKER 
GO
ALTER DATABASE [LosaLogData] SET AUTO_UPDATE_STATISTICS_ASYNC OFF 
GO
ALTER DATABASE [LosaLogData] SET DATE_CORRELATION_OPTIMIZATION OFF 
GO
ALTER DATABASE [LosaLogData] SET TRUSTWORTHY OFF 
GO
ALTER DATABASE [LosaLogData] SET ALLOW_SNAPSHOT_ISOLATION OFF 
GO
ALTER DATABASE [LosaLogData] SET PARAMETERIZATION SIMPLE 
GO
ALTER DATABASE [LosaLogData] SET READ_COMMITTED_SNAPSHOT OFF 
GO
ALTER DATABASE [LosaLogData] SET HONOR_BROKER_PRIORITY OFF 
GO
ALTER DATABASE [LosaLogData] SET RECOVERY SIMPLE 
GO
ALTER DATABASE [LosaLogData] SET  MULTI_USER 
GO
ALTER DATABASE [LosaLogData] SET PAGE_VERIFY CHECKSUM  
GO
ALTER DATABASE [LosaLogData] SET DB_CHAINING OFF 
GO
ALTER DATABASE [LosaLogData] SET FILESTREAM( NON_TRANSACTED_ACCESS = OFF ) 
GO
ALTER DATABASE [LosaLogData] SET TARGET_RECOVERY_TIME = 60 SECONDS 
GO
ALTER DATABASE [LosaLogData] SET DELAYED_DURABILITY = DISABLED 
GO
ALTER DATABASE [LosaLogData] SET ACCELERATED_DATABASE_RECOVERY = OFF  
GO
EXEC sys.sp_db_vardecimal_storage_format N'LosaLogData', N'ON'
GO
ALTER DATABASE [LosaLogData] SET QUERY_STORE = ON
GO
ALTER DATABASE [LosaLogData] SET QUERY_STORE (OPERATION_MODE = READ_WRITE, CLEANUP_POLICY = (STALE_QUERY_THRESHOLD_DAYS = 30), DATA_FLUSH_INTERVAL_SECONDS = 900, INTERVAL_LENGTH_MINUTES = 60, MAX_STORAGE_SIZE_MB = 1000, QUERY_CAPTURE_MODE = AUTO, SIZE_BASED_CLEANUP_MODE = AUTO, MAX_PLANS_PER_QUERY = 200, WAIT_STATS_CAPTURE_MODE = ON)
GO
USE [LosaLogData]
GO
/****** Object:  User [lstglog]    Script Date: 06/09/2026 01:10:56 ******/
CREATE USER [lstglog] FOR LOGIN [lstglog] WITH DEFAULT_SCHEMA=[dbo]
GO
/****** Object:  User [lstggame]    Script Date: 06/09/2026 01:10:56 ******/
CREATE USER [lstggame] WITHOUT LOGIN WITH DEFAULT_SCHEMA=[dbo]
GO
/****** Object:  Table [dbo].[GAME_LOG]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[GAME_LOG](
	[RegDate] [datetime] NOT NULL,
	[LogIDX] [bigint] IDENTITY(1,1) NOT NULL,
	[LogType] [int] NOT NULL,
	[AccountIDX] [int] NOT NULL,
	[UserID] [varchar](20) NOT NULL,
	[UserName] [varchar](20) NOT NULL,
	[UserLevel] [int] NOT NULL,
	[UserExp] [int] NOT NULL,
	[CpType] [int] NOT NULL,
	[TBLIDX] [int] NOT NULL,
	[ObjCode] [int] NOT NULL,
	[EventType] [tinyint] NOT NULL,
	[Param1] [int] NOT NULL,
	[Param2] [int] NOT NULL,
	[Param3] [int] NOT NULL,
	[Param4] [int] NOT NULL,
	[Param5] [varchar](50) NOT NULL,
	[UserIP] [varchar](15) NOT NULL,
 CONSTRAINT [PK_GAME_LOG_LogIndex] PRIMARY KEY CLUSTERED 
(
	[RegDate] ASC,
	[LogIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Log_buy_gold_item]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Log_buy_gold_item](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickname] [varchar](12) NOT NULL,
	[itemType] [int] NOT NULL,
	[itemCode] [int] NOT NULL,
	[amount] [int] NOT NULL,
	[userIP] [varchar](15) NOT NULL,
	[note] [varchar](200) NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Log_buy_item]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Log_buy_item](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[userID] [varchar](12) NOT NULL,
	[applyType] [char](2) NOT NULL,
	[userIP] [varchar](15) NOT NULL,
	[playType] [int] NOT NULL,
	[modeType] [int] NOT NULL,
	[sub1Type] [int] NOT NULL,
	[sub2Type] [int] NOT NULL,
	[buyplace] [int] NULL,
	[classType] [int] NOT NULL,
	[kindred] [int] NOT NULL,
	[decoType] [int] NOT NULL,
	[decoCode] [int] NOT NULL,
	[itemType] [int] NOT NULL,
	[buyType] [int] NOT NULL,
	[amount] [int] NOT NULL,
	[note] [varchar](100) NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Index [idx_regDate]    Script Date: 06/09/2026 01:10:56 ******/
CREATE CLUSTERED INDEX [idx_regDate] ON [dbo].[Log_buy_item]
(
	[regDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_buy_item_accessory]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_buy_item_accessory](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [nvarchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[itemCode] [int] NOT NULL,
	[eventType] [int] NOT NULL,
	[amount] [int] NOT NULL,
	[userIP] [varchar](15) NOT NULL,
	[note] [varchar](100) NULL,
	[regDate] [datetime] NOT NULL,
 CONSTRAINT [PK_log_buy_item_accessory] PRIMARY KEY CLUSTERED 
(
	[regDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_buy_item_class]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_buy_item_class](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[classType] [int] NOT NULL,
	[limitType] [int] NOT NULL,
	[eventType] [int] NOT NULL,
	[amount] [int] NOT NULL,
	[userIP] [varchar](15) NOT NULL,
	[note] [varchar](100) NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_buy_item_costume]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_buy_item_costume](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [nvarchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[itemCode] [int] NOT NULL,
	[eventType] [int] NOT NULL,
	[amount] [int] NOT NULL,
	[userIP] [varchar](15) NOT NULL,
	[note] [varchar](100) NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Index [CL_log_buy_item_costume]    Script Date: 06/09/2026 01:10:56 ******/
CREATE CLUSTERED INDEX [CL_log_buy_item_costume] ON [dbo].[log_buy_item_costume]
(
	[regDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_buy_item_decoration]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_buy_item_decoration](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](20) NOT NULL,
	[classType] [int] NOT NULL,
	[kindred] [int] NOT NULL,
	[itemType] [int] NOT NULL,
	[itemCode] [int] NOT NULL,
	[eventType] [int] NOT NULL,
	[amount] [int] NOT NULL,
	[userIP] [varchar](15) NOT NULL,
	[note] [varchar](100) NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_buy_item_equip]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_buy_item_equip](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](20) NOT NULL,
	[itemType] [int] NOT NULL,
	[itemCode] [int] NOT NULL,
	[reinforce] [int] NOT NULL,
	[machineType] [int] NOT NULL,
	[limitDate] [int] NOT NULL,
	[eventType] [int] NOT NULL,
	[amount] [int] NOT NULL,
	[userIP] [varchar](15) NOT NULL,
	[note] [varchar](100) NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_buy_item_gold]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_buy_item_gold](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[itemType] [int] NOT NULL,
	[itemCode] [int] NOT NULL,
	[itemValue] [int] NOT NULL,
	[amount] [int] NOT NULL,
	[cpType] [smallint] NOT NULL,
	[billingID] [varchar](64) NULL,
	[buyType] [tinyint] NOT NULL,
	[userIP] [varchar](15) NOT NULL,
	[note] [varchar](100) NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_buy_item_sbox]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_buy_item_sbox](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [nvarchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[presentType] [smallint] NOT NULL,
	[value1] [int] NOT NULL,
	[value2] [int] NOT NULL,
	[amount] [int] NOT NULL,
	[cpType] [smallint] NOT NULL,
	[billingID] [varchar](64) NOT NULL,
	[logType] [int] NOT NULL,
	[userIP] [varchar](15) NOT NULL,
	[note] [nvarchar](100) NULL,
	[limitDate] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_buy_item_special]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_buy_item_special](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](15) NOT NULL,
	[itemType] [int] NOT NULL,
	[itemValue] [int] NOT NULL,
	[eventType] [int] NOT NULL,
	[amount] [int] NOT NULL,
	[userIP] [varchar](15) NOT NULL,
	[note] [varchar](100) NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_accessory_upgrade]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_accessory_upgrade](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [nvarchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[itemCode] [int] NOT NULL,
	[nowUpgradeValue] [int] NOT NULL,
	[resultUpgradeValue] [int] NOT NULL,
	[pieceCode] [int] NOT NULL,
	[logType] [tinyint] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_awake]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_awake](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [nvarchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[classType] [int] NOT NULL,
	[pieceCode] [int] NOT NULL,
	[pieceUseCount] [smallint] NOT NULL,
	[awakeType] [tinyint] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Index [CI_log_data_awake]    Script Date: 06/09/2026 01:10:56 ******/
CREATE CLUSTERED INDEX [CI_log_data_awake] ON [dbo].[log_data_awake]
(
	[regDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_cardmatching]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_cardmatching](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[AccountIDX] [int] NOT NULL,
	[NickName] [nvarchar](20) NOT NULL,
	[UserLevel] [int] NOT NULL,
	[LogType] [int] NOT NULL,
	[MissionType] [int] NOT NULL,
	[MissionMark1] [int] NOT NULL,
	[MissionMark2] [int] NOT NULL,
	[UserMark1] [int] NOT NULL,
	[UserMark2] [int] NOT NULL,
	[RewardStep] [int] NOT NULL,
	[ItemType] [int] NOT NULL,
	[ItemCode] [int] NOT NULL,
	[ItemValue] [int] NOT NULL,
	[RegDate] [datetime] NOT NULL,
 CONSTRAINT [PK_log_data_cardmatching] PRIMARY KEY CLUSTERED 
(
	[RegDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_character]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_character](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](20) NOT NULL,
	[playType] [int] NOT NULL,
	[modeType] [int] NOT NULL,
	[subType1] [int] NOT NULL,
	[subType2] [int] NOT NULL,
	[playTime] [int] NOT NULL,
	[characterType] [int] NOT NULL,
	[kill] [int] NOT NULL,
	[death] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_cheating]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_cheating](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[userID] [varchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[userIP] [varchar](20) NOT NULL,
	[mainIDX] [int] NOT NULL,
	[subIDX] [int] NOT NULL,
	[regDate] [datetime] NOT NULL,
 CONSTRAINT [PK_log_data_cheating] PRIMARY KEY CLUSTERED 
(
	[regDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_clover]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_clover](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[friendIDX] [int] NOT NULL,
	[eventType] [smallint] NOT NULL,
	[CloverCount] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Index [idx_regDate]    Script Date: 06/09/2026 01:10:56 ******/
CREATE CLUSTERED INDEX [idx_regDate] ON [dbo].[log_data_clover]
(
	[regDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_custom_medal]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_custom_medal](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[AccountIDX] [int] NOT NULL,
	[MedalIDX] [int] NOT NULL,
	[MedalCode] [int] NOT NULL,
	[BasicGrowth1] [int] NOT NULL,
	[BasicGrowth2] [int] NOT NULL,
	[BasicGrowth3] [int] NOT NULL,
	[BasicGrowth4] [int] NOT NULL,
	[SkillGrowth1] [int] NOT NULL,
	[SkillGrowth2] [int] NOT NULL,
	[SkillGrowth3] [int] NOT NULL,
	[SkillGrowth4] [int] NOT NULL,
	[LimitType] [int] NOT NULL,
	[LimitDate] [datetime] NOT NULL,
	[LogType] [tinyint] NOT NULL,
	[regDate] [datetime] NOT NULL,
 CONSTRAINT [PK_log_data_custom_medal] PRIMARY KEY CLUSTERED 
(
	[regDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_DiceGame]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_DiceGame](
	[AccountIDX] [int] NOT NULL,
	[GameState] [int] NOT NULL,
	[DiceCountUsed] [int] NOT NULL,
	[BoardCountUsed] [int] NOT NULL,
	[RewardCountUsed] [int] NOT NULL,
	[Position] [int] NOT NULL,
	[Trace01] [int] NOT NULL,
	[Trace02] [int] NOT NULL,
	[Trace03] [int] NOT NULL,
	[Trace04] [int] NOT NULL,
	[Trace05] [int] NOT NULL,
	[Trace06] [int] NOT NULL,
	[Board] [tinyint] NOT NULL,
	[RewardStep01] [int] NOT NULL,
	[RewardStep02] [int] NOT NULL,
	[RewardStep03] [int] NOT NULL,
	[RewardStep04] [int] NOT NULL,
	[RewardStep05] [int] NOT NULL,
	[RewardStep06] [int] NOT NULL,
	[RewardStep07] [int] NOT NULL,
	[RewardStep08] [int] NOT NULL,
	[RewardStep09] [int] NOT NULL,
	[RewardStep10] [int] NOT NULL,
	[StartTime] [datetime] NULL,
	[EndTime] [datetime] NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_excavating_new]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_excavating_new](
	[RegDate] [datetime] NOT NULL,
	[LogIDX] [bigint] IDENTITY(1,1) NOT NULL,
	[LogType] [int] NOT NULL,
	[AccountIDX] [int] NOT NULL,
	[UserID] [varchar](20) NOT NULL,
	[UserName] [varchar](20) NOT NULL,
	[ExLevel] [int] NOT NULL,
	[ExEXP] [int] NOT NULL,
	[CType] [int] NOT NULL,
	[CLevel] [int] NOT NULL,
	[CExpert] [int] NOT NULL,
	[RCount] [tinyint] NOT NULL,
	[MapID] [int] NOT NULL,
	[X] [int] NOT NULL,
	[Y] [int] NOT NULL,
	[Z] [int] NOT NULL,
	[RewardType] [int] NOT NULL,
	[Index] [int] NOT NULL,
	[Price] [int] NOT NULL,
	[Mutiple] [int] NOT NULL,
	[ResultMoney] [int] NOT NULL,
	[UserIP] [varchar](15) NOT NULL,
 CONSTRAINT [PK_log_data_excavating_new_idx] PRIMARY KEY CLUSTERED 
(
	[RegDate] ASC,
	[LogIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_gear_upgrade]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_gear_upgrade](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [nvarchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[gearCode] [int] NOT NULL,
	[nowUpgradeValue] [tinyint] NOT NULL,
	[resultUpgradeValue] [tinyint] NOT NULL,
	[pieceCode] [int] NOT NULL,
	[logType] [tinyint] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Index [CL_log_data_gear_upgrade]    Script Date: 06/09/2026 01:10:56 ******/
CREATE CLUSTERED INDEX [CL_log_data_gear_upgrade] ON [dbo].[log_data_gear_upgrade]
(
	[regDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_league_present]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_league_present](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [nvarchar](20) NOT NULL,
	[leagueIDX] [int] NOT NULL,
	[currentRound] [tinyint] NOT NULL,
	[presentCode1] [int] NOT NULL,
	[presentCode2] [int] NOT NULL,
	[presentCode3] [int] NOT NULL,
	[presentCode4] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_localinfo]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_localinfo](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[os] [nvarchar](100) NOT NULL,
	[ie] [nvarchar](100) NOT NULL,
	[directx] [nvarchar](100) NOT NULL,
	[cpu] [nvarchar](100) NOT NULL,
	[gpu] [nvarchar](100) NOT NULL,
	[memory] [nvarchar](100) NOT NULL,
	[systemResolution] [nvarchar](20) NOT NULL,
	[losaResolution] [nvarchar](20) NOT NULL,
	[screenType] [tinyint] NOT NULL,
	[userIP] [varchar](15) NOT NULL,
	[regDate] [datetime] NOT NULL,
	[hdd_serial] [nvarchar](100) NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_matchmode]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_matchmode](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[AccountIDX] [int] NOT NULL,
	[nickName] [nvarchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[Tier1] [int] NOT NULL,
	[Tier2] [int] NOT NULL,
	[MatchTime] [int] NOT NULL,
	[logType] [int] NOT NULL,
	[regDate] [datetime] NOT NULL,
	[RoomNumber] [int] NOT NULL,
 CONSTRAINT [PK_log_data_matchmode] PRIMARY KEY CLUSTERED 
(
	[regDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_matchmode2]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_matchmode2](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[AccountIDX] [int] NOT NULL,
	[nickName] [nvarchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[Tier1] [int] NOT NULL,
	[Tier2] [int] NOT NULL,
	[MatchTime] [int] NOT NULL,
	[MatchPoint] [int] NOT NULL,
	[WinningStreakCount] [int] NOT NULL,
	[logType] [int] NOT NULL,
	[MatchType] [int] NOT NULL,
	[RoomNumber] [int] NOT NULL,
	[regDate] [datetime] NOT NULL,
 CONSTRAINT [PK_log_data_matchmode2] PRIMARY KEY CLUSTERED 
(
	[regDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_medal]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_medal](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](20) NOT NULL,
	[medalType] [int] NOT NULL,
	[limitType] [int] NOT NULL,
	[logType] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Index [idx_regDate]    Script Date: 06/09/2026 01:10:56 ******/
CREATE CLUSTERED INDEX [idx_regDate] ON [dbo].[log_data_medal]
(
	[regDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_medal_extend]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_medal_extend](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[classType] [int] NOT NULL,
	[slotNum] [tinyint] NOT NULL,
	[period] [int] NOT NULL,
	[logType] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_oakbarrel]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_oakbarrel](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[AccountIDX] [int] NOT NULL,
	[NickName] [nvarchar](20) NOT NULL,
	[UserLevel] [int] NOT NULL,
	[ItemType] [int] NOT NULL,
	[ItemCode] [int] NOT NULL,
	[ItemValue] [int] NOT NULL,
	[SwordCount] [int] NOT NULL,
	[LogType] [int] NOT NULL,
	[RegDate] [datetime] NOT NULL,
 CONSTRAINT [PK_log_data_oakbarrel] PRIMARY KEY CLUSTERED 
(
	[RegDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_pcroom]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_pcroom](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](20) NOT NULL,
	[playTime] [int] NOT NULL,
	[userIP] [varchar](15) NOT NULL,
	[pcnum] [int] NOT NULL,
	[logType] [int] NOT NULL,
	[pcrType] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_peso]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_peso](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](20) NOT NULL,
	[peso] [int] NOT NULL,
	[logType] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_pet]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_pet](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [nvarchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[petIDX] [int] NOT NULL,
	[petCode] [int] NOT NULL,
	[petRank] [tinyint] NOT NULL,
	[petLevel] [int] NOT NULL,
	[petExp] [int] NOT NULL,
	[itemCode] [int] NOT NULL,
	[logType] [tinyint] NOT NULL,
	[logSubType] [int] NOT NULL,
	[logNote] [varchar](200) NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Index [CI_log_data_pet]    Script Date: 06/09/2026 01:10:56 ******/
CREATE CLUSTERED INDEX [CI_log_data_pet] ON [dbo].[log_data_pet]
(
	[regDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_piece_divide]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_piece_divide](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [nvarchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[itemType] [int] NOT NULL,
	[itemCode] [int] NOT NULL,
	[amount] [int] NOT NULL,
	[materialCode] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_piece_mix]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_piece_mix](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [nvarchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[mixType] [smallint] NOT NULL,
	[mixCode] [int] NOT NULL,
	[resultType] [tinyint] NOT NULL,
	[useA] [int] NOT NULL,
	[useB] [int] NOT NULL,
	[resultA] [int] NOT NULL,
	[resultB] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_piece_obtain]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_piece_obtain](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [nvarchar](50) NOT NULL,
	[userLevel] [int] NOT NULL,
	[playTime] [int] NOT NULL,
	[step] [tinyint] NOT NULL,
	[amount] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_play]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_play](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](20) NOT NULL,
	[playType] [int] NOT NULL,
	[modeType] [int] NOT NULL,
	[subType1] [int] NOT NULL,
	[subType2] [int] NOT NULL,
	[playTime] [int] NOT NULL,
	[deathTime] [int] NOT NULL,
	[peso] [int] NOT NULL,
	[win] [int] NOT NULL,
	[lose] [int] NOT NULL,
	[kill] [int] NOT NULL,
	[death] [int] NOT NULL,
	[roomNumber] [int] NOT NULL,
	[contribution] [int] NOT NULL,
	[logType] [int] NOT NULL,
	[pcnum] [int] NOT NULL,
	[blueTeamMember] [int] NOT NULL,
	[redTeamMember] [int] NOT NULL,
	[regDate] [datetime] NOT NULL,
 CONSTRAINT [PK_log_data_play_regDate] PRIMARY KEY CLUSTERED 
(
	[regDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_play_old]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_play_old](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](20) NOT NULL,
	[playType] [int] NOT NULL,
	[modeType] [int] NOT NULL,
	[subType1] [int] NOT NULL,
	[subType2] [int] NOT NULL,
	[playTime] [int] NOT NULL,
	[deathTime] [int] NOT NULL,
	[peso] [int] NOT NULL,
	[win] [int] NOT NULL,
	[lose] [int] NOT NULL,
	[kill] [int] NOT NULL,
	[death] [int] NOT NULL,
	[logType] [int] NOT NULL,
	[pcnum] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_quest]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_quest](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[mainIDX] [int] NOT NULL,
	[subIDX] [int] NOT NULL,
	[logType] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_quest_vfun]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_quest_vfun](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[mainIDX] [int] NOT NULL,
	[subIDX] [int] NOT NULL,
	[channelingType] [int] NOT NULL,
	[success] [int] NOT NULL,
	[reason] [varchar](256) NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_roominfo]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_roominfo](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[RoomNumber] [int] NOT NULL,
	[BlueWin] [int] NOT NULL,
	[BlueLose] [int] NOT NULL,
	[BlueTeamMember] [int] NOT NULL,
	[RedWin] [int] NOT NULL,
	[RedLose] [int] NOT NULL,
	[RedTeamMember] [int] NOT NULL,
	[PlayType] [int] NOT NULL,
	[ModeType] [int] NOT NULL,
	[ModeSubType] [int] NOT NULL,
	[RegDate] [datetime] NOT NULL,
	[Tier1] [int] NOT NULL,
	[Tier2] [int] NOT NULL,
	[logType] [int] NOT NULL,
	[playTime] [int] NOT NULL,
 CONSTRAINT [PK_log_data_roominfo] PRIMARY KEY CLUSTERED 
(
	[RegDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_spirit_change]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_spirit_change](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[AccountIDX] [int] NOT NULL,
	[NickName] [nvarchar](20) NOT NULL,
	[UserLevel] [int] NOT NULL,
	[SetSpiritCode] [int] NOT NULL,
	[SetSpiritCount] [int] NOT NULL,
	[GetSpiritCode] [int] NOT NULL,
	[GetSpiritCount] [int] NOT NULL,
	[CriticalType] [tinyint] NOT NULL,
	[regDate] [datetime] NOT NULL,
 CONSTRAINT [PK_log_data_spirit_change] PRIMARY KEY CLUSTERED 
(
	[regDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_spirit_divide]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_spirit_divide](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[AccountIDX] [int] NOT NULL,
	[NickName] [nvarchar](20) NOT NULL,
	[UserLevel] [int] NOT NULL,
	[HeroType] [int] NOT NULL,
	[SpiritCode] [int] NOT NULL,
	[SpiritCount] [int] NOT NULL,
	[SoulStoneCount] [int] NOT NULL,
	[CriticalType] [tinyint] NOT NULL,
	[regDate] [datetime] NOT NULL,
 CONSTRAINT [PK_log_data_spirit_divide] PRIMARY KEY CLUSTERED 
(
	[regDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_spirit_mix]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_spirit_mix](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[AccountIDX] [int] NOT NULL,
	[NickName] [nvarchar](20) NOT NULL,
	[UserLevel] [int] NOT NULL,
	[HeroType] [int] NOT NULL,
	[SpiritCode] [int] NOT NULL,
	[SpiritCount] [int] NOT NULL,
	[UnknownSpiritCode] [int] NOT NULL,
	[UnknownSpiritCount] [int] NOT NULL,
	[regDate] [datetime] NOT NULL,
 CONSTRAINT [PK_log_data_spirit_mix] PRIMARY KEY CLUSTERED 
(
	[regDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_spirit_obtain]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_spirit_obtain](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[AccountIDX] [int] NOT NULL,
	[NickName] [nvarchar](20) NOT NULL,
	[UserLevel] [int] NOT NULL,
	[SpiritCode] [int] NOT NULL,
	[SpiritCount] [int] NOT NULL,
	[Value] [int] NOT NULL,
	[logType] [tinyint] NOT NULL,
	[regDate] [datetime] NOT NULL,
 CONSTRAINT [PK_log_data_spirit_obtain] PRIMARY KEY CLUSTERED 
(
	[regDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_spirit_peso]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_spirit_peso](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[AccountIDX] [int] NOT NULL,
	[NickName] [nvarchar](20) NOT NULL,
	[UserLevel] [int] NOT NULL,
	[PieceCount] [int] NOT NULL,
	[DimensionCount] [int] NOT NULL,
	[Peso] [int] NOT NULL,
	[regDate] [datetime] NOT NULL,
 CONSTRAINT [PK_log_data_spirit_peso] PRIMARY KEY CLUSTERED 
(
	[regDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_time]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_time](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](20) NOT NULL,
	[playTime] [int] NOT NULL,
	[logType] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_trade]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_trade](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickname] [varchar](20) NOT NULL,
	[itemType] [int] NOT NULL,
	[value1] [int] NOT NULL,
	[value2] [int] NOT NULL,
	[price] [bigint] NOT NULL,
	[logType] [int] NOT NULL,
	[userIP] [varchar](16) NULL,
	[note] [varchar](100) NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_tutorial]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_data_tutorial](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [varchar](20) NOT NULL,
	[step] [int] NOT NULL,
	[playTime] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_define]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_define](
	[objName] [varchar](32) NOT NULL,
	[code] [int] NOT NULL,
	[itemNameKO] [varchar](100) NULL,
	[itemNameID] [varchar](100) NULL,
	[itemNameEN] [varchar](100) NULL,
 CONSTRAINT [PK_define_manage] PRIMARY KEY CLUSTERED 
(
	[objName] ASC,
	[code] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_error_billing]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_error_billing](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[cpType] [smallint] NOT NULL,
	[errType] [int] NOT NULL,
	[note] [varchar](200) NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_event_bingo]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_event_bingo](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [nvarchar](20) NOT NULL,
	[choiceType] [tinyint] NOT NULL,
	[number] [tinyint] NOT NULL,
	[stateType] [tinyint] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_event_cash]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_event_cash](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickName] [nvarchar](20) NOT NULL,
	[userLevel] [int] NOT NULL,
	[itemCode] [int] NOT NULL,
	[amount] [int] NOT NULL,
	[eventType] [tinyint] NOT NULL,
	[userIP] [nvarchar](16) NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Log_game_concurrent]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Log_game_concurrent](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[serverID] [bigint] NOT NULL,
	[serverIP] [varchar](15) NOT NULL,
	[serverPort] [int] NOT NULL,
	[serverName] [varchar](32) NULL,
	[concurrent] [int] NOT NULL,
	[cpType] [smallint] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Index [idx_regDate]    Script Date: 06/09/2026 01:10:56 ******/
CREATE CLUSTERED INDEX [idx_regDate] ON [dbo].[Log_game_concurrent]
(
	[regDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Log_play_character]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Log_play_character](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[userID] [varchar](12) NOT NULL,
	[playType] [int] NOT NULL,
	[modeType] [int] NOT NULL,
	[sub1Type] [int] NOT NULL,
	[sub2Type] [int] NOT NULL,
	[playTime] [int] NOT NULL,
	[characterType] [int] NOT NULL,
	[cnt_kill] [int] NOT NULL,
	[cnt_killed] [int] NOT NULL,
	[cnt_selfkilled] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Index [idx_regDate]    Script Date: 06/09/2026 01:10:56 ******/
CREATE CLUSTERED INDEX [idx_regDate] ON [dbo].[Log_play_character]
(
	[regDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Log_play_game]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Log_play_game](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[userID] [varchar](12) NOT NULL,
	[playType] [int] NOT NULL,
	[modeType] [int] NOT NULL,
	[sub1Type] [int] NOT NULL,
	[sub2Type] [int] NOT NULL,
	[playTime] [int] NOT NULL,
	[deathTime] [int] NOT NULL,
	[basicPeso] [int] NOT NULL,
	[total_peso] [int] NOT NULL,
	[total_emblem] [int] NOT NULL,
	[total_play_num] [int] NOT NULL,
	[total_round] [int] NOT NULL,
	[total_contribute] [int] NOT NULL,
	[total_win] [int] NOT NULL,
	[total_lose] [int] NOT NULL,
	[cnt_kill] [int] NOT NULL,
	[cnt_killed] [int] NOT NULL,
	[cnt_selfkilled] [int] NOT NULL,
	[logType] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Index [idx_regDate]    Script Date: 06/09/2026 01:10:56 ******/
CREATE CLUSTERED INDEX [idx_regDate] ON [dbo].[Log_play_game]
(
	[regDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Log_play_tutorial]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Log_play_tutorial](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[userID] [varchar](12) NOT NULL,
	[step] [int] NOT NULL,
	[playTime] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Index [idx_regDate]    Script Date: 06/09/2026 01:10:56 ******/
CREATE CLUSTERED INDEX [idx_regDate] ON [dbo].[Log_play_tutorial]
(
	[regDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_present]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_present](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickname] [varchar](20) NOT NULL,
	[receiveIDX] [int] NOT NULL,
	[presentType] [smallint] NOT NULL,
	[value1] [int] NOT NULL,
	[value2] [int] NOT NULL,
	[eventType] [int] NOT NULL,
	[userIP] [varchar](15) NOT NULL,
	[note] [varchar](100) NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Log_system_info]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Log_system_info](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[logType] [varchar](32) NULL,
	[serverIP] [varchar](15) NULL,
	[concurrent] [int] NULL,
	[cpu] [int] NULL,
	[memory] [int] NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Index [idx_regDate]    Script Date: 06/09/2026 01:10:56 ******/
CREATE CLUSTERED INDEX [idx_regDate] ON [dbo].[Log_system_info]
(
	[regDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_temp_bckim]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_temp_bckim](
	[accountIDX] [int] NULL,
	[regdate] [datetime] NULL,
	[regdate_str] [varchar](10) NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_trace_prisoner_option]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_trace_prisoner_option](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[BlueMember] [int] NOT NULL,
	[RedMember] [int] NOT NULL,
	[BlueMaxMember] [int] NOT NULL,
	[RedMaxMember] [int] NOT NULL,
	[TeamMixOption] [int] NOT NULL,
	[regDate] [datetime] NOT NULL,
	[closedRoomOption] [int] NOT NULL,
	[userModeOption] [int] NOT NULL,
 CONSTRAINT [PK_log_trace_prisoner_option] PRIMARY KEY CLUSTERED 
(
	[regDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_trace_prisoner_start]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_trace_prisoner_start](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[ServerID] [bigint] NOT NULL,
	[MakeRoom] [int] NOT NULL,
	[QuickStart] [int] NOT NULL,
	[ListSelect] [int] NOT NULL,
	[RegDate] [datetime] NOT NULL,
 CONSTRAINT [PK_log_trace_prisoner_start] PRIMARY KEY CLUSTERED 
(
	[RegDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Log_use_item]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Log_use_item](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[nickname] [varchar](20) NOT NULL,
	[itemType] [int] NOT NULL,
	[itemCode] [int] NOT NULL,
	[rowIDX] [int] NOT NULL,
	[colIDX] [int] NOT NULL,
	[userIP] [varchar](15) NOT NULL,
	[note] [varchar](200) NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[LosaGamelog_TEMP_QUEST_LOGIN_BCKIM]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[LosaGamelog_TEMP_QUEST_LOGIN_BCKIM](
	[AccountIDX] [int] NOT NULL,
	[Term_05220612] [int] NULL,
	[Term_06120619] [int] NULL,
	[Term_05220619] [int] NULL,
	[progress] [int] NULL,
	[result] [int] NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[LosaGamelog_TEMP_QUEST_LOGIN_BCKIM_logdate]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[LosaGamelog_TEMP_QUEST_LOGIN_BCKIM_logdate](
	[AccountIDX] [int] NOT NULL,
	[logincnt] [int] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[LosaGamelog_TEMP_QUEST_LOGIN_BCKIM_progress]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[LosaGamelog_TEMP_QUEST_LOGIN_BCKIM_progress](
	[AccountIDX] [int] NOT NULL,
	[progress] [int] NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[SQL_ERROR_LOG]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[SQL_ERROR_LOG](
	[ErrorID] [int] IDENTITY(1,1) NOT NULL,
	[AccountIDX] [int] NOT NULL,
	[ErrorNo] [int] NOT NULL,
	[ErrorProcedure] [varchar](50) NULL,
	[ErrorLine] [int] NULL,
	[ErrorServerity] [int] NULL,
	[ErrorState] [int] NULL,
	[ErrorMessage] [varchar](500) NOT NULL,
	[ErrorTime] [datetime] NOT NULL,
PRIMARY KEY CLUSTERED 
(
	[ErrorID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[TBL_LOG_HACK]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[TBL_LOG_HACK](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[AccountIDX] [int] NOT NULL,
	[NickName] [nvarchar](20) NOT NULL,
	[UserLevel] [int] NOT NULL,
	[PlayType] [int] NOT NULL,
	[ModeType] [int] NOT NULL,
	[SubType1] [int] NOT NULL,
	[SubType2] [int] NOT NULL,
	[ItemType] [int] NOT NULL,
	[ItemCode] [int] NOT NULL,
	[ItemValue] [int] NOT NULL,
	[RegDate] [datetime] NOT NULL,
 CONSTRAINT [PK_TBL_LOG_HACK] PRIMARY KEY CLUSTERED 
(
	[RegDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[TBL_LOG_HACK_MESSAGE]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[TBL_LOG_HACK_MESSAGE](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[AccountIDX] [int] NOT NULL,
	[Msg] [nvarchar](1000) NOT NULL,
	[RegDate] [datetime] NOT NULL,
 CONSTRAINT [PK_TBL_LOG_HACK_MESSAGE] PRIMARY KEY CLUSTERED 
(
	[RegDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
SET ANSI_PADDING ON
GO
/****** Object:  Index [idx_userID]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [idx_userID] ON [dbo].[Log_buy_item]
(
	[userID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_log_buy_item_accessory_accountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NC_log_buy_item_accessory_accountIDX] ON [dbo].[log_buy_item_accessory]
(
	[accountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_log_buy_item_costume_accountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NC_log_buy_item_costume_accountIDX] ON [dbo].[log_buy_item_costume]
(
	[accountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_log_data_awake_accountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NC_log_data_awake_accountIDX] ON [dbo].[log_data_awake]
(
	[accountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_log_data_awake_idx]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NC_log_data_awake_idx] ON [dbo].[log_data_awake]
(
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_log_data_cheating_accountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NC_log_data_cheating_accountIDX] ON [dbo].[log_data_cheating]
(
	[accountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [idx_accountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [idx_accountIDX] ON [dbo].[log_data_clover]
(
	[accountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [idx_eventType]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [idx_eventType] ON [dbo].[log_data_clover]
(
	[eventType] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_log_data_excavating_new_AccountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NC_log_data_excavating_new_AccountIDX] ON [dbo].[log_data_excavating_new]
(
	[AccountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_log_data_pet_accountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NC_log_data_pet_accountIDX] ON [dbo].[log_data_gear_upgrade]
(
	[accountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_log_data_pet_idx]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NC_log_data_pet_idx] ON [dbo].[log_data_gear_upgrade]
(
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [idx_accountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [idx_accountIDX] ON [dbo].[log_data_medal]
(
	[accountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [idx_eventType]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [idx_eventType] ON [dbo].[log_data_medal]
(
	[logType] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_log_data_oakbarrel_accountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NC_log_data_oakbarrel_accountIDX] ON [dbo].[log_data_oakbarrel]
(
	[AccountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NX_log_data_pet_accountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NX_log_data_pet_accountIDX] ON [dbo].[log_data_pet]
(
	[accountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NX_log_data_pet_idx]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NX_log_data_pet_idx] ON [dbo].[log_data_pet]
(
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NX_log_data_play_accountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NX_log_data_play_accountIDX] ON [dbo].[log_data_play]
(
	[accountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NX_log_data_play_logType]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NX_log_data_play_logType] ON [dbo].[log_data_play]
(
	[logType] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_log_data_spirit_change_AccountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NC_log_data_spirit_change_AccountIDX] ON [dbo].[log_data_spirit_change]
(
	[AccountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_log_data_spirit_divide_AccountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NC_log_data_spirit_divide_AccountIDX] ON [dbo].[log_data_spirit_divide]
(
	[AccountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_log_data_spirit_mix_AccountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NC_log_data_spirit_mix_AccountIDX] ON [dbo].[log_data_spirit_mix]
(
	[AccountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_log_data_spirit_obtain_AccountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NC_log_data_spirit_obtain_AccountIDX] ON [dbo].[log_data_spirit_obtain]
(
	[AccountIDX] ASC,
	[logType] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_log_data_spirit_peso_AccountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NC_log_data_spirit_peso_AccountIDX] ON [dbo].[log_data_spirit_peso]
(
	[AccountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
SET ANSI_PADDING ON
GO
/****** Object:  Index [idx_userID]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [idx_userID] ON [dbo].[Log_play_character]
(
	[userID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
SET ANSI_PADDING ON
GO
/****** Object:  Index [idx_userID]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [idx_userID] ON [dbo].[Log_play_game]
(
	[userID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
SET ANSI_PADDING ON
GO
/****** Object:  Index [idx_userID]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [idx_userID] ON [dbo].[Log_play_tutorial]
(
	[userID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
SET ANSI_PADDING ON
GO
/****** Object:  Index [idx_logType]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [idx_logType] ON [dbo].[Log_system_info]
(
	[logType] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_TBL_LOG_HACK_AccountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NC_TBL_LOG_HACK_AccountIDX] ON [dbo].[TBL_LOG_HACK]
(
	[AccountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_TBL_LOG_HACK_MESSAGE_AccountIDX]    Script Date: 06/09/2026 01:10:56 ******/
CREATE NONCLUSTERED INDEX [NC_TBL_LOG_HACK_MESSAGE_AccountIDX] ON [dbo].[TBL_LOG_HACK_MESSAGE]
(
	[AccountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
ALTER TABLE [dbo].[Log_buy_gold_item] ADD  CONSTRAINT [DF_Log_buy_gold_item_itemType]  DEFAULT ((0)) FOR [itemType]
GO
ALTER TABLE [dbo].[Log_buy_gold_item] ADD  CONSTRAINT [DF_Log_buy_gold_item_itemCode]  DEFAULT ((0)) FOR [itemCode]
GO
ALTER TABLE [dbo].[Log_buy_gold_item] ADD  CONSTRAINT [DF_Log_buy_gold_item_amount]  DEFAULT ((0)) FOR [amount]
GO
ALTER TABLE [dbo].[Log_buy_gold_item] ADD  CONSTRAINT [DF_Log_buy_gold_item_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[Log_buy_item] ADD  CONSTRAINT [DF_Log_buy_item_playType]  DEFAULT ((0)) FOR [playType]
GO
ALTER TABLE [dbo].[Log_buy_item] ADD  CONSTRAINT [DF_Log_buy_item_modeType]  DEFAULT ((0)) FOR [modeType]
GO
ALTER TABLE [dbo].[Log_buy_item] ADD  CONSTRAINT [DF_Log_buy_item_sub1Type]  DEFAULT ((0)) FOR [sub1Type]
GO
ALTER TABLE [dbo].[Log_buy_item] ADD  CONSTRAINT [DF_Log_buy_item_sub2Type]  DEFAULT ((0)) FOR [sub2Type]
GO
ALTER TABLE [dbo].[Log_buy_item] ADD  CONSTRAINT [DF_Log_buy_item_buyplace]  DEFAULT ((0)) FOR [buyplace]
GO
ALTER TABLE [dbo].[Log_buy_item] ADD  CONSTRAINT [DF_Log_buy_item_classType]  DEFAULT ((0)) FOR [classType]
GO
ALTER TABLE [dbo].[Log_buy_item] ADD  CONSTRAINT [DF_Log_buy_item_kindred]  DEFAULT ((-1)) FOR [kindred]
GO
ALTER TABLE [dbo].[Log_buy_item] ADD  CONSTRAINT [DF_Log_buy_item_decoType]  DEFAULT ((-1)) FOR [decoType]
GO
ALTER TABLE [dbo].[Log_buy_item] ADD  CONSTRAINT [DF_Log_buy_item_decoCode]  DEFAULT ((-1)) FOR [decoCode]
GO
ALTER TABLE [dbo].[Log_buy_item] ADD  CONSTRAINT [DF_Log_buy_item_itemType]  DEFAULT ((0)) FOR [itemType]
GO
ALTER TABLE [dbo].[Log_buy_item] ADD  CONSTRAINT [DF_Log_buy_item_buyType]  DEFAULT ((0)) FOR [buyType]
GO
ALTER TABLE [dbo].[Log_buy_item] ADD  CONSTRAINT [DF_Log_buy_item_amount]  DEFAULT ((0)) FOR [amount]
GO
ALTER TABLE [dbo].[Log_buy_item] ADD  CONSTRAINT [DF_Log_buy_item_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_buy_item_accessory] ADD  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_buy_item_accessory] ADD  DEFAULT ((0)) FOR [itemCode]
GO
ALTER TABLE [dbo].[log_buy_item_accessory] ADD  DEFAULT ((0)) FOR [eventType]
GO
ALTER TABLE [dbo].[log_buy_item_accessory] ADD  DEFAULT ((0)) FOR [amount]
GO
ALTER TABLE [dbo].[log_buy_item_accessory] ADD  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_buy_item_class] ADD  CONSTRAINT [DF_log_buy_item_class_userLevel]  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_buy_item_class] ADD  CONSTRAINT [DF_log_buy_item_class_classType]  DEFAULT ((0)) FOR [classType]
GO
ALTER TABLE [dbo].[log_buy_item_class] ADD  CONSTRAINT [DF_log_buy_item_class_limitType]  DEFAULT ((0)) FOR [limitType]
GO
ALTER TABLE [dbo].[log_buy_item_class] ADD  CONSTRAINT [DF_log_buy_item_class_eventType]  DEFAULT ((0)) FOR [eventType]
GO
ALTER TABLE [dbo].[log_buy_item_class] ADD  CONSTRAINT [DF_log_buy_item_class_amount]  DEFAULT ((0)) FOR [amount]
GO
ALTER TABLE [dbo].[log_buy_item_class] ADD  CONSTRAINT [DF_log_buy_item_class_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_buy_item_costume] ADD  CONSTRAINT [DF_log_buy_item_costume_userLevel]  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_buy_item_costume] ADD  CONSTRAINT [DF_log_buy_item_costume_itemCode]  DEFAULT ((0)) FOR [itemCode]
GO
ALTER TABLE [dbo].[log_buy_item_costume] ADD  CONSTRAINT [DF_log_buy_item_costume_eventType]  DEFAULT ((0)) FOR [eventType]
GO
ALTER TABLE [dbo].[log_buy_item_costume] ADD  CONSTRAINT [DF_log_buy_item_costume_amount]  DEFAULT ((0)) FOR [amount]
GO
ALTER TABLE [dbo].[log_buy_item_costume] ADD  CONSTRAINT [DF_log_buy_item_costume_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_buy_item_decoration] ADD  CONSTRAINT [DF_log_buy_item_decoration_classType]  DEFAULT ((0)) FOR [classType]
GO
ALTER TABLE [dbo].[log_buy_item_decoration] ADD  CONSTRAINT [DF_log_buy_item_decoration_kindred]  DEFAULT ((0)) FOR [kindred]
GO
ALTER TABLE [dbo].[log_buy_item_decoration] ADD  CONSTRAINT [DF_log_buy_item_decoration_itemType]  DEFAULT ((0)) FOR [itemType]
GO
ALTER TABLE [dbo].[log_buy_item_decoration] ADD  CONSTRAINT [DF_log_buy_item_decoration_itemCode]  DEFAULT ((0)) FOR [itemCode]
GO
ALTER TABLE [dbo].[log_buy_item_decoration] ADD  CONSTRAINT [DF_log_buy_item_decoration_eventType]  DEFAULT ((0)) FOR [eventType]
GO
ALTER TABLE [dbo].[log_buy_item_decoration] ADD  CONSTRAINT [DF_log_buy_item_decoration_amount]  DEFAULT ((0)) FOR [amount]
GO
ALTER TABLE [dbo].[log_buy_item_decoration] ADD  CONSTRAINT [DF_log_buy_item_decoration_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_buy_item_equip] ADD  CONSTRAINT [DF_log_buy_item_equip_itemType]  DEFAULT ((0)) FOR [itemType]
GO
ALTER TABLE [dbo].[log_buy_item_equip] ADD  CONSTRAINT [DF_log_buy_item_equip_itemCode]  DEFAULT ((0)) FOR [itemCode]
GO
ALTER TABLE [dbo].[log_buy_item_equip] ADD  CONSTRAINT [DF_log_buy_item_equip_reinforce]  DEFAULT ((0)) FOR [reinforce]
GO
ALTER TABLE [dbo].[log_buy_item_equip] ADD  CONSTRAINT [DF_log_buy_item_equip_machineType]  DEFAULT ((0)) FOR [machineType]
GO
ALTER TABLE [dbo].[log_buy_item_equip] ADD  CONSTRAINT [DF_log_buy_item_equip_limitDate]  DEFAULT ((0)) FOR [limitDate]
GO
ALTER TABLE [dbo].[log_buy_item_equip] ADD  CONSTRAINT [DF_log_buy_item_equip_eventType]  DEFAULT ((0)) FOR [eventType]
GO
ALTER TABLE [dbo].[log_buy_item_equip] ADD  CONSTRAINT [DF_log_buy_item_equip_amount]  DEFAULT ((0)) FOR [amount]
GO
ALTER TABLE [dbo].[log_buy_item_equip] ADD  CONSTRAINT [DF_log_buy_item_equip_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_buy_item_gold] ADD  CONSTRAINT [DF_log_buy_item_gold_userLevel]  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_buy_item_gold] ADD  CONSTRAINT [DF_log_buy_item_gold_itemType]  DEFAULT ((0)) FOR [itemType]
GO
ALTER TABLE [dbo].[log_buy_item_gold] ADD  CONSTRAINT [DF_log_buy_item_gold_itemCode]  DEFAULT ((0)) FOR [itemCode]
GO
ALTER TABLE [dbo].[log_buy_item_gold] ADD  CONSTRAINT [DF_log_buy_item_gold_itemValue]  DEFAULT ((0)) FOR [itemValue]
GO
ALTER TABLE [dbo].[log_buy_item_gold] ADD  CONSTRAINT [DF_log_buy_item_gold_amount]  DEFAULT ((0)) FOR [amount]
GO
ALTER TABLE [dbo].[log_buy_item_gold] ADD  CONSTRAINT [DF_log_buy_item_gold_cpType]  DEFAULT ((0)) FOR [cpType]
GO
ALTER TABLE [dbo].[log_buy_item_gold] ADD  CONSTRAINT [DF_log_buy_item_gold_buyType]  DEFAULT ((0)) FOR [buyType]
GO
ALTER TABLE [dbo].[log_buy_item_gold] ADD  CONSTRAINT [DF_log_buy_item_gold_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_buy_item_sbox] ADD  CONSTRAINT [DF_log_buy_item_sbox_userLevel]  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_buy_item_sbox] ADD  CONSTRAINT [DF_log_buy_item_sbox_presentType]  DEFAULT ((0)) FOR [presentType]
GO
ALTER TABLE [dbo].[log_buy_item_sbox] ADD  CONSTRAINT [DF_log_buy_item_sbox_value1]  DEFAULT ((0)) FOR [value1]
GO
ALTER TABLE [dbo].[log_buy_item_sbox] ADD  CONSTRAINT [DF_log_buy_item_sbox_value2]  DEFAULT ((0)) FOR [value2]
GO
ALTER TABLE [dbo].[log_buy_item_sbox] ADD  CONSTRAINT [DF_log_buy_item_sbox_amount]  DEFAULT ((0)) FOR [amount]
GO
ALTER TABLE [dbo].[log_buy_item_sbox] ADD  CONSTRAINT [DF_log_buy_item_sbox_cpType]  DEFAULT ((0)) FOR [cpType]
GO
ALTER TABLE [dbo].[log_buy_item_sbox] ADD  CONSTRAINT [DF_log_buy_item_sbox_billingID]  DEFAULT ((0)) FOR [billingID]
GO
ALTER TABLE [dbo].[log_buy_item_sbox] ADD  CONSTRAINT [DF_log_buy_item_sbox_logType]  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[log_buy_item_sbox] ADD  CONSTRAINT [DF_log_buy_item_sbox_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_buy_item_special] ADD  CONSTRAINT [DF_log_buy_item_special_itemType]  DEFAULT ((0)) FOR [itemType]
GO
ALTER TABLE [dbo].[log_buy_item_special] ADD  CONSTRAINT [DF_log_buy_item_special_itemValue]  DEFAULT ((0)) FOR [itemValue]
GO
ALTER TABLE [dbo].[log_buy_item_special] ADD  CONSTRAINT [DF_log_buy_item_special_eventType]  DEFAULT ((0)) FOR [eventType]
GO
ALTER TABLE [dbo].[log_buy_item_special] ADD  CONSTRAINT [DF_log_buy_item_special_amount]  DEFAULT ((0)) FOR [amount]
GO
ALTER TABLE [dbo].[log_buy_item_special] ADD  CONSTRAINT [DF_log_buy_item_special_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_accessory_upgrade] ADD  CONSTRAINT [DF__log_data___userL__7FD5EEA5]  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_data_accessory_upgrade] ADD  CONSTRAINT [DF__log_data___itemC__00CA12DE]  DEFAULT ((0)) FOR [itemCode]
GO
ALTER TABLE [dbo].[log_data_accessory_upgrade] ADD  CONSTRAINT [DF__log_data___nowUp__01BE3717]  DEFAULT ((0)) FOR [nowUpgradeValue]
GO
ALTER TABLE [dbo].[log_data_accessory_upgrade] ADD  CONSTRAINT [DF__log_data___resul__02B25B50]  DEFAULT ((0)) FOR [resultUpgradeValue]
GO
ALTER TABLE [dbo].[log_data_accessory_upgrade] ADD  CONSTRAINT [DF__log_data___piece__03A67F89]  DEFAULT ((0)) FOR [pieceCode]
GO
ALTER TABLE [dbo].[log_data_accessory_upgrade] ADD  CONSTRAINT [DF__log_data___logTy__049AA3C2]  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[log_data_accessory_upgrade] ADD  CONSTRAINT [DF__log_data___regDa__058EC7FB]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_awake] ADD  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_data_awake] ADD  DEFAULT ((0)) FOR [classType]
GO
ALTER TABLE [dbo].[log_data_awake] ADD  DEFAULT ((0)) FOR [pieceCode]
GO
ALTER TABLE [dbo].[log_data_awake] ADD  DEFAULT ((0)) FOR [pieceUseCount]
GO
ALTER TABLE [dbo].[log_data_awake] ADD  DEFAULT ((0)) FOR [awakeType]
GO
ALTER TABLE [dbo].[log_data_awake] ADD  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_cardmatching] ADD  DEFAULT ((0)) FOR [LogType]
GO
ALTER TABLE [dbo].[log_data_cardmatching] ADD  DEFAULT ((0)) FOR [MissionType]
GO
ALTER TABLE [dbo].[log_data_cardmatching] ADD  DEFAULT ((0)) FOR [MissionMark1]
GO
ALTER TABLE [dbo].[log_data_cardmatching] ADD  DEFAULT ((0)) FOR [MissionMark2]
GO
ALTER TABLE [dbo].[log_data_cardmatching] ADD  DEFAULT ((0)) FOR [UserMark1]
GO
ALTER TABLE [dbo].[log_data_cardmatching] ADD  DEFAULT ((0)) FOR [UserMark2]
GO
ALTER TABLE [dbo].[log_data_cardmatching] ADD  DEFAULT ((0)) FOR [RewardStep]
GO
ALTER TABLE [dbo].[log_data_cardmatching] ADD  DEFAULT ((0)) FOR [ItemType]
GO
ALTER TABLE [dbo].[log_data_cardmatching] ADD  DEFAULT ((0)) FOR [ItemCode]
GO
ALTER TABLE [dbo].[log_data_cardmatching] ADD  DEFAULT ((0)) FOR [ItemValue]
GO
ALTER TABLE [dbo].[log_data_cardmatching] ADD  DEFAULT (getdate()) FOR [RegDate]
GO
ALTER TABLE [dbo].[log_data_character] ADD  CONSTRAINT [DF_log_data_character_playType]  DEFAULT ((0)) FOR [playType]
GO
ALTER TABLE [dbo].[log_data_character] ADD  CONSTRAINT [DF_log_data_character_modeType]  DEFAULT ((0)) FOR [modeType]
GO
ALTER TABLE [dbo].[log_data_character] ADD  CONSTRAINT [DF_log_data_character_subType1]  DEFAULT ((0)) FOR [subType1]
GO
ALTER TABLE [dbo].[log_data_character] ADD  CONSTRAINT [DF_log_data_character_subType2]  DEFAULT ((0)) FOR [subType2]
GO
ALTER TABLE [dbo].[log_data_character] ADD  CONSTRAINT [DF_log_data_character_playTime]  DEFAULT ((0)) FOR [playTime]
GO
ALTER TABLE [dbo].[log_data_character] ADD  CONSTRAINT [DF_log_data_character_characterType]  DEFAULT ((0)) FOR [characterType]
GO
ALTER TABLE [dbo].[log_data_character] ADD  CONSTRAINT [DF_log_data_character_kill]  DEFAULT ((0)) FOR [kill]
GO
ALTER TABLE [dbo].[log_data_character] ADD  CONSTRAINT [DF_log_data_character_death]  DEFAULT ((0)) FOR [death]
GO
ALTER TABLE [dbo].[log_data_character] ADD  CONSTRAINT [DF_log_data_character_regDate]  DEFAULT ((0)) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_cheating] ADD  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_data_cheating] ADD  DEFAULT ((0)) FOR [mainIDX]
GO
ALTER TABLE [dbo].[log_data_cheating] ADD  DEFAULT ((0)) FOR [subIDX]
GO
ALTER TABLE [dbo].[log_data_cheating] ADD  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_clover] ADD  DEFAULT ((0)) FOR [friendIDX]
GO
ALTER TABLE [dbo].[log_data_clover] ADD  DEFAULT ((0)) FOR [eventType]
GO
ALTER TABLE [dbo].[log_data_clover] ADD  DEFAULT ((0)) FOR [CloverCount]
GO
ALTER TABLE [dbo].[log_data_clover] ADD  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_custom_medal] ADD  DEFAULT ((0)) FOR [AccountIDX]
GO
ALTER TABLE [dbo].[log_data_custom_medal] ADD  DEFAULT ((0)) FOR [MedalIDX]
GO
ALTER TABLE [dbo].[log_data_custom_medal] ADD  DEFAULT ((0)) FOR [MedalCode]
GO
ALTER TABLE [dbo].[log_data_custom_medal] ADD  DEFAULT ((0)) FOR [BasicGrowth1]
GO
ALTER TABLE [dbo].[log_data_custom_medal] ADD  DEFAULT ((0)) FOR [BasicGrowth2]
GO
ALTER TABLE [dbo].[log_data_custom_medal] ADD  DEFAULT ((0)) FOR [BasicGrowth3]
GO
ALTER TABLE [dbo].[log_data_custom_medal] ADD  DEFAULT ((0)) FOR [BasicGrowth4]
GO
ALTER TABLE [dbo].[log_data_custom_medal] ADD  DEFAULT ((0)) FOR [SkillGrowth1]
GO
ALTER TABLE [dbo].[log_data_custom_medal] ADD  DEFAULT ((0)) FOR [SkillGrowth2]
GO
ALTER TABLE [dbo].[log_data_custom_medal] ADD  DEFAULT ((0)) FOR [SkillGrowth3]
GO
ALTER TABLE [dbo].[log_data_custom_medal] ADD  DEFAULT ((0)) FOR [SkillGrowth4]
GO
ALTER TABLE [dbo].[log_data_custom_medal] ADD  DEFAULT ((0)) FOR [LimitType]
GO
ALTER TABLE [dbo].[log_data_custom_medal] ADD  DEFAULT ((0)) FOR [LogType]
GO
ALTER TABLE [dbo].[log_data_custom_medal] ADD  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_gear_upgrade] ADD  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_data_gear_upgrade] ADD  DEFAULT ((0)) FOR [gearCode]
GO
ALTER TABLE [dbo].[log_data_gear_upgrade] ADD  DEFAULT ((0)) FOR [nowUpgradeValue]
GO
ALTER TABLE [dbo].[log_data_gear_upgrade] ADD  DEFAULT ((0)) FOR [resultUpgradeValue]
GO
ALTER TABLE [dbo].[log_data_gear_upgrade] ADD  DEFAULT ((0)) FOR [pieceCode]
GO
ALTER TABLE [dbo].[log_data_gear_upgrade] ADD  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[log_data_gear_upgrade] ADD  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_league_present] ADD  CONSTRAINT [DF_log_data_league_present_leagueIDX]  DEFAULT ((0)) FOR [leagueIDX]
GO
ALTER TABLE [dbo].[log_data_league_present] ADD  CONSTRAINT [DF_log_data_league_present_presentCode1]  DEFAULT ((0)) FOR [presentCode1]
GO
ALTER TABLE [dbo].[log_data_league_present] ADD  CONSTRAINT [DF_log_data_league_present_presentCode2]  DEFAULT ((0)) FOR [presentCode2]
GO
ALTER TABLE [dbo].[log_data_league_present] ADD  CONSTRAINT [DF_log_data_league_present_presentCode3]  DEFAULT ((0)) FOR [presentCode3]
GO
ALTER TABLE [dbo].[log_data_league_present] ADD  CONSTRAINT [DF_log_data_league_present_presentCode4]  DEFAULT ((0)) FOR [presentCode4]
GO
ALTER TABLE [dbo].[log_data_league_present] ADD  CONSTRAINT [DF_log_data_league_present_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_localinfo] ADD  CONSTRAINT [DF_log_data_localinfo_userLevel]  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_data_localinfo] ADD  CONSTRAINT [DF_log_data_localinfo_systemResolution]  DEFAULT ((0)) FOR [systemResolution]
GO
ALTER TABLE [dbo].[log_data_localinfo] ADD  CONSTRAINT [DF_log_data_localinfo_losaResolution]  DEFAULT ((0)) FOR [losaResolution]
GO
ALTER TABLE [dbo].[log_data_localinfo] ADD  CONSTRAINT [DF_log_data_localinfo_screenType]  DEFAULT ((0)) FOR [screenType]
GO
ALTER TABLE [dbo].[log_data_localinfo] ADD  CONSTRAINT [DF_log_data_localinfo_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_localinfo] ADD  DEFAULT (NULL) FOR [hdd_serial]
GO
ALTER TABLE [dbo].[log_data_matchmode] ADD  DEFAULT ((0)) FOR [MatchTime]
GO
ALTER TABLE [dbo].[log_data_matchmode] ADD  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[log_data_matchmode] ADD  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_matchmode] ADD  DEFAULT ((0)) FOR [RoomNumber]
GO
ALTER TABLE [dbo].[log_data_matchmode2] ADD  CONSTRAINT [DF__log_data___Match__43F60EC8]  DEFAULT ((0)) FOR [MatchTime]
GO
ALTER TABLE [dbo].[log_data_matchmode2] ADD  CONSTRAINT [DF_log_data_matchmode2_MatchPoint]  DEFAULT ((0)) FOR [MatchPoint]
GO
ALTER TABLE [dbo].[log_data_matchmode2] ADD  CONSTRAINT [DF__log_data___Winni__44EA3301]  DEFAULT ((0)) FOR [WinningStreakCount]
GO
ALTER TABLE [dbo].[log_data_matchmode2] ADD  CONSTRAINT [DF__log_data___logTy__45DE573A]  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[log_data_matchmode2] ADD  CONSTRAINT [DF_log_data_matchmode2_MatchType]  DEFAULT ((0)) FOR [MatchType]
GO
ALTER TABLE [dbo].[log_data_matchmode2] ADD  CONSTRAINT [DF__log_data___RoomN__46D27B73]  DEFAULT ((0)) FOR [RoomNumber]
GO
ALTER TABLE [dbo].[log_data_matchmode2] ADD  CONSTRAINT [DF__log_data___regDa__47C69FAC]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_medal] ADD  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[log_data_medal] ADD  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_medal_extend] ADD  CONSTRAINT [DF_log_data_medal_extend_userLevel]  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_data_medal_extend] ADD  CONSTRAINT [DF_log_data_medal_extend_classType]  DEFAULT ((0)) FOR [classType]
GO
ALTER TABLE [dbo].[log_data_medal_extend] ADD  CONSTRAINT [DF_log_data_medal_extend_slotNum]  DEFAULT ((0)) FOR [slotNum]
GO
ALTER TABLE [dbo].[log_data_medal_extend] ADD  CONSTRAINT [DF_log_data_medal_extend_period]  DEFAULT ((0)) FOR [period]
GO
ALTER TABLE [dbo].[log_data_medal_extend] ADD  CONSTRAINT [DF_log_data_medal_extend_logType]  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[log_data_medal_extend] ADD  CONSTRAINT [DF_log_data_medal_extend_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_oakbarrel] ADD  DEFAULT ((0)) FOR [ItemType]
GO
ALTER TABLE [dbo].[log_data_oakbarrel] ADD  DEFAULT ((0)) FOR [ItemCode]
GO
ALTER TABLE [dbo].[log_data_oakbarrel] ADD  DEFAULT ((0)) FOR [ItemValue]
GO
ALTER TABLE [dbo].[log_data_oakbarrel] ADD  DEFAULT ((0)) FOR [SwordCount]
GO
ALTER TABLE [dbo].[log_data_oakbarrel] ADD  DEFAULT ((0)) FOR [LogType]
GO
ALTER TABLE [dbo].[log_data_oakbarrel] ADD  DEFAULT (getdate()) FOR [RegDate]
GO
ALTER TABLE [dbo].[log_data_pcroom] ADD  CONSTRAINT [DF_log_data_pcroom_playTime]  DEFAULT ((0)) FOR [playTime]
GO
ALTER TABLE [dbo].[log_data_pcroom] ADD  CONSTRAINT [DF_log_data_pcroom_pcnum]  DEFAULT ((0)) FOR [pcnum]
GO
ALTER TABLE [dbo].[log_data_pcroom] ADD  CONSTRAINT [DF_log_data_pcroom_logType]  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[log_data_pcroom] ADD  CONSTRAINT [DF_log_data_pcroom_pcrType]  DEFAULT ((0)) FOR [pcrType]
GO
ALTER TABLE [dbo].[log_data_pcroom] ADD  CONSTRAINT [DF_log_data_pcroom_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_peso] ADD  CONSTRAINT [DF_log_data_peso_peso]  DEFAULT ((0)) FOR [peso]
GO
ALTER TABLE [dbo].[log_data_peso] ADD  CONSTRAINT [DF_log_data_peso_logType]  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[log_data_peso] ADD  CONSTRAINT [DF_log_data_peso_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_pet] ADD  CONSTRAINT [DF_log_data_pet_userLevel]  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_data_pet] ADD  CONSTRAINT [DF__log_data___petCo__23BE4960]  DEFAULT ((0)) FOR [petCode]
GO
ALTER TABLE [dbo].[log_data_pet] ADD  CONSTRAINT [DF__log_data___petRa__24B26D99]  DEFAULT ((0)) FOR [petRank]
GO
ALTER TABLE [dbo].[log_data_pet] ADD  CONSTRAINT [DF__log_data___petLe__25A691D2]  DEFAULT ((0)) FOR [petLevel]
GO
ALTER TABLE [dbo].[log_data_pet] ADD  CONSTRAINT [DF__log_data___petEx__269AB60B]  DEFAULT ((0)) FOR [petExp]
GO
ALTER TABLE [dbo].[log_data_pet] ADD  CONSTRAINT [DF_log_data_pet_itemCode]  DEFAULT ((0)) FOR [itemCode]
GO
ALTER TABLE [dbo].[log_data_pet] ADD  CONSTRAINT [DF__log_data___logTy__278EDA44]  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[log_data_pet] ADD  CONSTRAINT [DF__log_data___logSu__2882FE7D]  DEFAULT ((0)) FOR [logSubType]
GO
ALTER TABLE [dbo].[log_data_pet] ADD  CONSTRAINT [DF__log_data___regDa__297722B6]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_piece_divide] ADD  CONSTRAINT [DF_log_data_piece_divide_userLevel]  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_data_piece_divide] ADD  CONSTRAINT [DF_log_data_piece_divide_itemType]  DEFAULT ((0)) FOR [itemType]
GO
ALTER TABLE [dbo].[log_data_piece_divide] ADD  CONSTRAINT [DF_log_data_piece_divide_itemCode]  DEFAULT ((0)) FOR [itemCode]
GO
ALTER TABLE [dbo].[log_data_piece_divide] ADD  CONSTRAINT [DF_log_data_piece_divide_amount]  DEFAULT ((0)) FOR [amount]
GO
ALTER TABLE [dbo].[log_data_piece_divide] ADD  CONSTRAINT [DF_log_data_piece_divide_materialType]  DEFAULT ((0)) FOR [materialCode]
GO
ALTER TABLE [dbo].[log_data_piece_divide] ADD  CONSTRAINT [DF_log_data_piece_divide_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_piece_mix] ADD  CONSTRAINT [DF_log_data_piece_fusion_userLevel]  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_data_piece_mix] ADD  CONSTRAINT [DF_log_data_piece_fusion_futionType]  DEFAULT ((0)) FOR [mixType]
GO
ALTER TABLE [dbo].[log_data_piece_mix] ADD  CONSTRAINT [DF_log_data_piece_fusion_futionCode]  DEFAULT ((0)) FOR [mixCode]
GO
ALTER TABLE [dbo].[log_data_piece_mix] ADD  CONSTRAINT [DF_log_data_piece_fusion_resultType]  DEFAULT ((0)) FOR [resultType]
GO
ALTER TABLE [dbo].[log_data_piece_mix] ADD  CONSTRAINT [DF_log_data_piece_fusion_useA]  DEFAULT ((0)) FOR [useA]
GO
ALTER TABLE [dbo].[log_data_piece_mix] ADD  CONSTRAINT [DF_log_data_piece_fusion_useB]  DEFAULT ((0)) FOR [useB]
GO
ALTER TABLE [dbo].[log_data_piece_mix] ADD  CONSTRAINT [DF_log_data_piece_fusion_resultA]  DEFAULT ((0)) FOR [resultA]
GO
ALTER TABLE [dbo].[log_data_piece_mix] ADD  CONSTRAINT [DF_log_data_piece_fusion_resultB]  DEFAULT ((0)) FOR [resultB]
GO
ALTER TABLE [dbo].[log_data_piece_mix] ADD  CONSTRAINT [DF_log_data_piece_fusion_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_piece_obtain] ADD  CONSTRAINT [DF_log_data_piece_obtain_userLevel]  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_data_piece_obtain] ADD  CONSTRAINT [DF_log_data_piece_obtain_playTime]  DEFAULT ((0)) FOR [playTime]
GO
ALTER TABLE [dbo].[log_data_piece_obtain] ADD  CONSTRAINT [DF_log_data_piece_obtain_step]  DEFAULT ((0)) FOR [step]
GO
ALTER TABLE [dbo].[log_data_piece_obtain] ADD  CONSTRAINT [DF_log_data_piece_obtain_amount]  DEFAULT ((0)) FOR [amount]
GO
ALTER TABLE [dbo].[log_data_piece_obtain] ADD  CONSTRAINT [DF_log_data_piece_obtain_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF__log_data___playT__0F824689]  DEFAULT ((0)) FOR [playType]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF__log_data___modeT__10766AC2]  DEFAULT ((0)) FOR [modeType]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF__log_data___subTy__116A8EFB]  DEFAULT ((0)) FOR [subType1]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF__log_data___subTy__125EB334]  DEFAULT ((0)) FOR [subType2]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF__log_data___playT__1352D76D]  DEFAULT ((0)) FOR [playTime]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF__log_data___death__1446FBA6]  DEFAULT ((0)) FOR [deathTime]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF__log_data_p__peso__153B1FDF]  DEFAULT ((0)) FOR [peso]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF__log_data_pl__win__162F4418]  DEFAULT ((0)) FOR [win]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF__log_data_p__lose__17236851]  DEFAULT ((0)) FOR [lose]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF__log_data_p__kill__18178C8A]  DEFAULT ((0)) FOR [kill]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF__log_data___death__190BB0C3]  DEFAULT ((0)) FOR [death]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF__log_data___roomN__19FFD4FC]  DEFAULT ((0)) FOR [roomNumber]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF__log_data___contr__1AF3F935]  DEFAULT ((0)) FOR [contribution]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF__log_data___logTy__1BE81D6E]  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF__log_data___pcnum__1CDC41A7]  DEFAULT ((0)) FOR [pcnum]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF_log_data_play_blueTeamMember]  DEFAULT ((0)) FOR [blueTeamMember]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF_log_data_play_redTeamMember]  DEFAULT ((0)) FOR [redTeamMember]
GO
ALTER TABLE [dbo].[log_data_play] ADD  CONSTRAINT [DF__log_data___regDa__1DD065E0]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_play_old] ADD  CONSTRAINT [DF_log_data_play_playType]  DEFAULT ((0)) FOR [playType]
GO
ALTER TABLE [dbo].[log_data_play_old] ADD  CONSTRAINT [DF_log_data_play_modeType]  DEFAULT ((0)) FOR [modeType]
GO
ALTER TABLE [dbo].[log_data_play_old] ADD  CONSTRAINT [DF_log_data_play_subType1]  DEFAULT ((0)) FOR [subType1]
GO
ALTER TABLE [dbo].[log_data_play_old] ADD  CONSTRAINT [DF_log_data_play_subType2]  DEFAULT ((0)) FOR [subType2]
GO
ALTER TABLE [dbo].[log_data_play_old] ADD  CONSTRAINT [DF_log_data_play_playTime]  DEFAULT ((0)) FOR [playTime]
GO
ALTER TABLE [dbo].[log_data_play_old] ADD  CONSTRAINT [DF_log_data_play_deathTime]  DEFAULT ((0)) FOR [deathTime]
GO
ALTER TABLE [dbo].[log_data_play_old] ADD  CONSTRAINT [DF_log_data_play_peso]  DEFAULT ((0)) FOR [peso]
GO
ALTER TABLE [dbo].[log_data_play_old] ADD  CONSTRAINT [DF_log_data_play_win]  DEFAULT ((0)) FOR [win]
GO
ALTER TABLE [dbo].[log_data_play_old] ADD  CONSTRAINT [DF_log_data_play_lose]  DEFAULT ((0)) FOR [lose]
GO
ALTER TABLE [dbo].[log_data_play_old] ADD  CONSTRAINT [DF_log_data_play_kill]  DEFAULT ((0)) FOR [kill]
GO
ALTER TABLE [dbo].[log_data_play_old] ADD  CONSTRAINT [DF_log_data_play_death]  DEFAULT ((0)) FOR [death]
GO
ALTER TABLE [dbo].[log_data_play_old] ADD  CONSTRAINT [DF_log_data_play_logType]  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[log_data_play_old] ADD  CONSTRAINT [DF_log_data_play_pcnum]  DEFAULT ((0)) FOR [pcnum]
GO
ALTER TABLE [dbo].[log_data_play_old] ADD  CONSTRAINT [DF_log_data_play_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_quest] ADD  CONSTRAINT [DF_log_data_quest_userLevel]  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_data_quest] ADD  CONSTRAINT [DF_log_data_quest_mainIDX]  DEFAULT ((0)) FOR [mainIDX]
GO
ALTER TABLE [dbo].[log_data_quest] ADD  CONSTRAINT [DF_log_data_quest_subIDX]  DEFAULT ((0)) FOR [subIDX]
GO
ALTER TABLE [dbo].[log_data_quest] ADD  CONSTRAINT [DF_log_data_quest_questType]  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[log_data_quest] ADD  CONSTRAINT [DF_log_data_quest_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_quest_vfun] ADD  CONSTRAINT [DF_log_data_quest_vfun_userLevel]  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_data_quest_vfun] ADD  CONSTRAINT [DF_log_data_quest_vfun_mainIDX]  DEFAULT ((0)) FOR [mainIDX]
GO
ALTER TABLE [dbo].[log_data_quest_vfun] ADD  CONSTRAINT [DF_log_data_quest_vfun_subIDX]  DEFAULT ((0)) FOR [subIDX]
GO
ALTER TABLE [dbo].[log_data_quest_vfun] ADD  CONSTRAINT [DF_log_data_quest_vfun_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_roominfo] ADD  DEFAULT ((0)) FOR [RoomNumber]
GO
ALTER TABLE [dbo].[log_data_roominfo] ADD  DEFAULT ((0)) FOR [BlueWin]
GO
ALTER TABLE [dbo].[log_data_roominfo] ADD  DEFAULT ((0)) FOR [BlueLose]
GO
ALTER TABLE [dbo].[log_data_roominfo] ADD  DEFAULT ((0)) FOR [BlueTeamMember]
GO
ALTER TABLE [dbo].[log_data_roominfo] ADD  DEFAULT ((0)) FOR [RedWin]
GO
ALTER TABLE [dbo].[log_data_roominfo] ADD  DEFAULT ((0)) FOR [RedLose]
GO
ALTER TABLE [dbo].[log_data_roominfo] ADD  DEFAULT ((0)) FOR [RedTeamMember]
GO
ALTER TABLE [dbo].[log_data_roominfo] ADD  DEFAULT ((0)) FOR [PlayType]
GO
ALTER TABLE [dbo].[log_data_roominfo] ADD  DEFAULT ((0)) FOR [ModeType]
GO
ALTER TABLE [dbo].[log_data_roominfo] ADD  DEFAULT ((0)) FOR [ModeSubType]
GO
ALTER TABLE [dbo].[log_data_roominfo] ADD  DEFAULT (getdate()) FOR [RegDate]
GO
ALTER TABLE [dbo].[log_data_roominfo] ADD  DEFAULT ((0)) FOR [Tier1]
GO
ALTER TABLE [dbo].[log_data_roominfo] ADD  DEFAULT ((0)) FOR [Tier2]
GO
ALTER TABLE [dbo].[log_data_roominfo] ADD  CONSTRAINT [DF_log_data_roominfo_logType]  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[log_data_roominfo] ADD  CONSTRAINT [DF_log_data_roominfo_playTime]  DEFAULT ((0)) FOR [playTime]
GO
ALTER TABLE [dbo].[log_data_spirit_change] ADD  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_spirit_divide] ADD  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_spirit_mix] ADD  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_spirit_obtain] ADD  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_spirit_peso] ADD  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_time] ADD  CONSTRAINT [DF_log_data_time_playTime]  DEFAULT ((0)) FOR [playTime]
GO
ALTER TABLE [dbo].[log_data_time] ADD  CONSTRAINT [DF_log_data_time_logType]  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[log_data_time] ADD  CONSTRAINT [DF_log_data_time_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_trade] ADD  CONSTRAINT [DF_log_data_trade_itemType]  DEFAULT ((0)) FOR [itemType]
GO
ALTER TABLE [dbo].[log_data_trade] ADD  CONSTRAINT [DF_log_data_trade_value1]  DEFAULT ((0)) FOR [value1]
GO
ALTER TABLE [dbo].[log_data_trade] ADD  CONSTRAINT [DF_log_data_trade_value2]  DEFAULT ((0)) FOR [value2]
GO
ALTER TABLE [dbo].[log_data_trade] ADD  CONSTRAINT [DF_log_data_trade_price]  DEFAULT ((0)) FOR [price]
GO
ALTER TABLE [dbo].[log_data_trade] ADD  CONSTRAINT [DF_log_data_trade_logType]  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[log_data_trade] ADD  CONSTRAINT [DF_log_data_trade_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_tutorial] ADD  CONSTRAINT [DF_log_data_tutorial_step]  DEFAULT ((0)) FOR [step]
GO
ALTER TABLE [dbo].[log_data_tutorial] ADD  CONSTRAINT [DF_log_data_tutorial_playTime]  DEFAULT ((0)) FOR [playTime]
GO
ALTER TABLE [dbo].[log_data_tutorial] ADD  CONSTRAINT [DF_log_data_tutorial_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_error_billing] ADD  CONSTRAINT [DF_log_error_billing_cpType]  DEFAULT ((0)) FOR [cpType]
GO
ALTER TABLE [dbo].[log_error_billing] ADD  CONSTRAINT [DF_log_error_billing_errType]  DEFAULT ((0)) FOR [errType]
GO
ALTER TABLE [dbo].[log_error_billing] ADD  CONSTRAINT [DF_log_error_billing_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_event_bingo] ADD  CONSTRAINT [DF_log_event_bingo_choiceType]  DEFAULT ((0)) FOR [choiceType]
GO
ALTER TABLE [dbo].[log_event_bingo] ADD  CONSTRAINT [DF_log_event_bingo_number]  DEFAULT ((0)) FOR [number]
GO
ALTER TABLE [dbo].[log_event_bingo] ADD  CONSTRAINT [DF_log_event_bingo_stateType]  DEFAULT ((0)) FOR [stateType]
GO
ALTER TABLE [dbo].[log_event_cash] ADD  CONSTRAINT [DF_log_event_cash_userLevel]  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_event_cash] ADD  CONSTRAINT [DF_log_event_cash_itemCode]  DEFAULT ((0)) FOR [itemCode]
GO
ALTER TABLE [dbo].[log_event_cash] ADD  CONSTRAINT [DF_log_event_cash_amount]  DEFAULT ((0)) FOR [amount]
GO
ALTER TABLE [dbo].[log_event_cash] ADD  CONSTRAINT [DF_log_event_cash_eventType]  DEFAULT ((0)) FOR [eventType]
GO
ALTER TABLE [dbo].[log_event_cash] ADD  CONSTRAINT [DF_log_event_cash_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[Log_game_concurrent] ADD  CONSTRAINT [DF_Log_game_concurrent_serverID]  DEFAULT ((0)) FOR [serverID]
GO
ALTER TABLE [dbo].[Log_game_concurrent] ADD  CONSTRAINT [DF_Log_game_concurrent_concurrent]  DEFAULT ((0)) FOR [concurrent]
GO
ALTER TABLE [dbo].[Log_game_concurrent] ADD  CONSTRAINT [DF_Log_game_concurrent_cpType]  DEFAULT ((0)) FOR [cpType]
GO
ALTER TABLE [dbo].[Log_game_concurrent] ADD  CONSTRAINT [DF_Log_game_concurrent_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[Log_play_character] ADD  CONSTRAINT [DF_Log_play_character_playType]  DEFAULT ((0)) FOR [playType]
GO
ALTER TABLE [dbo].[Log_play_character] ADD  CONSTRAINT [DF_Log_play_character_modeType]  DEFAULT ((0)) FOR [modeType]
GO
ALTER TABLE [dbo].[Log_play_character] ADD  CONSTRAINT [DF_Log_play_character_sub1Type]  DEFAULT ((0)) FOR [sub1Type]
GO
ALTER TABLE [dbo].[Log_play_character] ADD  CONSTRAINT [DF_Log_play_character_sub2Type]  DEFAULT ((0)) FOR [sub2Type]
GO
ALTER TABLE [dbo].[Log_play_character] ADD  CONSTRAINT [DF_Log_play_character_playTime]  DEFAULT ((0)) FOR [playTime]
GO
ALTER TABLE [dbo].[Log_play_character] ADD  CONSTRAINT [DF_Log_play_character_characterType]  DEFAULT ((0)) FOR [characterType]
GO
ALTER TABLE [dbo].[Log_play_character] ADD  CONSTRAINT [DF_Log_play_character_cnt_kill]  DEFAULT ((0)) FOR [cnt_kill]
GO
ALTER TABLE [dbo].[Log_play_character] ADD  CONSTRAINT [DF_Log_play_character_cnt_killed]  DEFAULT ((0)) FOR [cnt_killed]
GO
ALTER TABLE [dbo].[Log_play_character] ADD  CONSTRAINT [DF_Log_play_character_cnt_selfkilled]  DEFAULT ((0)) FOR [cnt_selfkilled]
GO
ALTER TABLE [dbo].[Log_play_character] ADD  CONSTRAINT [DF_Log_play_character_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_playType]  DEFAULT ((0)) FOR [playType]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_modeType]  DEFAULT ((0)) FOR [modeType]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_sub1Type]  DEFAULT ((0)) FOR [sub1Type]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_sub2Type]  DEFAULT ((0)) FOR [sub2Type]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_playTime]  DEFAULT ((0)) FOR [playTime]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_waitTime]  DEFAULT ((0)) FOR [deathTime]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_basePeso]  DEFAULT ((0)) FOR [basicPeso]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_get_peso]  DEFAULT ((0)) FOR [total_peso]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_get_emblem]  DEFAULT ((0)) FOR [total_emblem]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_total_play_num]  DEFAULT ((0)) FOR [total_play_num]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_total_round]  DEFAULT ((0)) FOR [total_round]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_total_contribute]  DEFAULT ((0)) FOR [total_contribute]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_total_win]  DEFAULT ((0)) FOR [total_win]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_total_lose]  DEFAULT ((0)) FOR [total_lose]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_cnt_kill]  DEFAULT ((0)) FOR [cnt_kill]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_cnt_killed]  DEFAULT ((0)) FOR [cnt_killed]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_cnt_selfkilled]  DEFAULT ((0)) FOR [cnt_selfkilled]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_logType]  DEFAULT ((0)) FOR [logType]
GO
ALTER TABLE [dbo].[Log_play_game] ADD  CONSTRAINT [DF_Log_play_game_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[Log_play_tutorial] ADD  CONSTRAINT [DF_Log_play_tutorial_step]  DEFAULT ((0)) FOR [step]
GO
ALTER TABLE [dbo].[Log_play_tutorial] ADD  CONSTRAINT [DF_Log_play_tutorial_playTime]  DEFAULT ((0)) FOR [playTime]
GO
ALTER TABLE [dbo].[Log_play_tutorial] ADD  CONSTRAINT [DF_Log_play_tutorial_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_present] ADD  CONSTRAINT [DF_log_present_presentType]  DEFAULT ((0)) FOR [presentType]
GO
ALTER TABLE [dbo].[log_present] ADD  CONSTRAINT [DF_log_present_value1]  DEFAULT ((0)) FOR [value1]
GO
ALTER TABLE [dbo].[log_present] ADD  CONSTRAINT [DF_log_present_value2]  DEFAULT ((0)) FOR [value2]
GO
ALTER TABLE [dbo].[log_present] ADD  CONSTRAINT [DF_log_present_eventType]  DEFAULT ((0)) FOR [eventType]
GO
ALTER TABLE [dbo].[log_present] ADD  CONSTRAINT [DF_log_present_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[Log_system_info] ADD  CONSTRAINT [DF_Log_system_info_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_trace_prisoner_option] ADD  DEFAULT ((0)) FOR [BlueMember]
GO
ALTER TABLE [dbo].[log_trace_prisoner_option] ADD  DEFAULT ((0)) FOR [RedMember]
GO
ALTER TABLE [dbo].[log_trace_prisoner_option] ADD  DEFAULT ((0)) FOR [BlueMaxMember]
GO
ALTER TABLE [dbo].[log_trace_prisoner_option] ADD  DEFAULT ((0)) FOR [RedMaxMember]
GO
ALTER TABLE [dbo].[log_trace_prisoner_option] ADD  DEFAULT ((0)) FOR [TeamMixOption]
GO
ALTER TABLE [dbo].[log_trace_prisoner_option] ADD  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_trace_prisoner_option] ADD  DEFAULT ((0)) FOR [closedRoomOption]
GO
ALTER TABLE [dbo].[log_trace_prisoner_option] ADD  DEFAULT ((0)) FOR [userModeOption]
GO
ALTER TABLE [dbo].[log_trace_prisoner_start] ADD  DEFAULT ((0)) FOR [ServerID]
GO
ALTER TABLE [dbo].[log_trace_prisoner_start] ADD  DEFAULT ((0)) FOR [MakeRoom]
GO
ALTER TABLE [dbo].[log_trace_prisoner_start] ADD  DEFAULT ((0)) FOR [QuickStart]
GO
ALTER TABLE [dbo].[log_trace_prisoner_start] ADD  DEFAULT ((0)) FOR [ListSelect]
GO
ALTER TABLE [dbo].[log_trace_prisoner_start] ADD  DEFAULT (getdate()) FOR [RegDate]
GO
ALTER TABLE [dbo].[Log_use_item] ADD  CONSTRAINT [DF_Log_use_item_itemType]  DEFAULT ((0)) FOR [itemType]
GO
ALTER TABLE [dbo].[Log_use_item] ADD  CONSTRAINT [DF_Log_use_item_itemCode]  DEFAULT ((0)) FOR [itemCode]
GO
ALTER TABLE [dbo].[Log_use_item] ADD  CONSTRAINT [DF_Log_use_item_rowIDX]  DEFAULT ((0)) FOR [rowIDX]
GO
ALTER TABLE [dbo].[Log_use_item] ADD  CONSTRAINT [DF_Log_use_item_colIDX]  DEFAULT ((0)) FOR [colIDX]
GO
ALTER TABLE [dbo].[Log_use_item] ADD  CONSTRAINT [DF_Log_use_item_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[LosaGamelog_TEMP_QUEST_LOGIN_BCKIM] ADD  DEFAULT ((0)) FOR [Term_05220612]
GO
ALTER TABLE [dbo].[LosaGamelog_TEMP_QUEST_LOGIN_BCKIM] ADD  DEFAULT ((0)) FOR [Term_06120619]
GO
ALTER TABLE [dbo].[LosaGamelog_TEMP_QUEST_LOGIN_BCKIM] ADD  DEFAULT ((0)) FOR [Term_05220619]
GO
ALTER TABLE [dbo].[LosaGamelog_TEMP_QUEST_LOGIN_BCKIM] ADD  DEFAULT ((0)) FOR [progress]
GO
ALTER TABLE [dbo].[LosaGamelog_TEMP_QUEST_LOGIN_BCKIM] ADD  DEFAULT ((0)) FOR [result]
GO
ALTER TABLE [dbo].[SQL_ERROR_LOG] ADD  DEFAULT (getdate()) FOR [ErrorTime]
GO
ALTER TABLE [dbo].[TBL_LOG_HACK] ADD  DEFAULT ((0)) FOR [UserLevel]
GO
ALTER TABLE [dbo].[TBL_LOG_HACK] ADD  DEFAULT ((0)) FOR [PlayType]
GO
ALTER TABLE [dbo].[TBL_LOG_HACK] ADD  DEFAULT ((0)) FOR [ModeType]
GO
ALTER TABLE [dbo].[TBL_LOG_HACK] ADD  DEFAULT ((0)) FOR [SubType1]
GO
ALTER TABLE [dbo].[TBL_LOG_HACK] ADD  DEFAULT ((0)) FOR [SubType2]
GO
ALTER TABLE [dbo].[TBL_LOG_HACK] ADD  DEFAULT ((0)) FOR [ItemType]
GO
ALTER TABLE [dbo].[TBL_LOG_HACK] ADD  DEFAULT ((0)) FOR [ItemCode]
GO
ALTER TABLE [dbo].[TBL_LOG_HACK] ADD  DEFAULT ((0)) FOR [ItemValue]
GO
ALTER TABLE [dbo].[TBL_LOG_HACK] ADD  DEFAULT (getdate()) FOR [RegDate]
GO
ALTER TABLE [dbo].[TBL_LOG_HACK_MESSAGE] ADD  DEFAULT (getdate()) FOR [RegDate]
GO
/****** Object:  StoredProcedure [dbo].[backup_log_item_gold_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[backup_log_item_gold_add]
(
	@accountIDX int
,	@nickName varchar(20)
,	@userLevel int
,	@itemType int
,	@itemCode int
,	@itemValue int
,	@amount int
,	@cpType smallint
,	@billingID varchar(64)
,	@userIP varchar(15)
,	@note varchar(100)
)
AS

SET NOCOUNT ON

INSERT INTO dbo.log_buy_item_gold (
	accountIDX, nickname, userLevel, itemType, itemCode, itemValue, amount, cpType, billingID, userIP, note, regDate
)
VALUES (
	@accountIDX, @nickname, @userLevel, @itemType, @itemCode, @itemValue, @amount, @cpType, @billingID, @userIP, @note, GETDATE()
)
GO
/****** Object:  StoredProcedure [dbo].[game_log_buy_gold_item]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[game_log_buy_gold_item]
(
	@accountIDX int
,	@nickname varchar(12)
,	@itemType int
,	@itemCode int
,	@amount int
,	@userIP varchar(15)
,	@note varchar(200)
)
AS

	INSERT INTO Log_buy_gold_item (accountIDX, nickname, itemType, itemCode, amount, userIP, note, regDate)
	VALUES (@accountIDX, @nickname, @itemType, @itemCode, @amount, @userIP, @note, getdate())
GO
/****** Object:  StoredProcedure [dbo].[game_log_buy_item]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[game_log_buy_item]
(
	@userID varchar(12)
,	@applyType char(2)
,	@amount int
,	@userIP varchar(16)
,	@playType int
,	@modeType int
,	@sub1Type int
,	@sub2Type int
,	@classType int
,	@kindred int
,	@decoType int
,	@decoCode int
,	@itemType int
,	@buyType int
,	@note varchar(100)
)
As

	INSERT INTO Log_buy_item (userID, applyType, userIP, playType, modeType, sub1Type, sub2Type, classType, kindred, decoType, decoCode, itemType, buyType, amount, note, regDate)
	VALUES (@userID, @applyType, @userIP, @playType, @modeType, @sub1Type, @sub2Type, @classType, @kindred, @decoType, @decoCode, @itemType, @buyType, @amount, @note, getdate())
GO
/****** Object:  StoredProcedure [dbo].[game_log_concurrent]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[game_log_concurrent]
(
	@serverID bigint
,	@serverIP varchar(15)
,	@serverPort int
,	@serverName varchar(32)
,	@concurrent int
,	@cpType smallint
)
As

	INSERT INTO Log_game_concurrent (serverID, serverIP, serverPort, serverName, concurrent, cpType, regDate)
	VALUES (@serverID, @serverIP, @serverPort, @serverName, @concurrent, @cpType, getdate())
GO
/****** Object:  StoredProcedure [dbo].[game_log_play_character]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[game_log_play_character]
(
	@userID varchar(12)
,	@playType int
,	@modeType int
,	@sub1Type int
,	@sub2Type int
,	@playTime int
,	@characterType int
,	@cnt_kill int
,	@cnt_killed int
,	@cnt_selfkilled int
)
As

	INSERT INTO Log_play_character (userID, playType, modeType, sub1Type, sub2Type, playTime, characterType, cnt_kill, cnt_killed, cnt_selfkilled, regDate)
	VALUES (@userID, @playType, @modeType, @sub1Type, @sub2Type, @playTime, @characterType, @cnt_kill, @cnt_killed, @cnt_selfkilled, getdate())
GO
/****** Object:  StoredProcedure [dbo].[game_log_play_game]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[game_log_play_game]
(
	@userID varchar(12)
,	@playType int
,	@modeType int
,	@sub1Type int
,	@sub2Type int
,	@playTime int
,	@deathTime int
,	@basicPeso int
,	@total_peso int
,	@total_emblem int
,	@total_play_num int
,	@total_round int
,	@total_contribute int
,	@total_win int
,	@total_lose int
,	@cnt_kill int
,	@cnt_killed int
,	@cnt_selfkilled int
,	@logType int
)
As

	INSERT INTO Log_play_game (userID, playType, modeType, sub1Type, sub2Type, playTime, deathTime, basicPeso, total_peso, total_emblem, total_play_num, total_round, total_contribute, total_win, total_lose, cnt_kill, cnt_killed, cnt_selfkilled, logType, regDate)
	VALUES (@userID, @playType, @modeType, @sub1Type, @sub2Type, @playTime, @deathTime, @basicPeso, @total_peso, @total_emblem, @total_play_num, @total_round, @total_contribute, @total_win, @total_lose, @cnt_kill, @cnt_killed, @cnt_selfkilled, @logType, getdate())
GO
/****** Object:  StoredProcedure [dbo].[game_log_play_tutorial]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[game_log_play_tutorial]
(
	@userID varchar(12)
,	@step int
,	@playTime int
)
As

	INSERT INTO Log_play_tutorial (userID, step, playTime, regDate)
	VALUES (@userID, @step, @playTime, getdate())
GO
/****** Object:  StoredProcedure [dbo].[game_log_system_add_data]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[game_log_system_add_data]
(
	@logType varchar(32)
,	@serverIP varchar(15)
,	@concurrent int
,	@cpu int
,	@memory int
)
As
	INSERT INTO Log_system_info (logType, serverIP, concurrent, cpu, memory)
	VALUES (@logType, @serverIP, @concurrent, @cpu, @memory)
GO
/****** Object:  StoredProcedure [dbo].[game_log_use_item]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[game_log_use_item]
(
	@accountIDX int
,	@nickname varchar(12)
,	@itemType int
,	@itemCode int
,	@itemRowIDX int
,	@itemColIDX int
,	@userIP varchar(15)
,	@note varchar(200)
)
AS
	INSERT INTO Log_use_item (accountIDX, nickname, itemType, itemCode, rowIDX, colIDX, userIP, note, regDate)
	VALUES (@accountIDX, @nickname, @itemType, @itemCode, @itemRowIDX, @itemColIDX, @userIP, @note, getDate())
GO
/****** Object:  StoredProcedure [dbo].[game_mission_data_delete]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================        
-- Subject  : dbo.game_mission_data_delete        
-- Author  : hryoon
-- DATE   : 2016-12-16        
-- Description : 미션데이터 초기화        
-- =============================================        
      
CREATE PROCEDURE [dbo].[game_mission_data_delete]          
 @YEAR VARCHAR(5),  
 @MONTH VARCHAR(5),  
 @DAY    VARCHAR(5)  
AS          
          
 DECLARE @REG_DATE VARCHAR(20)  
 SET @REG_DATE = @YEAR+'-'+@MONTH+'-'+@DAY  
 SET NOCOUNT ON;          
 SET LOCK_TIMEOUT 10000;          
    
  DELETE FROM dbo.userMissionDB WHERE RegDate < @REG_DATE
GO
/****** Object:  StoredProcedure [dbo].[log_data_accessory_upgrade_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_accessory_upgrade_add]      
(      
 @accountIDX    INT,   -- 유저 인덱스      
 @nickName    NVARCHAR(20), -- 유저 닉네임      
 @userLevel    INT,   -- 유저 레벨      
 @itemCode    INT,   -- 강화 악세서리 코드    
 @nowUpgradeValue  INT,  -- 업그레이드 전 강화 값      
 @resultUpgradeValue  INT,  -- 업그레이드 이후 강화 값      
 @pieceCode    INT,   -- 강화에 사용한 악세서리 코드      
 @logType    INT    -- 로그 구분 [1: 강화 성공, 2: 강화 실패]      
)       
AS      
      
SET NOCOUNT ON      
SET LOCK_TIMEOUT 10000      
      
BEGIN      
      
 INSERT dbo.log_data_accessory_upgrade (      
  accountIDX, nickName, userLevel, itemCode, nowUpgradeValue, resultUpgradeValue, pieceCode, logType, regDate      
 )      
 VALUES (      
  @accountIDX, @nickName, @userLevel, @itemCode, @nowUpgradeValue, @resultUpgradeValue, @pieceCode, @logType, GETDATE()      
 )      
      
END
GO
/****** Object:  StoredProcedure [dbo].[log_data_awake_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_awake_add]
(
	@accountIDX		INT,			-- 사용자 고유키
	@nickName		NVARCHAR(20),	-- 닉네임
	@userLevel		INT,			-- 유저 레벨
	@classType		INT,			-- 용병 코드
	@pieceCode		INT,			-- 조각 코드
	@pieceUseCount	SMALLINT,		-- 사용 된 조각 개수
	@awakeType		TINYINT			-- 각성 타입 (1: 일반각성, 2: 초월각성)
)	
AS

SET NOCOUNT ON

BEGIN

INSERT dbo.log_data_awake (
	accountIDX, nickName, userLevel, classType, pieceCode, pieceUseCount, awakeType, regDate
)
VALUES 	(
	@accountIDX, @nickName, @userLevel, @classType, @pieceCode, @pieceUseCount, @awakeType, GETDATE()
)

END
GO
/****** Object:  StoredProcedure [dbo].[log_data_cardmatching_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Subject		: log_data_cardmatching_add
-- Author		: bckim
-- Date			: 2018-07-01
-- Description	: 카드 매칭 로그 
-- // 2018-06-18 by bckim, 카드 짝 마추기 이벤트 추가
-- =============================================

CREATE PROCEDURE [dbo].[log_data_cardmatching_add]

	@ACCOUNT_IDX		INT,
	@NICK_NAME			NVARCHAR(20),
	@USER_LEVEL			INT,
	@LOG_TYPE			INT,
	
	@MISSION_TYPE		INT,
	@MISSION_MARK1		INT,
	@MISSION_MARK2		INT,
	@USER_MARK1			INT,
	@USER_MARK2			INT,
	
	@REWARD_STEP		INT,
	@ITEM_TYPE			INT,
	@ITEM_CODE			INT,
	@ITEM_VALUE			INT	
AS

	SET NOCOUNT ON;
	SET LOCK_TIMEOUT 10000;

	INSERT INTO log_data_cardmatching (AccountIDX, NickName,UserLevel,LogType,MissionType,MissionMark1,MissionMark2,UserMark1,UserMark2,RewardStep,ItemType,ItemCode, ItemValue,RegDate)
	VALUES (@ACCOUNT_IDX, @NICK_NAME, @USER_LEVEL, @LOG_TYPE, @MISSION_TYPE,@MISSION_MARK1,@MISSION_MARK2,@USER_MARK1,@USER_MARK2,@REWARD_STEP, @ITEM_TYPE, @ITEM_CODE, @ITEM_VALUE, GETDATE())
GO
/****** Object:  StoredProcedure [dbo].[log_data_character_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_character_add]
(
	@accountIDX int
,	@nickName varchar(20)
,	@playType int
,	@modeType int
,	@subType1 int
,	@subType2 int
,	@playTime int
,	@characterType int
,	@kill int
,	@death int
)
As

	INSERT INTO log_data_character (accountIDX, nickName, playType, modeType, subType1, subType2, playTime, characterType, [kill], death, regDate)
	VALUES (@accountIDX, @nickName, @playType, @modeType, @subType1, @subType2, @playTime, @characterType, @kill, @death, getdate())
GO
/****** Object:  StoredProcedure [dbo].[log_data_cheating_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- IF OBJECT_ID('dbo.log_data_pet_add', 'P') IS NOT NULL
-- 	DROP PROC dbo.log_data_pet_add
-- 
-- GO

-- =============================================
-- Subject		: dbo.log_data_cheating_add
-- Author		: hryoon
-- DATE			: 2017-01-13
-- Description	: 치팅퀘스트로그추가
-- =============================================


CREATE PROCEDURE [dbo].[log_data_cheating_add]

	@accountIDX		INT,
	@userID			VARCHAR(20),
	@userLevel		INT,
	@userIP			VARCHAR(20),
	@mainIDX		INT,
	@subIDX			INT 

AS

	SET NOCOUNT ON;
	SET LOCK_TIMEOUT 10000;

	INSERT INTO dbo.log_data_cheating (
		accountIDX, userID, userLevel, userIP, mainIDX, subIDX, regDate
	)
	VALUES (
		@accountIDX, @userID, @userLevel, @userIP, @mainIDX, @subIDX, GETDATE()
	)
GO
/****** Object:  StoredProcedure [dbo].[log_data_clover_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_clover_add]
(
	@userIDX int
,	@FriendIDX int
,	@Type smallint
,	@Count int
)
AS

SET NOCOUNT ON

INSERT INTO dbo.log_data_clover( accountIDX, friendIDX, eventType, CloverCount, regDate )
VALUES( @userIDX, @FriendIDX, @Type, @Count, GETDATE() )
GO
/****** Object:  StoredProcedure [dbo].[log_data_custom_medal_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
--IF OBJECT_ID('dbo.log_data_custom_medal_add','P') IS NOT NULL   
--  
-- DROP PROC dbo.log_data_custom_medal_add  
--  
--GO  
  
-- =============================================  
-- Subject  : dbo.log_data_custom_medal_add  
-- Author  : galura  
-- Date   : 2017-03-16  
-- Description : 커스텀 메달 로그 Insert  
-- =============================================  
  
CREATE PROCEDURE [dbo].[log_data_custom_medal_add]  
  
 @ACCOUNT_IDX  INT,  
 @MEDAL_IDX   INT,  
 @MEDAL_CODE   INT,  
 @BASIC_GROWTH1  INT,  
 @BASIC_GROWTH2  INT,  
 @BASIC_GROWTH3  INT,  
 @BASIC_GROWTH4  INT,  
 @SKILL_GROWTH1  INT,  
 @SKILL_GROWTH2  INT,  
 @SKILL_GROWTH3  INT,  
 @SKILL_GROWTH4  INT,  
 @LIMIT_TYPE   INT,  
 @LIMIT_DATE   DATETIME,  
 @LOG_TYPE   TINYINT  
  
AS  
  
 SET NOCOUNT ON;  
 SET LOCK_TIMEOUT 10000;  
   
 INSERT INTO dbo.log_data_custom_medal (  
  AccountIDX, MedalIDX, MedalCode, BasicGrowth1, BasicGrowth2, BasicGrowth3, BasicGrowth4, SkillGrowth1, SkillGrowth2, SkillGrowth3, SkillGrowth4, LimitType, LimitDate, LogType, RegDate  
 )  
 VALUES (  
  @ACCOUNT_IDX, @MEDAL_IDX, @MEDAL_CODE, @BASIC_GROWTH1, @BASIC_GROWTH2, @BASIC_GROWTH3, @BASIC_GROWTH4, @SKILL_GROWTH1, @SKILL_GROWTH2, @SKILL_GROWTH3, @SKILL_GROWTH4, @LIMIT_TYPE, @LIMIT_DATE, @LOG_TYPE, GETDATE()  
 )
GO
/****** Object:  StoredProcedure [dbo].[log_data_DiceGame_update]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================          
-- Subject  : dbo.log_data_DiceGame_update
-- Author  : bckim          
-- DATE   : 2018-08-30            
-- Description : // 2018-08-30 by bckim, 주사위 이벤트 추가_ 로그 
-- =============================================          
          
CREATE PROCEDURE [dbo].[log_data_DiceGame_update]          
          
 @ACCOUNT_IDX		INT, 
 @Game_State		INT, 
 @DiceCountUsed		INT, 
 @BoardCountUsed	INT, 
 @RewardCountUsed	INT,  
 @Position			INT,             
 @Trace01			INT,
 @Trace02			INT,
 @Trace03			INT,
 @Trace04			INT,
 @Trace05			INT,
 @Trace06			INT,
 @Board				TINYINT,    
 @Reward01			INT,
 @Reward02			INT,
 @Reward03			INT,
 @Reward04			INT,
 @Reward05			INT,
 @Reward06			INT,
 @Reward07			INT,
 @Reward08			INT,
 @Reward09			INT,
 @Reward10			INT 
AS          
 SET NOCOUNT ON;          
 SET LOCK_TIMEOUT 10000;          
	
BEGIN TRAN
	IF EXISTS( SELECT * FROM [dbo].[log_data_DiceGame] WHERE @ACCOUNT_IDX = [AccountIDX] AND [GameState] = 0 )
		BEGIN 				
			UPDATE [dbo].[log_data_DiceGame] WITH (XLOCK, ROWLOCK) 
			SET [GameState] = @Game_State,[DiceCountUsed] = [DiceCountUsed] + @DiceCountUsed,[BoardCountUsed] = [BoardCountUsed] + @BoardCountUsed,	[RewardCountUsed] = [RewardCountUsed] + @RewardCountUsed, 
			[Position] = @Position,[Trace01] = @Trace01,[Trace02] = @Trace02,[Trace03] = @Trace03,[Trace04] = @Trace04,[Trace05] = @Trace05,[Trace06] = @Trace06,[Board] = @Board,
			[RewardStep01] = @Reward01,[RewardStep02] = @Reward02,[RewardStep03] = @Reward03,[RewardStep04] = @Reward04,[RewardStep05] = @Reward05,
			[RewardStep06] = @Reward06,[RewardStep07] = @Reward07,[RewardStep08] = @Reward08,[RewardStep09] = @Reward09,[RewardStep10] = @Reward10, [EndTime] = GETDATE()
			WHERE @ACCOUNT_IDX = [AccountIDX] AND [GameState] = 0
		END
	ELSE
		BEGIN
			INSERT INTO [dbo].[log_data_DiceGame] WITH(XLOCK, ROWLOCK) 
			([AccountIDX],[GameState],[DiceCountUsed],[BoardCountUsed],[RewardCountUsed],[Position],[Trace01],[Trace02],[Trace03],[Trace04],[Trace05],[Trace06],[Board],
			[RewardStep01],[RewardStep02],[RewardStep03],[RewardStep04],[RewardStep05],[RewardStep06],[RewardStep07],[RewardStep08],[RewardStep09],[RewardStep10],[StartTime],[EndTime] )
			VALUES ( @ACCOUNT_IDX,@Game_State,@DiceCountUsed,@BoardCountUsed,@RewardCountUsed,@Position,@Trace01,@Trace02,@Trace03,@Trace04,@Trace05,@Trace06,@Board,
			@Reward01,@Reward02,@Reward03,@Reward04,@Reward05,@Reward06,@Reward07,@Reward08,@Reward09,@Reward10, GETDATE(),GETDATE())
		END 
COMMIT TRAN
GO
/****** Object:  StoredProcedure [dbo].[log_data_gear_upgrade_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_gear_upgrade_add]
(
	@accountIDX				INT,			-- 유저 인덱스
	@nickName				NVARCHAR(20),	-- 유저 닉네임
	@userLevel				INT,			-- 유저 레벨
	@gearCode				INT,			-- 강화 장비 코드
	@nowUpgradeValue		TINYINT,		-- 업그레이드 전 강화 값
	@resultUpgradeValue		TINYINT,		-- 업그레이드 이후 강화 값
	@pieceCode				INT,			-- 조각 코드
	@logType				INT				-- 로그 구분 [1: 강화 성공, 2: 강화 실패]
)	
AS

SET NOCOUNT ON
SET LOCK_TIMEOUT 10000

BEGIN

	INSERT dbo.log_data_gear_upgrade (
		accountIDX, nickName, userLevel, gearCode, nowUpgradeValue, resultUpgradeValue, pieceCode, logType, regDate
	)
	VALUES (
		@accountIDX, @nickName, @userLevel, @gearCode, @nowUpgradeValue, @resultUpgradeValue, @pieceCode, @logType, GETDATE()
	)

END
GO
/****** Object:  StoredProcedure [dbo].[log_data_league_present_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_league_present_add]
(
	@accountIDX int
,	@nickName nvarchar(20)
,	@leagueIDX int
,	@currentRound tinyint
,	@presentCode1 int
,	@presentCode2 int
,	@presentCode3 int
,	@presentCode4 int
)
AS

SET NOCOUNT ON

INSERT INTO dbo.log_data_league_present (
	accountIDX, nickName, leagueIDX, currentRound, presentCode1, presentCode2, presentCode3, presentCode4, regDate
)
VALUES (
	@accountIDX, @nickName, @leagueIDX, @currentRound, @presentCode1, @presentCode2, @presentCode3, @presentCode4, GETDATE()
)
GO
/****** Object:  StoredProcedure [dbo].[log_data_localinfo_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_localinfo_add]
(
	@accountIDX int
,	@nickName varchar(20)
,	@userLevel int
,	@os nvarchar(100)
,	@ie nvarchar(100)
,	@directx nvarchar(100)
,	@cpu nvarchar(100)
,	@gpu nvarchar(100)
,	@memory nvarchar(100)
,	@systemResolution nvarchar(20)
,	@losaResolution nvarchar(20)
,	@screenType tinyint
,	@userIP varchar(15)
)
AS

SET NOCOUNT ON

INSERT INTO dbo.log_data_localinfo (
	accountIDX, nickName, userLevel, os, ie, directx, cpu, gpu, memory, systemResolution, losaResolution, screenType, userIP, regDate
)
VALUES (
	@accountIDX, @nickName, @userLevel, @os, @ie, @directx, @cpu, @gpu, @memory, @systemResolution, @losaResolution, @screenType, @userIP, GETDATE()
)
GO
/****** Object:  StoredProcedure [dbo].[log_data_localinfo_add_2]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_localinfo_add_2]
(
	@accountIDX int
,	@nickName varchar(20)
,	@userLevel int
,	@os nvarchar(100)
,	@ie nvarchar(100)
,	@directx nvarchar(100)
,	@cpu nvarchar(100)
,	@gpu nvarchar(100)
,	@memory nvarchar(100)
,	@systemResolution nvarchar(20)
,	@losaResolution nvarchar(20)
,	@screenType tinyint
,	@userIP varchar(15)
,	@user_hdd_serial nvarchar(100)
)
AS

SET NOCOUNT ON

INSERT INTO dbo.log_data_localinfo (
	accountIDX, nickName, userLevel, os, ie, directx, cpu, gpu, memory, systemResolution, losaResolution, screenType, userIP, regDate, hdd_serial
)
VALUES (
	@accountIDX, @nickName, @userLevel, @os, @ie, @directx, @cpu, @gpu, @memory, @systemResolution, @losaResolution, @screenType, @userIP, GETDATE(),@user_hdd_serial
)
GO
/****** Object:  StoredProcedure [dbo].[log_data_matchmode_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
--IF OBJECT_ID('dbo.log_data_matchmode_add', 'U') IS NOT NULL
--	DROP TABLE dbo.log_data_matchmode_add
--
--GO

-- =============================================
-- Subject		: dbo.log_data_matchmode_add
-- Author		: galura
-- Date			: 2016-08-19
-- Description	: 랭킹전 관련 로그
-- =============================================


CREATE PROCEDURE [dbo].[log_data_matchmode_add]

	@ACCOUNT_IDX		INT,
	@NICK_NAME			NVARCHAR(20),
	@USER_LEVEL			INT,
	@TIER1				INT,
	@TIER2				INT,
	@MATCH_TIME			INT,
	@LOG_TYPE			INT,
	@ROOM_NUMBER		INT

AS

	SET NOCOUNT ON;
	SET LOCK_TIMEOUT 10000;

	INSERT INTO dbo.log_data_matchmode (AccountIDX, nickName, userLevel, Tier1, Tier2, MatchTime, logType, regDate, RoomNumber)
	VALUES (@ACCOUNT_IDX, @NICK_NAME, @USER_LEVEL, @TIER1, @TIER2, @MATCH_TIME, @LOG_TYPE, GETDATE(), @ROOM_NUMBER);
GO
/****** Object:  StoredProcedure [dbo].[log_data_matchmode2_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- IF OBJECT_ID('dbo.log_data_matchmode2_add', 'P') IS NOT NULL        
--  DROP PROC dbo.log_data_matchmode2_add        
        
-- GO        
        
-- =============================================        
-- Subject  : dbo.log_data_matchmode2_add        
-- Author  : galura        
-- DATE   : 2016-12-07        
-- Description : 1:1 모드 관련 로그        
-- =============================================        
        
        
CREATE PROCEDURE [dbo].[log_data_matchmode2_add]        
        
 @ACCOUNT_IDX   INT,        
 @NICK_NAME    NVARCHAR(20),        
 @USER_LEVEL    INT,        
 @TIER1     INT,        
 @TIER2     INT,        
 @MATCH_TIME    INT,        
 @MATCH_POINT INT,        
 @WINNING_STREAK_COUNT INT,        
 @LOG_TYPE    INT,        
 @ROOM_NUMBER   INT,  
 @MATCH_TYPE INT        
        
AS        
        
        
 SET NOCOUNT ON;        
 SET LOCK_TIMEOUT 10000;        
        
 INSERT INTO dbo.log_data_matchmode2 (AccountIDX, nickName, userLevel, Tier1, Tier2, MatchTime, MatchPoint, WinningStreakCount, logType, MatchType, RoomNumber, regDate)        
 VALUES (@ACCOUNT_IDX, @NICK_NAME, @USER_LEVEL, @TIER1, @TIER2, @MATCH_TIME, @MATCH_POINT, @WINNING_STREAK_COUNT, @LOG_TYPE, @MATCH_TYPE, @ROOM_NUMBER, GETDATE());
GO
/****** Object:  StoredProcedure [dbo].[log_data_medal_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_medal_add]
(
	@accountIDX int
,	@nickName varchar(20)
,	@medalType int
,	@limitType int
,	@logType int
)
As

	INSERT INTO log_data_medal (accountIDX, nickName, medalType, limitType, logType, regDate)
	VALUES (@accountIDX, @nickName, @medalType, @limitType, @logType, getdate())
GO
/****** Object:  StoredProcedure [dbo].[log_data_medal_extend_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_medal_extend_add]
(
	@accountIDX int
,	@nickName varchar(20)
,	@userLevel int
,	@classType int
,	@slotNum tinyint
,	@period int
,	@logType int
)
AS

INSERT INTO dbo.log_data_medal_extend (
	accountIDX, nickName, userLevel, classType, slotNum, period, logType, regDate
)
VALUES (
	@accountIDX, @nickName, @userLevel, @classType, @slotNum, @period, @logType, getdate()
)
GO
/****** Object:  StoredProcedure [dbo].[log_data_oakbarrel_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
--IF OBJECT_ID('dbo.log_data_oakbarrel_add','P') IS NOT NULL	
--
--	DROP PROC dbo.log_data_oakbarrel_add
--
--GO

-- =============================================
-- Subject		: dbo.log_data_oakbarrel_add
-- Author		: galura
-- Date			: 2016-07-01
-- Description	: 오크통 로그
-- =============================================

CREATE PROCEDURE [dbo].[log_data_oakbarrel_add]

	@ACCOUNT_IDX		INT,
	@NICK_NAME			NVARCHAR(20),
	@USER_LEVEL			INT,
	@ITEM_TYPE			INT,
	@ITEM_CODE			INT,
	@ITEM_VALUE			INT,
	@SWORD_COUNT		INT,
	@LOG_TYPE			INT

AS

	SET NOCOUNT ON;
	SET LOCK_TIMEOUT 10000;

	INSERT INTO log_data_oakbarrel (AccountIDX, NickName, UserLevel, ItemType, ItemCode, ItemValue, SwordCount, LogType, RegDate)
	VALUES (@ACCOUNT_IDX, @NICK_NAME, @USER_LEVEL, @ITEM_TYPE, @ITEM_CODE, @ITEM_VALUE, @SWORD_COUNT, @LOG_TYPE, GETDATE())
GO
/****** Object:  StoredProcedure [dbo].[log_data_pcroom_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_pcroom_add]
(
	@accountIDX int
,	@nickName varchar(20)
,	@playTime int
,	@userIP varchar(15)
,	@pcnum int
,	@logType int
,	@pcrType int
)
AS

INSERT INTO log_data_pcroom (accountIDX, nickName, playTime, userIP, pcnum, logType, pcrType, regDate)
VALUES (@accountIDX, @nickName, @playTime, @userIP, @pcnum, @logType, @pcrType, getdate())
GO
/****** Object:  StoredProcedure [dbo].[log_data_peso_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_peso_add]
(
	@accountIDX int
,	@nickName varchar(20)
,	@peso int
,	@logType int
)
As

	INSERT INTO log_data_peso (accountIDX, nickName, peso, logType, regDate)
	VALUES (@accountIDX, @nickName, @peso, @logType, getdate())
GO
/****** Object:  StoredProcedure [dbo].[log_data_pet_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_pet_add]

	@ACCOUNT_IDX	INT,			-- 사용자 고유키
	@NICKNAME		NVARCHAR(20),	-- 닉네임
	@USERLEVEL		INT,			-- 유저 레벨
	@PET_IDX		INT,			-- 펫 고유키
	@PTE_CODE		INT,			-- 펫 인덱스
	@PET_RANK		TINYINT,		-- 랭크
	@PET_LEVEL		INT,			-- 레벨
	@PET_EXP		INT,			-- 경험치
	@ITEM_CODE		INT,			-- 펫 획득 가챠 코드
	@LOG_TYPE		TINYINT,		-- 로그 구분 - 1: 생성 2: 삭제 3: 장착 4: 랭크업 5: 레벨업 6: 육성 7:교배
	@LOG_SUB_TYPE	INT,			-- LogType에 해당하는 값. 없으면 0;
	@LOG_NOTE		VARCHAR(200)	-- 기타 비고란.
	
AS

	SET NOCOUNT ON
	SET LOCK_TIMEOUT 10000

	BEGIN

		INSERT dbo.log_data_pet WITH(ROWLOCK, XLOCK) (accountIDX, nickName, userLevel, petIDX, petCode, petRank, petLevel, petExp, itemCode, logType, logSubType, logNote) VALUES
		(@ACCOUNT_IDX, @NICKNAME, @USERLEVEL, @PET_IDX, @PTE_CODE, @PET_RANK, @PET_LEVEL, @PET_EXP, @ITEM_CODE, @LOG_TYPE, @LOG_SUB_TYPE, @LOG_NOTE)
	END
GO
/****** Object:  StoredProcedure [dbo].[log_data_piece_divide_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_piece_divide_add]
(
	@accountIDX int
,	@nickName nvarchar(20)
,	@userLevel int
,	@itemType int
,	@itemCode int
,	@amount int
,	@materialCode int
)
AS

SET NOCOUNT ON

INSERT INTO dbo.log_data_piece_divide (
	accountIDX, nickName, userLevel, itemType, itemCode, amount, materialCode, regDate
)
VALUES (
	@accountIDX, @nickName, @userLevel, @itemType, @itemCode, @amount, @materialCode, GETDATE()
)
GO
/****** Object:  StoredProcedure [dbo].[log_data_piece_mix_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_piece_mix_add]
(
	@accountIDX int
,	@nickName nvarchar(20)
,	@userLevel int
,	@mixType smallint
,	@mixCode int
,	@resultType tinyint
,	@useA int
,	@useB int
,	@resultA int
,	@resultB int
)
AS

SET NOCOUNT ON

INSERT INTO dbo.log_data_piece_mix (
	accountIDX, nickName, userLevel, mixType, mixCode, resultType, useA, useB, resultA, resultB, regDate
)
VALUES (
	@accountIDX, @nickName, @userLevel, @mixType, @mixCode, @resultType, @useA, @useB, @resultA, @resultB, GETDATE()
)
GO
/****** Object:  StoredProcedure [dbo].[log_data_piece_obtain_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_piece_obtain_add]
(
	@accountIDX int
,	@nickName nvarchar(20)
,	@userLevel int
,	@playTime int
,	@step tinyint
,	@amount int
)
AS

SET NOCOUNT ON

INSERT INTO dbo.log_data_piece_obtain (
	accountIDX, nickName, userLevel, playTime, step, amount, regDate
)
VALUES (
	@accountIDX, @nickName, @userLevel, @playTime, @step, @amount, GETDATE()
)
GO
/****** Object:  StoredProcedure [dbo].[log_data_play_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- IF OBJECT_ID('dbo.log_data_play_add', 'P') IS NOT NULL
-- 	DROP PROC dbo.log_data_play_add
-- 
-- GO

-- =============================================
-- Subject		: dbo.log_data_play_add
-- Authod		: 
-- Date			: 
-- Description	: 
-- =============================================

CREATE PROCEDURE [dbo].[log_data_play_add]

	@accountIDX			INT,
	@nickName			VARCHAR(20),
	@playType			INT,
	@modeType			INT,
	@subType1			INT,
	@subType2			INT,
	@playTime			INT,
	@deathTime			INT,
	@peso				INT,
	@win				INT,
	@lose				INT,
	@kill				INT,
	@death				INT,
	@logType			INT,
	@pcnum				INT,
	@roomNumber			INT,
	@contribution		INT,
	@blueTeamMember		INT,
	@redTeamMember		INT

AS  

	SET NOCOUNT ON;
	SET LOCK_TIMEOUT 10000;

	INSERT INTO dbo.log_data_play (
		accountIDX, nickName, playType, modeType, subType1, subType2, playTime, deathTime, peso, win, lose, [kill], death, roomNumber, contribution, logType, pcnum, blueTeamMember, redTeamMember, regDate
	)
	VALUES (
		@accountIDX, @nickName, @playType, @modeType, @subType1, @subType2, @playTime, @deathTime, @peso, @win, @lose, @kill, @death, @roomNumber, @contribution, @logType, @pcnum, @blueTeamMember, @redTeamMember, GETDATE()
	)
GO
/****** Object:  StoredProcedure [dbo].[log_data_play_room_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================      
-- Subject  : dbo.log_data_play_room_add      
-- Author  : galura      
-- Date   : 2016-06-13      
-- Description : 룸 정보      
-- =============================================      
      
CREATE PROCEDURE [dbo].[log_data_play_room_add]      
      
 @RoomNumber   INT,      
 @BlueWin   INT,      
 @BlueLose   INT,      
 @BlueTeamMember  INT,      
 @RedWin    INT,      
 @RedLose   INT,      
 @RedTeamMember  INT,      
 @PlayType   INT,      
 @ModeType   INT,      
 @ModeSubType  INT,      
 @Tier1    INT,      
 @Tier2    INT,    
 @LogType  INT,
 @playTime INT      
      
AS          
      
SET NOCOUNT ON;      
SET LOCK_TIMEOUT 10000;      
      
INSERT INTO dbo.log_data_roominfo (      
 RoomNumber, BlueWin, BlueLose, BlueTeamMember, RedWin, RedLose, RedTeamMember, PlayType, ModeType, ModeSubType, RegDate, Tier1, Tier2, LogType, playTime      
)      
VALUES (      
 @RoomNumber, @BlueWin, @BlueLose, @BlueTeamMember, @RedWin, @RedLose, @RedTeamMember, @PlayType, @ModeType, @ModeSubType, GETDATE(), @Tier1, @Tier2, @LogType, @playTime      
)
GO
/****** Object:  StoredProcedure [dbo].[log_data_quest_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_quest_add]
(
	@accountIDX int
,	@nickName varchar(20)
,	@userLevel int
,	@mainIDX int
,	@subIDX int
,	@logType int
)
As

	INSERT INTO log_data_quest (accountIDX, nickName, userLevel, mainIDX, subIDX, logType, regDate)
	VALUES (@accountIDX, @nickName, @userLevel, @mainIDX, @subIDX, @logType, getdate())
GO
/****** Object:  StoredProcedure [dbo].[log_data_quest_vfun_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_quest_vfun_add]
(
	@accountIDX int
,	@nickName varchar(20)
,	@userLevel int
,	@mainIDX int
,	@subIDX int
,	@channelingType int
,	@success int
,	@reason varchar(256)
)
As

	INSERT INTO log_data_quest_vfun (accountIDX, nickName, userLevel, mainIDX, subIDX, channelingType, success, reason, regDate)
	VALUES (@accountIDX, @nickName, @userLevel, @mainIDX, @subIDX, @channelingType,@success,@reason,getdate())
GO
/****** Object:  StoredProcedure [dbo].[log_data_spirit_change_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- IF OBJECT_ID('dbo.log_data_spirit_change_add', 'P') IS NOT NULL
-- 	DROP PROC dbo.log_data_spirit_change_add

-- GO

-- =============================================
-- Subject		: dbo.log_data_spirit_change_add
-- Author		: galura
-- DATE			: 2016-12-07
-- Description	: 1:1 모드 관련 로그
-- =============================================


CREATE PROCEDURE [dbo].[log_data_spirit_change_add]

	@ACCOUNT_IDX		INT,
	@NICK_NAME			NVARCHAR(20),
	@USER_LEVEL			INT,
	@SET_SPIRIT_CODE	INT,
	@SET_SPIRIT_COUNT	INT,
	@GET_SPIRIT_CODE	INT,
	@GET_SPIRIT_COUNT	INT,
	@CRITICAL_TYPE		TINYINT

AS

	SET NOCOUNT ON;
	SET LOCK_TIMEOUT 10000;

	INSERT INTO dbo.log_data_spirit_change (
		AccountIDX, NickName, UserLevel, SetSpiritCode, SetSpiritCount, GetSpiritCode, GetSpiritCount, CriticalType, regDate
	)
	VALUES (
		@ACCOUNT_IDX, @NICK_NAME, @USER_LEVEL, @SET_SPIRIT_CODE, @SET_SPIRIT_COUNT, @GET_SPIRIT_CODE, @GET_SPIRIT_COUNT, @CRITICAL_TYPE, GETDATE()
	);
GO
/****** Object:  StoredProcedure [dbo].[log_data_spirit_divide_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- IF OBJECT_ID('dbo.log_data_spirit_divide_add', 'P') IS NOT NULL
-- 	DROP PROC dbo.log_data_spirit_divide_add

-- GO

-- =============================================
-- Subject		: dbo.log_data_spirit_divide_add
-- Author		: galura
-- DATE			: 2016-12-07
-- Description	: 1:1 모드 관련 로그
-- =============================================


CREATE PROCEDURE [dbo].[log_data_spirit_divide_add]

	@ACCOUNT_IDX		INT,
	@NICK_NAME			NVARCHAR(20),
	@USER_LEVEL			INT,
	@HERO_TYPE			INT,
	@SPIRIT_CODE		INT,
	@SPIRIT_COUNT		INT,
	@SOUL_STONE_COUNT	INT,
	@CRITICAL_TYPE		TINYINT

AS

	SET NOCOUNT ON;
	SET LOCK_TIMEOUT 10000;

	INSERT INTO dbo.log_data_spirit_divide (
		AccountIDX, NickName, UserLevel, HeroType, SpiritCode, SpiritCount, SoulStoneCount, CriticalType, regDate
	)
	VALUES (
		@ACCOUNT_IDX, @NICK_NAME, @USER_LEVEL, @HERO_TYPE, @SPIRIT_CODE, @SPIRIT_COUNT, @SOUL_STONE_COUNT, @CRITICAL_TYPE, GETDATE()
	);
GO
/****** Object:  StoredProcedure [dbo].[log_data_spirit_mix_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- IF OBJECT_ID('dbo.log_data_spirit_mix_add', 'P') IS NOT NULL
-- 	DROP PROC dbo.log_data_spirit_mix_add

-- GO

-- =============================================
-- Subject		: dbo.log_data_spirit_mix_add
-- Author		: galura
-- DATE			: 2016-12-07
-- Description	: 1:1 모드 관련 로그
-- =============================================


CREATE PROCEDURE [dbo].[log_data_spirit_mix_add]

	@ACCOUNT_IDX			INT,
	@NICK_NAME				NVARCHAR(20),
	@USER_LEVEL				INT,
	@HERO_TYPE				INT,
	@SPIRIT_CODE			INT,
	@SPIRIT_COUNT			INT,
	@UNKNOWN_SPIRIT_CODE	INT,
	@UNKNOWN_SPIRIT_COUNT	INT

AS

	SET NOCOUNT ON;
	SET LOCK_TIMEOUT 10000;

	INSERT INTO dbo.log_data_spirit_mix (
		AccountIDX, NickName, UserLevel, HeroType, SpiritCode, SpiritCount, UnknownSpiritCode, UnknownSpiritCount, regDate
	)
	VALUES (
		@ACCOUNT_IDX, @NICK_NAME, @USER_LEVEL, @HERO_TYPE, @SPIRIT_CODE, @SPIRIT_COUNT, @UNKNOWN_SPIRIT_CODE, @UNKNOWN_SPIRIT_COUNT, GETDATE()
	);
GO
/****** Object:  StoredProcedure [dbo].[log_data_spirit_obtain_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- IF OBJECT_ID('dbo.log_data_spirit_obtain_add', 'P') IS NOT NULL
-- 	DROP PROC dbo.log_data_spirit_obtain_add

-- GO

-- =============================================
-- Subject		: dbo.log_data_spirit_obtain_add
-- Author		: galura
-- DATE			: 2016-12-07
-- Description	: 1:1 모드 관련 로그
-- =============================================


CREATE PROCEDURE [dbo].[log_data_spirit_obtain_add]

	@ACCOUNT_IDX	INT,
	@NICK_NAME		NVARCHAR(20),
	@USER_LEVEL		INT,
	@SPIRIT_CODE	INT,
	@SPIRIT_COUNT	INT,
	@VALUE			INT,
	@LOG_TYPE		TINYINT

AS

	SET NOCOUNT ON;
	SET LOCK_TIMEOUT 10000;

	INSERT INTO dbo.log_data_spirit_obtain (
		AccountIDX, NickName, UserLevel, SpiritCode, SpiritCount, Value, logType, regDate
	)
	VALUES (
		@ACCOUNT_IDX, @NICK_NAME, @USER_LEVEL, @SPIRIT_CODE, @SPIRIT_COUNT, @VALUE, @LOG_TYPE, GETDATE()
	);
GO
/****** Object:  StoredProcedure [dbo].[log_data_spirit_peso_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- IF OBJECT_ID('dbo.log_data_spirit_peso_add', 'P') IS NOT NULL
-- 	DROP PROC dbo.log_data_spirit_peso_add

-- GO

-- =============================================
-- Subject		: dbo.log_data_spirit_peso_add
-- Author		: galura
-- DATE			: 2016-12-07
-- Description	: 1:1 모드 관련 로그
-- =============================================


CREATE PROCEDURE [dbo].[log_data_spirit_peso_add]

	@ACCOUNT_IDX		INT,
	@NICK_NAME			NVARCHAR(20),
	@USER_LEVEL			INT,
	@PIECE_COUNT		INT,
	@DIMENSION_COUNT	INT,
	@GET_PESO			INT

AS

	SET NOCOUNT ON;
	SET LOCK_TIMEOUT 10000;

	INSERT INTO dbo.log_data_spirit_peso (
		AccountIDX, NickName, UserLevel, PieceCount, DimensionCount, Peso, regDate
	)
	VALUES (
		@ACCOUNT_IDX, @NICK_NAME, @USER_LEVEL, @PIECE_COUNT, @DIMENSION_COUNT, @GET_PESO, GETDATE()
	);
GO
/****** Object:  StoredProcedure [dbo].[log_data_time_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_time_add]
(
	@accountIDX int
,	@nickName varchar(20)
,	@playTime int
,	@logType int
)
As

	INSERT INTO log_data_time (accountIDX, nickName, playTime, logType, regDate)
	VALUES (@accountIDX, @nickName, @playTime, @logType, getdate())
GO
/****** Object:  StoredProcedure [dbo].[log_data_trade_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_trade_add]
(
	@accountIDX int
,	@nickname varchar(20)
,	@itemType int
,	@value1 int
,	@value2 int
,	@price bigint
,	@logType int
,	@userIP varchar(16)
,	@note varchar(100)
)
AS

	INSERT INTO log_data_trade (accountIDX, nickname, itemType, value1, value2, price, logType, userIP, note, regDate)
	VALUES (@accountIDX, @nickName, @itemType, @value1, @value2, @price, @logType, @userIP, @note, getdate())
GO
/****** Object:  StoredProcedure [dbo].[log_data_tutorial_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_data_tutorial_add]
(
	@accountIDX int
,	@nickName varchar(20)
,	@step int
,	@playTime int
)
As

	INSERT INTO log_data_tutorial (accountIDX, nickName, step, playTime, regDate)
	VALUES (@accountIDX, @nickName, @step, @playTime, getdate())
GO
/****** Object:  StoredProcedure [dbo].[log_error_billing_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_error_billing_add]
(
	@cpType smallint
,	@errType int
,	@note varchar(200)
)
AS
	INSERT INTO log_error_billing (cpType, errType, note, regDate)
	VALUES (@cpType, @errType, @note, getdate())
GO
/****** Object:  StoredProcedure [dbo].[log_errorLog_set_log]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Subject		: dbo.log_errorLog_set_log
-- DBA			: KeuN
-- DATE			: 2014-11-26 10:10:00
-- Description	: 예외 처리 및 SQL 에러 로그
-- =============================================


CREATE PROC [dbo].[log_errorLog_set_log]

	@ACCOUNT_IDX		INT

AS
		
	SET NOCOUNT ON
	
	DECLARE @ERROR_NO			INT
	DECLARE @ERROR_PROC			VARCHAR(50)
	DECLARE @ERROR_LINE			INT
	DECLARE @ERROR_SEVERITY		INT
	DECLARE @ERROR_STATE		INT
	DECLARE @ERROR_MESSAGE		VARCHAR(500)
				
	SELECT  @ERROR_NO		= ERROR_NUMBER(), 
			@ERROR_PROC		= ERROR_PROCEDURE(), 
			@ERROR_LINE		= ERROR_LINE(), 
			@ERROR_SEVERITY	= ERROR_SEVERITY(), 
			@ERROR_STATE	= ERROR_STATE(), 
			@ERROR_MESSAGE	= ERROR_MESSAGE()


	BEGIN TRAN

		INSERT dbo.SQL_ERROR_LOG VALUES 
		(@ACCOUNT_IDX, @ERROR_NO, @ERROR_PROC, @ERROR_LINE, @ERROR_SEVERITY, @ERROR_STATE, @ERROR_MESSAGE, GETDATE())

	COMMIT TRAN
	
	RETURN 1
GO
/****** Object:  StoredProcedure [dbo].[log_event_bingo_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_event_bingo_add]
(
	@accountIDX int
,	@nickName	nvarchar( 20 )
,	@choiceType tinyint
,	@number		tinyint
,	@stateType	tinyint
)
AS

SET NOCOUNT ON

INSERT INTO dbo.log_event_bingo (accountIDX, nickName, choiceType, number, stateType, regDate)
VALUES (@accountIDX, @nickName, @choiceType, @number, @stateType, GETDATE())
GO
/****** Object:  StoredProcedure [dbo].[log_event_cash_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_event_cash_add]
(
	@accountIDX int
,	@nickName nvarchar(20)
,	@userLevel int
,	@itemCode int
,	@amount int
,	@eventType tinyint
,	@userIP nvarchar(16)
)
AS

SET NOCOUNT ON

INSERT INTO dbo.log_event_cash (accountIDX, nickName, userLevel, itemCode, amount, eventType, userIP, regDate)
VALUES (@accountIDX, @nickName, @userLevel, @itemCode, @amount, @eventType, @userIP, GETDATE())
GO
/****** Object:  StoredProcedure [dbo].[log_Excavating_log_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_Excavating_log_add]

	@LOG_TYPE		INT,		
	@ACCOUNT_IDX	INT,
	@USER_ID		VARCHAR(20),	
	@USER_NAME		VARCHAR(20),
	@USER_EX_LEVEL	INT,
	
	@USER_EX_EXP	INT,
	@CLASS_TYPE		INT,
	@CLASS_LEVEL	INT,	
	@CLASS_EXPERT	INT,
	@REMAIN_COUNT	INT,
	
	@MAP_ID			INT,
	@X				INT,
	@Y				INT,
	@Z				INT,
	@REWARD_TYPE	INT,	
	
	@INDEX			INT,
	@PRICE			INT,
	@MULIPLE		INT,
	@RESULT_MONEY	BIGINT,
	@USER_IP		VARCHAR(15)	
AS

	SET NOCOUNT ON
	SET LOCK_TIMEOUT 10000

	DECLARE @REG_DATE			DATETIME
	SET @REG_DATE=GETDATE()

	BEGIN TRY
		
		BEGIN TRAN
			INSERT dbo.log_data_excavating_new WITH (XLOCK, ROWLOCK) ([RegDate],	
			[LogType],[AccountIDX],[UserID],[UserName],[ExLevel],
			[ExEXP],[CType],[CLevel],[CExpert],[RCount],
			[MapID],[X],[Y],[Z],[RewardType],
			[Index],[Price],[Mutiple],[ResultMoney],[UserIP] )
			VALUES (@REG_DATE, 
			@LOG_TYPE,@ACCOUNT_IDX,@USER_ID, @USER_NAME,@USER_EX_LEVEL,
			@USER_EX_EXP,@CLASS_TYPE,@CLASS_LEVEL,@CLASS_EXPERT,@REMAIN_COUNT,	
			@MAP_ID,@X,@Y,@Z,@REWARD_TYPE,
			@INDEX,@PRICE,@MULIPLE,@RESULT_MONEY,@USER_IP
			)
		COMMIT TRAN
				
	END TRY
	BEGIN CATCH

		ROLLBACK TRAN				
		EXEC dbo.log_errorLog_set_log @ACCOUNT_IDX
		RETURN -1

	END CATCH

	RETURN 0;
GO
/****** Object:  StoredProcedure [dbo].[log_gameLog_set_log]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- Subject		: dbo.dbo.log_gameLog_set_log
-- DBA			: KeuN
-- DATE			: 2014-11-26 10:10:00
-- Description	: 통합 게임 로그 Insert
-- =============================================


CREATE PROCEDURE [dbo].[log_gameLog_set_log]

	@LOG_TYPE			INT,
	@ACCOUNT_IDX		INT,
	@USER_ID			VARCHAR(20),
	@USER_NAME			VARCHAR(20),
	@USER_LEVEL			INT,
	@USER_EXP			INT,
	@CHANNEL_TYPE		INT,
	@TBL_INDEX			INT,
	@OBJECT_CODE		INT,
	@EVENT_TYPE			TINYINT,	
	@PARAM_1			INT,
	@PARAM_2			INT,
	@PARAM_3			INT,
	@PARAM_4			INT,
	@PARAM_5			VARCHAR(50),
	@USER_IP			VARCHAR(15)
	
AS

	SET NOCOUNT ON
	SET LOCK_TIMEOUT 10000

	DECLARE @nReturn			INT	
	DECLARE @REG_DATE			DATETIME

	SET @nReturn=0			-- True
	SET @REG_DATE=GETDATE()

	

	BEGIN TRY
		
		BEGIN TRAN
			INSERT dbo.GAME_LOG WITH (XLOCK, ROWLOCK) (RegDate, LogType, AccountIDX, UserID, UserName, UserLevel, UserExp, CpType, TBLIDX, ObjCode, EventType, Param1, Param2, Param3, Param4, Param5, UserIP)
			VALUES (@REG_DATE, @LOG_TYPE, @ACCOUNT_IDX, @USER_ID, @USER_NAME, @USER_LEVEL, @USER_EXP, @CHANNEL_TYPE, @TBL_INDEX, @OBJECT_CODE, @EVENT_TYPE, @PARAM_1, @PARAM_2, @PARAM_3, @PARAM_4, @PARAM_5, @USER_IP)
		COMMIT TRAN
				
	END TRY
	BEGIN CATCH

		ROLLBACK TRAN				
		EXEC dbo.log_errorLog_set_log @ACCOUNT_IDX
		RETURN -1

	END CATCH

	RETURN 0;
GO
/****** Object:  StoredProcedure [dbo].[log_item_accessory_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_item_accessory_add]

	@accountIDX		INT,
	@nickName		NVARCHAR(20),
	@userLevel		INT,
	@itemCode		INT,
	@eventType		INT,
	@amount			INT,
	@userIP			VARCHAR(15),
	@note			VARCHAR(100)

AS

SET NOCOUNT ON;

INSERT INTO dbo.log_buy_item_accessory (
	accountIDX, nickName, userLevel, itemCode, eventType, amount, userIP, note, regDate
)
VALUES (
	@accountIDX, @nickName, @userLevel, @itemCode, @eventType, @amount, @userIP, @note, GETDATE()
)
GO
/****** Object:  StoredProcedure [dbo].[log_item_class_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_item_class_add]
(
	@accountIDX int
,	@nickName varchar(20)
,	@userLevel int
,	@classType int
,	@limitType int
,	@eventType int
,	@amount int
,	@userIP varchar(15)
,	@note varchar(100)
)
AS
	INSERT INTO log_buy_item_class (accountIDX, nickName, userLevel, classType, limitType, eventType, amount, userIP, note, regDate)
	VALUES (@accountIDX, @nickName, @userLevel, @classType, @limitType, @eventType, @amount, @userIP, @note, getdate())
GO
/****** Object:  StoredProcedure [dbo].[log_item_costume_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_item_costume_add]
(
	@accountIDX INT
,	@nickName NVARCHAR(20)
,	@userLevel INT
,	@itemCode INT
,	@eventType INT
,	@amount INT
,	@userIP VARCHAR(15)
,	@note VARCHAR(100)
)
AS

SET NOCOUNT ON

INSERT INTO dbo.log_buy_item_costume (
	accountIDX, nickName, userLevel, itemCode, eventType, amount, userIP, note, regDate
)
VALUES (
	@accountIDX, @nickName, @userLevel, @itemCode, @eventType, @amount, @userIP, @note, GETDATE()
)
GO
/****** Object:  StoredProcedure [dbo].[log_item_decoration_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_item_decoration_add]
(
	@accountIDX int
,	@nickName varchar(20)
,	@classType int
,	@kindred int
,	@itemType int
,	@itemCode int
,	@eventType int
,	@amount int
,	@userIP varchar(15)
,	@note varchar(100)
)
AS
	INSERT INTO log_buy_item_decoration (accountIDX, nickName, classType, kindred, itemType, itemCode, eventType, amount, userIP, note, regDate)
	VALUES (@accountIDX, @nickName, @classType, @kindred, @itemType, @itemCode, @eventType, @amount, @userIP, @note, getdate())
GO
/****** Object:  StoredProcedure [dbo].[log_item_equip_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_item_equip_add]
(
	@accountIDX int
,	@nickName varchar(20)
,	@itemType int
,	@itemCode int
,	@reinforce int
,	@machineType int
,	@limitDate int
,	@eventType int
,	@amount int
,	@userIP varchar(15)
,	@note varchar(100)
)
AS
	INSERT INTO log_buy_item_equip (accountIDX, nickName, itemType, itemCode, reinforce, machineType, limitDate, eventType, amount, userIP, note, regDate)
	VALUES (@accountIDX, @nickName, @itemType, @itemCode, @reinforce, @machineType, @limitDate, @eventType, @amount, @userIP, @note, getdate())
GO
/****** Object:  StoredProcedure [dbo].[log_item_gold_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_item_gold_add]
(
	@accountIDX int
,	@nickName varchar(20)
,	@userLevel int
,	@itemType int
,	@itemCode int
,	@itemValue int
,	@amount int
,	@cpType smallint
,	@billingID varchar(64)
,	@userIP varchar(15)
,	@note varchar(100)
,	@buyType tinyint
)
AS

SET NOCOUNT ON

INSERT INTO dbo.log_buy_item_gold (
	accountIDX, nickname, userLevel, itemType, itemCode, itemValue, amount, cpType, billingID, buyType, userIP, note, regDate
)
VALUES (
	@accountIDX, @nickname, @userLevel, @itemType, @itemCode, @itemValue, @amount, @cpType, @billingID, @buyType, @userIP, @note, GETDATE()
)
GO
/****** Object:  StoredProcedure [dbo].[log_item_sbox_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_item_sbox_add]
(
	@accountIDX INT
,	@nickName NVARCHAR(20)
,	@userLevel INT
,	@presentType SMALLINT
,	@value1 INT
,	@value2 INT
,	@amount INT
,	@cpType INT
,	@billingID VARCHAR(64)
,	@logType INT
,	@userIP VARCHAR(15)
,	@note NVARCHAR(100)
,	@limitDate INT
)
AS

SET NOCOUNT ON

INSERT INTO dbo.log_buy_item_sbox (
	accountIDX, nickName, userLevel, presentType, value1, value2, amount, cpType, billingID, logType, userIP, note, limitDate, regDate
)
VALUES (
	@accountIDX, @nickName, @userLevel, @presentType, @value1, @value2, @amount, @cpType, @billingID, @logType, @userIP, @note, @limitDate, GETDATE()
)
GO
/****** Object:  StoredProcedure [dbo].[log_item_special_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_item_special_add]
(
	@accountIDX int
,	@nickName varchar(20)
,	@itemType int
,	@itemValue int
,	@eventType int
,	@amount int
,	@userIP varchar(15)
,	@note varchar(100)
)
AS
	INSERT INTO log_buy_item_special (accountIDX, nickName, itemType, itemValue, eventType, amount, userIP, note, regDate)
	VALUES (@accountIDX, @nickName, @itemType, @itemValue, @eventType, @amount, @userIP, @note, getdate())
GO
/****** Object:  StoredProcedure [dbo].[log_present_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[log_present_add]
(
	@accountIDX int
,	@nickname varchar(20)
,	@receiveIDX int
,	@presentType smallint
,	@value1 int
,	@value2 int
,	@eventType int
,	@userIP varchar(15)
,	@note varchar(100)
)
AS

	INSERT INTO log_present (accountIDX, nickname, receiveIDX, presentType, value1, value2, eventType, userIP, note, regDate)
	VALUES (@accountIDX, @nickName, @receiveIDX, @presentType, @value1, @value2, @eventType, @userIP, @note, getdate())
GO
/****** Object:  StoredProcedure [dbo].[log_trace_prisoner_option_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
--IF OBJECT_ID('dbo.log_trace_prisoner_option_add','P') IS NOT NULL	
--
--	DROP PROC dbo.log_trace_prisoner_option_add
--
--GO

-- =============================================
-- Subject		: dbo.log_trace_prisoner_option_add
-- Author		: galura
-- Date			: 2016-08-26
-- Description	: 포로탈출 방 옵션
-- =============================================

CREATE PROCEDURE [dbo].[log_trace_prisoner_option_add]

	@BLUE_MEMBER			INT,
	@RED_MEMBER				INT,
	@BLUE_MAX_MEMBER		INT,
	@RED_MAX_MEMBER			INT,
	@TEAM_MIX_OPTION		INT

AS

	SET NOCOUNT ON;
	SET LOCK_TIMEOUT 10000;

	INSERT INTO dbo.log_trace_prisoner_option (BlueMember, RedMember, BlueMaxMember, RedMaxMember, TeamMixOption, RegDate)
	VALUES (@BLUE_MEMBER, @RED_MEMBER, @BLUE_MAX_MEMBER, @RED_MAX_MEMBER, @TEAM_MIX_OPTION, GETDATE())
GO
/****** Object:  StoredProcedure [dbo].[log_trace_prisoner_start_add]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
--IF OBJECT_ID('dbo.log_trace_prisoner_start_add','P') IS NOT NULL	
--
--	DROP PROC dbo.log_trace_prisoner_start_add
--
--GO

-- =============================================
-- Subject		: dbo.log_trace_prisoner_start_add
-- Author		: galura
-- Date			: 2016-08-26
-- Description	: 포로탈출 시작 방법 로그
-- =============================================

CREATE PROCEDURE [dbo].[log_trace_prisoner_start_add]

	@SERVER_ID		BIGINT,
	@MAKE_ROOM		INT,
	@QUICK_START	INT,
	@LIST_SELECT	INT

AS

	SET NOCOUNT ON;
	SET LOCK_TIMEOUT 10000;

	INSERT INTO dbo.log_trace_prisoner_start (ServerID, MakeRoom, QuickStart, ListSelect, RegDate)
	VALUES (@SERVER_ID, @MAKE_ROOM, @QUICK_START, @LIST_SELECT, GETDATE())
GO
/****** Object:  StoredProcedure [dbo].[log_view_buy_class]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_buy_class]
	@SEARCH_NICK		VARCHAR(20),
	@SEARCH_SDATE		VARCHAR(10),
	@SEARCH_EDATE		VARCHAR(10)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TOP 1000 accountIdx, nickname, classtype, limittype, eventtype, amount, userip, note, regdate
	FROM log_buy_item_class WITH(NOLOCK)
	WHERE CASE WHEN @SEARCH_NICK='' THEN '' ELSE nickname END = @SEARCH_NICK
		AND regdate >= @SEARCH_SDATE AND regdate < DATEADD(day, 1, @SEARCH_EDATE)
	ORDER BY idx DESC
END
GO
/****** Object:  StoredProcedure [dbo].[log_view_buy_deco]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_buy_deco]
	@SEARCH_NICK		VARCHAR(20),
	@SEARCH_SDATE		VARCHAR(10),
	@SEARCH_EDATE		VARCHAR(10)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TOP 1000 accountIdx, nickname, classtype, kindred, itemtype, itemcode, eventtype, amount, userip, note, regdate
	FROM log_buy_item_decoration WITH(NOLOCK)
	WHERE CASE WHEN @SEARCH_NICK='' THEN '' ELSE nickname END = @SEARCH_NICK
		AND regdate >= @SEARCH_SDATE AND regdate < DATEADD(day, 1, @SEARCH_EDATE)
	ORDER BY idx DESC
END
GO
/****** Object:  StoredProcedure [dbo].[log_view_buy_equip]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_buy_equip]
	@SEARCH_NICK		VARCHAR(20),
	@SEARCH_SDATE		VARCHAR(10),
	@SEARCH_EDATE		VARCHAR(10)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TOP 1000 accountIdx, nickname, itemtype, itemcode, reinforce, machinetype, limitdate, eventtype, amount, userip, note, regdate
	FROM log_buy_item_equip WITH(NOLOCK)
	WHERE 
		CASE WHEN @SEARCH_NICK='' THEN '' ELSE nickname END = @SEARCH_NICK
		AND regdate >= @SEARCH_SDATE AND regdate < DATEADD(day, 1, @SEARCH_EDATE)
	ORDER BY idx DESC
END
GO
/****** Object:  StoredProcedure [dbo].[log_view_buy_gold]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_buy_gold]
	@SEARCH_NICK		VARCHAR(20),
	@SEARCH_SDATE		VARCHAR(10),
	@SEARCH_EDATE		VARCHAR(10)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TOP 1000 accountIdx, nickname, userLevel, itemType, itemCode, itemValue, amount, cpType, billingID, userIP, note, regdate
	FROM log_buy_item_gold WITH(NOLOCK)
	WHERE 
		CASE WHEN @SEARCH_NICK='' THEN '' ELSE nickname END = @SEARCH_NICK
		AND regdate >= @SEARCH_SDATE AND regdate < DATEADD(day, 1, @SEARCH_EDATE)
	ORDER BY idx DESC
END
GO
/****** Object:  StoredProcedure [dbo].[log_view_buy_sbox]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_buy_sbox]
	@SEARCH_NICK		VARCHAR(20),
	@SEARCH_SDATE		VARCHAR(10),
	@SEARCH_EDATE		VARCHAR(10)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TOP 1000 accountIdx, nickname, userLevel, presentType, value1, value2, amount, cpType, billingID, logType, userIP, note, limitDate, regdate
	FROM log_buy_item_sbox WITH(NOLOCK)
	WHERE 
		CASE WHEN @SEARCH_NICK='' THEN '' ELSE nickname END = @SEARCH_NICK
		AND regdate >= @SEARCH_SDATE AND regdate < DATEADD(day, 1, @SEARCH_EDATE)
	ORDER BY idx DESC
END
GO
/****** Object:  StoredProcedure [dbo].[log_view_buy_special]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_buy_special]
	@SEARCH_NICK		VARCHAR(20),
	@SEARCH_SDATE		VARCHAR(10),
	@SEARCH_EDATE		VARCHAR(10)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TOP 1000 accountIdx, nickname, itemtype, itemValue, eventtype, amount, userip, note, regdate
	FROM log_buy_item_special WITH(NOLOCK)
	WHERE 
		CASE WHEN @SEARCH_NICK='' THEN '' ELSE nickname END = @SEARCH_NICK
		AND regdate >= @SEARCH_SDATE AND regdate < DATEADD(day, 1, @SEARCH_EDATE)
	ORDER BY idx DESC
END
GO
/****** Object:  StoredProcedure [dbo].[log_view_data_medal]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_data_medal]
	@SEARCH_NICK		VARCHAR(20),
	@SEARCH_SDATE		VARCHAR(10),
	@SEARCH_EDATE		VARCHAR(10)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TOP 1000 accountIdx, nickname, medalType, limitType, logType, regdate
	FROM log_data_medal WITH(NOLOCK)
	WHERE 
		CASE WHEN @SEARCH_NICK='' THEN '' ELSE nickname END = @SEARCH_NICK
		AND regdate >= @SEARCH_SDATE AND regdate < DATEADD(day, 1, @SEARCH_EDATE)
	ORDER BY idx DESC
END
GO
/****** Object:  StoredProcedure [dbo].[log_view_data_peso]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_data_peso]
	@SEARCH_NICK		VARCHAR(20),
	@SEARCH_SDATE		VARCHAR(10),
	@SEARCH_EDATE		VARCHAR(10)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TOP 1000 accountIdx, nickname, peso, logtype, regdate
	FROM log_data_peso WITH(NOLOCK)
	WHERE 
		CASE WHEN @SEARCH_NICK='' THEN '' ELSE nickname END = @SEARCH_NICK
		AND regdate >= @SEARCH_SDATE AND regdate < DATEADD(day, 1, @SEARCH_EDATE)
	ORDER BY idx DESC
END
GO
/****** Object:  StoredProcedure [dbo].[log_view_data_play]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_data_play]
	@SEARCH_NICK		VARCHAR(20),
	@SEARCH_SDATE		VARCHAR(10),
	@SEARCH_EDATE		VARCHAR(10)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TOP 1000 accountIdx, nickname, playtype, modetype, subtype1, subtype2, playtime, deathtime, peso, win, lose, [kill], death, logtype, pcnum, regdate
	FROM log_data_play WITH(NOLOCK)
	WHERE 
		CASE WHEN @SEARCH_NICK='' THEN '' ELSE nickname END = @SEARCH_NICK
		AND regdate >= @SEARCH_SDATE AND regdate < DATEADD(day, 1, @SEARCH_EDATE)
	ORDER BY idx DESC
END
GO
/****** Object:  StoredProcedure [dbo].[log_view_data_quest]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_data_quest]
	@SEARCH_NICK		VARCHAR(20),
	@SEARCH_SDATE		VARCHAR(10),
	@SEARCH_EDATE		VARCHAR(10)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TOP 1000 accountIdx, nickname, userlevel, mainidx, subidx, logtype, regdate
	FROM log_data_quest WITH(NOLOCK)
	WHERE 
		CASE WHEN @SEARCH_NICK='' THEN '' ELSE nickname END = @SEARCH_NICK
		AND regdate >= @SEARCH_SDATE AND regdate < DATEADD(day, 1, @SEARCH_EDATE)
	ORDER BY idx DESC
END
GO
/****** Object:  StoredProcedure [dbo].[log_view_data_time]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_data_time]
	@SEARCH_NICK		VARCHAR(20),
	@SEARCH_SDATE		VARCHAR(10),
	@SEARCH_EDATE		VARCHAR(10)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TOP 1000 accountIdx, nickname, playtime, logtype, regdate
	FROM log_data_time WITH(NOLOCK)
	WHERE 
		CASE WHEN @SEARCH_NICK='' THEN '' ELSE nickname END = @SEARCH_NICK
		AND regdate >= @SEARCH_SDATE AND regdate < DATEADD(day, 1, @SEARCH_EDATE)
	ORDER BY idx DESC
END
GO
/****** Object:  StoredProcedure [dbo].[log_view_data_trade]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_data_trade]
	@SEARCH_NICK		VARCHAR(20),
	@SEARCH_SDATE		VARCHAR(10),
	@SEARCH_EDATE		VARCHAR(10)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TOP 1000 accountIdx, nickname, itemType, value1, value2, price, logType, userIP, note, regdate
	FROM log_data_trade WITH(NOLOCK)
	WHERE 
		CASE WHEN @SEARCH_NICK='' THEN '' ELSE nickname END = @SEARCH_NICK
		AND regdate >= @SEARCH_SDATE AND regdate < DATEADD(day, 1, @SEARCH_EDATE)
	ORDER BY idx DESC
END
GO
/****** Object:  StoredProcedure [dbo].[log_view_define_data]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_define_data]
	@OBJ_NAME		VARCHAR(32)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT code, ISNULL(itemNameKO, '') AS itemNameKO, ISNULL(itemNameID, '') AS itemNameID
	FROM log_define WITH(NOLOCK)
	WHERE objName=@OBJ_NAME
	ORDER BY code ASC
END
GO
/****** Object:  StoredProcedure [dbo].[log_view_piece_divide]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_piece_divide]
	@SEARCH_NICK		VARCHAR(20),
	@SEARCH_SDATE		VARCHAR(10),
	@SEARCH_EDATE		VARCHAR(10)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TOP 1000 accountIdx, nickName, userLevel, itemType, itemCode, amount, materialCode, regdate
	FROM log_data_piece_divide WITH(NOLOCK)
	WHERE 
		CASE WHEN @SEARCH_NICK='' THEN '' ELSE nickname END = @SEARCH_NICK
		AND regdate >= @SEARCH_SDATE AND regdate < DATEADD(day, 1, @SEARCH_EDATE)
	ORDER BY idx DESC
END
GO
/****** Object:  StoredProcedure [dbo].[log_view_piece_mix]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_piece_mix]
	@SEARCH_NICK		VARCHAR(20),
	@SEARCH_SDATE		VARCHAR(10),
	@SEARCH_EDATE		VARCHAR(10)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TOP 1000 accountIdx, nickName, userLevel, mixType, mixCode, resultType, useA, useB, resultA, resultB, regdate
	FROM log_data_piece_mix WITH(NOLOCK)
	WHERE 
		CASE WHEN @SEARCH_NICK='' THEN '' ELSE nickname END = @SEARCH_NICK
		AND regdate >= @SEARCH_SDATE AND regdate < DATEADD(day, 1, @SEARCH_EDATE)
	ORDER BY idx DESC
END
GO
/****** Object:  StoredProcedure [dbo].[log_view_piece_obtain]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_piece_obtain]
	@SEARCH_NICK		VARCHAR(20),
	@SEARCH_SDATE		VARCHAR(10),
	@SEARCH_EDATE		VARCHAR(10)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TOP 1000 accountIdx, nickName, userLevel, playTime, step, amount, regdate
	FROM log_data_piece_obtain WITH(NOLOCK)
	WHERE 
		CASE WHEN @SEARCH_NICK='' THEN '' ELSE nickname END = @SEARCH_NICK
		AND regdate >= @SEARCH_SDATE AND regdate < DATEADD(day, 1, @SEARCH_EDATE)
	ORDER BY idx DESC
END
GO
/****** Object:  StoredProcedure [dbo].[log_view_present]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- =============================================
-- 
-- =============================================
CREATE PROCEDURE [dbo].[log_view_present]
	@SEARCH_NICK		VARCHAR(20),
	@SEARCH_SDATE		VARCHAR(10),
	@SEARCH_EDATE		VARCHAR(10)
AS
BEGIN
	SET NOCOUNT ON;

	SELECT TOP 1000 accountIdx, nickname, presenttype, value1, value2, eventtype, regdate
	FROM log_present WITH(NOLOCK)
	WHERE 
		CASE WHEN @SEARCH_NICK='' THEN '' ELSE nickname END = @SEARCH_NICK
		AND regdate >= @SEARCH_SDATE AND regdate < DATEADD(day, 1, @SEARCH_EDATE)
	ORDER BY idx DESC
END
GO
/****** Object:  StoredProcedure [dbo].[USP_LOG_HACK_MESSAGE_SET_ADD]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- IF OBJECT_ID('dbo.USP_LOG_HACK_MESSAGE_SET_ADD', 'P') IS NOT NULL
-- 	DROP PROC dbo.USP_LOG_HACK_MESSAGE_SET_ADD

-- GO

-- =============================================
-- Subject		: dbo.USP_LOG_HACK_MESSAGE_SET_ADD
-- Author		: galura
-- Date			: 2017-06-13
-- Description	: 해킹 로그 추가
-- =============================================


CREATE PROCEDURE [dbo].[USP_LOG_HACK_MESSAGE_SET_ADD]

	@ACCOUNT_IDX	INT,
	@MSG			NVARCHAR(1000)

AS

	SET NOCOUNT ON;
	SET LOCK_TIMEOUT 10000;

	INSERT INTO dbo.TBL_LOG_HACK_MESSAGE (
		AccountIDX, Msg, RegDate
	)
	VALUES (
		@ACCOUNT_IDX, @MSG, GETDATE()
	);
GO
/****** Object:  StoredProcedure [dbo].[USP_LOG_HACK_SET_ADD]    Script Date: 06/09/2026 01:10:56 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
-- IF OBJECT_ID('dbo.USP_LOG_HACK_SET_ADD', 'P') IS NOT NULL
-- 	DROP PROC dbo.USP_LOG_HACK_SET_ADD

-- GO

-- =============================================
-- Subject		: dbo.USP_LOG_HACK_SET_ADD
-- Author		: galura
-- Date			: 2017-05-18
-- Description	: 해킹 로그 추가
-- =============================================


CREATE PROCEDURE [dbo].[USP_LOG_HACK_SET_ADD]

	@ACCOUNT_IDX	INT,
	@NICK_NAME		NVARCHAR(20),
	@USER_LEVEL		INT,
	@PLAY_TYPE		INT,
	@MODE_TYPE		INT,
	@SUB_TYPE1		INT,
	@SUB_TYPE2		INT,
	@ITEM_TYPE		INT,
	@ITEM_CODE		INT,
	@ITEM_VALUE		INT

AS

	SET NOCOUNT ON;
	SET LOCK_TIMEOUT 10000;

	INSERT INTO dbo.TBL_LOG_HACK (
		AccountIDX, NickName, UserLevel, PlayType, ModeType, SubType1, SubType2, ItemType, ItemCode, ItemValue, RegDate
	)
	VALUES (
		@ACCOUNT_IDX, @NICK_NAME, @USER_LEVEL, @PLAY_TYPE, @MODE_TYPE, @SUB_TYPE1, @SUB_TYPE2, @ITEM_TYPE, @ITEM_CODE, @ITEM_VALUE, GETDATE()
	);
GO
USE [master]
GO
ALTER DATABASE [LosaLogData] SET  READ_WRITE 
GO
