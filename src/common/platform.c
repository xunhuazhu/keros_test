#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include <errno.h>


#include "uni_platform.h"
#include <pthread.h>
#include <semaphore.h>

#include <unistd.h>

#include <stdarg.h>
#include <sys/time.h>


#ifdef __cplusplus
extern "C" {
#endif  // C++

//-----------------------------------------
typedef struct {
	sem_t	sem;
	pthread_cond_t	cond;
	pthread_condattr_t cond_attr;
	pthread_mutex_t	mutex;	//没用
} platform_sem;
//-----------------------------------------
static int g_net_status = 0; //0:disconnected; 1:connected
//-----------------------------------------
int uni_debug_print(int level, const char *fmt, ...)
{
	char 	buff[10240];
    va_list 	vl;

	va_start(vl, fmt);
    vsnprintf (buff, 10240, fmt, vl);
    printf("[LEVEL-%d] %s", level, buff);
    fflush(stdout);
    va_end (vl);

    return 0;
}



/********************************************************
 *	@function		uni_thread_create
 *					Create a thread handle and run thread
 *
 *	@thread			-output thread handle
 *	@thread_param	-input	create thread param
 *
 *	@return			if success return 0; error return -1
 * ******************************************************/
int uni_thread_create(void **thread, struct thread_param *param, void (*start_routine)(void*), void *args)
{
	pthread_t*	_p = malloc(sizeof(pthread_t));

	if (pthread_create(_p, NULL, (void*(*)(void*))start_routine, args) != 0) {
		return -1;
	} else {
		*thread = _p;
		return 0;
	}
#if 0
	pthread_t*	_p = malloc(sizeof(pthread_t));
	pthread_attr_t*		pthread_attr = NULL;
	pthread_attr_t 		thread_attr;
	struct sched_param 	schedule_param;

	if (param) {
		pthread_attr_init(&thread_attr);
		schedule_param.sched_priority = param->th_priority;
		pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED); //有这行，设置优先级才会生效
		pthread_attr_setschedpolicy(&thread_attr, SCHED_RR);
		pthread_attr_setschedparam(&thread_attr, &schedule_param); 
		pthread_attr = &thread_attr;
	}
	if (pthread_create(_p, pthread_attr, (void*(*)(void*))start_routine, args) != 0) {
		return -1;
	} else {
		*thread = _p;
		return 0;
	}

	if (pthread_attr)
		pthread_attr_destroy(pthread_attr);
#endif
}

/********************************************************
 *	@function		uni_thread_detach
 *					detach thread
 *
 *	@thread			thread handle
 * ******************************************************/
int uni_thread_detach(void *thread)
{
	return pthread_detach(pthread_self());
}

/********************************************************
 *	@function		uni_thread_destroy
 *					destroy thread
 *
 *	@thread			-input	thread handle
 * ******************************************************/
int uni_thread_destroy(void *thread)
{
	//pthread_cancel(*(pthread_t*)thread);
	free(thread);
	return 0;
}

int uni_thread_exit(void *thread)
{
	pthread_exit(thread);
	return 0;
}

int uni_sem_destroy(void *sem);

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
int uni_sem_init(void **sem, unsigned int value)
{
	int	ret = 0;
	//pthread_condattr_t cond_attr;
	
	platform_sem*	_s = calloc(1, sizeof(platform_sem));	
	if (!_s) {
		printf( "sem_init err.\r\n");
		return -1;
	}

	do {
		pthread_condattr_init(&_s->cond_attr);
		ret = pthread_condattr_setclock(&_s->cond_attr, CLOCK_MONOTONIC);
		if (ret != 0) {
			printf("condaddr setclock:");
			ret = -4;
			break;
		}

		ret = sem_init(&_s->sem, 0, value);
		if (ret != 0) {
			ret = -2;
			break;
		}

		ret = pthread_cond_init(&_s->cond, &_s->cond_attr);
		if (ret != 0) {
			ret = -3;
			sem_destroy(&_s->sem);
			break;
		}
		*sem = _s;
		ret = 0;
	} while (0);

	if (ret < 0) {
		printf("sem_init error.\r\n");
		uni_sem_destroy(_s);
	}
	printf("sem init end.\r\n");
	return ret;
}

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
int uni_sem_wait(void *sem)
{
	platform_sem*	p = sem;
	return sem_wait(&p->sem);
}

