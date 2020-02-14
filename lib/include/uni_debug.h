#ifndef __UNI_DEBUG_H__
#define __UNI_DEBUG_H__
//------------------------------------
//错误码定义
enum {
	UNI_ERROR_INIT = 1001,
	UNI_ERROR_NO_MEMORY,
	UNI_ERROR_RECV_MSG,
};

//调试信息LEVEL定义
enum DEBUG_MODE_E {
	DEBUG_ERR = 0,		//ERROR
	DEBUG_WARNING,		//WARNING
	DEBUG_INFO,			//INFO
	DEBUG_NORMAL = 6,	//DEBUG
};

//模块编号定义
enum DEBUG_MODULE_E {
	DEBUG_MODULE_IOT = 0,
	DEBUG_MODULE_ASR,
	DEBUG_MODULE_MAIN,
	DEBUG_MODULE_HTTP,
	DEBUG_MODULE_WAVE,
	DEBUG_MODULE_KEROS,	//通用模块
	DEBUG_MODULE_MAX
};

/** 
 * 调试接口初始化
 */
void keros_debug_init(void);

/** 
 * 模式设置
 * mode : enum DEBUG_MODULE_E
 */
void keros_debug_mode_set(unsigned int mode, char level);
void keros_debug_mode_set_all(char level);

/** 
 * 获取某个模块的调试信息等级
 */
int keros_debug_mode_level_get(char mode);

int keros_debug_head_print(int mode, int level,const char *file, int line);
void keros_debug_show(void);

#endif
