USE [master]
GO
/****** Object:  Database [LosaGame_log]    Script Date: 06/09/2026 01:09:51 ******/
CREATE DATABASE [LosaGame_log]
 CONTAINMENT = NONE
 ON  PRIMARY 
( NAME = N'LosaGame_log', FILENAME = N'/var/opt/mssql/data/LosaGame_log.mdf' , SIZE = 8192KB , MAXSIZE = UNLIMITED, FILEGROWTH = 1024KB )
 LOG ON 
( NAME = N'LosaGame_log_log', FILENAME = N'/var/opt/mssql/data/LosaGame_log_log.ldf' , SIZE = 1280KB , MAXSIZE = 2048GB , FILEGROWTH = 10%)
 WITH CATALOG_COLLATION = DATABASE_DEFAULT, LEDGER = OFF
GO
ALTER DATABASE [LosaGame_log] SET COMPATIBILITY_LEVEL = 160
GO
IF (1 = FULLTEXTSERVICEPROPERTY('IsFullTextInstalled'))
begin
EXEC [LosaGame_log].[dbo].[sp_fulltext_database] @action = 'enable'
end
GO
ALTER DATABASE [LosaGame_log] SET ANSI_NULL_DEFAULT OFF 
GO
ALTER DATABASE [LosaGame_log] SET ANSI_NULLS OFF 
GO
ALTER DATABASE [LosaGame_log] SET ANSI_PADDING OFF 
GO
ALTER DATABASE [LosaGame_log] SET ANSI_WARNINGS OFF 
GO
ALTER DATABASE [LosaGame_log] SET ARITHABORT OFF 
GO
ALTER DATABASE [LosaGame_log] SET AUTO_CLOSE OFF 
GO
ALTER DATABASE [LosaGame_log] SET AUTO_SHRINK OFF 
GO
ALTER DATABASE [LosaGame_log] SET AUTO_UPDATE_STATISTICS ON 
GO
ALTER DATABASE [LosaGame_log] SET CURSOR_CLOSE_ON_COMMIT OFF 
GO
ALTER DATABASE [LosaGame_log] SET CURSOR_DEFAULT  GLOBAL 
GO
ALTER DATABASE [LosaGame_log] SET CONCAT_NULL_YIELDS_NULL OFF 
GO
ALTER DATABASE [LosaGame_log] SET NUMERIC_ROUNDABORT OFF 
GO
ALTER DATABASE [LosaGame_log] SET QUOTED_IDENTIFIER OFF 
GO
ALTER DATABASE [LosaGame_log] SET RECURSIVE_TRIGGERS OFF 
GO
ALTER DATABASE [LosaGame_log] SET  DISABLE_BROKER 
GO
ALTER DATABASE [LosaGame_log] SET AUTO_UPDATE_STATISTICS_ASYNC OFF 
GO
ALTER DATABASE [LosaGame_log] SET DATE_CORRELATION_OPTIMIZATION OFF 
GO
ALTER DATABASE [LosaGame_log] SET TRUSTWORTHY OFF 
GO
ALTER DATABASE [LosaGame_log] SET ALLOW_SNAPSHOT_ISOLATION OFF 
GO
ALTER DATABASE [LosaGame_log] SET PARAMETERIZATION SIMPLE 
GO
ALTER DATABASE [LosaGame_log] SET READ_COMMITTED_SNAPSHOT OFF 
GO
ALTER DATABASE [LosaGame_log] SET HONOR_BROKER_PRIORITY OFF 
GO
ALTER DATABASE [LosaGame_log] SET RECOVERY SIMPLE 
GO
ALTER DATABASE [LosaGame_log] SET  MULTI_USER 
GO
ALTER DATABASE [LosaGame_log] SET PAGE_VERIFY CHECKSUM  
GO
ALTER DATABASE [LosaGame_log] SET DB_CHAINING OFF 
GO
ALTER DATABASE [LosaGame_log] SET FILESTREAM( NON_TRANSACTED_ACCESS = OFF ) 
GO
ALTER DATABASE [LosaGame_log] SET TARGET_RECOVERY_TIME = 60 SECONDS 
GO
ALTER DATABASE [LosaGame_log] SET DELAYED_DURABILITY = DISABLED 
GO
ALTER DATABASE [LosaGame_log] SET ACCELERATED_DATABASE_RECOVERY = OFF  
GO
EXEC sys.sp_db_vardecimal_storage_format N'LosaGame_log', N'ON'
GO
ALTER DATABASE [LosaGame_log] SET QUERY_STORE = ON
GO
ALTER DATABASE [LosaGame_log] SET QUERY_STORE (OPERATION_MODE = READ_WRITE, CLEANUP_POLICY = (STALE_QUERY_THRESHOLD_DAYS = 30), DATA_FLUSH_INTERVAL_SECONDS = 900, INTERVAL_LENGTH_MINUTES = 60, MAX_STORAGE_SIZE_MB = 1000, QUERY_CAPTURE_MODE = AUTO, SIZE_BASED_CLEANUP_MODE = AUTO, MAX_PLANS_PER_QUERY = 200, WAIT_STATS_CAPTURE_MODE = ON)
GO
USE [LosaGame_log]
GO
/****** Object:  User [lstggame]    Script Date: 06/09/2026 01:09:51 ******/
CREATE USER [lstggame] FOR LOGIN [lstggame] WITH DEFAULT_SCHEMA=[dbo]
GO
/****** Object:  Table [dbo].[log_cash]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_cash](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[userIP] [varchar](16) NOT NULL,
	[DBID] [varchar](16) NOT NULL,
	[applyType] [char](2) NOT NULL,
	[amount] [int] NOT NULL,
	[note] [varchar](500) NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_class_buytime]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_class_buytime](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[classType] [int] NOT NULL,
	[buyTime] [bigint] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_class_rental_history]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_class_rental_history](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[friendIDX] [int] NOT NULL,
	[classType] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_data_clover]    Script Date: 06/09/2026 01:09:51 ******/
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
/****** Object:  Index [index]    Script Date: 06/09/2026 01:09:51 ******/
CREATE CLUSTERED INDEX [index] ON [dbo].[log_data_clover]
(
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_event_marble]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_event_marble](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountidx] [int] NOT NULL,
	[userID] [varchar](20) NOT NULL,
	[cpType] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_event_openbeta_coin]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_event_openbeta_coin](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[coin] [int] NOT NULL,
	[flag] [int] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_guild_mark]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_guild_mark](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[guildIDX] [int] NOT NULL,
	[accountIDX] [int] NOT NULL,
	[havepeso] [bigint] NOT NULL,
	[markprice] [int] NOT NULL,
	[nowmark] [int] NOT NULL,
	[changemark] [int] NOT NULL,
	[flag] [int] NOT NULL,
	[userIP] [varchar](15) NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_league_cheer]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_league_cheer](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[cheerIDX] [int] NOT NULL,
	[startDate] [int] NOT NULL,
	[accountIDX] [int] NOT NULL,
	[leagueIDX] [int] NOT NULL,
	[teamIDX] [int] NOT NULL,
	[regDate] [datetime] NOT NULL,
	[insertDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_league_info_user]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_league_info_user](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[infoIDX] [int] NULL,
	[leagueIDX] [int] NULL,
	[leagueName] [nvarchar](20) NULL,
	[maxRound] [smallint] NULL,
	[bannerA] [int] NULL,
	[bannerB] [int] NULL,
	[modeType] [int] NULL,
	[maxcount] [tinyint] NULL,
	[autoType] [tinyint] NULL,
	[announce] [nvarchar](512) NULL,
	[recruitDate] [int] NULL,
	[waitDate] [int] NULL,
	[regDate] [datetime] NULL,
	[insertDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_league_list]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_league_list](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[leagueIDX] [int] NULL,
	[accountIDX] [int] NULL,
	[startDate] [int] NULL,
	[endDate] [int] NULL,
	[leagueType] [tinyint] NULL,
	[leagueState] [tinyint] NULL,
	[regDate] [datetime] NULL,
	[insertDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_league_list_user]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_league_list_user](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[leagueIDX] [int] NULL,
	[accountIDX] [int] NULL,
	[startDate] [int] NULL,
	[endDate] [int] NULL,
	[leagueType] [tinyint] NULL,
	[leagueState] [tinyint] NULL,
	[regDate] [datetime] NULL,
	[insertDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_league_round_user]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_league_round_user](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[roundIDX] [int] NULL,
	[infoIDX] [int] NULL,
	[roundType1_playDate] [int] NULL,
	[roundType1_presentCode1] [int] NULL,
	[roundType1_presentCode2] [int] NULL,
	[roundType1_presentCode3] [int] NULL,
	[roundType1_presentCode4] [int] NULL,
	[roundType2_playDate] [int] NULL,
	[roundType2_presentCode1] [int] NULL,
	[roundType2_presentCode2] [int] NULL,
	[roundType2_presentCode3] [int] NULL,
	[roundType2_presentCode4] [int] NULL,
	[roundType3_playDate] [int] NULL,
	[roundType3_presentCode1] [int] NULL,
	[roundType3_presentCode2] [int] NULL,
	[roundType3_presentCode3] [int] NULL,
	[roundType3_presentCode4] [int] NULL,
	[roundType4_playDate] [int] NULL,
	[roundType4_presentCode1] [int] NULL,
	[roundType4_presentCode2] [int] NULL,
	[roundType4_presentCode3] [int] NULL,
	[roundType4_presentCode4] [int] NULL,
	[roundType5_playDate] [int] NULL,
	[roundType5_presentCode1] [int] NULL,
	[roundType5_presentCode2] [int] NULL,
	[roundType5_presentCode3] [int] NULL,
	[roundType5_presentCode4] [int] NULL,
	[roundType6_playDate] [int] NULL,
	[roundType6_presentCode1] [int] NULL,
	[roundType6_presentCode2] [int] NULL,
	[roundType6_presentCode3] [int] NULL,
	[roundType6_presentCode4] [int] NULL,
	[roundType7_playDate] [int] NULL,
	[roundType7_presentCode1] [int] NULL,
	[roundType7_presentCode2] [int] NULL,
	[roundType7_presentCode3] [int] NULL,
	[roundType7_presentCode4] [int] NULL,
	[roundType8_playDate] [int] NULL,
	[roundType8_presentCode1] [int] NULL,
	[roundType8_presentCode2] [int] NULL,
	[roundType8_presentCode3] [int] NULL,
	[roundType8_presentCode4] [int] NULL,
	[roundType9_playDate] [int] NULL,
	[roundType9_presentCode1] [int] NULL,
	[roundType9_presentCode2] [int] NULL,
	[roundType9_presentCode3] [int] NULL,
	[roundType9_presentCode4] [int] NULL,
	[roundType10_playDate] [int] NULL,
	[roundType10_presentCode1] [int] NULL,
	[roundType10_presentCode2] [int] NULL,
	[roundType10_presentCode3] [int] NULL,
	[roundType10_presentCode4] [int] NULL,
	[roundType11_playDate] [int] NULL,
	[roundType11_presentCode1] [int] NULL,
	[roundType11_presentCode2] [int] NULL,
	[roundType11_presentCode3] [int] NULL,
	[roundType11_presentCode4] [int] NULL,
	[regDate] [datetime] NULL,
	[insertDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_league_team]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_league_team](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[teamIDX] [int] NULL,
	[leagueIDX] [int] NULL,
	[startDate] [int] NULL,
	[teamName] [nvarchar](20) NULL,
	[leaderIDX] [int] NULL,
	[leagueStartPosition] [smallint] NULL,
	[leaguePosition] [smallint] NULL,
	[maxcount] [tinyint] NULL,
	[cheerPoint] [int] NULL,
	[currentRound] [tinyint] NULL,
	[factionPoint] [int] NULL,
	[factionType] [tinyint] NULL,
	[regDate] [datetime] NULL,
	[insertDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_league_team_member]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_league_team_member](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[teamMemberIDX] [int] NULL,
	[startDate] [int] NULL,
	[accountIDX] [int] NULL,
	[leagueIDX] [int] NULL,
	[teamIDX] [int] NULL,
	[joinType] [tinyint] NULL,
	[regDate] [datetime] NULL,
	[insertDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_league_team_member_user]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_league_team_member_user](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[teamMemberIDX] [int] NOT NULL,
	[startDate] [int] NOT NULL,
	[accountIDX] [int] NOT NULL,
	[leagueIDX] [int] NOT NULL,
	[teamIDX] [int] NOT NULL,
	[joinType] [tinyint] NOT NULL,
	[regDate] [datetime] NOT NULL,
	[insertDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_league_team_user]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_league_team_user](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[teamIDX] [int] NULL,
	[leagueIDX] [int] NULL,
	[startDate] [int] NULL,
	[teamName] [nvarchar](20) NULL,
	[leaderIDX] [int] NULL,
	[leagueStartPosition] [smallint] NULL,
	[leaguePosition] [smallint] NULL,
	[maxcount] [tinyint] NULL,
	[cheerPoint] [int] NULL,
	[currentRound] [tinyint] NULL,
	[factionPoint] [int] NULL,
	[factionType] [tinyint] NULL,
	[regDate] [datetime] NULL,
	[insertDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_level_playtime]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_level_playtime](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[userLevel] [int] NOT NULL,
	[playTime] [bigint] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_mannerpoint]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_mannerpoint](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[playType] [int] NOT NULL,
	[modeType] [int] NOT NULL,
	[sub1Type] [int] NOT NULL,
	[sub2Type] [int] NOT NULL,
	[reportIDX] [int] NULL,
	[reportNick] [varchar](20) NULL,
	[reportpubIP] [varchar](15) NOT NULL,
	[reportpriIP] [varchar](15) NOT NULL,
	[receiveIDX] [nchar](10) NULL,
	[receiveNick] [varchar](20) NULL,
	[receivepubIP] [varchar](15) NOT NULL,
	[roomInfo] [varchar](2000) NULL,
	[chatInfo] [varchar](3000) NULL,
	[note] [varchar](500) NULL,
	[reason] [varchar](2000) NULL,
	[flag] [tinyint] NOT NULL,
	[resultType] [tinyint] NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_mannerpoint_sitevisit]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_mannerpoint_sitevisit](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[accountIDX] [int] NOT NULL,
	[point] [smallint] NOT NULL,
	[userIP] [varchar](15) NOT NULL,
	[regDate] [datetime] NOT NULL
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[log_tooni_migration]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[log_tooni_migration](
	[idx] [int] IDENTITY(1,1) NOT NULL,
	[TooniIDX] [int] NOT NULL,
	[TooniID] [varchar](32) NOT NULL,
	[AccountIDX] [int] NOT NULL,
	[RegDate] [datetime] NOT NULL,
 CONSTRAINT [PK_log_tooni_migration] PRIMARY KEY CLUSTERED 
(
	[RegDate] ASC,
	[idx] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Index [accountidx]    Script Date: 06/09/2026 01:09:51 ******/
CREATE NONCLUSTERED INDEX [accountidx] ON [dbo].[log_data_clover]
(
	[accountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [regDate]    Script Date: 06/09/2026 01:09:51 ******/
CREATE NONCLUSTERED INDEX [regDate] ON [dbo].[log_data_clover]
(
	[regDate] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
/****** Object:  Index [NC_log_tooni_migration_AccountIDX]    Script Date: 06/09/2026 01:09:51 ******/
CREATE NONCLUSTERED INDEX [NC_log_tooni_migration_AccountIDX] ON [dbo].[log_tooni_migration]
(
	[AccountIDX] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
SET ANSI_PADDING ON
GO
/****** Object:  Index [NC_log_tooni_migration_TooniID]    Script Date: 06/09/2026 01:09:51 ******/
CREATE NONCLUSTERED INDEX [NC_log_tooni_migration_TooniID] ON [dbo].[log_tooni_migration]
(
	[TooniID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, SORT_IN_TEMPDB = OFF, DROP_EXISTING = OFF, ONLINE = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
GO
ALTER TABLE [dbo].[log_class_buytime] ADD  CONSTRAINT [DF_log_class_buytime_classType]  DEFAULT ((0)) FOR [classType]
GO
ALTER TABLE [dbo].[log_class_buytime] ADD  CONSTRAINT [DF_log_class_buytime_buyTime]  DEFAULT ((0)) FOR [buyTime]
GO
ALTER TABLE [dbo].[log_class_buytime] ADD  CONSTRAINT [DF_log_class_buytime_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_class_rental_history] ADD  CONSTRAINT [DF_log_class_rental_history_classType]  DEFAULT ((0)) FOR [classType]
GO
ALTER TABLE [dbo].[log_class_rental_history] ADD  CONSTRAINT [DF_log_class_rental_history_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_data_clover] ADD  CONSTRAINT [DF_log_clover_friendIdx]  DEFAULT ((0)) FOR [friendIDX]
GO
ALTER TABLE [dbo].[log_data_clover] ADD  CONSTRAINT [DF_log_clover_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_event_marble] ADD  CONSTRAINT [DF_log_event_marble_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_event_openbeta_coin] ADD  CONSTRAINT [DF_log_event_openbeta_coin_flag]  DEFAULT ((0)) FOR [flag]
GO
ALTER TABLE [dbo].[log_event_openbeta_coin] ADD  CONSTRAINT [DF_log_event_openbeta_coin_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_guild_mark] ADD  CONSTRAINT [DF_log_guild_mark_flag]  DEFAULT ((0)) FOR [flag]
GO
ALTER TABLE [dbo].[log_guild_mark] ADD  CONSTRAINT [DF_log_guild_mark_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_league_cheer] ADD  CONSTRAINT [DF_log_league_cheer_insertDate]  DEFAULT (getdate()) FOR [insertDate]
GO
ALTER TABLE [dbo].[log_league_info_user] ADD  CONSTRAINT [DF_log_league_info_user_insertDate]  DEFAULT (getdate()) FOR [insertDate]
GO
ALTER TABLE [dbo].[log_league_list] ADD  CONSTRAINT [DF_log_league_list_insertDate]  DEFAULT (getdate()) FOR [insertDate]
GO
ALTER TABLE [dbo].[log_league_list_user] ADD  CONSTRAINT [DF_log_league_list_user_insertDate]  DEFAULT (getdate()) FOR [insertDate]
GO
ALTER TABLE [dbo].[log_league_round_user] ADD  CONSTRAINT [DF_log_league_round_user_insertDate]  DEFAULT (getdate()) FOR [insertDate]
GO
ALTER TABLE [dbo].[log_league_team] ADD  CONSTRAINT [DF_log_league_team_leagueStartPosition]  DEFAULT ((0)) FOR [leagueStartPosition]
GO
ALTER TABLE [dbo].[log_league_team] ADD  CONSTRAINT [DF_log_league_team_insertDate]  DEFAULT (getdate()) FOR [insertDate]
GO
ALTER TABLE [dbo].[log_league_team_member] ADD  CONSTRAINT [DF_log_league_team_member_insertDate]  DEFAULT (getdate()) FOR [insertDate]
GO
ALTER TABLE [dbo].[log_league_team_member_user] ADD  CONSTRAINT [DF_log_league_team_member_user_insertDate]  DEFAULT (getdate()) FOR [insertDate]
GO
ALTER TABLE [dbo].[log_league_team_user] ADD  CONSTRAINT [DF_log_league_team_user_insertDate]  DEFAULT (getdate()) FOR [insertDate]
GO
ALTER TABLE [dbo].[log_level_playtime] ADD  CONSTRAINT [DF_log_level_playtime_userLevel]  DEFAULT ((0)) FOR [userLevel]
GO
ALTER TABLE [dbo].[log_level_playtime] ADD  CONSTRAINT [DF_log_level_playtime_playTime]  DEFAULT ((0)) FOR [playTime]
GO
ALTER TABLE [dbo].[log_level_playtime] ADD  CONSTRAINT [DF_log_level_playtime_regDate]  DEFAULT (getdate()) FOR [regDate]
GO
ALTER TABLE [dbo].[log_mannerpoint] ADD  CONSTRAINT [DF_log_mannerpoint_flag]  DEFAULT ((0)) FOR [flag]
GO
ALTER TABLE [dbo].[log_mannerpoint] ADD  CONSTRAINT [DF_log_mannerpoint_resultType]  DEFAULT ((0)) FOR [resultType]
GO
ALTER TABLE [dbo].[log_tooni_migration] ADD  DEFAULT (getdate()) FOR [RegDate]
GO
/****** Object:  StoredProcedure [dbo].[game_item_etc_get_self_index]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE PROCEDURE [dbo].[game_item_etc_get_self_index]
(
	@accountIDX int
)
AS
	SELECT TOP 1 idx FROM userItemEtcDB WHERE accountIDX = @accountIDX ORDER BY idx DESC
GO
/****** Object:  StoredProcedure [dbo].[log_data_clover_add]    Script Date: 06/09/2026 01:09:51 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
create PROCEDURE [dbo].[log_data_clover_add]
(
	@userIDX	int
,	@FriendIDX	int
,	@Type		smallint
,	@Count		int
)
AS

SET NOCOUNT ON

-- 클로버 정보가 없을때 insert
INSERT INTO dbo.log_data_clover( accountIDX, friendIDX, eventType, CloverCount, regDate )
VALUES( @userIDX, @FriendIDX, @Type, @Count, GETDATE() );
GO
USE [master]
GO
ALTER DATABASE [LosaGame_log] SET  READ_WRITE 
GO
