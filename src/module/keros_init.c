#include <stdio.h>
#include "iniparser.h"
#include "keros.h"
#include "uni_cli.h"


static struct uni_iot_info *info;

struct uni_iot_info *uni_get_iot_info(void)
{
	return info;
}

void iot_event_hand(struct uni_iot_event *event)
{
	printf("[%s-%d]iot event type = %d.\r\n", __func__, __LINE__, event->type);
	switch (event->type) {
	case UNI_IOT_EVENT_BOOTPUSH:
	case UNI_IOT_EVENT_REMOTEOD:
		printf("iot url = %s\r\n", event->remoteod_info.music_url);
		printf("audio_id = %s\r\n", event->remoteod_info.audio_id);
		printf("has_next = %d\r\n", event->remoteod_info.has_next);
		break;
	case UNI_IOT_EVENT_CTRLCMD:
		printf("iot ctrl cmd id = %d.\r\n", event->ctrlcmd_info.cmd_type);
		break;
	case UNI_IOT_EVENT_MSG:
		printf("iot msg(%d): %s\r\n", event->msg.msg_code, event->msg.msg);
		break;
	default:
		printf("Unknow event type(%d)", event->type);
	}

	printf("iot_event_hand out.\r\n");
}



int keros_init(char *filename)
{
	printf("%s\n",__func__);
	dictionary *ini;
	int interval;
	char *str;
	int debug_level;

	//struct uni_iot_info info = {0};
	info = malloc(sizeof(struct uni_iot_info));
	if(info == NULL)
	{
		printf("malloc uni_iot_info failed .\n");
		return -1;
	}

	memset(info,0,sizeof(struct uni_iot_info));

	ini = iniparser_load(filename);//parser the file
	if(ini == NULL)
	{
		 printf("can not open %s","config.ini");
		 return -1;
	}
	
	//printf("\n%s:\n",iniparser_getsecname(ini,0));//get section name
	str = iniparser_getstring(ini,"iot_info:appkey",NULL);
	if(str == NULL){
		return -1;
	}
	//strcpy(board->device_type,buf);
	printf("[iot_info]: %s\n",str);

     /* 填写设备信息 */
    strcpy(info->appkey, str);
	str = iniparser_getstring(ini,"iot_info:clientname",NULL);
	strcpy(info->clientname, str);
	str = iniparser_getstring(ini,"iot_info:app_secret",NULL);
	strcpy(info->app_secret, str);
	str = iniparser_getstring(ini,"iot_info:udid",NULL);
	strcpy(info->udid, str);

	printf("appkey: %s\r\nclientname: %s\r\nsecret: %s\r\nudid: %s\r\n", 
						info->appkey,info->clientname,info->app_secret,info->udid);

	debug_level = iniparser_getint(ini,"iot_info:debug_level",1);
	printf("debug_level: %d\n",debug_level);

	interval = iniparser_getint(ini,"iot_info:heart_interval",5);
	info->heart_interval = interval;

	str = iniparser_getstring(ini,"dev_info:app_version",NULL);
	strcpy(info->app_version, str);
	str = iniparser_getstring(ini,"dev_info:mac",NULL);
	strcpy(info->mac, str);
	str = iniparser_getstring(ini,"dev_info:hard_version",NULL);
	strcpy(info->hard_version, str);
	str = iniparser_getstring(ini,"dev_info:os",NULL);
	strcpy(info->os, str);
	str = iniparser_getstring(ini,"dev_info:sn",NULL);
	strcpy(info->sn, str);

	strcpy(info->wifi_ssid, "kar500_wifi");
	info->cap = UNI_CAP_ALARM | UNI_CAP_GUI;
	info->audio_codec_format = UNI_AUDIO_CODEC_OPUS;
	info->need_push = 1;
	info->tts_type = 1;

	iniparser_freedict(ini);

	/* 注册平台相关接口 */
	uni_socket_register();
	register_platform();
	keros_debug_mode_set_all(debug_level);
	
	_switch_env(4);
	if (uni_iot_agent_init(info, iot_event_hand) < 0) {
		printf("uni_iot_agent_init error.\r\n");
		return -1;
	}
	
	set_wifi_connect(1);

	return 0;
}

int iot_init_cli(int argc, char *argv[])
{
	if(argc < 2){
		return -1;
	}

	char *configfile = argv[1];

	keros_init(configfile);

	return 0;
}


int iot_test_init(void)
{
	uni_cli_cmd_register("iot_init", iot_init_cli, "iot lc init");
	return 0;
}
