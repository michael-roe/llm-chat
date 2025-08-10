import json

def sql_escape(str):
  for c in str:
    if c == '"':
      print("\\\"", end="")
    elif c == '\'':
      print("\\\'", end="")
    elif c == '\\':
      print("\\\\", end="")
    elif c == '\n':
      print("\\n", end="")
    elif c == '\r':
      print("\\r", end="")
    elif c == '\b':
      print("\\b", end="")
    elif c == '\t':
      print("\\t", end="")
    else:
      print(c, end="")

#
# I used the following field widths:
#
# media_type 255
# genre 255
# character_name 255
# media_source 255
# description 511
# scenario 767
#
# character_name cannot be a primary key because it contains values that differ
# only in case
#
# scenario contains newline characters
#
# various other fields contain single and double quotes
#

file = open('character_codex.json')

data = json.load(file)

print("USE character_codex;")

for rec in data:
  print("INSERT INTO train (media_type, genre, character_name, media_source,", end="")
  print(" description, scenario) VALUES ", end="")

  print("(", end="")

  print("'", end="")
  sql_escape(rec["media_type"])
  print("'", end="")

  print(", '", end="")
  sql_escape(rec["genre"])
  print("'", end="")

  print(", '", end="")
  sql_escape(rec["character_name"])
  print("'", end="")

  print(", '", end="")
  sql_escape(rec["media_source"])
  print("'", end="")

  print(", '", end="")
  sql_escape(rec["description"]);
  print("'", end="")

  print(", '", end="")
  sql_escape(rec["scenario"])
  print("'", end="")

  print(");")

