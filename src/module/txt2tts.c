#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#include "keros.h"
#include "module.h"


#define TTS_END_STR			"EOF"

static sem_t	g_tts_sem;	//用于同步tts流处理流程
//static sem_t	g_tts_start_sem;	//用于同步tts文件形成
int		g_tts_end_flag;		//用于同步tts流处理流程
pthread_t	g_tts_thread;	//tts流处理线程句柄


void* handle_tts_data(void* arg)
{
	keros_queue* tts_queue = (keros_queue*)arg;
	int			mem_fd = 0;
	uni_audio_data_t*	get_data = NULL;

	pthread_detach(pthread_self());
	printf("%s\n",__func__);
	mem_fd = open("tts.opus", O_CREAT | O_RDWR | O_TRUNC, 0666);
	if (mem_fd < 0) {
		printf("Error: Open tts file error.");
	} else {
		//sem_post(&g_tts_start_sem);
		if (tts_queue != NULL) {
			while (1) {
				if (g_tts_end_flag == 1) {
					break;
				}
				//sleep(1);
				get_data = (uni_audio_data_t*)tts_queue->get(tts_queue, 2000);
				if (get_data == NULL) {
					printf("Error: get iot tts timeout. wait and try again\r\n");
					continue;
				}
				printf("record get tts data type = %d, len = %d\r\n", get_data->type, get_data->len);
				write(mem_fd, get_data->data, get_data->len);
				if (get_data->type == UNI_AUDIO_DATA_FINISH) {	//数据接收完成
					printf("get iot tts end.");
					break;
				}
				printf("destory...\r\n");
				uni_audio_data_destroy(get_data);
				printf("destory end.\r\n");
				get_data = NULL;
			}
		} else {
			printf("tts play task queue is NULL."); 
		}
	}
	if (get_data) {
		uni_audio_data_destroy(get_data);
		get_data = NULL;
	}
	write(mem_fd, TTS_END_STR, 3);

	printf("post sem.\r\n");
	//接收完毕
	//sem_post(&g_tts_sem);

	uni_destroy_asr_result();
	
	if (mem_fd > 0)
		close(mem_fd);
	printf("TTS data get end.\r\n");
	return NULL;
}


int txt2tts(void)
{
	struct dev_identity_info dev_info = {0};
	struct txt2tts*	tts = NULL;
	struct uni_iot_info *info;

	info = uni_get_iot_info();

	strcpy(dev_info.appkey, info->appkey);
	strcpy(dev_info.app_secret, info->app_secret);
	strcpy(dev_info.clientname, info->clientname);
	strcpy(dev_info.udid, info->udid);
	tts = uni_txt2tts_start("万事开头难，然后中间难，最后结尾难", &dev_info, NULL, 0);
	pthread_create(&g_tts_thread, NULL, handle_tts_data, (void*)tts->tts_queue);
		
	return 0;
}
