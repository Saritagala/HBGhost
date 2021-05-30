CREATE DATABASE HBGhost
GO

USE HBGhost
GO

CREATE TABLE [dbo].[Characters]
(
	[Character-ID] [int] PRIMARY KEY IDENTITY(1,1) NOT NULL,
	[Account-ID] [int] NOT NULL,
	[Character-Name] [nchar](10) NOT NULL,
	[Character-Location] [nchar](10) NULL DEFAULT ('NONE'),
	[Character-Guild-Name] [varchar](20) NULL DEFAULT ('NONE'),
	[Character-Guild-GUID] [int] NULL DEFAULT ((-1)),
	[Character-Guild-Rank] [int] NULL DEFAULT ((-1)),
	[Character-Loc-Map] [varchar](10) NULL DEFAULT ('default'),
	[Character-Loc-X] [int] NULL DEFAULT ((-1)),
	[Character-Loc-Y] [int] NULL DEFAULT ((-1)),
	[Character-HP] [int] NULL DEFAULT ((37)),
	[Character-MP] [int] NULL DEFAULT ((27)),
	[Character-SP] [int] NULL DEFAULT ((22)),
	[Character-Level] [int] NULL DEFAULT ((1)),
	[Character-Rating] [int] NULL DEFAULT ((0)),
	[Character-Strength] [int] NULL DEFAULT ((10)),
	[Character-Intelligence] [int] NULL DEFAULT ((10)),
	[Character-Vitality] [int] NULL DEFAULT ((10)),
	[Character-Dexterity] [int] NULL DEFAULT ((10)),
	[Character-Magic] [int] NULL DEFAULT ((10)),
	[Character-Charisma] [int] NULL DEFAULT ((10)),
	[Character-Luck] [int] NULL DEFAULT ((0)),
	[Character-Experience] [int] NULL DEFAULT ((0)),
	[Character-LU-Pool] [int] NULL DEFAULT ((0)),
	[Character-Ek-Count] [int] NULL DEFAULT ((0)),
	[Character-Pk-Count] [int] NULL DEFAULT ((0)),
	[Character-Reward-Gold] [int] NULL DEFAULT ((0)),
	[Character-Down-Skill-Index] [int] NULL DEFAULT ((-1)),
	[Character-ID1] [int] NOT NULL DEFAULT ((0)),
	[Character-ID2] [int] NOT NULL DEFAULT ((0)),
	[Character-ID3] [int] NOT NULL DEFAULT ((0)),
	[Character-Sex] [int] NULL DEFAULT ((1)),
	[Character-Skin] [int] NULL DEFAULT ((1)),
	[Character-Hair-Style] [int] NULL DEFAULT ((1)),
	[Character-Hair-Colour] [int] NULL DEFAULT ((1)),
	[Character-Underwear] [int] NULL DEFAULT ((1)),
	[Character-Hunger] [int] NULL DEFAULT ((100)),
	[Character-Shutup-Time] [int] NULL DEFAULT ((0)),
	[Character-Rating-Time] [int] NULL DEFAULT ((1200)),
	[Character-Force-Time] [int] NULL DEFAULT ((0)),
	[Character-SP-Time] [int] NULL DEFAULT ((0)),
	[Character-Admin-Level] [int] NULL DEFAULT ((0)),

	[Character-Quest-Number1] [int] NULL DEFAULT ((0)),
	[Character-Quest-Number2] [int] NULL DEFAULT ((0)),
	[Character-Quest-Number3] [int] NULL DEFAULT ((0)),
	[Character-Quest-Number4] [int] NULL DEFAULT ((0)),
	[Character-Quest-Number5] [int] NULL DEFAULT ((0)),
	
	[Character-Quest-ID1] [int] NULL DEFAULT ((0)),
	[Character-Quest-ID2] [int] NULL DEFAULT ((0)),
	[Character-Quest-ID3] [int] NULL DEFAULT ((0)),
	[Character-Quest-ID4] [int] NULL DEFAULT ((0)),
	[Character-Quest-ID5] [int] NULL DEFAULT ((0)),
	
	[Character-Quest-Count1] [int] NULL DEFAULT ((0)),
	[Character-Quest-Count2] [int] NULL DEFAULT ((0)),
	[Character-Quest-Count3] [int] NULL DEFAULT ((0)),
	[Character-Quest-Count4] [int] NULL DEFAULT ((0)),
	[Character-Quest-Count5] [int] NULL DEFAULT ((0)),

	[Character-Quest-Reward1] [int] NULL DEFAULT ((0)),
	[Character-Quest-Reward2] [int] NULL DEFAULT ((0)),
	[Character-Quest-Reward3] [int] NULL DEFAULT ((0)),
	[Character-Quest-Reward4] [int] NULL DEFAULT ((0)),
	[Character-Quest-Reward5] [int] NULL DEFAULT ((0)),
	
	[Character-Quest-Amount1] [int] NULL DEFAULT ((0)),
	[Character-Quest-Amount2] [int] NULL DEFAULT ((0)),
	[Character-Quest-Amount3] [int] NULL DEFAULT ((0)),
	[Character-Quest-Amount4] [int] NULL DEFAULT ((0)),
	[Character-Quest-Amount5] [int] NULL DEFAULT ((0)),
	
	[Character-Quest-Completed1] [int] NULL DEFAULT ((0)),
	[Character-Quest-Completed2] [int] NULL DEFAULT ((0)),
	[Character-Quest-Completed3] [int] NULL DEFAULT ((0)),
	[Character-Quest-Completed4] [int] NULL DEFAULT ((0)),
	[Character-Quest-Completed5] [int] NULL DEFAULT ((0)),
	
	[Character-Contribution] [int] NULL DEFAULT ((0)),
	[Character-War-Contribution] [int] NULL DEFAULT ((0)),
	[Character-Event-ID] [int] NULL DEFAULT ((0)),
	[Character-Criticals] [int] NULL DEFAULT ((0)),
	[Character-Ability-Time] [int] NULL DEFAULT ((0)),
	[Character-Lock-Map] [nchar](10) NULL DEFAULT ('NONE'),
	[Character-Lock-Time] [int] NULL DEFAULT ((0)),
	[Character-Crusade-Job] [int] NULL DEFAULT ((0)),
	[Character-Crusade-GUID] [bigint] NULL DEFAULT ((0)),
	[Character-Construct-Points] [int] NULL DEFAULT ((0)),
	[Character-Death-Time] [int] NULL DEFAULT ((0)),
	[Character-Party-ID] [int] NULL DEFAULT ((0)),
	[Character-Majestics] [int] NULL DEFAULT ((0)),
	[Character-Appr1] [int] NULL DEFAULT ((0)),
	[Character-Appr2] [int] NULL DEFAULT ((0)),
	[Character-Appr3] [int] NULL DEFAULT ((0)),
	[Character-Appr4] [int] NULL DEFAULT ((0)),
	[Character-Appr-Colour] [int] NULL DEFAULT ((0)),
	[Character-Magic-Mastery] [varchar](max) NULL DEFAULT ('0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000'),
	[Character-Skill-Mastery] [varchar](max) NULL DEFAULT ('100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100 100'),
	[Character-Skill-SSN] [varchar](max) NULL DEFAULT ('0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0'),
	[Character-Equip-Status] [varchar](max) NULL DEFAULT ('0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000'),
	[Character-Item-Pos-X] [varchar](max) NULL DEFAULT ('40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40 40'),
	[Character-Item-Pos-Y] [varchar](max) NULL DEFAULT ('30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 36 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30 30'),
	
	[Character-Deaths] [int] NULL DEFAULT ((0)),
	[Character-TotalDGKills] [int] NULL DEFAULT ((0)),
	[Character-DGPoints] [int] NULL DEFAULT ((0)),
	[Character-TotalDGDeaths] [int] NULL DEFAULT ((0)),
	[Character-DGKills] [int] NULL DEFAULT ((0)),
	[Character-Max-Ek] [int] NULL DEFAULT ((0)),
	[Character-Heldenian-GUID] [int] NULL DEFAULT ((0)),
	[Character-Wanted-Level] [int] NULL DEFAULT ((0)),
	[Character-Coin-Points] [int] NULL DEFAULT ((0)),
	[Character-Team] [int] NULL DEFAULT ((0)),
	[Character-Class] [int] NULL DEFAULT ((0)),

	[Character-Last-Login] [varchar](50) NULL
) 
GO

