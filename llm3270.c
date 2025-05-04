#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iconv.h>

#define PORT 17002

#define TELNET_EOR 239
#define TELNET_SE 240
#define TELNET_BREAK 243
#define TELNET_GA 249
#define TELNET_SB 250
#define TELNET_WILL 251
#define TELNET_WONT 252
#define TELNET_DO 253
#define TELNET_DONT 254
#define TELNET_IAC 255

#define TELNET_OPT_BINARY 0
#define TELNET_OPT_GO_AHEAD 1
#define TELNET_OPT_TERMINAL 24
#define TELNET_OPT_EOR 25
#define TELNET_OPT_ENVIRON 39

#define TELNET_TERMINAL_IS 0
#define TELNET_TERMINAL_SEND 1

#define TELNET_ENVIRON_IS 0
#define TELNET_ENVIRON_SEND 1

#define TELNET_ENVIRON_VAR 0
#define TELNET_ENVIRON_VALUE 1
#define TELNET_ENVIRON_ESC 2
#define TELNET_ENVIRON_USERVAR 3

#define STATE_DATA 0
#define STATE_DATA_IAC 1
#define STATE_WILL 2
#define STATE_WONT 3
#define STATE_DO 4
#define STATE_DONT 5
#define STATE_OPT 6
#define STATE_OPT_IAC 7

#define IBM_READ_MODIFIED_ALL 0x6e
#define IBM_ERASE_ALL_UNPROTECTED 0x6f
#define IBM_WRITE 0xf1
#define IBM_READ_BUFFER 0xf2
#define IBM_WRITE_STRUCTURED 0xf3
#define IBM_WRITE_ERASE 0xf5
#define IBM_READ_MODIFIED 0xf6

#define IBM_GRAPHIC_ESCAPE 0x08
#define IBM_SET_BUFFER_ADDRESS 0x11
#define IBM_INSERT_CURSOR 0x13
#define IBM_DUP 0x1c
#define IBM_START_FIELD 0x1d
#define IBM_FIELD_MARK 0x1e

#define IBM_WCC_MDT 0x1
#define IBM_WCC_GO_AHEAD 0x2
#define IBM_WCC_BELL 0x4
#define IBM_WCC_PRINT 0x8
#define IBM_WCC_RESERVED 0x40
#define IBM_WCC_PARITY 0x80

#define IBM_ATTR_MDT 0x01
#define IBM_ATTR_NON_DETECT 0x00
#define IBM_ATTR_DETECT 0x04
#define IBM_ATTR_INTENSE 0x08
#define IBM_ATTR_NON_DISPLAY 0x0c
#define IBM_ATTR_NUMERIC 0x10
#define IBM_ATTR_PROTECTED 0x20

#define IBM_AID_PA1 0x6c
#define IBM_AID_CLEAR 0x6d
#define IBM_AID_PA2 0x6e
#define IBM_AID_PA3 0x6b
#define IBM_AID_PF10 0x7a
#define IBM_AID_PF11 0x7b
#define IBM_AID_PF12 0x7c
#define IBM_AID_ENTER 0x7d
#define IBM_AID_PF1 0xf1
#define IBM_AID_PF2 0xf2
#define IBM_AID_PF3 0xf3
#define IBM_AID_PF4 0xf4
#define IBM_AID_PF5 0xf5
#define IBM_AID_PF6 0xf6
#define IBM_AID_PF7 0xf7
#define IBM_AID_PF8 0xf8
#define IBM_AID_PF9 0xf9

static wchar_t cp310[]  = {
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x25ca, 0x2227, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x2228, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x2191, 0x2193, 0x2264, 0x2308, 0x230a, 0x2192, 
0x2395, 0x258c, 0x2590, 0x2580, 0x2584, 0x2588, 0x20, 0x20, 
0x20, 0x20, 0x2283, 0x2282, 0x20, 0x20, 0x20, 0x2190, 
0x20, 0x20, 0x2500, 0x2219, 0x2099, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x2229, 0x222a, 0x22a5, 0x20, 0x2265, 0x2218, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x2207, 0x2206, 0x22a4, 0x20, 0x2260, 0x2502, 
0x20, 0x207d, 0x207a, 0x220e, 0x2514, 0x250c, 0x251c, 0x2534, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2342, 0x20, 
0x20, 0x207e, 0x208b, 0x253c, 0x2518, 0x2510, 0x2524, 0x252c, 
0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
0x2261, 0x2081, 0x2082, 0x2083, 0x20, 0x20, 0x20, 0x20, 
0x20, 0x20, 0x20, 0x20, 0x2235, 0x20, 0x20, 0x20, 
0x2070, 0x20, 0x20, 0x20, 0x2074, 0x2075, 0x2076, 0x2077, 
0x2078, 0x2079, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20};

