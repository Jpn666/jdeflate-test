#include <ctoolbox/ctoolbox.h>
#include <ctoolbox/str2int.h>
#include <jdeflate/inflator.h>
#if defined(__MSVC__)
	#include <windows.h>
#else
	#include <time.h>
#endif
#include <stdio.h>
#include <stdlib.h>


uint32
decompress(TInflator* state, uint8* src, uintxx n, uint8* buffer, uintxx block)
{
	uintxx result;
	uint32 j;

	j = 0;
	inflator_setsrc(state, src, n);
	do {
		do {
			inflator_settgt(state, buffer, block);
			result = inflator_inflate(state, 1);

			j += (uint32) inflator_tgtend(state);
		} while (result == INFLT_TGTEXHSTD);
	} while (result == INFLT_SRCEXHSTD);

	if (result == INFLT_OK) {
		return j;
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
	uintxx factor;
	uint32 r;
#if defined(__MSVC__)
	uint32 tbgn;
	uint32 tend;
#else
	struct timespec tbgn;
	struct timespec tend;
#endif
	double e;
	TToIntResult result;
	TInflator* inflator;

	if (argc != 3) {
		puts("Usage: thisprogram blocksize-log-2 <input file>");
		return 0;
	}

	result = strtou32((void*) argv[1], NULL, 10);
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

	n = readinto(argv[2], &input);
	if (n == 0) {
		puts("failed to read file");
		return 0;
	}
	block = NULL;

	inflator = inflator_create(0, NULL);
	if (inflator == NULL) {
		puts("failed to create inflator");
		goto L_ERROR;
	}

	block = malloc(factor);
	if (block == NULL) {
		goto L_ERROR;
	}

#if defined(__MSVC__)
	tbgn = GetTickCount();
	r = decompress(inflator, input, n, block, factor);
	tend = GetTickCount();
#else
	clock_gettime(CLOCK_REALTIME, &tbgn);
	r = decompress(inflator, input, n, block, factor);
	clock_gettime(CLOCK_REALTIME, &tend);
#endif
	if (r) {
		e = getelapseptime(tbgn, tend);
		if (r) {
			printf("seconds:%.8f bytes:%u", e, r);
			putchar(0x0a);
		}
	}
	else {
		puts("stream error");
	}

L_ERROR:
	if (inflator) {
		inflator_destroy(inflator);
	}
	free(input);
	free(block);
	return 0;
}
