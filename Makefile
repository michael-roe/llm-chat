
MYSQL_LIBS=-lmysqlclient -lz  -lzstd -lssl -lcrypto -lresolv -lm

all: llm3270 sba

llm3270: llm3270.c
	gcc -o llm3270 llm3270.c

sba: sba.c
	gcc -o sba sba.c

poetry: poetry.c
	gcc -o poetry poetry.c

xmltojson: xmltojson.c
	gcc -I /usr/include/libxml2 -o xmltojson xmltojson.c -lxml2

codex_search: codex_search.c
	gcc -o codex_search codex_search.c $(MYSQL_LIBS)
