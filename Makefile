
all: llm3270 sba

llm3270: llm3270.c
	gcc -o llm3270 llm3270.c

sba: sba.c
	gcc -o sba sba.c

poetry: poetry.c
	gcc -o poetry poetry.c
