#pragma once
#include <vector>
#include <set>
#include "BatchCompileDefine.h"
#include "BaseCommand.h"

class BatchScanner
{
public:
	typedef	std::vector<const char*>	VECCHARTOKEN;
	typedef std::set<int>				SETINDEXES;
	typedef SETINDEXES::iterator		INDEXESITER;
	
	enum CommandStateType
	{
		CS_COMMAND,
		CS_OPTION,
		CS_TABINDEX,
		CS_STRING,
		CS_DONE,
		CS_FAIL,
		CS_END
	};

public:
	BatchScanner(void);
	~BatchScanner(void);

private:
	void Init();
	void Destroy();

	void InitCommand();
	void InitOption();

public:
	// virtual ??
	BaseCommand* Scanner( const char* commandLine );

private:
	BOOL SplitToken( const char* commandLine, int commandLen, VECCHARTOKEN& tokens );

	void JumpSplitChar( const char* commandLine, int commandLen, int& index );
	void FindSplitChar( const char* commandLine, int commandLen, int& index );

private:
	BaseCommand* MakeCommand( VECCHARTOKEN& tokens );
	BaseCommand* GetCommand( DWORD type );

	CommandStateType CommandState( const char* command, DWORD& commandType );
	CommandStateType OptionState( const char* command, DWORD& optionBit );
	CommandStateType TabIndexState( const char* command, SETINDEXES& tabIndexes );	

	int	GetCommandType( const char* command );
	int GetOptionType( const char* command );
	BOOL GetDigit( const char* command, DWORD& digit );
	BOOL IsDigit( const char ch )	{ return ( ('0' <= ch) && ( ch <= '9') ); }

	BOOL TokenCompare( const char* token, const char* command );
	int GetServiceNameLen( const char* serviceName );

private:
	char m_commandTokens[BCT_END][MAX_TOKEN_BUFFER_SIZE];
	char m_optionTokens[BOT_END][MAX_TOKEN_BUFFER_SIZE];
};

