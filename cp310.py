# IBM Code Page 310

# This table omits characters that are in ISO-Latin-1.
# The idea is that these characters will be mapped to the EBCDIC
# code page that is a permutation of Latin 1, not CP310.
#
# I have also omitted some of the APL characters.

from_unicode = {
  "\u25CA": 0x70,
  "\u2227": 0x71,
  "\u2228": 0x78,
  "\u2191": 0x8a,
  "\u2193": 0x8b,
  "\u2264": 0x8c,
  "\u2308": 0x8d,
  "\u230A": 0x8e,
  "\u2192": 0x8f,
  "\u2395": 0x90,
  "\u258C": 0x91,
  "\u2590": 0x92,
  "\u2580": 0x93,
  "\u2584": 0x94,
  "\u2588": 0x95,
  "\u2283": 0x9a,
  "\u2282": 0x9b,
  "\u2190": 0x9f,
  "\u2500": 0xa2,
  "\u2022": 0xa3,
  "\u2099": 0xa4,
  "\u2229": 0xaa,
  "\u222A": 0xab,
  "\u22A5": 0xac,
  "\u2265": 0xae,
  "\u2218": 0xaf,
  "\u2207": 0xba,
  "\u2206": 0xbb,
  "\u22A4": 0xbc,
  "\u2260": 0xbe,
  "\u2502": 0xbf,
  "\u207D": 0xc1,
  "\u207A": 0xc2,
  # I'm not sure what the mapping for 0xc3 should be
  "\u2514": 0xc4,
  "\u250C": 0xc5,
  "\u251C": 0xc6,
  "\u2534": 0xc7,
  # 0xca NAND doesn't have the same glyph as Unicode
  # 0xcb NOR doesn't have the same glyph as Unicode
  "\u207E": 0xd1,
  "\u208B": 0xd2,
  "\u253c": 0xd3,
  "\u2518": 0xd4,
  "\u2510": 0xd5,
  "\u2524": 0xd6,
  "\u252C": 0xd7,
  "\u2261": 0xe0,
  "\u2081": 0xe1,
  "\u2082": 0xe2,
  "\u2083": 0xe3,
  "\u2070": 0xf0,
  # SUPERSCRIPT 1 is in ISO Latin 1
  # SUPERSCRIPT 2 is in ISO Latin 1
  # SUPERSCRIPT 3 is in ISO Latin 1
  "\u2074": 0xf4,
  "\u2075": 0xf5,
  "\u2076": 0xf6,
  "\u2077": 0xf7,
  "\u2078": 0xf8,
  "\u2079": 0xf9
  
}

