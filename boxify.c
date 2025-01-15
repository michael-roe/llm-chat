#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
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
 *
 * [osb] draws [.
 *
 * [csb] draws ].
 */

static int text_width;

#define STACK_SIZE 20

#define STACK_NULL 0
#define STACK_BOX  1
#define STACK_LI   2
#define STACK_LIST 3
#define STACK_CODE 4

struct stack_item {
  int element;
  int width;
};

static int stack_count;

static struct stack_item boxify_stack[STACK_SIZE];

static void boxify_push(int element, int new_width)
{
  if (stack_count >= STACK_SIZE)
  {
    fwprintf(stderr, L"Stack overflow\n");
    exit(-1);
  }
  boxify_stack[stack_count].element = element;
  boxify_stack[stack_count].width = text_width;
  text_width = new_width;
  stack_count++;
}

static void boxify_pop()
{
  if (stack_count == 0)
  {
    fwprintf(stderr, L"Stack underflow\n");
    exit(-1);
  }
  stack_count--;
  text_width = boxify_stack[stack_count].width;
}
 
static int text_in_buffer;

wchar_t buff[128];
wchar_t *out_ptr;

void boxify_start(int width)
{
  text_width = width;
  out_ptr = buff;
  text_in_buffer = 0;
  stack_count = 0;
  boxify_push(STACK_NULL, width);
}

void line_prefix(wchar_t *buff, wchar_t **ptr, int internal)
{
int i;
int to_do;

  to_do = stack_count - internal;

  if (0)
  {
    **ptr = '0' + internal;
    (*ptr)++;
  }

  for (i=0; i<to_do; i++)
  {
    if (boxify_stack[i].element == STACK_BOX)
    {
      **ptr = '|';
      (*ptr)++;
      **ptr = ' ';
      (*ptr)++;
    }
  }
}

void flush_para(wchar_t *buff, wchar_t **ptr, void (*callback)(wchar_t *str),
  int internal)
{
  if (text_in_buffer)
  {
    **ptr = '\0';
    callback(buff);
    *ptr = buff;
    line_prefix(buff, ptr, internal);
    text_in_buffer = 0;
  }
}

void do_hr(wchar_t *buff, wchar_t **ptr, void (*callback)(wchar_t *str),
  int internal)
{
int i;
int width;

  if (internal && (stack_count > 0))
  {
    width = boxify_stack[stack_count - 1].width;
  }
  else
  {
    width = text_width;
  }

  flush_para(buff, ptr, callback, internal);

  **ptr = (wchar_t) '+';
  (*ptr)++;
  for (i=1; i<width - 1; i++)
  {
    **ptr = (wchar_t) '-';
    (*ptr)++;
  }
  **ptr = '+';
  (*ptr)++;
  **ptr = '\0';
  text_in_buffer = 1;
  flush_para(buff, ptr, callback, 0);
}

void do_box(wchar_t *buff, wchar_t **ptr, void (*callback)(wchar_t *str))
{
  boxify_push(STACK_BOX, text_width - 4);
  do_hr(buff, ptr, callback, 1);
}

