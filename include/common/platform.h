#ifndef	__PLATFORM_H__
#define	__PLATFORM_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "uni_platform.h"

/********************************************************
 *	线程优先级宏定义， 请根据平台设置优先级的值
 *
 * ******************************************************/
#define OS_PRIORITY_NORMAL                  1   //normal
#define OS_PRIORITY_HIGH                    2   //high
#define OS_PRIORITY_REALTIME                3   //realtime (highest)

#define UNI_WAITFOREVER		-1

/* THREAD STACK SIZE */
#define STACK_SIZE							1024
#define STACK_SMALL_SIZE                    STACK_SIZE*1
#define STACK_NORMAL_SIZE                   STACK_SIZE*2
#define STACK_MIDDLE_SIZE                   STACK_SIZE*4
#define STACK_LARGE_SIZE                    STACK_SIZE*6



/********************************************************
 *	@function		uni_thread_create
 *					Create a thread handle and run thread
 *
 *	@thread			-output thread handle
 *	@thread_param	-input	create thread param
 *
 *	@return			if success return 0; error return -1
 * ******************************************************/
int uni_thread_create(void **thread, struct thread_param *param, void (*start_routine)(void*), void *args);

/********************************************************
 *	@function		uni_thread_detach
 *			detach thread	
 *			Linux系统下调用 pthread_detach
 *			RToS系统此函数直接返回
 *					
 *
 *	@thread			thread handle
 * ******************************************************/
int uni_thread_detach(void *thread);

/********************************************************
 *	@function		uni_thread_destroy
 *					destroy thread
 *
 *	@thread			-input	thread handle
 * ******************************************************/
int uni_thread_destroy(void *thread);

/********************************************************
 *	@function		uni_thread_exit
 *			现在退出函数, 在线程中调用， 调用后该线程
 *			退出。
 *			Linux系统下调用 pthread_exit
 *					
 *
 *	@thread			thread handle
 * ******************************************************/
int uni_thread_exit(void *thread);


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
int uni_sem_init(void **sem, unsigned int value);

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
int uni_sem_wait(void *sem);

/********************************************************
 *	function	uni_sem_post
 *		释放信号量
 *	@sem
 *		信号量句柄
 *
 * ******************************************************/
int uni_sem_post(void *sem);

/********************************************************
 *	function	uni_sem_destroy
 *		销毁信号量控制块
 *	@sem
 *		信号量句柄
 * ******************************************************/
int uni_sem_destroy(void *sem);

/********************************************************
 *	function	uni_sem_wait_timeout
 *		在times时间内等待信号量，如果为等到信号量超时退出
 *	@sem
 *		信号量句柄
 *	@times
 *		超时时间 单位ms
 * ******************************************************/
int uni_sem_wait_timeout(void *sem,int times);

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
int uni_mutex_init(void **mutex);

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
int uni_mutex_lock(void *mutex);

/********************************************************
 *	function	uni_mutex_unlock
 *		释放线程锁
 *	@mutex
 *		线程锁句柄
 *	return 
 *		success 0
 *		fail	-1
 * ******************************************************/
int uni_mutex_unlock(void *mutex);

/********************************************************
 *	function	uni_mutex_destroy
 *		销毁线程锁控制块
 *	@mutex
 *		线程锁句柄
 *	return
 *		success 0
 *		fail	-1
 * ******************************************************/
int uni_mutex_destroy(void *mutex);

/* sleep */
void uni_usleep(unsigned int us);
void uni_msleep(unsigned int ms);
void uni_sleep(unsigned int s);

/* stdlib */
void *uni_malloc(size_t size);
void uni_free(void *ptr);


/********************************************************
 *	function	uni_mac_get
 *		获取mac地址
 *	@mac	-output
 *		mac地址 长度6bytes
 * ******************************************************/
void uni_mac_get(unsigned char *mac);

/********************************************************
 *	function	uni_wifi_connect_get
 *		获取WiFi连接状态
 *	return
 *		1	WiFi已连接
 *		0	WiFi未连接
 * ******************************************************/
int uni_wifi_connect_get(void);

/********************************************************
 *	function	uni_wait_wifi_connect
 *		等待WiFi连接。 如果WiFi未连接此函数阻塞直到
 *		WiFi连接后返回
 *	return 
 *		0
 * ******************************************************/
int uni_wait_wifi_connect(void);

/********************************************************
 *	function	uni_systime_get
 *		获取系统时间,单位ms
 *	return
 *		返回系统时间
 * ******************************************************/
unsigned int uni_systime_get(void);


#endif

