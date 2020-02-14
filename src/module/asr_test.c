#include "record.h"
#include "uni_cli.h"
#include "uni_platform.h"
//#include "platform.h"



static struct {
	uni_sem_t	event_sem;
	int			result;

	wave_info_t *wave;
}g_asr_test;

static void _online_event_handle(uni_asr_event_E event, void* own_param, void *param)
{
	uni_asr_result_t*	result = (uni_asr_result_t*)param;
	char*	str = NULL;
	int		i = 0;
	//int		num = sizeof(g_online_cmd_word)/sizeof(g_online_cmd_word[0]);
	
	printf("[%s-%d] Event type = %d\r\n", __func__, __LINE__, event);

	switch (event) {
	case UNI_ASR_EVENT_ERRMSG:
		printf("errMSg: %s.\r\n", result->result_text);
		break;
	case UNI_ASR_EVENT_RESULT_URL:
		printf("url: %s\r\n", result->url_info.url);
		printf("audioID = %s\r\n", result->url_info.audioID);
		printf("audio name = %s\r\n", result->url_info.name);
		printf("list size = %d\r\n", result->url_info.play_list_size);
		printf("audio index = %d\r\n", result->url_info.index);
		break;
	case UNI_ASR_EVENT_RESULT_TTS:
		printf("tts text result: %s\r\n", result->result_text);
		//handle_ttsplay(result);
		break;
	case UNI_ASR_EVENT_RESULT:
		if (result->result_text) {
			printf("asr result: \r\n%s\r\n", result->result_text);
		} else {
			printf("asr result error.\r\n");
		}
		break;
	case UNI_ASR_EVENT_NETWORK_ERR:
		if (result && result->result_text) {
			printf("errMSg: %s.\r\n", result->result_text);
		}
		printf("net work error\r\n");
		break;
	}
	uni_sem_post(g_asr_test.event_sem);
}

static int asr_test_cli(int argc, char *argv[])
{
	if(argc < 2){
		return -1;
	}
	char *audio_file = argv[1];

	record_set_filename(audio_file);
	unsigned int flag = UNI_ASR_FLAG_VAD_START | UNI_ASR_FLAG_RST_NLU | UNI_ASR_FLAG_RST_KAR;
	uni_online_asr_start(_online_event_handle, flag, 10000, NULL);
	uni_sem_wait(g_asr_test.event_sem);
	if(g_asr_test.result != 0){
		printf("asr test fail\n");
	}else{
		printf("asr test success\n");
	}
	return 0;
}

static int wave_handle_cb(wave_info_t* pinfo)
{
	printf("wifi ssid: %s\r\n", pinfo->ssid);
	printf("wifi passwd: %s\r\n", pinfo->passwd);
	printf("wifi account: %s\r\n", pinfo->account);
	g_asr_test.wave = pinfo;
	uni_sem_post(g_asr_test.event_sem);
	return 0;
}

static int wave_test_cli(int argc, char *argv[])
{
	if(argc < 5){
		return -1;
	}
	char *file = argv[1];
	record_set_filename(file);

	uni_multi_wave_analysis_start(wave_handle_cb, 5000, 1);
	uni_sem_wait(g_asr_test.event_sem);

	int ret = 0;
	if(strcmp(argv[2],g_asr_test.wave->ssid)){
		printf("ssid analysis fail\n");
		ret |= 1;
	}
	if(strcmp(argv[3],g_asr_test.wave->passwd)){
		printf("password analysis fail\n");
		ret |= 2;
	}
	if(strcmp(argv[4],g_asr_test.wave->account)){
		printf("account analysis fail\n");
		ret |= 4;
	}

	if(ret == 0){
		printf("wave test success\n");
	}

	return 0;
}

void asr_test_init(void)
{
	memset(&g_asr_test,0,sizeof(g_asr_test));
	uni_sem_init(&g_asr_test.event_sem,0);

	uni_cli_cmd_register("asr_test", asr_test_cli, "asr_test audio_filename");
	uni_cli_cmd_register("wave_test", wave_test_cli, "wave_test audio_filename ssid password account");
}

