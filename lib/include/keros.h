#ifndef	__KEROS_H__
#define __KEROS_H__

#include "stdio.h"
#include "string.h"
#include "uni_platform.h"
#include "keros_queue.h"

/**
 * ERROR CODE
 */
#define UNI_ERRORCODE_BASE			1000
#define UNI_PARAM_ERROR				(UNI_ERRORCODE_BASE + 1)
#define UNI_MEMORY_ERROR			(UNI_ERRORCODE_BASE + 2)
#define UNI_CREATE_THREAD_ERROR		(UNI_ERRORCODE_BASE + 3)
#define UNI_CREATE_SEM_ERROR		(UNI_ERRORCODE_BASE + 4)
#define UNI_CREATE_MUTEX_ERROR		(UNI_ERRORCODE_BASE + 5)
#define UNI_CONNECT_SERVER_FAILD	(UNI_ERRORCODE_BASE + 6)

/**
 * 回到消息参数为字符串形式的统一接口类型
 */
typedef void (*char_msg_cb_t)(char*);

/*=================================================================
                          音频数据相关
=================================================================*/
typedef enum uni_audio_data_type{
	UNI_AUDIO_DATA_TYPE_PCM,
	UNI_AUDIO_DATA_TYPE_SPEEX,
	UNI_AUDIO_DATA_TYPE_MP3,
	UNI_AUDIO_DATA_FINISH,
}uni_audio_data_type_t;

typedef struct uni_audio_data{
        uni_audio_data_type_t   type;
        unsigned char                   channels:5;
        unsigned char                   buf_flag:1;
        short                           len;
        unsigned char*                  data;
}uni_audio_data_t;

/********************************************************
 *	function	uni_audio_data_create
 *		创建一个uni_audio_data_t 结构体
 *	return 
 *		success		uni_audio_data_t 指针
 *		fail		NULL
 * ******************************************************/
uni_audio_data_t *uni_audio_data_create(void);

/********************************************************
 *	function	uni_audio_data_create_buf
 *		创建一个uni_audio_data_t 结构体，并给data字段分配
 *		长度为len的内存
 *	@len	
 *		data 长度
 *	return 
 *		success		uni_audio_data_t 指针
 *		fail		NULL
 * ******************************************************/
uni_audio_data_t *uni_audio_data_create_buf(int len);

/********************************************************
 *	function	uni_audio_data_destroy
 *		销毁uni_audio_data_t结构体
 *	@data
 *		结构体指针
 * ******************************************************/
int uni_audio_data_destroy(uni_audio_data_t *data);


/**
 * 在RTOS系统上运行时，建议先调用此接口，以优化内存使用
 * (单麦时可不使用)
 */
void uni_record_init(void);

/*=================================================================
                          IOT相关接口
=================================================================*/
/**
 * IOT回调消息类型
 */
enum uni_iot_event_type {
	UNI_IOT_EVENT_BOOTPUSH, 	//开机推送消息
	UNI_IOT_EVENT_REMOTEOD, 	//手机端点播
	UNI_IOT_EVENT_CTRLCMD,		//控制命令
	UNI_IOT_EVENT_MSG,			//状态信息、错误信息
};

/**
 * IOT在线命令词类型
 */
enum uni_ctrlcmd_type {
	UNI_CTRLCMD_PLAY,
	UNI_CTRLCMD_PLAY_NEXT,
	UNI_CTRLCMD_PLAY_PREV,
	UNI_CTRLCMD_PLAY_PAUSE,
	UNI_CTRLCMD_PLAY_CONT,
	UNI_CTRLCMD_VOL_UP, 		// 5
	UNI_CTRLCMD_VOL_DOWN,
	UNI_CTRLCMD_NEW_WECHATMSG,
};

/**
 * 错误码
 */
enum uni_msgcode_E {
	UNI_MSGCODE_CNT_SERVER,		// 正在连接云端
	UNI_MSGCODE_CNT_OK,			// 连接云端成功
	UNI_MSGCODE_DISCONNECT,		// 已断开链接
	UNI_MSGCODE_NETWORK_ERR,	// 网络异常
	UNI_MSGCODE_MAX,
};


/**
 * 云端推送音频信息结构体
 * 在iot消息类型（uni_iot_event中的type，下同）为
 * UNI_IOT_EVENT_BOOTPUSH 时关注
 */
