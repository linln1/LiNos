#ifndef __DEBUG_KDEBUG_H__
#define __DEBUG_KDEBUG_H__

#include <../libs/ctype.h>

void print_kerninfo(void);
void print_stackframe(void);
void print_debuginfo(uintptr_t eip);

#endif /* !__KERN_DEBUG_KDEBUG_H__ */
