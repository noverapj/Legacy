#ifndef __ioLocalManagerParent_h__
#define __ioLocalManagerParent_h__

#define MAX_PASSWORD 20

class __EX ioLocalManagerParent 
{
public:
	enum LocalType
	{
		LCT_KOREA		= 119483910, // 타입값은 보안을 위해서 순차적으로 증가하지 않고 복잡하게 만듬.
		LCT_US			= 365849202,
		LCT_CHINA		= 917382048,
		LCT_TAIWAN		= 521837584,
		LCT_JAPAN		= 213527182,
		LCT_MALAYSIA	= 493820302,
		LCT_INDONESIA	= 938400398,
		LCT_VIETNAM		= 783920174,
		LCT_THAILAND	= 800125411,
		LCT_LATIN		= 458278951,
		LCT_BRAZIL		= 329859423,
		LCT_PHILIPPINE	= 429590349,

		LCT_EU			= 647291028,	//Nexon EU   6개국어 넥슨유럽에서 서비스
		LCT_GERMANY		= 647291029,	//Nexon EU
		LCT_FRANCE		= 647291030,	//Nexon EU
		LCT_ITALIA		= 647291031,	//Nexon EU
		LCT_POLAND		= 647291032,	//Nexon EU
		LCT_TURKEY		= 647291033,	//Nexon EU
	};

protected:
	static LocalType m_eLocalType;

public:
	static LocalType GetLocalType() { return m_eLocalType; }
	static DWORD GetNativeIMEConvMode( LocalType eLocalType );
	static void  GetPacPassword( OUT char *szPassword, IN const int iPasswordSize, IN const int iPasswordType, IN LocalType eLocalType );
	static void  EncryptDecryptData( OUT char *szResultData, IN const int iResultSize, IN const char *szSourceData, IN const int iSourceSize, IN bool bPassword );
	static void  GetChangedFontScale( OUT float &rfFontScale );
	static bool  IsVPrintf();
	static bool  IsIME();

public:
	ioLocalManagerParent(void);
	virtual ~ioLocalManagerParent(void);
};

#endif // __ioLocalManagerParent_h__