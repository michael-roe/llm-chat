#include <stdio.h>
#include <stddef.h>
#include <wchar.h>

#include "boxify.h"

/*
 * Takes an input text containing BBCode formatting and outpys a result
 * that uses Unicode box-drawing characters.
 *
 * blank line - ends the current paragraph
 *
 * The start tags of paragraph level elements ([box], [ul], [ol], etc.)
 * end the current paragraph. It is an error for there to be any text on
 * the line (apart from other paragraph level start tags) before a
 * paragraph level start tag.
 *
 * The paragraph level end tags ([/box], [/ul], [/ol] etc.) end the current
 * paragraph. It is an error for there to be any text on the line (apart from
 * other paragraph level end tags) after a paragraph level end tag.
 *
 * [hr] ends the current paragraph and draws a horizontal line between the
 * sides of the innermost enclosing box. It is an error for there to be any
 * text on the line before or after an [hr] tag.
 *
 * [box][/box] draws a box around a sequence of paragraphs. Different styles
 * of box are available with [box=heavy].
 *
 * [li][/li] draws an element of a list.
 *
 * [ul][/li] draws an unordered list. Different styles of unordered list
 * are available.
 *
 * [ol][/ol] draws an ordered list.
 *
 * [b] and [i] are inline element. They can be preceded and/or followed by
 * text on the same line.
 *
 * [b][/b] draws text in a bold face font.
 *
 * [i][/i] draws text in an italic font.
 */

static int text_width;

wchar_t buff[128];
wchar_t *out_ptr;

void boxify_start(int width)
{
  text_width = width;
  out_ptr = buff;
}

void flush_para(wchar_t *buff, wchar_t **ptr, void (*callback)(wchar_t *str))
{
  if (*ptr != buff)
  {
    **ptr = '\0';
    callback(buff);
    *ptr = buff;
    **ptr = '\0';
  }
}

void do_hr(wchar_t *buff, wchar_t **ptr, void (*callback)(wchar_t *str))
{
int i;

  flush_para(buff, ptr, callback);

  (*ptr)[0] = (wchar_t) '+';
  for (i=1; i<text_width - 1; i++)
  {
    (*ptr)[i] = (wchar_t) '-';
  }
  (*ptr)[text_width - 1] = '+';
  (*ptr)[text_width] = '\0';
  (*callback)(buff);
  *ptr = buff;
}

void do_box(wchar_t *buff, wchar_t **ptr, void (*callback)(wchar_t *str))
{
  do_hr(buff, ptr, callback);
}

void do_end_box(wchar_t *buff, wchar_t **ptr, void (*callback)(wchar_t *str))
{
  do_hr(buff, ptr, callback);
}

void boxify_line(void (*callback)(wchar_t *str), wchar_t *str)
{
wchar_t *in_ptr;

  in_ptr = str;

  /*
   * If the input line is completely blank, flush any remaining text and
   * add a blank line.
   */
 
  if (*in_ptr == '\0')
  {
    flush_para(buff, &out_ptr, callback);
    (*callback)(buff);
  }

  while (*in_ptr)
  {
    if (wcsncmp(in_ptr, L"[box]", 5) == 0)
    {
      in_ptr += 5;
      do_box(buff, &out_ptr, callback);
    }
    else if (wcsncmp(in_ptr, L"[/box]", 6) == 0)
    {
      in_ptr += 6;
      do_end_box(buff, &out_ptr, callback);
    }
    else if (wcsncmp(in_ptr, L"[ul]", 4) == 0)
    {
      in_ptr += 4;
      flush_para(buff, &out_ptr, callback);
    }
    else if (wcsncmp(in_ptr, L"[/ul]", 5) == 0)
    {
      in_ptr += 5;
      flush_para(buff, &out_ptr, callback);
    }
    else if (wcsncmp(in_ptr, L"[li]", 4) == 0)
    {
      in_ptr += 4;
      flush_para(buff, &out_ptr, callback);
      *out_ptr = L'*';
      out_ptr++;
      *out_ptr = L' ';
      out_ptr++;
    }
    else if (wcsncmp(in_ptr, L"[/li]", 5) == 0)
    {
      in_ptr += 5;
      flush_para(buff, &out_ptr, callback);
    }
    else if (wcsncmp(in_ptr, L"[hr]", 4) == 0)
    {
      in_ptr += 4;
      do_hr(buff, &out_ptr, callback);
    }
    else
    {
      *out_ptr = *in_ptr;
      in_ptr++;
      out_ptr++;
    }
  }
  if (out_ptr != buff)
  {
    *out_ptr = L' ';
    out_ptr++;
  }
}

void boxify_end(void (*callback)(wchar_t *str))
{
  flush_para(buff, &out_ptr, callback);
}
