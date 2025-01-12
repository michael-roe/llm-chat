#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>

#include "boxify.h"

static void callback(wchar_t *str)
{
  fwprintf(stderr, L"%ls\n", str);
}

int main(int argc, char **argv)
{
wchar_t buff[256];
int len;

  setlocale(LC_ALL, getenv("LANG"));

  boxify_start(79);
  while (fgetws(buff, sizeof(buff) - 1, stdin))
  {
    len = wcslen(buff);
    while ((len > 0) && (buff[len - 1] == '\n'))
    {
      buff[len - 1] = 0;
      len--;
    }
    
    boxify_line(callback, buff);
  }
  boxify_end(callback);

  return 0;
}