unsigned char do_terminal_msg[] =
  {TELNET_IAC, TELNET_DO, TELNET_OPT_TERMINAL};

unsigned char terminal_msg[] =
  {TELNET_IAC, TELNET_SB, TELNET_OPT_TERMINAL, TELNET_TERMINAL_SEND,
  TELNET_IAC, TELNET_SE};

unsigned char do_environ_msg[] =
  {TELNET_IAC, TELNET_DO, TELNET_OPT_ENVIRON};

unsigned char environ_msg[] =
  {TELNET_IAC, TELNET_SB, TELNET_OPT_ENVIRON, TELNET_ENVIRON_SEND,
  TELNET_ENVIRON_USERVAR,
  'C', 'O', 'D', 'E', 'P', 'A', 'G', 'E',
  TELNET_ENVIRON_USERVAR,
  'C', 'H', 'A', 'R', 'S', 'E', 'T',
  TELNET_ENVIRON_USERVAR,
  'K', 'B', 'D', 'T', 'Y', 'P', 'E',
  /* 'U', 'S', 'E', 'R', */
  /* 'D', 'E', 'V', 'N', 'A', 'M', 'E', */
  TELNET_IAC, TELNET_SE};

unsigned char options_msg[] =
  {TELNET_IAC, TELNET_DO, TELNET_OPT_EOR,
    TELNET_IAC, TELNET_WILL, TELNET_OPT_EOR,
    TELNET_IAC, TELNET_DO, TELNET_OPT_BINARY,
    TELNET_IAC, TELNET_WILL, TELNET_OPT_BINARY,
    TELNET_IAC, TELNET_WILL, TELNET_OPT_GO_AHEAD};

unsigned char screen_msg[] =
  {IBM_WRITE_ERASE, IBM_WCC_PARITY | IBM_WCC_RESERVED | IBM_WCC_GO_AHEAD,

    IBM_GRAPHIC_ESCAPE, 0xc5,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xa2,

    IBM_SET_BUFFER_ADDRESS, 0xc1, 0x4d,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xd5,

    IBM_SET_BUFFER_ADDRESS, 0xc1, 0x50,
    IBM_GRAPHIC_ESCAPE, 0x85,
    0x40, 0x40, 0x40,
    0xd3, 0x96, 0x87, 0x89, 0x95,

    IBM_SET_BUFFER_ADDRESS, 0xc2, 0x5f,
    IBM_GRAPHIC_ESCAPE, 0x85,

    IBM_SET_BUFFER_ADDRESS, 0xc2, 0x60,
    IBM_GRAPHIC_ESCAPE, 0xc6,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xa2,

    IBM_SET_BUFFER_ADDRESS, 0xc3, 0x6d,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xd6,

    IBM_SET_BUFFER_ADDRESS, 0xc3, 0xf0,
    IBM_GRAPHIC_ESCAPE, 0x85,
    IBM_START_FIELD, IBM_ATTR_PROTECTED | IBM_ATTR_INTENSE,
    0xe4, 0xa2, 0x85, 0x99,
    IBM_START_FIELD, 0x40,
    IBM_INSERT_CURSOR,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    IBM_START_FIELD, IBM_ATTR_PROTECTED,

    IBM_SET_BUFFER_ADDRESS, 0xc4, 0x7e,
    IBM_START_FIELD, IBM_ATTR_PROTECTED,
    IBM_GRAPHIC_ESCAPE, 0x85,

    IBM_SET_BUFFER_ADDRESS, 0xc5, 0x40,
    IBM_GRAPHIC_ESCAPE, 0x85,
    IBM_START_FIELD, IBM_ATTR_PROTECTED | IBM_ATTR_INTENSE,
    0xd7, 0x81, 0xa2, 0xa2, 0xa6, 0x96, 0x99, 0x84,

    IBM_START_FIELD, 0x40 | IBM_ATTR_NUMERIC,
    0x40, 0x40, 0x40, 0x40, 0x40, 0x40,
    IBM_START_FIELD, IBM_ATTR_PROTECTED,

    IBM_SET_BUFFER_ADDRESS, 0xc6, 0x4e,
    IBM_START_FIELD, IBM_ATTR_PROTECTED,
    IBM_GRAPHIC_ESCAPE, 0x85,

    IBM_SET_BUFFER_ADDRESS, 0xc6, 0x50,
    IBM_GRAPHIC_ESCAPE, 0xc6,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xa2,

    IBM_SET_BUFFER_ADDRESS, 0xc7, 0x5d,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xd6,

    IBM_SET_BUFFER_ADDRESS, 0xc7, 0x60,
    IBM_GRAPHIC_ESCAPE, 0x85,

    IBM_SET_BUFFER_ADDRESS, 0xc8, 0x6f,
    IBM_GRAPHIC_ESCAPE, 0x85,

    IBM_SET_BUFFER_ADDRESS, 0xc8, 0xf0,
    IBM_GRAPHIC_ESCAPE, 0xc4,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xa2,

    IBM_SET_BUFFER_ADDRESS, 0xc9, 0x7d,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xd4,

    TELNET_IAC, TELNET_EOR};

