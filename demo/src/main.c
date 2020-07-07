#include <stdio.h>
#include <evol/evol.h>

int
main(int argc, char **argv)
{
  evolengine_t *evol = evol_create();

  evol_parse_args(evol, argc, argv);

  evol_init(evol);

  evolmodule_t rendering_module = evol_loadmodule("Rendering");

  int (*add_fn)(int, int) =
    (int (*)(int, int))evol_getmodfunc(rendering_module, "mod1_add");

  if (add_fn) {
    printf("addition of 1 + 2 = %d\n", add_fn(1, 2));
  }

  if (rendering_module) {
    evol_unloadmodule(rendering_module);
  }

  evol_deinit(evol);

  evol_destroy(evol);

  return 0;
}
