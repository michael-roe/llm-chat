#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

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

  setlocale(LC_ALL, getenv("LANG"));

  if (argc < 2)
  {
    fprintf(stderr, "Usage: codex_search <name>\n");
    exit(-1);
  }

  if (2*strlen(argv[1]) + 1 > sizeof(escaped))
  {
    fprintf(stderr, "Name is too long.\n");
    exit(-1);
  }

  mbstowcs(name_wcs, argv[1], sizeof(name_wcs));

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

  mysql_real_escape_string_quote(conn, escaped, argv[1],
    strlen(argv[1]), '\'');

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

  not_first = 0;
  while ((row = mysql_fetch_row(res)) != NULL)
  {
    if (not_first)
    {
      printf("\n");
    }
    else
    {
      not_first = 1;
    }
    printf("%s\n\n%s\n", 
      row[0] ? row[0] : "NULL",
      row[1] ? row[1] : "NULL");
  }
  
  mysql_close(conn);
  
  return 0;
}

