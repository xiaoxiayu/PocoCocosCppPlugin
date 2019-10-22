#ifdef _XX_AUTOTEST_
#include "xx_server.h"

#include "AppDelegate.h"
#include <iostream>

void* ServerRun(void* data) {
	XXServerRun((XXTimeJob*)data);
	return NULL;
}

void XXServerStart() {
	XXTimeJob* screen = new XXTimeJob;
	screen->ScheduleReg();

#ifdef _WIN32
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ServerRun, (void*)screen, 0, NULL);
#else
	pthread_t thread = 0;
	pthread_create(&thread, NULL, ServerRun, (void*)screen);
#endif
}

#endif //_XX_AUTOTSET_