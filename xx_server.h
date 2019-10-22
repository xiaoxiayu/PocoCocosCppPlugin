#ifndef _XX_AUTOTEST_SERVER_H
#define _XX_AUTOTEST_SERVER_H

#include "xx_timejob.h"
#include "clsocket/PassiveSocket.h"

struct XX_Header_t {
	int len;
};

typedef struct XX_NET_DATA {
	XX_Header_t	dataHeader;
	char			dataStr[MAX_PACKET];
}XXNetData;

typedef struct XX_SOCKET_INFO {
	std::string		ip;
	int				port;
	int				socket;

	std::string		production;

	void*			sender_thread;
}SOCKET_INFO;


typedef struct XX_DO_ACTION_DATA {
	CActiveSocket*	pClient;
	XXTimeJob*	pScreen;
} XX_ADATA;



void XXDataSend(CActiveSocket* pClient, std::string data);
bool XXServerRun(XXTimeJob* screen);


#endif // _XX_AUTOTEST_SERVER_H