/********************************************************
 *	function	uni_sem_post
 *		释放信号量
 *	@sem
 *		信号量句柄
 *
 * ******************************************************/
int uni_sem_post(void *sem)
{
	platform_sem*	p = sem;

	sem_post(&p->sem);
	pthread_cond_signal(&p->cond);
	
	return 0;
}

/********************************************************
 *	function	uni_sem_destroy
 *		销毁信号量控制块
 *	@sem
 *		信号量句柄
 * ******************************************************/
int uni_sem_destroy(void *sem)
{
	platform_sem*	p = (platform_sem*)sem;

	pthread_condattr_destroy(&p->cond_attr);
	pthread_cond_destroy(&p->cond);
	sem_destroy(&p->sem);
	free(p);

	return 0;
}

/********************************************************
 *	function	uni_sem_wait_timeout
 *		在times时间内等待信号量，如果为等到信号量超时退出
 *	@sem
 *		信号量句柄
 *	@times
 *		超时时间 单位ms
 * ******************************************************/
int uni_sem_wait_timeout(void *sem, int ms)
{
#if 1
	platform_sem*	p = sem;
	long long		nsec = 0;
	long long 		timeout_ms = ms;
	struct timespec now = {0};

	if (ms < 0) {
		return sem_wait(&p->sem);		
	} else {
		clock_gettime(CLOCK_REALTIME, &now);
		nsec = timeout_ms * 1000000 + now.tv_nsec;
		now.tv_sec += nsec/1000000000;
		now.tv_nsec = nsec%1000000000;
		return sem_timedwait(&p->sem, &now);
	}
#else
	struct timespec	now;
	long long		timeout_ms = ms; //1s
	long long		nsec = 0;
	int				ret = 0;
	platform_sem*	p = sem;

	if (ms < 0)
		timeout_ms = 0;

	clock_gettime(CLOCK_MONOTONIC, &now);
	
	nsec = timeout_ms * 1000000 + now.tv_nsec;
	now.tv_sec += nsec/1000000000;
	now.tv_nsec = nsec%1000000000;
	
	if (sem_trywait(&p->sem) == 0) {
		return 0;
	} else {
		ret = pthread_cond_timedwait(&p->cond, &p->mutex, &now);
		if (ret == 0) {
			sem_trywait(&p->sem);
			return 0;
		} else {
			return -1;
		}
	}
#endif
}

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
int uni_mutex_init(void **mutex)
{
	pthread_mutex_t* _m = malloc(sizeof(pthread_mutex_t));
	
	pthread_mutex_init(_m, NULL);

	*mutex = _m;

	return 0;
}

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
int uni_mutex_lock(void *mutex)
{
	return pthread_mutex_lock(mutex);
}

/********************************************************
 *	function	uni_mutex_unlock
 *		释放线程锁
 *	@mutex
 *		线程锁句柄
 *	return 
 *		success 0
 *		fail	-1
 * ******************************************************/
int uni_mutex_unlock(void *mutex)
{
	return pthread_mutex_unlock(mutex);
}

/********************************************************
 *	function	uni_mutex_destroy
 *		销毁线程锁控制块
 *	@mutex
 *		线程锁句柄
 *	return
 *		success 0
 *		fail	-1
 * ******************************************************/
int uni_mutex_destroy(void *mutex)
{
	pthread_mutex_destroy(mutex);
	free(mutex);

	return 0;
}

/* sleep */
void uni_usleep(unsigned int us)
{
	usleep(us);
}
void uni_msleep(unsigned int ms)
{
	usleep(ms*1000);
}
void uni_sleep(unsigned int s)
{
	sleep(s);
}

/* stdlib */
void *uni_malloc(size_t size)
{
	return calloc(1, size);
}
void uni_free(void *ptr)
{
	free(ptr);
}

