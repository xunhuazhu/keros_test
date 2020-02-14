#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include <execinfo.h>
#include <signal.h>

#include "keros.h"
//#include "record.h"
#include "keros_queue.h"
//----------------------------
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "module.h"
#include "uni_cli.h"


int main(int argc, char* argv[])
{
	int ret;
#if 0
	struct uni_iot_info *info;

	ret = keros_init();
	if(ret){
		printf("keros_int failed.\n");
		return ret;
	}

	info = uni_get_iot_info();
#endif

	uni_cli_init();
	asr_test_init();
	iot_test_init();

	while(1){
		sleep(5);
	};
		#if 0
				printf("clientname: %s\n",info->clientname);
				printf("get udid ret = %d.\r\n", \
					uni_get_udid_from_server(transId, getudid, info->appkey, info->clientname, "11223344"));
				printf("get transID = %s, udid = %s.\r\n", transId, getudid);
		#endif
				//uni_iot_deviceinfo_upload(50);
	printf("---------------\n");
	//txt2tts();
	//uni_save_test_result("hello","PASS");
	printf("quit.\r\n");

	return 0;
}