unsigned char screen_update_msg[] =
  {IBM_WRITE_ERASE, IBM_WCC_PARITY | IBM_WCC_RESERVED | IBM_WCC_GO_AHEAD,
    IBM_GRAPHIC_ESCAPE, 0xc5,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xa2,

    IBM_SET_BUFFER_ADDRESS, 0xc1, 0x4d,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xd5,

    IBM_SET_BUFFER_ADDRESS, 0xc1, 0x50,
    IBM_GRAPHIC_ESCAPE, 0x85,
    0x40, 0x40, 0x40,
    0xd8, 0xa4, 0x85, 0x99, 0xa8,

    IBM_SET_BUFFER_ADDRESS, 0xc2, 0x5f,
    IBM_GRAPHIC_ESCAPE, 0x85,

    IBM_SET_BUFFER_ADDRESS, 0xc2, 0x60,
    IBM_GRAPHIC_ESCAPE, 0xc6,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xa2,

    IBM_SET_BUFFER_ADDRESS, 0xc3, 0x6d,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xd6,

    IBM_SET_BUFFER_ADDRESS, 0xc3, 0xf0,
    IBM_GRAPHIC_ESCAPE, 0x85,
    IBM_START_FIELD, 0x40,
    IBM_INSERT_CURSOR,
    
    IBM_SET_BUFFER_ADDRESS, 0xc4, 0x7e,
    IBM_START_FIELD, IBM_ATTR_PROTECTED,
    IBM_GRAPHIC_ESCAPE, 0x85,

    IBM_SET_BUFFER_ADDRESS, 0xc5, 0x40,
    IBM_GRAPHIC_ESCAPE, 0x85,
    IBM_START_FIELD, 0x40,

    IBM_SET_BUFFER_ADDRESS, 0xc6, 0x4e,
    IBM_START_FIELD, IBM_ATTR_PROTECTED,
    IBM_GRAPHIC_ESCAPE, 0x85,

    IBM_SET_BUFFER_ADDRESS, 0xc6, 0x50,
    IBM_GRAPHIC_ESCAPE, 0x85,
    IBM_START_FIELD, 0x40,

    IBM_SET_BUFFER_ADDRESS, 0xc7, 0x5e,
    IBM_START_FIELD, IBM_ATTR_PROTECTED,
    IBM_GRAPHIC_ESCAPE, 0x85,

    IBM_SET_BUFFER_ADDRESS, 0xc7, 0x60,
    IBM_GRAPHIC_ESCAPE, 0x85,
    IBM_START_FIELD, 0x40,

    IBM_SET_BUFFER_ADDRESS, 0xc8, 0x6e,
    IBM_START_FIELD, IBM_ATTR_PROTECTED,
    IBM_GRAPHIC_ESCAPE, 0x85,

    IBM_SET_BUFFER_ADDRESS, 0xc8, 0xf0,
    IBM_GRAPHIC_ESCAPE, 0x85,
    IBM_START_FIELD, 0x40,

    IBM_SET_BUFFER_ADDRESS, 0xc9, 0x7e,
    IBM_START_FIELD, IBM_ATTR_PROTECTED,
    IBM_GRAPHIC_ESCAPE, 0x85,

    /* line 8 */
    IBM_SET_BUFFER_ADDRESS, 0x4a, 0x40,
    IBM_GRAPHIC_ESCAPE, 0x85,
    IBM_START_FIELD, 0x40,

    IBM_SET_BUFFER_ADDRESS, 0x4b, 0x4e,
    IBM_START_FIELD, IBM_ATTR_PROTECTED,
    IBM_GRAPHIC_ESCAPE, 0x85,

    /* line 9 */
    IBM_SET_BUFFER_ADDRESS, 0x4b, 0x50,
    IBM_GRAPHIC_ESCAPE, 0x85,
    IBM_START_FIELD, 0x40,

    IBM_SET_BUFFER_ADDRESS, 0x4c, 0x5e,
    IBM_START_FIELD, IBM_ATTR_PROTECTED,
    IBM_GRAPHIC_ESCAPE, 0x85,

    IBM_SET_BUFFER_ADDRESS, 0xd9, 0x40,
    IBM_GRAPHIC_ESCAPE, 0x85,
    IBM_START_FIELD, 0x40,

    IBM_SET_BUFFER_ADDRESS, 0x5a, 0x4e,
    IBM_START_FIELD, IBM_ATTR_PROTECTED,
    IBM_GRAPHIC_ESCAPE, 0x85,

    IBM_SET_BUFFER_ADDRESS, 0x5a, 0x50,
    IBM_GRAPHIC_ESCAPE, 0xc6,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xa2,

    IBM_SET_BUFFER_ADDRESS, 0x5b, 0x5d,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xd6,

    IBM_SET_BUFFER_ADDRESS, 0x5b, 0x60,
    IBM_GRAPHIC_ESCAPE, 0x85,

    IBM_SET_BUFFER_ADDRESS, 0x5c, 0x6f,
    IBM_GRAPHIC_ESCAPE, 0x85,

    IBM_SET_BUFFER_ADDRESS, 0x5c, 0xf0,
    IBM_GRAPHIC_ESCAPE, 0xc4,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xa2,

    IBM_SET_BUFFER_ADDRESS, 0x5d, 0x7d,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xa2,
    IBM_GRAPHIC_ESCAPE, 0xd4,

    TELNET_IAC, TELNET_EOR};

