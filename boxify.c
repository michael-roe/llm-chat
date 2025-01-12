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