CREATE TABLE [dbo].[Guilds]
(
	[Guild-ID] [int] PRIMARY KEY IDENTITY(1,1) NOT NULL,
	[Guild-Name] [varchar](10) NOT NULL,
	[Guild-GUID] [int] NULL,
	[Guild-Location] [varchar](10) NULL,
	[Guild-Master] [varchar](10) NULL
)
GO

CREATE TABLE [dbo].[BankItems]
(
	[Item-ID] [int] PRIMARY KEY IDENTITY(1,1) NOT NULL,
	[Character-ID] [int] NOT NULL,
	[ItemName] [varchar](20) NULL,
	[ItemNum1] [int] NULL,
	[ItemNum2] [int] NULL,
	[ItemNum3] [int] NULL,
	[ItemNum4] [int] NULL,
	[ItemNum5] [int] NULL,
	[Item-Colour] [int] NULL,
	[ItemNum7] [int] NULL,
	[ItemNum8] [int] NULL,
	[ItemNum9] [int] NULL,
	[ItemNum10] [int] NULL,
	[Item-Attribute] [int] NULL,
	[Item-Element1] [int] NULL,
	[Item-Element2] [int] NULL,
	[Item-Element3] [int] NULL,
	[Item-Element4] [int] NULL
) 
GO

CREATE TABLE [dbo].[Accounts]
(
	[Account-ID] [int] PRIMARY KEY IDENTITY(1,1) NOT NULL,
	[Account-Name] [varchar](10) NOT NULL,
	[Account-Password] [varchar](10) NOT NULL,
	[Account-Valid-Time] [int] NULL DEFAULT ((0)),
	[Account-Valid-Year] [int] NOT NULL DEFAULT ((3029)),
	[Account-Valid-Month] [int] NULL DEFAULT ((12)),
	[Account-Valid-Day] [int] NULL DEFAULT ((31)),
	[Account-Change-Password] [varchar](10) NULL,
	[Account-Gender] [nchar](6) NULL,
	[Account-Age] [int] NULL,
	[Account-Birth-Year] [int] NULL,
	[Account-Birth-Month] [int] NULL,
	[Account-Birth-Day] [int] NULL,
	[Account-Real-Name] [varchar](50) NULL,
	[Account-SSN] [varchar](50) NULL,
	[Account-Email] [varchar](50) NOT NULL,
	[Account-Quiz] [varchar](max) NOT NULL,
	[Account-Answer] [varchar](max) NOT NULL,
	[Account-Created-By] [varchar](max) NULL,
	[Account-Created-Date] [datetime] NULL,
	[Account-Locked] [int] NOT NULL DEFAULT ((0))
)
GO