static char codepage[8];

static iconv_t rx_conv;

static iconv_t tx_conv;

static int not_first_screen = 0;

static int state;

unsigned char data[8192];

int data_count = 0;

void rx_data(int c)
{
  if (data_count < sizeof(data) - 1)
  {
    data[data_count] = c;
    data_count++;
  }
}

unsigned char suboption[128];

int suboption_count = 0;

void rx_suboption(int c)
{
  if (suboption_count < sizeof(suboption) - 1)
  {
    suboption[suboption_count] = c;
    suboption_count++;
  }
}

void end_suboption()
{
  suboption[suboption_count] = '\0';
}

/*
 * The IBM 3270 encodes buffer addresses using 6 bits per printable
 * character. This is similar to the Internet MIME encoding, except
 * it's in EBCDIC with a different translation table.
 */

/*
 * This lookup table is in ISO 8859-1, not UTF-8, so it is
 * not safe to represent it as a string constant.
 */

static int ascii_bit_pack[] = {
  ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
  'H', 'I', 0xa2, '.', '<', '(', '+', '|',
  '&', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', '!', '$', '*', ')', ';', 0xac,
  '-', '/', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z', 0xa6, ',', '%', '_', '>', '?',
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', ':', '#', '@', '\'', '=', '"' 
};


static int unpack_char(char c)
{
int i;

  i = 0;
  while (i < 64)
  {
    if (c == ascii_bit_pack[i])
      return i;
    i++;
  }
  return 0;
}

