#ifndef _STDARG_H
#define _STDARG_H


#ifndef __VA_LIST
#define __VA_LIST
typedef char *va_list;
#endif /* __VA_LIST */

/* 从list中找到param1,指针指向下一个*/
#define va_start(list,param1)   ( list = (va_list)&param1+ sizeof(param1) )

/* 返回类型为mode的第一个参数 */
#define va_arg(list,mode)   ( (mode *) ( list += sizeof(mode) ) )[-1]

/* 返回 */
#define va_end(list) ( list = (va_list)0 )

/**/
//void a(char* arg1, char* arg2, char* arg3)
//{
//    va_list list;
//    ......
//        return;
//}
/*
 *                            +---------------------------------+
 *                            |                                 |
 *                            |				arg3			    | 
 *                            |                                 |
 *                            +---------------------------------+
 *                            |                                 |
 *                            |				arg2			    | 
 *                            |                                 |
 *                            +---------------------------------+ 
 *                            |                                 |
 *                            |				arg1			    | 
 *                            |                                 |
 *                            +---------------------------------+ 
 *                            |             list                |
 *                            +---------------------------------+ 
 *                            |           User stack            |
 *                            +---------------------------------+
*/

#endif