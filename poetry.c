/*
 * poetry.c
 *
 * Check that the input file follows the metrical rules for Sanskit poetry.
 *
 * This is a prototype for an evaluation function to be used in reinforcement
 * learning.
 *
 * Reject the input if:
 *  - It contains characters outside ISO Latin 1 + Sanskrit transliteration
 *  - A line isn't empty, isn't "such and such a person says", and doesn't
 *    contain exactly 16 vowels.
 *
 * This currently doesn't check the pattern of long and short syllables,
 * because the rules are complicated...
 * - a long vowel makes the syllable long
 * - more than one consonant makes the syllable long
 * - anusvara makes the syllable long
 * - visarga makes the syllable long
 */

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>

#define A_MACRON 0x0101
#define E_MACRON 0x0113
#define I_MACRON 0x012b
#define O_MACRON 0x014d
#define U_MACRON 0x016b

#define R_DOT 0x1e5b
#define R_DOT_MACRON 0x1e5d

#define D_DOT 0x1e0d
#define H_DOT 0x1e25
#define M_DOT 0x1e43
#define N_DOT 0x1e47
#define S_DOT 0x1e63
#define T_DOT 0x1e6d

#define N_DOT_ABOVE 0x1e45

#define S_ACUTE 0x015b

#define DANDA 0x0964
#define DOUBLE_DANDA 0x0965

static wchar_t repertoire[] = {
  A_MACRON, I_MACRON, U_MACRON, R_DOT, R_DOT_MACRON,
  D_DOT, H_DOT, M_DOT, N_DOT, S_DOT, T_DOT,
  N_DOT_ABOVE, S_ACUTE, DANDA, DOUBLE_DANDA};

static wchar_t short_vowels[] = {
  'a', 'i', 'u', R_DOT};

static wchar_t long_vowels[] = {
  A_MACRON, I_MACRON, U_MACRON, 'e', 'o', R_DOT_MACRON};

static wchar_t vowels[] = {
  'a', A_MACRON, 'i', I_MACRON, 'u', U_MACRON, 'e', 'o',
  R_DOT, R_DOT_MACRON};

int main(int argc, char **argv)
{
wchar_t buff[1024];
int len;
int i;
wchar_t *cp;
int count;

  setlocale(LC_ALL, getenv("LANG"));

  while (!feof(stdin))
  {
    fgetws(buff, sizeof(buff), stdin);
    len = wcslen(buff);
    if ((len > 0) && (buff[len - 1] == '\n'))
    {
      buff[len - 1] = 0;
      len--;
    }

    /*
     * ISO 15919 has some minor differences from IAST.
     * Here we convert some of the ISO 15919 characters to IAST.
     */

    cp = buff;
    while (*cp)
    {
      if (*cp == E_MACRON)
        *cp = 'e';
      else if (*cp == O_MACRON)
        *cp = 'o';
#if 1
      else if ((*cp > 255) && (wcschr(repertoire, *cp) == 0))
        fwprintf(stderr, L"%04x %lc\n", *cp, *cp);
#endif
      cp++;
    }
    wprintf(L"%ls\n", buff);
    fflush(stdout);
    count = 0;
    cp = buff;
    while (*cp)
    {
      if (wcsncmp(cp, L"ai", 2) == 0)
      {
        count++;
        wprintf(L"* ");
        cp++;
      }
      else if (wcsncmp(cp, L"au", 2) == 0)
      {
        count++;
        wprintf(L"* ");
        cp++;
      }
      else if (wcschr(long_vowels, *cp))
      {
        count++;
        wprintf(L"*");
      }
      else if (wcschr(short_vowels, *cp))
      {
        count++;
        wprintf(L"-");
      }
      else
        wprintf(L" ");
      cp++;
    } 
    wprintf(L" (%d) \n", count);
    if ((count != 0) && (count != 16))
    {
      fwprintf(stderr, L"%ls\n", buff);
    }
  }
  return 0;
}

