#include <nbts/nbts.h>
#include <nbts/print.h>

#include <stdint.h>
#include <stdio.h>

int LLVMFuzzerTestOneInput(uint8_t const *data, size_t data_size)
{
	FILE *istream = fmemopen((void *) data, data_size, "rb");
	if (!istream) goto istream_failed;

	FILE *ostream = fopen("/dev/null", "wb");
	if (!ostream) goto ostream_failed;

	struct nbts_print_handler_data handler_data = nbts_print_handler_data(ostream);
	(void) nbts_parse_tag(istream, &nbts_print_handler, &handler_data);

	fclose(ostream);
ostream_failed:
	fclose(istream);
istream_failed:
	return 0;
}