int buffer_address(char c1, char c2)
{
char *ptr1;
char *ptr2;
int index1;
int index2;
int i;

  index1 = unpack_char(c1);
  index2 = unpack_char(c2);

  return (index1 << 6) | index2;
}

unsigned char translated[8192];

void rx_record(int session_fd)
{
int i;
size_t in_left;
size_t out_left;
char *in_ptr;
char *out_ptr;
wchar_t c;
int addr;

  if (data_count == 0)
  {
    return;
  }
  else
  {
    fwprintf(stdout, L"<SCREEN>");
    switch (data[0])
    {
      case IBM_AID_CLEAR:
        fwprintf(stdout, L"<CLEAR>");
        break;
      case IBM_AID_PA1:
        fwprintf(stdout, L"<PA1>");
        break;
      case IBM_AID_PA2:
        fwprintf(stdout, L"<PA2>");
        break;
      case IBM_AID_PA3:
        fwprintf(stdout, L"<PA3>");
        break;
      case IBM_AID_ENTER:
        fwprintf(stdout, L"<ENTER>");
        break;
      case IBM_AID_PF1:
        fwprintf(stdout, L"<PF1>");
        break;
      case IBM_AID_PF2:
        fwprintf(stdout, L"<PF2>");
        break;
      case IBM_AID_PF3:
        fwprintf(stdout, L"<PF3>");
        break;
      case IBM_AID_PF4:
        fwprintf(stdout, L"<PF4>");
        break;
      case IBM_AID_PF5:
        fwprintf(stdout, L"<PF5>");
        break;
      case IBM_AID_PF6:
        fwprintf(stdout, L"<PF6>");
        break;
      case IBM_AID_PF7:
        fwprintf(stdout, L"<PF7>");
        break;
      case IBM_AID_PF8:
        fwprintf(stdout, L"<PF8>");
        break;
      case IBM_AID_PF9:
        fwprintf(stdout, L"<PF9>");
        break;
      case IBM_AID_PF10:
        fwprintf(stdout, L"<PF10>");
        break;
      case IBM_AID_PF11:
        fwprintf(stdout, L"<PF11>");
        break;
      case IBM_AID_PF12:
        fwprintf(stdout, L"<PF12>");
        break;

      default:
        fwprintf(stdout, L"<AID id=%02x>", data[0]);
     }
  }
  if ((data_count > 0) && (data[0] == IBM_AID_CLEAR))
  {
    /*
     * CLEAR is a "short read" and doesn't have data after it.
     */
    /* fwprintf(stderr, L"(redrawing the screen)"); */
    /* fflush(stderr); */
    if (not_first_screen)
    {
      write(session_fd, screen_update_msg, sizeof(screen_update_msg));
    }
    else
    {
      write(session_fd, screen_msg, sizeof(screen_msg));
    }
    fwprintf(stdout, L"</SCREEN>\n");
    return;
  }
  else if (data_count < 4)
  {
    fwprintf(stderr, L"(less than 4 bytes)"); 
    fflush(stderr);
    if (not_first_screen)
    {
      write(session_fd, screen_update_msg, sizeof(screen_update_msg));
    }
    else 
    {
      write(session_fd, screen_msg, sizeof(screen_msg));
    }
    fwprintf(stdout, L"</SCREEN>\n");
    return;
  }

  not_first_screen = 1;
  in_left = data_count - 3;
  out_left = sizeof(translated);
  in_ptr = data + 3;
  out_ptr = translated;
  /* do iconv */
  iconv(rx_conv, &in_ptr, &in_left, &out_ptr, &out_left);
  for (i=0; i<data_count - 3; i++)
  {
    if (data[i + 3] == IBM_SET_BUFFER_ADDRESS)
    {
      addr = buffer_address(translated[2*i + 3], translated[2*i + 5]);
      fwprintf(stdout, L"<SBA x=%d y=%d>", addr%80, addr/80);
      i += 2; /* Skip the two bytes of buffer address */
    }
    else if (data[i + 3] == IBM_GRAPHIC_ESCAPE)
    {
      fwprintf(stdout, L"%lc", cp310[data[i + 4]]);
       i += 1;
    }
    else if (data[i + 3] == IBM_DUP)
    {
      fwprintf(stdout, L"<DUP>");
    }
    else if (data[i + 3] == IBM_FIELD_MARK)
    {
      fwprintf(stdout, L"<FM>");
    }
    else
    {
      c = (translated[2*i] << 8) | translated[2*i + 1];
      if (c == '<')
      {
        fwprintf(stdout, L"&lt;");
      }
      else if (c == '>')
      {
        fwprintf(stdout, L"&gt;");
      }
      else if (c == '"')
      {
        fwprintf(stdout, L"&quot;");
      }
      else
      {
        fwprintf(stdout, L"%lc", c);
      }
      fflush(stderr);
    }
  }

  fwprintf(stdout, L"</SCREEN>\n");
  fflush(stdout);

  write(session_fd, screen_update_msg, sizeof(screen_update_msg));
  data_count = 0;
}

