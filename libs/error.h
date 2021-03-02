#ifndef _ERROR_H
#define _ERROR_H


#define ERROR_UNSPECIFIED        1    // 未知
#define ERROR_BAD_PROC            2    // 坏程序
#define ERROR_INVAL                3    // 非法
#define ERROR_NO_MEM            4    // OOM
#define ERROR_NO_FREE_PROC        5    // 超出当前进程地址空间
#define ERROR_FAULT                6    // 错误

/* 最大错误数 */
#define MAXERROR            6

#endif /* !__LIBS_ERROR_H__ */