#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "errno.h"
#include <semaphore.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "keros.h"
//---------------------------------------
#define	MIC_PERIOD	16

unsigned int uni_systime_get(void);

struct record_t{
	int file_fd;
	char filename[256];
}g_record;



void *record_start(void)
{	

	int fd = open(g_record.filename,O_RDWR, 0666);
	if(fd < 0){
		printf("Open file %s fail\r\n",g_record.filename);
		return NULL;
	}
	g_record.file_fd = fd;
	
	printf("open pcm ok.\r\n");

	return (void*)g_record.file_fd;
}

uni_record_data_t *record_data_get(void *hd, int timeout_ms)
{
	uni_record_data_t*	data = 0;
	
	//printf("read mic ret = %d\r\n", ret);
	data = calloc(1, sizeof(uni_record_data_t));
	
	data->type = UNI_RECORD_DATA_TYPE_DOUBLE;
	data->len = 16 * 16 * 2;
	data->left_buf = malloc(data->len * 2);
	memset(data->left_buf,0,data->len * 2);
	data->time_ms = uni_systime_get();

	char buf[16*16*2] = {0};

	//write(file_fd, data->left_buf, data->len*2);
	read(g_record.file_fd,buf,data->len/2);
	int i;
	for(i = 0; i < data->len/4; i++){
		data->left_buf[4*i] = buf[2*i];
		data->left_buf[4*i+1] = buf[2*i + 1];
	}

	return data;
}


void record_data_destroy(uni_record_data_t *data)
{
	if (data) {
		if (data->aec_buf)
			free(data->aec_buf);
		if (data->left_buf)
			free(data->left_buf);
		free(data);
	}
	return ;
}

int record_stop(void *hd)
{
	if(g_record.file_fd){
		close(g_record.file_fd);
	}
	g_record.file_fd = 0;
	return 0;
}

void record_set_filename(char *name)
{
	if(name == NULL){
		return;
	}

	memset(g_record.filename,0,sizeof(g_record.filename));
	strcpy(g_record.filename,name);
}


