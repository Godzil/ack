#include <lib.h>
#define chdir	_chdir

int chdir(name)
char *name;
{
  return(_callm3(FS, CHDIR, 0, name));
}
