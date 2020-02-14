#ifndef	__UNI_PLATFORM_H__
#define	__UNI_PLATFORM_H__
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


/**************************************************
 用户需要实现 uni_socket_opt 及 uni_platform_opt 结
 构体中的接口，并分别通过uni_platform_opt_register 
 和 uni_tcp_opt_register 进行注册
**************************************************/


/**
 * 线程优先级宏定义， 请根据平台设置优先级的值
 */
enum os_priority_E {
	KEROS_PRIORITY_NORMAL = 1,	//normal
	KEROS_PRIORITY_HIGH,		//high
	KEROS_PRIORITY_REALTIME	//realtime (highest)
};

#define UNI_WAITFOREVER		-1

/**
 *THREAD STACK SIZE 
 */
enum th_stack_size_E {
	STACK_SIZE        = 1024,
	STACK_SMALL_SIZE  = STACK_SIZE*1,
	STACK_NORMAL_SIZE = STACK_SIZE*2,
	STACK_MIDDLE_SIZE = STACK_SIZE*4,
	STACK_LARGE_SIZE  = STACK_SIZE*6
};

/**
 * 类型定义
 */
typedef void* uni_sem_t;
typedef void* uni_mutex_t;
typedef void* uni_pthread_t;
typedef void* uni_timer_t;
typedef int uni_socket_t;

/**
 * 网络相关API结构体
 */
struct uni_socket_opt{
	/********************************************************
	 *	function uni_get_domain_ip
	 *		通过域名获取IP
	 *	@domain
	 *		域名
	 *	@ip		--output
	 *		ip地址
	 *	return
	 *		0: success
	 *		-1: fail
	 * ******************************************************/
	int (*uni_get_domain_ip)(const char *domain, unsigned int *ip);

	/********************************************************
	 *	function uni_tcp_connect
	 *		创建tcp连接, 成功返回tcp连接句柄
	 *	@ip	
	 *		服务器ip地址
	 *	@port
	 *		服务器tcp端口号
	 *	@recv_timeout
	 *		tcp连接的接收超时时间,单位ms
	 *	@send_timeout
	 *		tcp连接的发送超时时间,单位ms
	 *	return
	 *		成功返回tcp连接句柄
	 *		失败返回 <0
	 *
	 * ******************************************************/
	uni_socket_t (*uni_tcp_connect)(unsigned int ip, unsigned short port, int recv_timeout, int send_timeout);
	
	/********************************************************
	 *	function uni_tcp_send
	 *		tcp发送函数
	 *	@socket	
	 *		tcp连接句柄
	 *	@buf
	 *		待发送的缓存数据
	 *	@len
	 *		发送的数据长度
	 *	@timeout_ms
	 *		>=0:发送超时时间;   -1:不超时等待
	 *	return
	 *		成功返回发送数据的长度
	 *		失败返回-1
	 * ******************************************************/
	int (*uni_tcp_send)(uni_socket_t socket, char *buf, int len, int timeout_ms);

	/********************************************************
	 *	function	uni_tcp_recv
	 *		tcp接收函数
	 *	@socket
	 *		tcp连接句柄
	 *	@buf
	 *		接收缓存区
	 *	@len
	 *		接收数据长度
	 *	@timeout_ms
	 *		>=0:接收超时时间;   -1:不超时等待
	 *	return
	 *		成功返回接收数据长度
	 *		失败返回-1
	 *
	 * ******************************************************/
	int (*uni_tcp_recv)(uni_socket_t socket, char *buf, int len, int timeout_ms);

	/********************************************************
	 *	function	uni_tcp_destroy
	 *		tcp连接销毁函数, 断开tcp连接并且销毁tcp句柄
	 *	@socket
	 *		tcp连接句柄
	 *	return
	 *		成功返回 0
	 *		失败返回 -1
	 *
	 * ******************************************************/
	int (*uni_tcp_destroy)(uni_socket_t socket);

	/********************************************************
	 *	function	uni_htons
	 *		本地字节序转为网络字节序
	 * ******************************************************/
	unsigned short (*uni_htons)(unsigned short n);		

