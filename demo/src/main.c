#include <stdio.h>
#include <evol/evol.h>

int
main(int argc, char **argv)
{
  evolengine_t *evol = evol_create();

  evol_parse_args(evol, argc, argv);

  evol_init(evol);

  evol_deinit(evol);

  evol_destroy(evol);

  return 0;
}