void rx_byte(int c, int session_fd)
{
int i;

  switch (state)
  {
    case STATE_DATA:
      if (c == TELNET_IAC)
      {
        /* fprintf(stderr, "[IAC]"); */
        state = STATE_DATA_IAC;
      }
      else
      {
        rx_data(c);
      }
      break;
    case STATE_DATA_IAC:
      switch (c)
      {
        case TELNET_IAC:
          rx_data(TELNET_IAC);
          state = STATE_DATA;
          break;
        case TELNET_WILL:
          state = STATE_WILL;
          break;
        case TELNET_WONT:
          state = STATE_WONT;
          break;
        case TELNET_DO:
          state = STATE_DO;
          break;
        case TELNET_DONT:
          state = STATE_DONT;
          break;
        case TELNET_SB:
          state = STATE_OPT;
          break;
        case TELNET_BREAK:
          /* fwprintf(stderr, L"[BREAK]"); */
          /* fflush(stderr); */
          not_first_screen = 0;
          write(session_fd, screen_msg, sizeof(screen_msg));
          state = STATE_DATA;
          break;
        case TELNET_EOR:
          /* fprintf(stderr, "[EOR]"); */
          rx_record(session_fd);
          state = STATE_DATA;
          break;
        default:
          state = STATE_DATA;
          break;
      }
      break;
    case STATE_WILL:
      if (c == TELNET_OPT_TERMINAL)
      {
        /* fprintf(stderr, "[WILL TERMINAL-TYPE]"); */
        write(session_fd, terminal_msg, sizeof(terminal_msg));
      }
      else if (c == TELNET_OPT_ENVIRON)
      {
        /* fprintf(stderr, "[WILL NEW-ENVIRON]"); */
        write(session_fd, environ_msg, sizeof(environ_msg));
      }
      else if (c == TELNET_OPT_EOR)
      {
        /* fprintf(stderr, "[WILL EOR]"); */
      }
      else if (c == TELNET_OPT_BINARY)
      {
        /* fprintf(stderr, "[WILL BINARY]"); */
      }
      else
      {
        fwprintf(stderr, L"[WILL %02x]", c);
        fflush(stderr);
      }
      state = STATE_DATA;
      break;
    case STATE_WONT:
      fwprintf(stderr, L"[WONT %02x]", c);
      fflush(stderr);
      state = STATE_DATA;
      break;
    case STATE_DO:
      switch (c)
      {
        case TELNET_OPT_BINARY:
          /* fprintf(stderr, "[DO BINARY]"); */
          break;
        case TELNET_OPT_GO_AHEAD:
          /* fprintf(stderr, "[DO GA]"); */
          break;
        case TELNET_OPT_EOR:
          /* fprintf(stderr, "[DO EOR]"); */
          break;
        default:
          fwprintf(stderr, L"[DO %02x]", c);
          fflush(stderr);
          break;
      }
      state = STATE_DATA;
      break;
    case STATE_DONT:
      fwprintf(stderr, L"[DONT %02x]", c);
      state = STATE_DATA;
      break;
    case STATE_OPT:
      if (c == TELNET_IAC)
      {
        /* fprintf(stderr, "[IAC]"); */
        state = STATE_OPT_IAC;
      }
      else
      {
        rx_suboption(c);
      }
      break;

    case STATE_OPT_IAC:
      if (c == TELNET_IAC)
      {
        rx_suboption(TELNET_IAC);
        state = STATE_OPT;
      }
      else if (c == TELNET_SE)
      {
        end_suboption();
        if ((suboption_count > 2) && (suboption[0] == TELNET_OPT_TERMINAL) &&
          (suboption[1] == TELNET_TERMINAL_IS))
        {
          /* fprintf(stderr, "[TERMINAL-TYPE] [IS] %s", suboption + 2); */
          if (strncmp(suboption + 2, "IBM-3278", 8) == 0)
          {
            /* fprintf(stderr, "(terminal is IBM-3278)"); */
            write(session_fd, do_environ_msg, sizeof(do_environ_msg));
          }
          else if (strncmp(suboption + 2, "IBM-3279", 8) == 0)
          {
            /* fprintf(stderr, "(terminal is IBM-3279)"); */
            write(session_fd, do_environ_msg, sizeof(do_environ_msg));
          }
          else
          {
            fwprintf(stderr, L"(terminal is %s)", suboption + 2);
            fflush(stderr);
          }
        }
        else if ((suboption_count > 2) &&
          (suboption[0] == TELNET_OPT_ENVIRON) &&
          (suboption[1] == TELNET_ENVIRON_IS))
        {
          /* fprintf(stderr, "[NEW-ENVIRON] [IS] "); */
#if 0
          for (i=2; i<suboption_count; i++)
          {
            if (suboption[i] < 5)
            {
              fwprintf(stderr, L"(%02x)", suboption[i]);
            }
            else
            {
              fwprintf(stderr, L"%c", suboption[i]);
            }
          }
#endif
          if ((suboption[2] == TELNET_ENVIRON_USERVAR) &&
            (strncmp(suboption + 3, "CODEPAGE", 8) == 0) &&
            (suboption[11] == TELNET_ENVIRON_VALUE) &&
            (suboption[12] != TELNET_ENVIRON_USERVAR)) 
          {
            codepage[0] = 'C';
            codepage[1] = 'P';
            codepage[2] = suboption[12];
            codepage[3] = suboption[13];
            codepage[4] = suboption[14];
            codepage[5] = '\0';
            /* fwprintf(stderr, L"[CODEPAGE = %s]", codepage); */
          }
          else
          {
            strcpy(codepage, "CP500");
          }
          tx_conv = iconv_open(codepage, "UTF16BE");
          rx_conv = iconv_open("UTF16BE", codepage);
          write(session_fd, options_msg, sizeof(options_msg));
        }
        else if (suboption_count > 2)
        {
          fwprintf(stderr, L"[%d] [%d] %s [SE]", suboption[0], suboption[1],
            suboption + 2);
        }
        else if (suboption_count == 1)
        {
          fwprintf(stderr, L"[%d] [SE]", suboption[0]);
        }
        else
        {
          fwprintf(stderr, L"[SE]");
        }
        suboption_count = 0;
        state = STATE_DATA;
      }
      break;

    default:
      break;
  }
}

void session(int session_fd)
{
int in_buffer;
int i;
unsigned char buff[1024];

  state = STATE_DATA;

  write(session_fd, do_terminal_msg, sizeof(do_terminal_msg));

  while ((in_buffer = read(session_fd, buff, sizeof(buff) - 1)) > 0)
  {
    for (i=0; i<in_buffer; i++)
    {
      rx_byte(buff[i], session_fd);
    }
  }
}

int main(int argc, char **argv)
{
int opt;
int server_fd;
int session_fd;
int addrlen;
int i;
struct sockaddr_in address;

  setlocale(LC_ALL, getenv("LANG"));

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
  {
    fwprintf(stderr, L"socket failed\n");
    exit(-1);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
    &opt, sizeof(opt)) < 0)
  {
    fwprintf(stderr, L"setsockopt failed\n");
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    fwprintf(stderr, L"bind failed\n");
    exit(-1);
  }

  if (listen(server_fd, 3) < 0)
  {
    fwprintf(stderr, L"listen failed\n");
    exit(-1);
  }

  session_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);

  if (session_fd < 0)
  {
    fwprintf(stderr, L"accept failed\n");
    exit(-1);
  }

  session(session_fd);

  fwprintf(stderr, L"\nDone\n");

  return 0;
}
