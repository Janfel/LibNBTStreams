#include <nbts/nbts.h>
#include <nbts/print.h>

#include <stdio.h>

int main()
{
	int err = 0;

	struct nbts_print_handler_data data = nbts_print_handler_data(stdout);
	if ((err = nbts_parse_tag(stdin, &nbts_print_handler, &data))) goto end;
	if ((err = fputc('\n', stdout))) goto end;

end:
	return err;
}
