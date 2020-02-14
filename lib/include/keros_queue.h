#ifndef __KEROS_QUEUE_H__
#define __KEROS_QUEUE_H__

#include "uni_platform.h"

typedef struct keros_queue_info{
	void *data;
}keros_queue_info;

typedef struct keros_queue{
	uni_sem_t				sem;
	uni_sem_t				put_sem;
	uni_mutex_t				mutex;
	int						r_index;
	int						w_index;
	keros_queue_info *info;
	short					max_num;
	short					current_num;

	// 0: queue是调用init创建的
	// 1: queue是调用create创建的
	// 此标志用于destroy函数, create创建的queue需要
	// free queue。init的不用
	unsigned char			is_point:1;
	void *(*get)(struct keros_queue *queue,int timeout);
	void *(*get_nolock)(struct keros_queue *queue);
	int (*put)(struct keros_queue *queue,void *data, int timeout);
	int (*is_full)(struct keros_queue *queue);
	int (*is_empty)(struct keros_queue *queue);
}keros_queue;

/********************************************************
 *	function	keros_queue_create
 *		创建一个队列句柄
 *
 *	@queue_max:
 *		队列最大长度
 *	return:
 *		成功返回队列句柄
 *		失败返回NULL
 *
 * ******************************************************/
keros_queue *keros_queue_create(int queue_max);

/********************************************************
 *	function	keros_queue_init
 *		初始化一个队列
 *	@queue
 *		队列句柄
 *	@queue_max
 *		队列最大长度
 *	return
 *		成功返回 0
 *		失败返回 -1
 * ******************************************************/
int	keros_queue_init(keros_queue *queue,int queue_max);

/********************************************************
 *	function	keros_queue_destroy
 *		销毁队列
 *	@queue:
 *		队列句柄
 *
 *  return
 *		成功返回 0
 *		失败返回 -1
 * ******************************************************/
int keros_queue_destroy(keros_queue *queue);

//--------------------------------
/* 定义一个环形缓冲区 */
typedef struct keros_circle_buffer{
	int		size;	//总大小
	int		valid_len;	//有效数据长度
	int		write_pos;
	int		read_pos;
	uni_sem_t	get_sem;
	uni_sem_t	put_sem;
	uni_mutex_t mutex;
	int		(*get)(struct keros_circle_buffer *buffer, unsigned char* p, int len, int flag);
	void*	(*get_nolock)(struct keros_circle_buffer *buffer);
	int 	(*put)(struct keros_circle_buffer *buffer, unsigned char* p, int len, int flag);
	int 	(*is_full)(struct keros_circle_buffer *buffer);
	int 	(*is_empty)(struct keros_circle_buffer *buffer);
	unsigned char* data;
} keros_circle_buffer_t;

/**
 * 创建一个环形缓冲区
 * 成功返回缓冲区句柄，失败返回NULL
 */
keros_circle_buffer_t* keros_circle_queue_create(int buffer_size);
int keros_circle_queue_destroy(keros_circle_buffer_t* buffer);

#endif