CREATE TABLE [dbo].[CharItems]
(
	[Item-ID] [int] PRIMARY KEY IDENTITY(1,1) NOT NULL,
	[Character-ID] [int] NOT NULL,
	[ItemName] [varchar](20) NULL,
	[ItemNum1] [int] NULL,
	[ItemNum2] [int] NULL,
	[ItemNum3] [int] NULL,
	[ItemNum4] [int] NULL,
	[ItemNum5] [int] NULL,
	[Item-Colour] [int] NULL,
	[ItemNum7] [int] NULL,
	[ItemNum8] [int] NULL,
	[ItemNum9] [int] NULL,
	[ItemNum10] [int] NULL,
	[Item-Attribute] [int] NULL,
	[Item-Element1] [int] NULL,
	[Item-Element2] [int] NULL,
	[Item-Element3] [int] NULL,
	[Item-Element4] [int] NULL
) 
GO

CREATE TABLE [dbo].[BankGuild]
(
	[Item-ID] [int] PRIMARY KEY IDENTITY(1,1) NOT NULL,
	[Guild-ID] [int] NOT NULL,
	[ItemName] [varchar](20) NULL,
	[ItemNum1] [int] NULL,
	[ItemNum2] [int] NULL,
	[ItemNum3] [int] NULL,
	[ItemNum4] [int] NULL,
	[ItemNum5] [int] NULL,
	[Item-Colour] [int] NULL,
	[ItemNum7] [int] NULL,
	[ItemNum8] [int] NULL,
	[ItemNum9] [int] NULL,
	[ItemNum10] [int] NULL,
	[Item-Attribute] [int] NULL,
	[Item-Element1] [int] NULL,
	[Item-Element2] [int] NULL,
	[Item-Element3] [int] NULL,
	[Item-Element4] [int] NULL
) 
GO

CREATE TABLE [dbo].[GuildMembers]
(
	[Member-ID] [int] PRIMARY KEY IDENTITY(1,1) NOT NULL,
	[Guild-ID] [int] NOT NULL,
	[Character-Name] [varchar](10) NOT NULL
)
GO
