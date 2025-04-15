import os
import cp310

for key, value in cp310.from_unicode.items():
  print(hex(value), end="")
  print(" ", end="")
  print(key)

print()

tr = {}

for i in range(256):
  tr[i] = " "

for key, value in cp310.from_unicode.items():
  tr[value] = key

for i in range(256):
  print(hex(ord(tr[i])), end="")
  print(", ", end="")
  if i % 8 == 7:
    print()
