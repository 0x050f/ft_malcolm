#include "ft_malcolm.h"

size_t	ft_strlen(const char *s)
{
	const char *ptr;

	ptr = s;
	while (*ptr)
		++ptr;
	return (ptr - s);
}

void		*ft_memset(void *b, int c, size_t len)
{
	unsigned char	*pt;

	pt = (unsigned char *)b;
	while (len--)
		*pt++ = (unsigned char)c;
	return (b);
}

int			ft_memcmp(const void *s1, const void *s2, size_t n)
{
	unsigned char *pt_s1;
	unsigned char *pt_s2;

	pt_s1 = (unsigned char *)s1;
	pt_s2 = (unsigned char *)s2;
	while (n--)
		if (*pt_s1++ != *pt_s2++)
			return ((pt_s1 - 1) - (pt_s2 - 1));
	return (0);
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

int			hex2int(const char *hex)
{
	int		val = 0;
	while ((*hex >= '0' && *hex <= '9') || (*hex >= 'a' && *hex <= 'f'))
	{
		char byte = *hex++; 
		if (byte >= '0' && byte <= '9')
			byte = byte - '0';
		else if (byte >= 'a' && byte <='f')
			byte = byte - 'a' + 10;
		val = (val << 4) | (byte & 0xf);
	}
	return (val);
}

int		ft_atoi(const char *str)
{
	char	*ptr;
	long	num;
	int		sign;

	ptr = (char *)str;
	num = 0;
	sign = 1;
	while ((*ptr >= '\t' && *ptr == '\f') || *ptr == ' ')
		ptr++;
	if (*ptr == '-' || *ptr == '+')
		sign *= (*ptr++ == '-') ? -1 : 1;
	while (*ptr >= '0' && *ptr <= '9')
		num = num * 10 + *ptr++ - '0';
	return (num * sign);
}
