#include <jdeflate/deflator.h>
#include <jdeflate/inflator.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


const uintxx targetsize = 2 * 1024 * 102;


void
test(int32 level, const uint8* source, uintxx sourcesize)
{
	uint8* target;
	TDeflator* deflator;

	if (source == NULL || sourcesize == 0) {
		return;
	}

	deflator = deflator_create(0, level, NULL);
	if (deflator == NULL) {
		return;
	}

	target = malloc(targetsize);
	if (target == NULL) {
		deflator_destroy(deflator);
		return;
	}

	deflator_setsrc(deflator, source, sourcesize);
	deflator_settgt(deflator, target, targetsize);
	deflator_deflate(deflator, 1);
	if (deflator->status == DEFLT_OK) {
		uint8* buffer;
		TInflator* inflator;

		inflator = inflator_create(0, NULL);
		if (inflator == NULL) {
			goto L_DONE;
		}

		buffer = malloc(sourcesize);
		if (buffer == NULL) {
			inflator_destroy(inflator);
			goto L_DONE;
		}

		inflator_setsrc(inflator, target, deflator_tgtend(deflator));
		inflator_settgt(inflator, buffer, sourcesize);
		inflator_inflate(inflator, 1);
		if (inflator->status == INFLT_OK) {
			assert(memcmp(source, buffer, sourcesize) == 0);
		}
		inflator_destroy(inflator);
		free(buffer);
	}

L_DONE:
	free(target);
	deflator_destroy(deflator);
}

int
LLVMFuzzerTestOneInput(const uint8* source, uintxx sourcesize)
{
	test(0, source, sourcesize);
	test(1, source, sourcesize);
	test(5, source, sourcesize);
	test(9, source, sourcesize);
	return 0;
}
