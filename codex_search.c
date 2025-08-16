#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>
#include "codex.h"

static wchar_t name_wcs[256];
static char escaped[256];
static char query[512];

int main(int argc, char **argv)
{
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char *user;
  char *password;
  int not_first;
  int opt;
  int json_mode;

  setlocale(LC_ALL, getenv("LANG"));

  json_mode = 0;

  while ((opt = getopt(argc, argv, "j")) != -1)
  {
    switch (opt)
    {
      case 'j':
        json_mode = 1;
        break;
      default:
        fprintf(stderr, "Usage: codex_search [-j] <name>\n");
        exit(-1);
    }
  }

  if (optind >= argc) 
  {
    fprintf(stderr, "Usage: codex_search [-j] <name>\n");
    exit(-1);
  }


  if (2*strlen(argv[optind]) + 1 > sizeof(escaped))
  {
    fprintf(stderr, "Name is too long.\n");
    exit(-1);
  }

  mbstowcs(name_wcs, argv[optind], sizeof(name_wcs));

  /*
   * Check that the name complies with the business rules for the database.
   * This is not the same as escaping SQL special characters, which we have
   * do as well, especially as character_name can legitimately contain a
   * single (or double) quote.
   */

  if (codex_check_prohibited(name_wcs))
  {
    fprintf(stderr, "Name contains a prohibited character.\n");
    exit (-1);
  }


  conn = mysql_init(NULL);
  if (conn == NULL)
  {
      fprintf(stderr, "mysql_init() failed\n");
      exit(1);
  }

  user = getenv("USER");
  password = getenv("MYSQL_PASS");

  if (password == NULL)
  {
    fprintf(stderr, "MYSQL_PASS not set\n");
    exit(-1);
  }

  if (mysql_real_connect(conn, "localhost", user, password, "character_codex",
    0, NULL, 0) == NULL)
  {
    fprintf(stderr, "mysql_real_connect failed\n");
    exit(-1);
  }

  /*
   * To prevent SQL injection attacks, we ned to escape special characters.
   * mysql_real_escape_string_quote doesn't check the length of the buffer.
   * so the buffer must be large enough to prevent buffer overflow attacks.
   */

  mysql_real_escape_string_quote(conn, escaped, argv[optind],
    strlen(argv[optind]), '\'');

  snprintf(query, sizeof(query),
    "SELECT character_name, description FROM train WHERE character_name LIKE '%%%s%%';",
    escaped);

  if (mysql_query(conn, query))
  {
    fprintf(stderr, "Query error: %s\n", mysql_error(conn));
    mysql_close(conn);
    exit(-1);
  }
    
  res = mysql_store_result(conn);
  if (res == NULL)
  {
    fprintf(stderr, "Result error: %s\n", mysql_error(conn));
    mysql_close(conn);
    exit(-1);
  }

  if (json_mode)
  {
    printf("[");
  }

  not_first = 0;
  while ((row = mysql_fetch_row(res)) != NULL)
  {
    if (not_first)
    {
      if (json_mode)
      {
        printf(", ");
      }
      else
      {
        printf("\n");
      }
    }
    else
    {
      not_first = 1;
    }

    if (json_mode)
    {
      /*
       * We need to escape JSON special characters here, especially quotes.
       */

      printf("{");
      if (row[0])
      {
        printf("\"character_name\":\"%s\", ", row[0]);
      }
      if (row[1])
      {
        printf("\"description\":\"%s\"", row[1]);
      }
      printf("}");
    }
    else
    {
      printf("%s\n\n", row[0] ? row[0] : "NULL");
      printf("%s\n", row[1] ? row[1] : "NULL");
    }
  }

  if (json_mode)
  {
    printf("]\n");
  }
  
  mysql_close(conn);
  
  return 0;
}

