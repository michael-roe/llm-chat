#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

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

#define TELNET_TERMINAL_IS 0
#define TELNET_TERMINAL_SEND 1

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
#define IBM_START_FIELD 0x1d

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

unsigned char initial_msg[] =
  {TELNET_IAC, TELNET_DO, TELNET_OPT_TERMINAL};

unsigned char terminal_msg[] =
  {TELNET_IAC, TELNET_SB, TELNET_OPT_TERMINAL, TELNET_TERMINAL_SEND,
  TELNET_IAC, TELNET_SE};

unsigned char options_msg[] =
  {TELNET_IAC, TELNET_DO, TELNET_OPT_EOR,
    TELNET_IAC, TELNET_WILL, TELNET_OPT_EOR,
    TELNET_IAC, TELNET_DO, TELNET_OPT_BINARY,
    TELNET_IAC, TELNET_WILL, TELNET_OPT_BINARY,
    TELNET_IAC, TELNET_WILL, TELNET_OPT_GO_AHEAD,
    TELNET_IAC, TELNET_GA};

unsigned char screen_msg[] =
  {IBM_WRITE_ERASE, IBM_WCC_PARITY | IBM_WCC_RESERVED | IBM_WCC_GO_AHEAD,
    0xc1, IBM_INSERT_CURSOR, TELNET_IAC, TELNET_EOR};

unsigned char screen_update_msg[] =
  {IBM_WRITE, IBM_WCC_PARITY | IBM_WCC_RESERVED | IBM_WCC_GO_AHEAD,
    0xc2, IBM_INSERT_CURSOR, TELNET_IAC, TELNET_EOR};

static int not_first_screen = 0;

static int state;

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

void rx_byte(int c, int session_fd)
{
  fprintf(stderr, "%02x ", c);
  switch (state)
  {
    case STATE_DATA:
      if (c == TELNET_IAC)
      {
        fprintf(stderr, "[IAC]");
        state = STATE_DATA_IAC;
      }
      break;
    case STATE_DATA_IAC:
      switch (c)
      {
        case TELNET_IAC:
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
          fprintf(stderr, "[BREAK]");
          if (not_first_screen)
          {
          write(session_fd, screen_update_msg, sizeof(screen_update_msg));
          }
          else
          {
            write(session_fd, screen_msg, sizeof(screen_msg));
            not_first_screen = 1;
          }
          state = STATE_DATA;
          break;
        case TELNET_EOR:
          fprintf(stderr, "[EOR]");
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
        fprintf(stderr, "[WILL TERMINAL-TYPE]");
        write(session_fd, terminal_msg, sizeof(terminal_msg));
      }
      else
      {
        fprintf(stderr, "[WILL %02x]", c);
      }
      state = STATE_DATA;
      break;
    case STATE_WONT:
      fprintf(stderr, "[WONT %02x]", c);
      state = STATE_DATA;
      break;
    case STATE_DO:
      fprintf(stderr, "[DO %02x]", c);
      state = STATE_DATA;
      break;
    case STATE_DONT:
      fprintf(stderr, "[DONT %02x]", c);
      state = STATE_DATA;
      break;
    case STATE_OPT:
      if (c == TELNET_IAC)
      {
        fprintf(stderr, "[IAC]");
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
          fprintf(stderr, "[TERMINAL-TYPE] [IS] %s", suboption + 2);
          if (strncmp(suboption + 2, "IBM-3279", 8) == 0)
          {
            fprintf(stderr, "(terminal is IBM-3279)");
            write(session_fd, options_msg, sizeof(options_msg));
          }
        }
        if (suboption_count > 2)
        {
          fprintf(stderr, "[%d] [%d] %s [SE]", suboption[0], suboption[1],
            suboption + 2);
        }
        else if (suboption_count == 1)
        {
          fprintf(stderr, "[%d] [SE]", suboption[0]);
        }
        else
        {
          fprintf(stderr, "[SE]");
        }
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

  write(session_fd, initial_msg, sizeof(initial_msg));

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

  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0)
  {
    fprintf(stderr, "socket failed\n");
    exit(-1);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
    &opt, sizeof(opt)) < 0)
  {
    fprintf(stderr, "setsockopt failed\n");
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
  {
    fprintf(stderr, "bind failed\n");
    exit(-1);
  }

  if (listen(server_fd, 3) < 0)
  {
    fprintf(stderr, "listen failed\n");
    exit(-1);
  }

  session_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);

  if (session_fd < 0)
  {
    fprintf(stderr, "accept failed\n");
    exit(-1);
  }

  session(session_fd);

  fprintf(stderr, "Done\n");

  return 0;
}