	/********************************************************
	 *	function	uni_htonl
	 *		本地字节序转为网络字节序
	 * ******************************************************/
	unsigned int (*uni_htonl)(unsigned int n);		

	/********************************************************
	 *	function	uni_ntohl
	 *		网络字节序转为本地字节序
	 * ******************************************************/
	unsigned int (*uni_ntohl)(unsigned int n);		
};

/**
 * 网络相关API注册接口
 * return: <0:失败; 0:成功
 */
int uni_tcp_opt_register(struct uni_socket_opt *opt);

/**
 * 线程创建参数结构体
 */
struct thread_param {
	int stack_size;    				// 线程栈大小, 推荐使用th_stack_size_E类型值
	enum os_priority_E th_priority;	// 线程优先级
	char task_name[16];				// 线程名称
};

/**
 * 平台API结构体
 */
struct uni_platform_opt {
	/********************************************************
	 *	@function		uni_thread_create
	 *					Create a thread handle and run thread
	 *
	 *	@thread			-output thread handle
	 *	@thread_param	-input	create thread param
	 *
	 *	@return			if success return 0; error return -1
	 * ******************************************************/
	int (*uni_thread_create)(uni_pthread_t* thread, struct thread_param *param, void (*start_routine)(void*), void *args);

	/********************************************************
	 *	@function		uni_thread_detach
	 *			detach thread	
	 *			Linux系统下调用 pthread_detach
	 *			RToS系统此函数直接返回
	 *					
	 *
	 *	@thread			thread handle
	 * ******************************************************/
	int (*uni_thread_detach)(uni_pthread_t thread);

	/********************************************************
	 *	@function		uni_thread_destroy
	 *					destroy thread
	 *
	 *	@thread			-input	thread handle
	 * ******************************************************/
	int (*uni_thread_destroy)(uni_pthread_t thread);

	/********************************************************
	 *	@function		uni_thread_exit
	 *			现在退出函数, 在线程中调用， 调用后该线程
	 *			退出。
	 *			Linux系统下调用 pthread_exit
	 *					
	 *
	 *	@thread			thread handle
	 * ******************************************************/
	int (*uni_thread_exit)(uni_pthread_t thread);


	/* semaphore */
	/********************************************************
	 *	function		uni_sem_init
	 *		初始化信号量。 调用此函数将创建一个信号量控制块,
	 *		控制块句柄通过参数 sem 返回。
	 *	@sem	-output
	 *		信号量句柄
	 *
	 *	@value	-input
	 *		信号量初始值
	 *
	 * return
	 *		success 0
	 *		fail	-1
	 * ******************************************************/
	int (*uni_sem_init)(uni_sem_t* sem, unsigned int value);

	/********************************************************
	 *	function	uni_sem_wait
	 *		等待信号量。 等待sem信号量，sem值为0时函数阻塞
	 *		直到sem值大于0 获取到信号量后退出。
	 *	@sem
	 *		信号量句柄
	 *	return
	 *		success 0
	 *		fail	-1
	 * ******************************************************/
	int (*uni_sem_wait)(uni_sem_t sem);

	/********************************************************
	 *	function	uni_sem_post
	 *		释放信号量
	 *	@sem
	 *		信号量句柄
	 *
	 * ******************************************************/
	int (*uni_sem_post)(uni_sem_t sem);

	/********************************************************
	 *	function	uni_sem_destroy
	 *		销毁信号量控制块
	 *	@sem
	 *		信号量句柄
	 * ******************************************************/
	int (*uni_sem_destroy)(uni_sem_t sem);

	/********************************************************
	 *	function	uni_sem_wait_timeout
	 *		在times时间内等待信号量，如果为等到信号量超时退出
	 *	@sem
	 *		信号量句柄
	 *	@times
	 *		>=0:超时时间;      -1:不超时等待
	 * ******************************************************/
	int (*uni_sem_wait_timeout)(uni_sem_t sem, int timeout_ms);

	/* mutex */
	/********************************************************
	 *	function	uni_mutex_init
	 *		线程锁初始化。创建一个线程锁控制块，
	 *		控制块句柄通过mutex返回
	 *	@mutex	-output
	 *		线程锁控制块句柄
	 *	return
	 *		success 0
	 *		fail	-1
	 *
	 * ******************************************************/
	int (*uni_mutex_init)(uni_mutex_t* mutex);

