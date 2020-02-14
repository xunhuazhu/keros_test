#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define  RESULT_FILE  "./test_result.txt"


int uni_save_test_result(char *item, char *result)
{
	int fd;
	char buf[32] = {0};

	printf("%s\t\t%s\n",item,result);	

	fd =  open(RESULT_FILE, O_RDWR | O_CREAT| O_APPEND, 0666); 
	if(fd < 0)
	{
		printf("open %s failed.\n",RESULT_FILE);
		return -1;
	}

	sprintf(buf,"%s\t\t%s\r\n",item,result);	
	write(fd,buf,strlen(buf));

	close(fd);	
	return 0;
}



