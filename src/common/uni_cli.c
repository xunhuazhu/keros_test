#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "uni_cli.h"
#include "platform.h"


static int get_char(char *c)
{
	if(fread(c,1,1,stdin) == 1){
		printf("%02x\n",*c);
		return 0;
	}
	printf("fread stdin fail\n");
	return -1;
}

static struct uni_cli_ctrl cli_ctrl;

static int is_char(char c)
{
	if( c >= ' ' && c <= 0x7e){
		return 1;
	}
	return 0;
}

static void rm_rn(char *buf)
{
	int len;
	if(buf == NULL){
		return;
	}
	while(1){
		len = strlen(buf);
		if(len <= 0){
			return;
		}
		if(buf[len-1] == '\n' ||
				buf[len-1] == '\r'){
			buf[len-1] = 0;
		}else{
			break;
		}
	}
}

static void uni_cli_param_destroy(struct uni_cli_param *cli_param)
{
	if(cli_param == NULL){
		return;
	}

	struct cli_param *param = cli_param->params;
	struct cli_param *tmp;
	while(param){
		tmp = param->next;
		uni_free(param);
		param = tmp;
	}

	if(cli_param->argv){
		uni_free(cli_param->argv);
	}
	uni_free(cli_param);
}

static struct uni_cli_param *cli_param_decode(char *buf)
{
	if(buf == NULL || strlen(buf) == 0){
		return NULL;
	}

	char *tmp;
	char *start = buf;
	int len;
	struct cli_param *param,*last = NULL;
	struct uni_cli_param *cli_param = uni_malloc(sizeof(struct uni_cli_param));
	memset(cli_param,0,sizeof(struct uni_cli_param));

	while(start != 0){
		tmp = strchr(start,' ');
		if(tmp == NULL){
			param = uni_malloc(sizeof(struct cli_param)+strlen(start)+1);
			memset(param,0,sizeof(struct cli_param)+strlen(start)+1);
			param->data = ((char*)param) + sizeof(struct cli_param);
			strcpy(param->data,start);
			if(cli_param->params == NULL){
				cli_param->params = param;
				last = param;
			}else{
				last->next = param;
				last = param;
			}
			cli_param->count++;
			break;
		}else{
			len = tmp - start;
			param = uni_malloc(sizeof(struct cli_param) + len + 1);
			memset(param,0,sizeof(struct cli_param)+ len +1);
			param->data = ((char*)param) + sizeof(struct cli_param);
			strncpy(param->data,start,len);
			start = tmp + 1;

			if(cli_param->params == NULL){
				cli_param->params = param;
				last = param;
			}else{
				last->next = param;
				last = param;
			}
			cli_param->count++;
		}
	}
	cli_param->argv = uni_malloc(sizeof(char*)*cli_param->count);
	param = cli_param->params;
	int count = 0;
	while(param){
		cli_param->argv[count++] = param->data;
		param = param->next;
	}

#if 0
	param = cli_param->params;
	count = 0;
	printf("cli_param num: %d\n",cli_param->count);
	while(param){
		printf("  %d: %s\n",count++,param->data);
		param = param->next;
	}
#endif
	return cli_param;
}

static int cli_cmd_run(struct uni_cli_param *cli_param)
{
	int ret;
	if(cli_param == NULL){
		return -1;
	}

	struct cli_param *param = cli_param->params;
	if(param == NULL){
		return -1;
	}

	uni_cli_cmd_t *cmd = cli_ctrl.head;
	while(cmd){
		int cmd_len = strlen(cmd->cmd);
		int param_len = strlen(param->data);
		int len = cmd_len > param_len?cmd_len:param_len;
		if(strncmp(cmd->cmd,param->data,len) == 0){
			if(cmd->handle){
				ret = cmd->handle(cli_param->count, cli_param->argv);
				if(ret == -1){
					printf("ERR: Param fail! Description: %s\n",cmd->descript);
				}
				return ret;
			}
		}
		cmd = cmd->next;
	}

	printf("Can't find command %s!\n",param->data);
	return -2;
}

static void cli_uart_recv_thread(void)
{
	char *line;
	int num;
	printf("cli thread init\n");
	printf("cli->");
	while(1){
#if 0
		char c;
		if(get_char(&c) == 0){
			if(c == '\r' || c == '\n'){
				//printf("\n");
				struct uni_cli_param *cli_param = cli_param_decode(cli_ctrl.recvbuf);
				if(cli_param){
					cli_cmd_run(cli_param);
					uni_cli_param_destroy(cli_param);
				}
				printf("\ncli->");
				memset(cli_ctrl.recvbuf,0,sizeof(cli_ctrl.recvbuf));
				cli_ctrl.index = 0;
			}else if(c == '\b'){
				if(cli_ctrl.index > 0){
					cli_ctrl.index--;
					cli_ctrl.recvbuf[cli_ctrl.index] = 0;
					//printf("\b");
				}
			}else if(is_char(c)){
				//printf("%c",c);
				cli_ctrl.recvbuf[cli_ctrl.index] = c;
				cli_ctrl.index++;
			}else{
				printf("\n%02x\n",c);
			}
		}else{

		}
#else
		getline(&line, &num, stdin);
		rm_rn(line);
		struct uni_cli_param *cli_param = cli_param_decode(line);
		if(cli_param){
			cli_cmd_run(cli_param);
			uni_cli_param_destroy(cli_param);
		}
		printf("\ncli->");
#endif
	}
}

int uni_cli_cmd_register(char *cmd_name,cli_cmd_handle *function,char *descript)
{
	if(cmd_name == NULL){
		return -1;
	}
	uni_cli_cmd_t *cmd = malloc(sizeof(uni_cli_cmd_t));
	cmd->cmd = malloc(strlen(cmd_name)+1);
	memset(cmd->cmd,0,strlen(cmd_name)+1);
	strcpy(cmd->cmd,cmd_name);
	cmd->handle = function;
	if(descript){
		cmd->descript = malloc(strlen(descript)+1);
		memset(cmd->descript,0,strlen(descript)+1);
		strcpy(cmd->descript,descript);
	}
	if(cli_ctrl.head == NULL){
		cli_ctrl.head = cmd;
	}else{
		uni_cli_cmd_t *tmp = cli_ctrl.head;
		while(tmp->next != NULL){
			tmp = tmp->next;
		}
		tmp->next = cmd;
	}
	return 0;
}

static int cli_help_handle(int argc, char *argv[])
{
	uni_cli_cmd_t *cmd = cli_ctrl.head;
	printf("Cli command list:\n");
	while(cmd){
		if(cmd->descript){
			printf("  %-20s  --%s\n",cmd->cmd, cmd->descript);
		}else{
			printf("  %-20s  --\n",cmd->cmd);
		}
		cmd = cmd->next;
	}
	return 0;
}

int uni_cli_init(void)
{
	memset(&cli_ctrl,0,sizeof(cli_ctrl));
	uni_pthread_t ptd;
	struct thread_param param = {512,OS_PRIORITY_HIGH,"cli"};
	uni_thread_create(&ptd, &param, cli_uart_recv_thread, NULL);
	//xTaskCreate( (TaskFunction_t)cli_uart_recv_thread, "cli", 512, NULL, 4, NULL );

	uni_cli_cmd_register("help",cli_help_handle,NULL);

	//uni_sys_cli_init();
	return 0;
}

