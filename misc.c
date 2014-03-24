#include <stdio.h>
#include <stdarg.h>

#include "misc.h"

#ifdef _DEBUG
void DBG(const char *fmt, ...)
{
	va_list	ap;
	va_start(ap, fmt);
	vprintf(fmt, ap);
	va_end(ap);
	printf("\n");
}
#else
void DBG(const char *fmt, ...)
{
	/* make compiler happy */
	fmt = NULL ;
}
#endif