void do_end_box(wchar_t *buff, wchar_t **ptr, void (*callback)(wchar_t *str))
{
  boxify_pop();
  do_hr(buff, ptr, callback, 0);
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
    flush_para(buff, &out_ptr, callback, 0);
    /*
     * Force a blank line to be output by setting text_in_buffer,
     * even though the buffer is empty.
     */
    text_in_buffer = 1;
    flush_para(buff, &out_ptr, callback, 0);
  }

  while (*in_ptr)
  {
    if (wcsncmp(in_ptr, L"[/code]", 7) == 0)
    {
      in_ptr += 7;
      flush_para(buff, &out_ptr, callback, 0);
      boxify_pop();
    }
    else if (wcsncmp(in_ptr, L"[osb]", 5) == 0)
    {
      *out_ptr = L'[';
      in_ptr += 5;
      out_ptr++;
      text_in_buffer = 1;
    }
    else if (wcsncmp(in_ptr, L"[csb]", 5) == 0)
    {
      *out_ptr = L']';
      out_ptr++;
      in_ptr += 5;
      text_in_buffer = 1;
    }
    else if ((boxify_stack[stack_count - 1].element == STACK_CODE) &&
      (*in_ptr == L'['))
    {
      /*
       * Inside a [code] block, open square bracket acts as just the
       * character rather than the start of a tag, unless it's either
       * part of [/code], [osb] or [csb].
       */
      *out_ptr = *in_ptr;
      in_ptr++;
      out_ptr++;
      text_in_buffer = 1;
    }
    else if (wcsncmp(in_ptr, L"[box]", 5) == 0)
    {
      in_ptr += 5;
      do_box(buff, &out_ptr, callback);
    }
    else if (wcsncmp(in_ptr, L"[/box]", 6) == 0)
    {
      in_ptr += 6;
      do_end_box(buff, &out_ptr, callback);
    }
    else if (wcsncmp(in_ptr, L"[code]", 6) == 0)
    {
      in_ptr += 6;
      flush_para(buff, &out_ptr, callback, 0);
      boxify_push(STACK_CODE, text_width);
    }
    else if (wcsncmp(in_ptr, L"[ul]", 4) == 0)
    {
      in_ptr += 4;
      flush_para(buff, &out_ptr, callback, 0);
    }
    else if (wcsncmp(in_ptr, L"[/ul]", 5) == 0)
    {
      in_ptr += 5;
      flush_para(buff, &out_ptr, callback, 0);
    }
    else if (wcsncmp(in_ptr, L"[ol]", 4) == 0)
    {
      in_ptr += 4;
      flush_para(buff, &out_ptr, callback, 0);
    }
    else if (wcsncmp(in_ptr, L"[/ol]", 5) == 0)
    {
      in_ptr += 5;
      flush_para(buff, &out_ptr, callback, 0);
    }
    else if (wcsncmp(in_ptr, L"[list]", 6) == 0)
    {
      in_ptr += 6;
      flush_para(buff, &out_ptr, callback, 0);
    }
    else if (wcsncmp(in_ptr, L"[/list]", 7) == 0)
    {
      in_ptr += 7;
      flush_para(buff, &out_ptr, callback, 0);
    }
    else if (wcsncmp(in_ptr, L"[li]", 4) == 0)
    {
      in_ptr += 4;
      flush_para(buff, &out_ptr, callback, 0);
      *out_ptr = 0x2022;
      out_ptr++;
      *out_ptr = L' ';
      out_ptr++;
      text_in_buffer = 1;
      boxify_push(STACK_LI, text_width);
    }
    else if (wcsncmp(in_ptr, L"[/li]", 5) == 0)
    {
      in_ptr += 5;
      boxify_pop();
      flush_para(buff, &out_ptr, callback, 0);
    }
    else if (wcsncmp(in_ptr, L"[hr]", 4) == 0)
    {
      in_ptr += 4;
      do_hr(buff, &out_ptr, callback, 1);
    }
    else if (wcsncmp(in_ptr, L"[i]", 3) == 0)
    {
      in_ptr += 3;
    }
    else if (wcsncmp(in_ptr, L"[/i]", 4) == 0)
    {
      in_ptr += 4;
    }
    else if (wcsncmp(in_ptr, L"[b]", 3) == 0)
    {
      in_ptr += 3;
    }
    else if (wcsncmp(in_ptr, L"[/b]", 4) == 0)
    {
      in_ptr += 4;
    }
    else if (wcsncmp(in_ptr, L"[u]", 3) == 0)
    {
      in_ptr += 3;
    }
    else if (wcsncmp(in_ptr, L"[/u]", 4) == 0)
    {
      in_ptr += 4;
    }
    else
    {
      *out_ptr = *in_ptr;
      in_ptr++;
      out_ptr++;
      text_in_buffer = 1;
    }
  }
  if (text_in_buffer)
  {
    if (boxify_stack[stack_count - 1].element == STACK_CODE)
    {
      /*
       * Inside a [code] block, a line break in the input results in a line
       * break in the output.
       */
      flush_para(buff, &out_ptr, callback, 0);
    }
    else
    {
      /*
       * Otherwise, new line is treated as white space.
       */
      *out_ptr = L' ';
      out_ptr++;
    }
  }
}

void boxify_end(void (*callback)(wchar_t *str))
{
  flush_para(buff, &out_ptr, callback, 0);
}
