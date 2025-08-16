/*
 * Check the characters in a string are permitted in  Character Codex.
 *
 * The recommended way to prevent SQL injection attacks is to use prepared
 * statements, or failing that, use mysql_real_escape_string() to escape
 * special characters.
 *
 * As a matter of "defense in depth", we are going to define a business rule
 * that a field must not contain certain SQL special characters (even though
 * they would be escaped).
 *
 * Unfortunately, character_name in Character Codex can legitimately contain
 * a single or double quote, so we really do have to use the recommended
 * method to escape these. In addition, the scenario field can contain
 * new line characters.
 *
 * Underscore, Percent, Backslash, Tab are prohibited.
 */

#include <stdio.h>
#include <wchar.h>
#include "codex.h"

int codex_check_prohibited(wchar_t *str)
{
int result;
wchar_t *cp;

  cp = str;
  while (*cp)
  {
    if (*cp == '\\')
    {
      return 1;
    }
    else if (*cp == '\b')
    {
      return 1;
    }
    else if (*cp == '\t')
    {
      return 1;
    }
    else if (*cp == '%')
    {
      return 1;
    }
    else if (*cp == '_')
    {
      return 1;
    }
    else
    {
      cp++;
    }
  }
  return 0;
}

