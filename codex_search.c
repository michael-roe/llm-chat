#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  char *user;
  char *password;

  conn = mysql_init(NULL);
  if (conn == NULL)
  {
      fprintf(stderr, "mysql_init() failed\n");
      exit(1);
  }

  user = getenv("USER");
  password = getenv("MYSQL_PASS");

  if (mysql_real_connect(conn, "localhost", user, password, "character_codex",
    0, NULL, 0) == NULL)
  {
    fprintf(stderr, "mysql_real_connect failed\n");
    exit(-1);
  }

  if (mysql_query(conn,
    "SELECT character_name, description FROM train WHERE character_name LIKE 'Alana'"))
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

  while ((row = mysql_fetch_row(res)) != NULL)
  {
    printf("%s\n\n%s\n", 
      row[0] ? row[0] : "NULL",
      row[1] ? row[1] : "NULL");
  }
  
  mysql_close(conn);
  
  return 0;
}