typedef struct {
	char*	music_url;
	char*	image_url;
	char*	video_url;
	char*	list;
	char	audio_id[32];
	char	index;
	char	has_next;
} push_info_t;

/**
 * 云端音频点播信息结构体
 * 在iot消息类型为 UNI_IOT_EVENT_REMOTEOD 时关注
 */
typedef push_info_t remoteod_info_t;

/**
 * 云端信息结构体
 * 在iot消息类型为 UNI_IOT_EVENT_CTRLCMD 时关注
 */
typedef struct {
	enum uni_ctrlcmd_type	cmd_type;
	char*	param;
} ctrlcmd_info_t;

typedef struct {
	enum uni_msgcode_E	msg_code;
	char*	msg;
} msg_t;

/**
 * IOT回调消息结构体，根据type值不同，关注联合中的不同成员
 */
struct uni_iot_event {
	enum uni_iot_event_type type;
	union {
		push_info_t 	push_info;
		remoteod_info_t remoteod_info;
		ctrlcmd_info_t	ctrlcmd_info;
		msg_t			msg;
	};
};

/** 
 * 能力集枚举值
 */
typedef enum {
	UNI_CAP_ALARM	= 0x1,
	UNI_CAP_GUI 	= 0x2,
} uni_cap_E;

typedef enum {
	UNI_AUDIO_CODEC_SPEEX,
	UNI_AUDIO_CODEC_OPUS,
	UNI_AUDIO_CODEC_UNKNOW
} uni_ac_format_E;

struct uni_iot_info{
	char	udid[64];
	char	appkey[64];
	char	wifi_ssid[64];
	char	clientname[32];
	char	app_secret[64];
	char	app_version[32];
	char	hard_version[16];
	char	sn[32];
	char	os[16]; 				// 设备操作系统名称，限最多15个字符
	char	mac[20];				// "8C:F7:10:68:1F:26" 形式
	char	cap;					// 能力集，以uni_cap_E类型值通过|组合传入
	uni_ac_format_E audio_codec_format;//暂时保留
	char	need_push;				// 1:需要推送； 0:不需要推送
	char	tts_type;				// 上传语音的编码格式。 0:speex; 1:opus
	int 	heart_interval; 		// 发送心跳的间隔时间(单位s)
};

/**
 * IOT回调函数类型
 */
typedef void (*iot_event_handle)(struct uni_iot_event *event);

/********************************************************
 *	function		uni_iot_agent_init
 *		iot代理初始化
 *	@info	-input
 *		iot代理初始化参数
 *	@event_handle
 *		iot event 处理函数
 *
 * ******************************************************/
int uni_iot_agent_init(struct uni_iot_info *info,
		iot_event_handle event_handle);



/*=================================================================
						  在线ASR相关
================================================================*/
/**
 * ASR 识别结果类型
 */
typedef enum {
	UNI_RESULT_TYPE_KAR,		// 文本消息中只有KAR结果
	UNI_RESULT_TYPE_NLU,		// 文本消息中只有NLU结果
	UNI_RESULT_TYPE_KARNLU,		// 文本消息中有KAR 和 NLU结果
	UNI_RESULT_TYPE_OPT,		// 文本消息中为命令词
	UNI_RESULT_TYPE_ERRMSG		// 文本消息为错误信息
}uni_result_type_E;

/**
 * 在线ASR回调消息类型
 */
typedef enum{
	UNI_ASR_EVENT_ERRMSG,		//云端返回错误消息
	UNI_ASR_EVENT_RESULT,		//给出KAR/NLU结果消息 再消息里区分时opt/kar/nlu/all
	UNI_ASR_EVENT_RESULT_URL,	//给出 +URL消息
	UNI_ASR_EVENT_RESULT_TTS,	//给出 +TTS流播放消息
	UNI_ASR_EVENT_NETWORK_ERR,	//给出网络异常消息
	UNI_ASR_EVENT_SYSTEM_ERR,	//系统错误（内部调用出错等）


	//下面这些暂时不用
	UNI_ASR_EVENT_STARTED,
	UNI_ASR_EVENT_VAD_END,
	UNI_ASR_EVENT_UNKNOW,
}uni_asr_event_E;

/**
 * uni_online_asr_start接口中的falg参数，可多个参数通过'|'传入
 */
