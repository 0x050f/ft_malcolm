#include "ft_malcolm.h"

size_t	ft_strlen(const char *s)
{
	const char *ptr;

	ptr = s;
	while (*ptr)
		++ptr;
	return (ptr - s);
}

void		*ft_memcpy(void *dst, const void *src, size_t n)
{
	char	*pt_src;
	char	*pt_dst;

	pt_src = (char *)src;
	pt_dst = (char *)dst;
	while (n--)
		*pt_dst++ = *pt_src++;
	return (dst);
}
