#include <util.h>

#include <frees/stdarg.h>
#include <frees/string.h>

#include "kmesg.h"
#include "kprintf.h"

// REAL: доделать printf
// REAL: проверка переполнения result'а
// REAL: поддержка signed

// ARGS should content last ',', for example: 'FILE *__restrict__ stream,'

int vkprintf( const char *__restrict__ format, va_list ap){
	char buf[ITOA_SIZE]; /* for itoa */
	char *buf_end = buf + ITOA_SIZE - 1;

	size_t result = 0;

	for(; *format != 0;){
		if(*format == '%'){
			++format;


			/* % */

			if(*format == '%'){
				kmesg_fputc('%');
				++result;
				++format;
				continue;
			}


			/* Flags */

			char pad = ' ';

			for(;;){
				if(*format == '0'){
					pad = '0';
					++format;
				}else{
					break;
				}
			}


			/* Field width */

			int width = 0;

			if('0' <= *format && *format <= '9'){
				width = *format - '0';
				++format;
			}


			/* Length modifier */

			int mod = 0;

			if(*format == 'l'){
				mod = 1;
				++format;
			}

			if(*format == 'l'){
				mod = 2;
				++format;
			}


			/* Conversion specifier */

			switch(*format){
				case 's': /* REAL: flags and width? */
					{
						const char *str = va_arg(ap, const char *);
						size_t len = strlen(str);
						kmesg_write(str, len);
						result += len;
					}
					break;

				case 'c': /* REAL: flags and width? */
					{
						char arg = char(va_arg(ap, int));
						kmesg_fputc(arg);
						++result;
					}
					break;

				case 'o':
				case 'd':
				case 'x':
					{
						unsigned long long num;

						switch(mod){
							case 0:
								num = va_arg(ap, unsigned);
								break;
							case 1:
								num = va_arg(ap, unsigned long);
								break;
							case 2:
								num = va_arg(ap, unsigned long long);
								break;
						}

						unsigned base;

						switch(*format){
							case 'o':
								base = 8;
								break;
							case 'd':
								base = 10;
								break;
							case 'x':
								base = 16;
								break;
						}

						const char *str = duo_printf_itoa_pad(num, base, buf, pad, width);
						int len = buf_end - str;
						kmesg_write(str, len);
						result += len;
					}
					break;

				case '0':
				default:
					return result;
			}

			++format;
		}else{
			kmesg_fputc(*format);
			++result;
			++format;
		}
	}
	return result;
}

int kprintf( const char *__restrict__ format, ...){
	va_list ap;
	va_start(ap, format);
	int result = vkprintf( format, ap);
	va_end(ap);

	return result;
}