enum uni_asr_flag{
	// 无特殊标识
	UNI_ASR_FLAG_NONE = 0,	

	//	当此标识设置时， uni_online_asr_start
	//	函数将阻塞，直到asr识别完成后退出
	UNI_ASR_FLAG_WAITING_DONE = 0x1,

	// 此标识用于启动VAD, 置位时开启VAD
	UNI_ASR_FLAG_VAD_START = 0x2,

	// 翻译模式标识
	UNI_ASR_FLAG_TRANSLATE = 0x4,

	// 获取NLU结果
	UNI_ASR_FLAG_RST_NLU = 0x8,

	// 获取KAR结果
	UNI_ASR_FLAG_RST_KAR = 0x10,
};

/**
 * 回调消息结构体
 * 根据event_type不同，关注联合中的不同成员
 * @当event_type 为UNI_ASR_EVENT_ERRMSG 或者UNI_ASR_EVENT_RESULT时，
 * 主要关注result_text信息
 * @当event_type 为UNI_ASR_EVENT_RESULT_URL时，关注
 * 联合中的url_info中的信息
 * @当event_tyep 为UNI_ASR_EVENT_RESULT_TTS是，关注
 * 联合中的tts_info中的信息
 */
typedef struct _asr_resoult{
	uni_asr_event_E event_type;
	char	srid[64];
	char*	result_text;			// 文本结果
	unsigned char	dialog_ctrl;	//0：任务完成后不需要启动识别； 1：需要；2：命令词
	union {
		struct {
			keros_queue* tts_queue;
		}tts_info;
		struct {
			char*	url;
			char	audioID[32];
			char	name[64];
			unsigned char play_list_size;
			unsigned char index;
		}url_info;
	};
}uni_asr_result_t;
/***************************************
{
    "type":x, 				// uni_result_type_E 类型值
    "nlu":{
    },
    "kar":{
        "asr_text":"xxxxx", //识别文本
        "asr_resp":"xxxxx", //回复文本
    },
}
***************************************/

/**
 * 在线ASR回调函数类型
 * @event: 回调消息类型
 * @owner_param: 用户私有参数
 * @param: 回调消息参数
 */
typedef void(*asr_event_cb_t)(uni_asr_event_E event, void* owner_param, uni_asr_result_t* result);
/********************************************************
 *	function:		uni_online_asr_start
 *		启动 online asr. 识别结果在event_handle 中返回, 
 *		其中event_handle中的 event 为uni_asr_event_E类型值, 
 *		param 为uni_asr_result_t 对象
 *		owner_param 为调用此接口是传入的param
 *	@event_handle:	
 *		online asr 事件回调函数 支持的事件
 *		见 uni_asr_event_t.
 *	@flag: asr 标识，详见 enum uni_asr_flag
 *	@timeout_ms:
 *		asr 超时时间。超过这个时间ASR将被强制结束并获取识别
 *		结果。
 *		<= 0 表示永不超时
 *	return: <0:启动失败; >=0:启动成功
 *
 * ******************************************************/
int uni_online_asr_start(asr_event_cb_t event_handle, unsigned int flag, int timeout_ms, void* owner_param);

/********************************************************
 *	function:		uni_online_asr_stop
 *		uni_online_asr_start在运行完成后会自动退出，如果在中途需要人为退出ASR
 *		则可以调用此接口
 *
 *	@flag:
 *		0: 正常退出asr，结束时会去获取识别结果
 *		1: 强制退出asr, 结束时不会去获取识别结果
 *
 * ******************************************************/
int uni_online_asr_stop(char flag);


/**
 * 收取完tts流之后调用此接
 */
void uni_destroy_asr_result(void);

/*=================================================================
						  离线ASR相关
=================================================================*/
/**
 * 离线引擎配置结构体
 */
struct offline_engine_config{
	char *model_file;		// 语音模型文件路径 
	char *wakeup_grammar;	// 唤醒语法模型文件路径
	char *recognize_grammar;// 识别语法模型文件路径
};

/**
 * 离线识别结果
 */
struct offline_engine_result {
	char	*text;	// 识别文本
	//char	*cmd;
	int		score;	// 打分
};

/**
 * 设置引擎编号
 * 默认为56号引擎，如果更换引擎，需要在离线引擎初始化前调用此接口进行修改
 */
