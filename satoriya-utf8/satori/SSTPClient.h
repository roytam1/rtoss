
#include <windows.h>
#include <string>
using std::string;


bool direct_sstp(
	const string& i_script = "\\0\\s[0]にこにこ。\\1\\s[10]にこにこ。\\e",
	const string& i_client_name = "送信者さん",
	HWND i_client_window = NULL);

#include "SakuraClient.h"
/*

class SSTPClient : public SakuraClient
{
public:
	SSTPClient() {}
	virtual ~SSTPClient() {}
	
	virtual string request(const string& i_request_string);

	// ん～

};
*/
