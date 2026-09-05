

#ifndef _ioModelEmitCommand_h_
#define _ioModelEmitCommand_h_

class ioParamCommand;

namespace ioModelEmitCommand
{
	class __EX CmdEmitterPos : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};

	class __EX CmdEmitterDir : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};

	class __EX CmdEmitterAngle : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};

	class __EX CmdEmitterEmitRate : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};

	class __EX CmdEmitterStartRotAxis : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};

	class __EX CmdEmiiterStartRotAngle : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};

	class __EX CmdEmitterSpeed : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};

	class __EX CmdEmitterLiveTime : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};

	class __EX CmdEmitterWaitTime : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
	
	class __EX CmdEmitterAlphaList : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};
}

#endif