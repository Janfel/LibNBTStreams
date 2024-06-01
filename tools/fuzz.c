#include <nbts/print.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t data_len) {
    // fmemopen isnt availbe for some reason so i just make a tempfile...
    FILE *tmp = tmpfile();
    if (!tmp) return 1;
    if (fwrite(Data, 1, data_len, tmp) != data_len) goto err;
    rewind(tmp);

    struct nbts_print_handler_data data = nbts_print_handler_data(stdout);
    if (nbts_parse_tag(tmp, &nbts_print_handler, &data)) goto err;
    if (fputc('\n', stdout) != 0) goto err;
    return 0;

err:
    fclose(tmp);
    return 1;
}