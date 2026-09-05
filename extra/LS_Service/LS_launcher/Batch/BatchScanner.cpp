#include "StdAfx.h"
#include "BatchScanner.h"
#include "PlayCommand.h"
#include "StopCommand.h"
#include "SyncCommand.h"
#include "QuitCommand.h"
#include "InstallCommand.h"
#include "UninstallCommand.h"


BatchScanner::BatchScanner(void)
{
	Init();
}


BatchScanner::~BatchScanner(void)
{
	Destroy();
}

void BatchScanner::Init()
{
	InitCommand();
	InitOption();
}

void BatchScanner::Destroy()
{
}

void BatchScanner::InitCommand()
{
	strcpy_s(m_commandTokens[BCT_PLAY],		"play" );
	strcpy_s(m_commandTokens[BCT_STOP],		"stop");
	strcpy_s(m_commandTokens[BCT_SYNC],		"sync");
	strcpy_s(m_commandTokens[BCT_INSTALL],	"install");
	strcpy_s(m_commandTokens[BCT_UNINSTALL],"uninstall");
	strcpy_s(m_commandTokens[BCT_BACKUP],	"backup");
	strcpy_s(m_commandTokens[BCT_REMOVE],	"remove");
	strcpy_s(m_commandTokens[BCT_WAIT],		"wait");
	strcpy_s(m_commandTokens[BCT_QUIT],		"quit");
}

void BatchScanner::InitOption()
{
	strcpy_s(m_optionTokens[BOT_SERVICE_WAIT],	"/w");
	strcpy_s(m_optionTokens[BOT_ALL_SERVICE],	"/a");
	strcpy_s(m_optionTokens[BOT_TABINDEX],		"/t");
}

BaseCommand* BatchScanner::Scanner( const char* commandLine )
{
	if( !commandLine )
		return NULL;

	int commandLen = strlen(commandLine);
	if( 0 == commandLen ) return NULL;

	VECCHARTOKEN tokens;
	if( SplitToken( commandLine, commandLen, tokens ) )
	{
		return MakeCommand( tokens );
	}
	return NULL;
}

BOOL BatchScanner::SplitToken( const char* commandLine, int commandLen, VECCHARTOKEN& tokens )
{
	if( !commandLine || strlen(commandLine) == 0 )
	{
		return FALSE;
	}

	int index = -1;	
	do
	{
		JumpSplitChar( commandLine, commandLen, index );  //공백을 제외한 첫글자 위치를 index에 넣어줌.

		if( commandLine[index] == NULL)
		{
			break;
		}

		tokens.push_back( &commandLine[index] ); //첫 문자열을 집어넣음.

		FindSplitChar(commandLine, commandLen, index ); //공백을 만날때 까지 index ++;
	}while(commandLine[index] != NULL); //배치 파일의 글자 시작 index를 VECCHARTOKEN에 push

	if( tokens.size() < 1 )
	{
		return FALSE;
	}

	return TRUE;
}

void BatchScanner::JumpSplitChar( const char* commandLine, int commandLen, int& index )
{
	while( (TOKEN_SPLIT_CHAR == commandLine[++index]) && (commandLen >= index) )
	{
	}
}

void BatchScanner::FindSplitChar( const char* commandLine, int commandLen, int& index )
{
	while( (TOKEN_SPLIT_CHAR != commandLine[++index]) && (commandLen >= index ) )
	{
		if( NULL == commandLine[index] )
			break;
	}
}

BaseCommand* BatchScanner::MakeCommand( VECCHARTOKEN& tokens )
{
	if( tokens.size() < 1 )
		return NULL;

	DWORD commandType	= BCT_NONE; //CommandState()를 통해 play,stop 같은 커멘트 타입이 정해짐.
	DWORD optionBit		= 0;
	SETINDEXES	tabIndexes;
	SETINDEXES	stringIndexes;
	
	int	index	= 0;
	CommandStateType state = CS_COMMAND; //배치파일 처음은 play stop 같은 commend임,  
	while( (CS_DONE != state) && (CS_FAIL != state) )
	{
		if( (unsigned int)index >= tokens.size() )
		{
			state = CS_DONE;
		}

		switch(state)
		{
		case CS_COMMAND:
			{
				state = CommandState( tokens[index], commandType );
				index++;
			}
			break;
		case CS_OPTION:
			{
				state = OptionState( tokens[index], optionBit );
				if( CS_STRING != state )	// 서비스네임이라면 인덱스 증가 시키면 안됨
				{
					index++;
				}
			}
			break;
		case CS_TABINDEX:
			{
				state = TabIndexState( tokens[index], tabIndexes );
				if( CS_TABINDEX == state )
				{
					index++;
				}
			}
			break;
		case CS_STRING:
			{
				stringIndexes.insert( index );
				state = CS_OPTION;
				index++;
			}
			break;
		case CS_DONE:
			{
			}
			break;
		default:
			break;
		}
	}

	if( CS_FAIL == state )
	{
		return NULL;
	}
	
	//분리한 각 코맨드 및 문자들 처리./////////////////
	BaseCommand* command = GetCommand( commandType );
	
	command->SetCommandText( tokens[0] );
	command->SetOption( optionBit );
	for( INDEXESITER it = tabIndexes.begin() ; it != tabIndexes.end() ; ++it )
	{
		int tabIndex = *it;
		command->AddIndex( tabIndex );
	}

	for( INDEXESITER it = stringIndexes.begin() ; it != stringIndexes.end() ; ++it )
	{
		int stringIndex = *it;
		command->AddString( tokens[stringIndex], GetServiceNameLen( tokens[stringIndex] ) );
	}

	return command; //batchCommand;
}