	/********************************************************
	 *	function	uni_mutex_lock
	 *		获取线程锁。获取不到锁函数将阻塞直到可以获取锁。
	 *	@mutex
	 *		线程锁句柄
	 *	return
	 *		success 0
	 *		fail	-1
	 *
	 * ******************************************************/
	int (*uni_mutex_lock)(uni_mutex_t mutex);

	/********************************************************
	 *	function	uni_mutex_unlock
	 *		释放线程锁
	 *	@mutex
	 *		线程锁句柄
	 *	return 
	 *		success 0
	 *		fail	-1
	 * ******************************************************/
	int (*uni_mutex_unlock)(uni_mutex_t mutex);

	/********************************************************
	 *	function	uni_mutex_destroy
	 *		销毁线程锁控制块
	 *	@mutex
	 *		线程锁句柄
	 *	return
	 *		success 0
	 *		fail	-1
	 * ******************************************************/
	int (*uni_mutex_destroy)(uni_mutex_t mutex);

	/* sleep */
	void (*uni_usleep)(unsigned int us);
	void (*uni_msleep)(unsigned int ms);
	void (*uni_sleep)(unsigned int s);

	/* stdlib */
	/**
	 * 申请空间，并清空为0
	 */
	void* (*uni_malloc)(int size);
	void (*uni_free)(void *ptr);


	/********************************************************
	 *	function	uni_mac_get
	 *		获取mac地址
	 *	@mac	-output
	 *		mac地址 长度6bytes
	 * ******************************************************/
	void* (*uni_mac_get)(unsigned char *mac);

	/********************************************************
	 *	function	uni_wifi_connect_get
	 *		获取WiFi连接状态
	 *	return
	 *		1	WiFi已连接
	 *		0	WiFi未连接
	 * ******************************************************/
	int (*uni_wifi_connect_get)(void);

	/********************************************************
	 *	function	uni_wait_wifi_connect
	 *		等待WiFi连接。 如果WiFi未连接此函数阻塞直到
	 *		WiFi连接后返回
	 *	return 
	 *		0
	 * ******************************************************/
	int (*uni_wait_wifi_connect)(void);

	/********************************************************
	 *	function	uni_systime_get
	 *		获取系统时间,单位ms
	 *	return
	 *		返回系统时间
	 * ******************************************************/
	unsigned int (*uni_systime_get)(void);
};

/**
 * 平台API注册接口
 */
int uni_platform_opt_register(struct uni_platform_opt* opt);


enum uni_record_data_type{
	UNI_RECORD_DATA_TYPE_SINGLE,
	UNI_RECORD_DATA_TYPE_DOUBLE,
	UNI_RECORD_DATA_TYPE_SINGLE_AEC,
	UNI_RECORD_DATA_TYPE_DOUBLE_AEC,
};

/********************************************************
 *	Datatype	struct _record_data; uni_record_data_t
 *		录音数据结构体，用于给keros传输原始录音
 *		原始录音必须时 16K 16bit
 *
 *	@type: 
 *		录音类型， 支持
 *		UNI_RECORD_DATA_TYPE_SINGLE     
 *			单声道PCM，音频数据存储在 left_buf中
 *		UNI_RECORD_DATA_TYPE_DOUBLE
 *			双声道PCM, 音频数据存储在 left_buf和right_buf中
 *		UNI_RECORD_DATA_TYPE_SINGLE_AEC,
 *			单声道+AEC PCM，mic音频存储在left_buf, AEC存储
 *			在aec_buf
 *		UNI_RECORD_DATA_TYPE_DOUBLE_AEC,
 *			双声道+AEC PCM，mic音频存储在left_buf right_buf, 
 *			AEC存储在aec_buf
 *	@len:
 *		数据长度(如果是多个通道，每个通道的数据长度是一致的)
 *	@left_buf
 *		左声道数据buffer, 或单声道的数据buffer
 *	@right_buf
 *		右声道数据buffer
 *	@aec_buf
 *		AEC数据buffer
 *
 *
 * ******************************************************/