void uni_offline_set_engine_id(int id);

/**
 * 离线识别回调函数类型
 */
typedef void (*offline_asr_cb_t)(struct offline_engine_result*, void*);
/********************************************************
 *	function:离线引擎初始化
 *		
 *	@config_file	--input
 *		configure file of recognize word.
 *	@event_func		--input
 *		callback interface for getting recognize result.
 *  return 
 *		0:successful; <0:failed
 * ******************************************************/
int uni_offline_engine_init(struct offline_engine_config *config_file, 
		offline_asr_cb_t offline_cb, void *param);

/** 
 * function:开始离线识别接口
 *			同一时刻只能是一种模式，切换模式时需要重启引擎
 * @mode:OFFLINE_WAKEUP_MODE(0); OFFLINE_ASR_MODE(1)
 *
 * return：成功返回0；失败返回负值
 */
int uni_offline_engine_start(int mode);


/** 
 * function:停止离线识别接口
 *
 * return：成功返回0；失败返回负值
 */
int uni_offline_engine_stop(void);


/*=================================================================
						  tts合成接口
=================================================================*/
enum TTS_OPT_E {
    UNI_TTS_OPT_IDLE,
    UNI_TTS_OPT_STOP
};

enum TTS_TYPE_E {
    UNI_TTS_TYPE_SPEEX,
    UNI_TTS_TYPE_OPUS
};

struct dev_identity_info {
    char	udid[64];
	char	appkey[64];
	char	clientname[32];
	char	app_secret[64];
};

struct speak_info {
	char	speaker[20];
	int		speed;
	int		tone;
};

struct txt2tts {
	void*			handle;
	keros_queue*	tts_queue;
};

/**
 * 文本转tts流接口
 * @txt: 文本信息
 * @info: 设备信息（appkey，appsecret，clientname，udid）
 * @speak: 发音参数设置（目前不支持，请传NULL）
 * @tts_type: 得到的tts流编码格式   0: speex;  1:opus
 * 
 * 返回值：正确返回本次操作句柄；错误返回NULL
 */
struct txt2tts* uni_txt2tts_start(char *txt, struct dev_identity_info* info, struct speak_info* speak, int tts_type);

/**
 * 销毁资源
 * 在每次完成转化后，或者调用 uni_txt2tts_stop 强制结束后调用此接口
 *
 * @handle: uni_txt2tts_start 接口返回句柄
 *
 * 返回值： 无
 */
void uni_txt2tts_destroy(struct txt2tts* handle);

/**
 * 停止接收tts流
 * 需要终端tts流接收时调用此接口，强制结束 
 *
 * @handle： uni_txt2tts_start 接口返回句柄
 * 
 * 返回值： 无
 */
void uni_txt2tts_stop(struct txt2tts* handle);

/** 
 * 获取错误码
 * 出错后可调用此接口获取错误码
 */
int uni_txt2tts_get_errcode(void);
/*=================================================================
						  声波配网相关
=================================================================*/
/**
 * 配网识别结果类型
 */
enum wave_analysis_result{
	UNI_ANALYSIS_SUCCESS = 0,
	UNI_ANALYSIS_TIMEOUT,
	UNI_ANALYSIS_CLOSE,
	UNI_ANALYSIS_ERROR,
};

/**
 * 声波配网识别结果
 */
typedef struct {
	char ssid[32+1];
	char passwd[64+1];
	char account[64+1];	// 如果配网时选择一建绑定，此成员保存绑定码
} wave_info_t;

typedef int (*wave_handle_cb_t)(wave_info_t* pinfo);

/**
 * 启动声波配网（带一键绑定）
 * @func: 回调函数接口，用来返回识别到的信息
 * @timeout_ms: 识别超时时间，单位ms
 * @bind_flag: 是否启用一键绑定（同时识别出绑定码）。1：启用； 0：不启用
 *
 * 返回值：1: 正在识别; 0:启动成功;  <0:启动失败
 */
int uni_multi_wave_analysis_start(wave_handle_cb_t func, int timeout_ms, int bind_flag);

/**
 * 获取到结果后调用此接口
 */
void uni_wave_analysis_stop(void);