void *_mm_malloc(size_t size)
{
	return uni_malloc(size);
}
void _mm_free(void *ptr)
{
	uni_free(ptr);
}

/********************************************************
 *	function	uni_mac_get
 *		获取mac地址
 *	@mac	-output
 *		mac地址 长度6bytes
 * ******************************************************/
void uni_mac_get(unsigned char *mac)
{
	return ;
}

/********************************************************
 *	function	uni_wifi_connect_get
 *		获取WiFi连接状态
 *	return
 *		1	WiFi已连接
 *		0	WiFi未连接
 * ******************************************************/
int uni_wifi_connect_get(void)
{
	return g_net_status;	//定义为已连接
}

/**
 * 调试用： 0：wifi断开； 1：wifi连接
 */
int set_wifi_connect(int flag)
{
	g_net_status = flag;
	printf("set wifi status %d<0:connect 1:disconnect>.\r\n", flag);
}

/********************************************************
 *	function	uni_wait_wifi_connect
 *		等待WiFi连接。 如果WiFi未连接此函数阻塞直到
 *		WiFi连接后返回
 *	return 
 *		0
 * ******************************************************/
int uni_wait_wifi_connect(void)
{
	while (1) {
		if (g_net_status == 1) {
			break;
		}
		sleep(1);
	}
	return 0;
}


/********************************************************
 *	function	uni_systick_get
 *		获取系统时间,单位ms
 *	return
 *		返回系统时间
 * ******************************************************/
unsigned int uni_systime_get(void)
{
#if 0
	struct timeval t;

	if (g_start_time.tv_sec <= 0) {
		gettimeofday(&g_start_time, NULL);
		return 0;
	}

	gettimeofday(&t, NULL);
	
	return (t.tv_sec - g_start_time.tv_sec)*1000 + (t.tv_usec - g_start_time.tv_usec)/1000;
#endif
	struct timespec tv = {0, 0};
	
	clock_gettime(CLOCK_MONOTONIC, &tv);
	return (tv.tv_sec * 1000 + tv.tv_nsec/1000000);
}


/********************************************************
 *	function	uni_timer_destroy
 *		销毁定时器
 *	@timer
 *		定时器句柄
 *
 *	return	success:0
 * ******************************************************/
int uni_timer_destroy(uni_timer_t *timer)
{
	return 0;
}

void print_time(char* str)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);

	if (str) {
		printf("%s time is : %lds - %ldus\r\n", str, tv.tv_sec, tv.tv_usec);
	} else {
		printf("this time is : %lds - %ldus\r\n", tv.tv_sec, tv.tv_usec);
	}
}

void register_platform(void)
{
	struct uni_platform_opt		opt;

	opt.uni_thread_create = uni_thread_create; 
	opt.uni_thread_detach = uni_thread_detach;
	opt.uni_thread_destroy = uni_thread_destroy;
	opt.uni_thread_exit = uni_thread_exit;

	opt.uni_sem_init = uni_sem_init;
	opt.uni_sem_wait = uni_sem_wait;
	opt.uni_sem_post = uni_sem_post;
	opt.uni_sem_wait_timeout = uni_sem_wait_timeout;
	opt.uni_sem_destroy = uni_sem_destroy;

	opt.uni_mutex_init = uni_mutex_init;
	opt.uni_mutex_lock = uni_mutex_lock;
	opt.uni_mutex_unlock = uni_mutex_unlock;
	opt.uni_mutex_destroy = uni_mutex_destroy;

	opt.uni_sleep = uni_sleep;
	opt.uni_msleep = uni_msleep;

	opt.uni_free = uni_free;
	opt.uni_malloc = uni_malloc;
	opt.uni_systime_get = uni_systime_get;
	opt.uni_wait_wifi_connect = uni_wait_wifi_connect;
	opt.uni_wifi_connect_get = uni_wifi_connect_get;

	uni_platform_opt_register(&opt);
}
#ifdef __cplusplus
}
#endif  // C++