typedef struct _record_data {
	enum uni_record_data_type type;
	unsigned int	time_ms;
	short	len;
	unsigned char	*left_buf;	//修改为左右声道数据混合传输，左左右右左左右右
	unsigned char	*right_buf;	//此成员弃用
	unsigned char	*aec_buf;
} uni_record_data_t;

/**
 * 解码器类型定义
 */
enum uni_coder_type{
	UNI_CODER_TYPE_SPEEX,
};

/**
 * 录音相关API结构体
 */
struct uni_record_opt{
	/********************************************************
	 *	录音队列大小
	 * ******************************************************/
	int		record_queue_size;

	/********************************************************
	 *	function		record_start
	 *					启动录音
	 *	return			success: record handler; fail: NULL 
	 * ******************************************************/
	void *(*record_start)(void);

	/********************************************************
	 *	function		record_stop
	 *					停止录音
	 *	@hd				record handler
	 *
	 *	return			success: 0; fail: -1
	 * ******************************************************/
	int (*record_stop)(void *hd);

	/********************************************************
	 *	function		record_data_get
	 *
	 *	@timeout_ms		超时时间, 如果没有录音数据等待 timeout_ms
	 *					毫秒 后返回; timeout_ms = -1表示永久等待
	 *
	 *	return			return uni_record_data_t point. 
	 *					timeout	return NULL;
	 *
	 * ******************************************************/
	uni_record_data_t *(*record_data_get)(void *hd, int timeout_ms);

	/********************************************************
	 *	function		record_data_destroy
	 *					释放录音数据
	 *	@data			data point
	 *
	 * ******************************************************/
	void (*record_data_destroy)(uni_record_data_t *data);

	/********************************************************
	 *	function:		decoder_init
	 *		初始化一个解码器
	 *
	 *	@type
	 *		解码器类型， 当前只支持UNI_CODER_TYPE_SPEEX
	 *
	 *	return
	 *		返回解码器句柄
	 * ******************************************************/
	int (*decoder_init)(enum uni_coder_type type);

	/********************************************************
	 *	function		decoder_uninit
	 *		销毁解码器
	 *	@hd
	 *		解码器句柄
	 * ******************************************************/
	int (*decoder_uninit)(int hd);

	/********************************************************
	 *	function		decode
	 *		解码函数，将in的编码音频解码为原始音频数据
	 *		存储到out中
	 *	@hd	
	 *		解码器句柄
	 *	@in 
	 *		输入待解码音频
	 *	@inlen
	 *		输入音频长度
	 *	@out
	 *		输出解码后音频
	 *	@outlen
	 *		输出解码后音频长度
	 *
	 *	return 
	 *		success 0,
	 *		failed -1
	 *		
	 * ******************************************************/
	int (*decode)(int hd, char *in, int inlen, char **out, int *outlen);

	/********************************************************
	 *	function:		encoder_init
	 *		初始化一个encoder
	 *
	 *	@type
	 *		码器类型， 当前只支持UNI_CODER_TYPE_SPEEX
	 *
	 *	return
	 *		返回解码器句柄
	 *
	 * ******************************************************/
	void *(*encoder_init)(void);

	/********************************************************
	 *
	 * ******************************************************/
	int (*encoder_uninit)(void *hd);

	/********************************************************
	 *	function		encode
	 *		编码函数，将in的原始音频编码
	 *		存储到out中
	 *	@hd	
	 *		编码器句柄
	 *	@in 
	 *		输入待编码音频
	 *	@inlen
	 *		输入音频长度
	 *	@out
	 *		输出编码后音频
	 *	@outlen
	 *		输出编码后音频长度
	 *
	 *	return 
	 *		success 0,
	 *		failed -1
	 *		
	 * ******************************************************/
	int (*encode)(void *hd, char *in, int inlen, unsigned char **out, int *outlen);
};
 
/**
 * 录音相关API注册接口
 * @queue_size: 录音数据缓冲队列大小
 * return: 0:成功; <0:失败
 */
int uni_record_opt_register(struct uni_record_opt* opt, int queue_size);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif

