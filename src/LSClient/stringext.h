#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

static inline std::string &tolower(std::string &s){
	std::locale loc;
	for (size_t i=0; i<s.length(); ++i)
		s[i] = tolower(s[i],loc);

	return s;
}

static inline std::string &toupper(std::string &s){
	std::locale loc;
	for (size_t i=0; i<s.length(); ++i)
		s[i] = toupper(s[i],loc);

	return s;
}

static inline std::vector<std::string> explode(const std::string& str, const std::string& delim ) 
{
    std::string next = "";
    std::vector<std::string> result;

    // For each character in the string
    for (std::string::const_iterator it = str.begin(); it != str.end(); it++) 
	{
    	// If we've hit the terminal character
		std::string::const_iterator ch = delim.begin();
		for( ; ch != delim.end(); ch++ )
		{
    		if (*it == *ch) 
			{
    			// If we have some characters accumulated
    			if (next.length() > 0) 
				{
    				// Add them to the result vector
    				result.push_back(next);
    				next = "";
    			}
				break;
    		} 
		}
		if( ch == delim.end() )
		{
    		// Accumulate the next character into the sequence
    		next += *it;
    	}
    }

	// put remaining part into result
	result.push_back( next );

    return result;
}

static inline std::string implode(std::vector<std::string>& strv, const std::string& delim ) 
{
	std::string		result = "";
	for( std::vector<std::string>::iterator it = strv.begin(); it != strv.end(); it++ )
	{
		if( result == "" )	result = *it;
		else
		{
			result = result + delim + *it;
		}
	}

	return result;
}