/*=================================================================
						  微聊相关
=================================================================*/
typedef enum {
	UNI_WECHAT_APP, 		//app调用接口
	UNI_WECHAT_PUBLICNO,	//公众号调用接口
	UNI_WECHAT_STREAM,		//以分段形式上传录音文件
	UNI_WECHAT_FILE 		//以文件形式上传录音文件
}uni_wechat_flag_E;

/**
 * 微聊回调函数接口
 * @event: 消息类型
 * @own_param: 用户私有参数
 * @param: 消息参数
 */
typedef void(*wechat_event_cb_t)(char event, void* own_param, void* param);

/********************************************************
 *	function	uni_wechat_start
 *		启动微聊发送 微信公众号版本, 微聊消息将发送到微信公众号上
 *
 *	@event_handle:
 *		微聊事件回调函数
 *		(暂时没用)
 *  @flag:微聊启动参数，主要是选择 app接口调用还是公众号接口调用；
 *		选择数据上传形式。应该使用uni_wechat_flag_E值，用"|"组合传递
 *	@timeout_ms:
 *		微聊超时时间， 超时后微聊将强制结束并发送
 *	return 
 *		0:	success
 *		-1: failed
 * ******************************************************/
int uni_wechat_start(wechat_event_cb_t event_handle, int flag, int timeout_ms, void* param);


/********************************************************
 *	function	uni_wechat_stop
 *		停止微聊发送
 *
 * ******************************************************/
int uni_wechat_stop(void);

/********************************************************
 *	function	uni_iot_request_chat
 *		请求微聊消息
 *
 * ******************************************************/
int uni_iot_request_chatmsg(char_msg_cb_t cb);

/*=================================================================
						  音乐播放相关
=================================================================*/
/**
 * 向云端请求播放列表
 * @cb：回调函数，接收返回的列表信息（字符串形式）
 * @page_index: 页编号
 * @page_sieze: 每页大小（有多少首歌曲）
 * return:0:成功； <0:失败
 */
int uni_music_request_list(char_msg_cb_t cb, int page_index, int page_size);

/**
 * 播放列表内跳转
 * @cb：回调函数，接收返回的列表信息（字符串形式）
 * @index: 跳转编号
 *
 * return:0:成功； <0:失败
 */
int uni_music_jumpto(char_msg_cb_t cb, int index);

/**
 * 请求播放音频
 * @cb：回调函数，接收返回的列表信息（字符串形式）
 *
 * return:0:成功； <0:失败
 */
int uni_music_request_audio(char_msg_cb_t cb);

/**
 * 请求播放下一首
 * @cb：回调函数，接收返回的列表信息（字符串形式）
 *
 * return:0:成功； <0:失败
 */
int uni_music_request_next(char_msg_cb_t cb);

/**
 * 请求播放上一首
 * @cb：回调函数，接收返回的列表信息（字符串形式）
 *
 * return:0:成功； <0:失败
 */
int uni_music_request_prev(char_msg_cb_t cb);

/*=================================================================
						  其他接口
=================================================================*/
/********************************************************
 *	function	uni_iot_deviceinfo_upload
 *		上传设备状态信息， 包括 电池电量 充电状态 SD卡容量等
 *	@power_percent
 *		电池电量百分比, 值为-1时 表示充电中
 *	return
 *		0: success
 *		-1: fail
 * ******************************************************/
int uni_iot_deviceinfo_upload(int power_percent);

/********************************************************
 *	function	uni_keros_version_get
 *		获取keros 版本号
 *	return
 *		keros version string point
 *		此返回值不用释放
 * ******************************************************/
char *uni_keros_version_get(void);

/********************************************************
 *	function	uni_server_connect_status_get
 *		获取iot server连接状态
 *	return
 *		1: iot已连接服务器
 *		0: iot未连接服务器
 *
 * ******************************************************/
int uni_server_connect_status_get(void);


enum iot_music_state_E {
	IOT_MUSIC_STATE_PLAY = 1,
	IOT_MUSIC_STATE_PAUSE = 2,
	IOT_MUSIC_STATE_STOP = 4
};
/********************************************************
 *	function	uni_iot_update_music_status
 *		上传播放状态
 *	@status		播放状态
 *		"1" :	开始播放
 *		"2" :	暂停播放
 *		"4" :	停止播放
 *	@cur_audio_id
 *		当前播放音乐ID
 *		
 * ******************************************************/
int uni_iot_update_music_status(enum iot_music_state_E state, char *cur_audio_id);