BaseCommand* BatchScanner::GetCommand( DWORD type )
{
	BaseCommand* command = NULL;
	switch( type )
	{
	case BCT_PLAY:
		command = new PlayCommand;
		break;
	case BCT_STOP:
		command = new StopCommand;
		break;
	case BCT_SYNC:
		command = new SyncCommand;
		break;
	case BCT_INSTALL:
		command = new InstallCommand;
		break;
	case BCT_UNINSTALL:
		command = new UninstallCommand;
		break;
	case BCT_QUIT:
		command = new QuitCommand;
		break;
	case BCT_BACKUP:
	case BCT_REMOVE:
	case BCT_WAIT:
	default:
		break;
	}
	return command;
}

BatchScanner::CommandStateType BatchScanner::CommandState( const char* command, DWORD& commandType )
{
	commandType = GetCommandType( command );

	if( BCT_END == commandType )
	{
		return CS_FAIL;
	}
	else
	{
		return CS_OPTION;
	}
}

BatchScanner::CommandStateType BatchScanner::OptionState( const char* command, DWORD& optionBit )
{
	DWORD option	= GetOptionType( command );

	if( BOT_END == option )
	{
		return CS_STRING;
	}
	else if( BOT_TABINDEX == option )
	{
		return CS_TABINDEX;
	}
	else
	{
		if( BITCHK( optionBit, option ) )
		{
			return CS_FAIL;
		}
		else
		{
			BITON( optionBit, option );   // /w (optionBit == 1), /a (potionBit == 5)
			return CS_OPTION;
		}
	}
}

BatchScanner::CommandStateType BatchScanner::TabIndexState( const char* command, SETINDEXES& tabIndexes )
{
	DWORD digit;
	if( GetDigit( command, digit ) )
	{
		tabIndexes.insert( digit );
		return CS_TABINDEX;
	}
	return CS_OPTION;
}

int	BatchScanner::GetCommandType( const char* command )
{
	int type = 0;
	for( type ; type < BCT_END ; type++ )
	{
		if( TokenCompare( m_commandTokens[type], command ) )
		{
			break;
		}
	}

	return type;
}

int BatchScanner::GetOptionType( const char* command )
{
	unsigned int type = 0;
	for( type ; type < BOT_END ; type++ )
	{
		if( TokenCompare( m_optionTokens[type], command ) )
		{
			break;
		}
	}

	return type;
}

BOOL BatchScanner::GetDigit( const char* command, DWORD& digit )
{
	digit	= 0;
	char ch = 0;
	while( true )
	{
		ch = *command;
		if( IsDigit( ch ) )
		{
			digit = digit*10 + ( ch - '0' );
			command++;
		}
		else
		{
			break;
		}
	}

	if( (TOKEN_SPLIT_CHAR == ch) || (NULL == ch) )
	{
		return TRUE;
	}
	return FALSE;
}

BOOL BatchScanner::TokenCompare( const char* token, const char* command )
{
	//미리 저장해둔 명령어 비교.
	unsigned int tokenLen = strlen(token);
	if( strlen(command) < tokenLen )
	{
		return FALSE;
	}	

	if( strncmp( token, command, tokenLen ) == 0)
	{
		if( (TOKEN_SPLIT_CHAR == command[tokenLen]) || 
			(NULL == command[tokenLen]) )
		{
			return TRUE;
		}
	}
	return FALSE;
}

int BatchScanner::GetServiceNameLen( const char* serviceName )
{
	int len = 0;
	while(true)
	{
		if( (TOKEN_SPLIT_CHAR == serviceName[len]) ||
			(NULL == serviceName[len]) )
		{
			break;
		}
		len++;
	}

	return len;
}