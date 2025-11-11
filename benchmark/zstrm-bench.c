#include <ctoolbox/ctoolbox.h>
#include <ctoolbox/str2int.h>
#include <jdeflate/zstrm.h>
#if defined(__MSVC__)
	#include <windows.h>
#else
	#include <time.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


uintxx
decompress(const TZStrm* state, uint8* src, uintxx n, uint8* tgt, uintxx block)
{
	uintxx j;

	j = 0;
	zstrm_setsource(state, src, n);
	do {
		j += zstrm_inflate(state, tgt, block);
	} while (state->state != ZSTRM_END);

	if (state->error == 0) {
		return j;
	}
	return 0;
}

struct TOutput {
	uintxx total;
};

static intxx
wcallback(const uint8* buffer, uintxx size, void* user)
{
	struct TOutput* out;

	(void) buffer;
	(void) user;
	out = user;
	out->total += size;
	return (intxx) size;
}

uintxx
compress(const TZStrm* state, uint8* src, uintxx n, uintxx block)
{
	uintxx j;
	struct TOutput user;

	user.total = 0;
	j = 0;
	zstrm_settargetfn(state, wcallback, &user);
	do {
		uintxx a;

		a = block;
		if (a > n) {
			a = n;
		}
		j = zstrm_deflate(state, src, a);
		if (j == 0) {
			zstrm_flush(state, 1);
		}
		src += j; n -= j;
	} while (state->state != ZSTRM_END);

	if (state->error == 0) {
		return user.total;
	}
	return 0;
}

#if defined(__MSVC__)
	#pragma warning(disable: 4996)
#endif

unsigned int
readinto(const char* fpath, unsigned char** buffer)
{
	FILE* handler;
	uintxx n;

	handler = fopen(fpath, "rb");
	if (handler == NULL) {
		return 0;
	}

	fseek(handler, 0, SEEK_END);
	n = (uintxx) ftell(handler);
	fseek(handler, 0, SEEK_SET);
	buffer[0] = NULL;
	if (n == 0) {
		goto L_ERROR;
	}

	buffer[0] = malloc(n);
	if (buffer[0] == NULL) {
		goto L_ERROR;
	}
	if (fread(buffer[0], 1, n, handler) ^ n) {
		goto L_ERROR;
	}
	fclose(handler);
	return (unsigned int) n;

L_ERROR:
	fclose(handler);
	return 0;
}

#if defined(__MSVC__)

CTB_INLINE double
getelapseptime(uint32 tbgn, uint32 tend)
{
	double bgns;
	double ends;
	
	bgns = (double) tbgn / 1000.0;
	ends = (double) tend / 1000.0;
	return ends - bgns;
}

#else

CTB_INLINE double
getelapseptime(struct timespec tbgb, struct timespec tend)
{
	double bgns;
	double ends;

	bgns = (double) tbgb.tv_sec + (double) tbgb.tv_nsec / 1000000000.0;
	ends = (double) tend.tv_sec + (double) tend.tv_nsec / 1000000000.0;
	return ends - bgns;
}

#endif


int
main(int argc, char* argv[])
{
	uint8* input;
	uint8* block;
	uintxx n;
	uintxx j;
	int32 level;
	uintxx factor;
	uint32 r;
	uintxx mode;
#if defined(__MSVC__)
	uint32 tbgn;
	uint32 tend;
#else
	struct timespec tbgn;
	struct timespec tend;
#endif
	double e;
	TToIntResult result;
	const TZStrm* zstrm;

	if (argc != 4 && argc != 5) {
		puts("Usage: thisprogram deflate <level> <blocksize-log-2> <input>");
		puts("Usage: thisprogram inflate <blocksize-log-2> <input>");
		return 0;
	}

	mode = 0;
	if (strcmp(argv[1], "deflate") == 0) {
		mode = 1;
		if (argc != 5) {
			puts("bad deflate parameters");
			return 0;
		}
	}
	if (strcmp(argv[1], "inflate") == 0) {
		mode = 2;
	}
	if (mode == 0) {
		puts("invalid mode (inflate or deflate)");
		return 0;
	}

	j = 2;
	level = 0;
	if (mode == 1) {
		result = strtoi32((void*) argv[j], NULL, 10);
		if (result.error) {
			puts("invalid compression level");
			return 0;
		}
		level = (int32) result.value.asi32;
		if (level < 0 || level > 9) {
			puts("invalid compression level");
			return 0;
		}
		j++;
	}

	result = strtou32((void*) argv[j], NULL, 10);
	if (result.error) {
		puts("invalid block size factor");
		return 0;
	}
	factor = (uintxx) result.value.asu32;
	if (factor > 28) {
		puts("invalid block size factor");
		return 0;
	}
	factor = (uintxx) 1u << factor;
	j++;

	n = readinto(argv[j], &input);
	if (n == 0) {
		puts("failed to read file");
		return 0;
	}

	if (mode == 2) {
		zstrm = zstrm_create(ZSTRM_INFLATE, 0, NULL);
		if (zstrm == NULL) {
			puts("failed to create zstrm");
			goto L_ERROR;
		}

		block = malloc(factor);
		if (block == NULL) {
			zstrm_destroy(zstrm);
			goto L_ERROR;
		}

#if defined(__MSVC__)
		tbgn = GetTickCount();
		r = (uint32) decompress(zstrm, input, n, block, factor);
		tend = GetTickCount();
#else
		clock_gettime(CLOCK_REALTIME, &tbgn);
		r = (uint32) decompress(zstrm, input, n, block, factor);
		clock_gettime(CLOCK_REALTIME, &tend);
#endif
		if (r) {
			e = getelapseptime(tbgn, tend);
			if (r) {
				printf("seconds:%.8f bytes:%u", e, r);
				putchar(0x0a);
			}
		}
		free(block);
		zstrm_destroy(zstrm);
	}

	if (mode == 1) {
		uint32 f;

		f = (uint32) ZSTRM_DEFLATE | (uint32) ZSTRM_DFLT;
		zstrm = zstrm_create(f, level, NULL);
		if (zstrm == NULL) {
			puts("failed to create zstrm");
			goto L_ERROR;
		}

#if defined(__MSVC__)
		tbgn = GetTickCount();
		r = (uint32) compress(zstrm, input, n, factor);
		tend = GetTickCount();
#else
		clock_gettime(CLOCK_REALTIME, &tbgn);
		r = (uint32) compress(zstrm, input, n, factor);
		clock_gettime(CLOCK_REALTIME, &tend);
#endif
		if (r) {
			e = getelapseptime(tbgn, tend);
			if (r) {
				printf("seconds:%.8f bytes:%u", e, r);
				putchar(0x0a);
			}
		}
		zstrm_destroy(zstrm);
	}

L_ERROR:
	free(input);
	return 0;
}