/********************************************************
 *	function	uni_get_udid_from_server
 *		从云端获取udid
 *	@transId		--output
 *		事物ID，输出参数。 从云端获取
 *	@udid			--output
 *		udid, 输出参数， 从云端获取
 *	@appkey			--input
 *		产品编号	输入参数
 *	@client_name	--input
 *		客户编号	输入参数
 *	@batchcode		--input
 *		生产批次号	输入参数
 *
 *	return
 *		0	成功
 *		-1	获取transId失败
 *		-2	获取UDID失败
 *		-3	校验UDID失败
 * ******************************************************/
int uni_get_udid_from_server(char *transId, char *udid, char *appkey, char *client_name, char *batchcode);


typedef struct ota_result{
	int		costTime;
	char	returnCode[6];
	char	message[64];
	char	downloadUrl[256];
	char	md5[64];
	int     version;
}uni_ota_info_t;

/********************************************************
 *function	uni_ota_check_update
 *		从云端获取ota 信息
 *param	
 *	@ver	当前版本
 *
 *	@udid	设备udid	
 *	
 *	@appkey	产品编号		
 *		
 *	@server 相对unisound 内外网服务 0: 内外， 1: 外网
 *		
 *	return
 *		NULL	失败或者无升级版本
 *		非NULL	有升级版本(获取使用后需free内存)
 *
 * ******************************************************/

struct ota_result *uni_ota_check_update(int ver, char *udid, char *appkey, int server);


/**
 * 同步云端时间，返回时间戳
 * @accuracy_flag: 0:返回ms级时间戳； 1:返回s级时间戳
 * 返回值：<0：失败;   成功返回（long long型）数值
 */
long long uni_sync_time_from_server(int accuracy_flag);



#define HTTP_BIND_ADMIN_SUCCESS           0
#define HTTP_BIND_USER_SUCCESS            3012
#define HTTP_BIND_USER_OVER_LIMIT         3010
#define HTTP_BIND_ACCOUNT_VIRIFY_FAILED   2011                                                                                                                                                           
#define HTTP_BIND_ACCOUNT_IS_VIRFIED      3016
#define HTTP_BIND_PHONE_IS_UNSUPPORTED    3029
#define HTTP_BIND_DUPLICATE               6002

#define HTTP_UNBIND_SUCCESS               0
#define HTTP_UNBIND_FAILED                3007

typedef struct {
	char account[32];
	char udid[64];
	char app_key[64];
	char secret[64];
} device_bind_info_t;
/********************************************************
 *function  uni_bind_device
 *      bind device to a phone
 *param 
 *  @info   device information
 *
 *  return
 *      0   success
 *      -1  failed
 *
 * ******************************************************/                                                                                                                                               
int uni_bind_device(device_bind_info_t *info);

/********************************************************
 *function  uni_bind_device
 *      unbind device from the phone
 *param 
 *  @info   device information
 *
 *  return
 *      0   success
 *      -1  failed
 *
 * ******************************************************/
int uni_unbind_device(device_bind_info_t *info);



/*=================================================================
						  以下未实现
=================================================================*/
/********************************************************
 *	function	uni_iot_request_fav
 *		收藏/取消收藏 音乐
 *	@cur_audio_id
 *		音乐ID
 *	return 
 *		success		0
 *		fail		-1
 * ******************************************************/
int uni_iot_request_fav(char *cur_audio_id);

/********************************************************
 *	function	uni_iot_request_tts_stop
 *		请求云端停止发送tts流
 *
 *	@ttsStreamId
 *		tts流ID
 * ******************************************************/
int uni_iot_request_tts_stop(char *ttsStreamId);

/********************************************************
 *	function	uni_iot_request_vad_status
 *		请求云端vad开关状态
 *
 * ******************************************************/
int uni_iot_request_vad_status(void);

/********************************************************
 *	function	uni_iot_request_play_fav
 *		请求播放收藏列表
 *		收藏结果在 iot event UNI_IOT_EVENT_FAV_RESULT 中
 *		返回
 *
 * ******************************************************/
int uni_iot_request_play_fav(void);


/********************************************************
 *	function	uni_iot_request_awaken_status
 *		请求云端唤醒开关状态
 *
 *
 * ******************************************************/
int uni_iot_request_awaken_status(void);

#endif /* _KEROS_H__ */

