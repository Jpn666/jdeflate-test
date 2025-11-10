#include <jdeflate/zstrm.h>
#include <stdlib.h>


const uintxx targetsize = 2 * 1024 * 102;

int
LLVMFuzzerTestOneInput(const uint8* source, uintxx sourcesize)
{
	uint8* target;
	const TZStrm* zstrm;

	if (source == NULL || sourcesize == 0) {
		return 0;
	}

	zstrm = zstrm_create(ZSTRM_INFLATE, 0, NULL);
	if (zstrm == NULL) {
		return 0;
	}

	target = malloc(targetsize);
	if (target == NULL) {
		zstrm_destroy(zstrm);
		return 0;
	}

	zstrm_setsource(zstrm, source, sourcesize);
	zstrm_inflate(zstrm, target, targetsize);
	free(target);

	zstrm_destroy(zstrm);
	return 0;
}
