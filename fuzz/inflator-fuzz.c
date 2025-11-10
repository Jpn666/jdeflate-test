#include <jdeflate/inflator.h>
#include <stdlib.h>


const uintxx targetsize = 2 * 1024 * 102;

int
LLVMFuzzerTestOneInput(const uint8* source, uintxx sourcesize)
{
	uint8* target;
	TInflator* inflator;

	if (source == NULL || sourcesize == 0) {
		return 0;
	}

	inflator = inflator_create(0, NULL);
	if (inflator == NULL) {
		return 0;
	}

	target = malloc(targetsize);
	if (target == NULL) {
		inflator_destroy(inflator);
		return 0;
	}

	inflator_setsrc(inflator, source, sourcesize);
	inflator_settgt(inflator, target, targetsize);
	inflator_inflate(inflator, 1);
	free(target);

	inflator_destroy(inflator);
	return 0;
}
