#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <locale.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

static wchar_t buff[8*1024];


/*
 * Print out the string, escaping both XML and JSON special characters.
 * The effect should as if the string is first XML encoded, and then
 * JSON encoded.
 *
 * Single quote is only a special character if we're printing out the value
 * of an XML attribute that is enclosed within single quotation marks.
 */

void escape_xml_json(wchar_t *str, int escape_apos)
{
wchar_t *cp;

  cp = str;
  while (*cp)
  {
    if (*cp == '<')
    {
      wprintf(L"&lt;");
    }
    else if (*cp == '>')
    {
      wprintf(L"&gt;");
    }
    else if (*cp == '&')
    {
      wprintf(L"&amp;");
    }
    else if (*cp == 0xA0)
    {
      wprintf(L"&#%02X;", *cp); /* &nbsp; isn't really a special character */
    }
    else if ((*cp == '\'') && escape_apos)
    {
      wprintf(L"&apos;");
    } 
    else if (*cp == '\\')
    {
      wprintf(L"\\\\");
    }
    else if (*cp == '"')
    {
      wprintf(L"\\\"");
    }
    else if (*cp == '\n')
    {
      /*
       * Newline usually won't occur, because all white space will have been
       * converted to spaces before this routine is called. Handle newline
       * in case this routine is called from somewhere that doesn't do that.
       */

      wprintf(L"\\n");
    }
    else if (*cp == '\t')
    {
      wprintf(L"\\t");
    }
    else if (*cp == '\r')
    {
      wprintf(L"\\r");
    }
    else if (*cp < 0x20)
    {
      /*
       * This case shouldn't be reached, because all the control characters
       * that are allowed in XML have been handled above. Handle this case
       * just in case something unexpected happens.
       */

      wprintf(L"&#x%02X", *cp);
    }
    else
    {
      wprintf(L"%lc", *cp);
    }
    cp++;
  }
}

void walk_tree(xmlNode *node)
{
xmlNode *current;
xmlChar *role;
wchar_t *cp;

  for (current = node; current; current = current->next)
  {
    if (current->type == XML_ELEMENT_NODE)
    {
      wprintf(L"<%s>", current->name);
      walk_tree(current->children);
      wprintf(L"</%s>", current->name);
    }
    else
    {
      mbstowcs(buff, xmlNodeGetContent(current), sizeof(buff));
      cp = buff;
      /* Convert newlines to spaces */
      while (*cp)
      {
        if (*cp == '\n')
        {
          *cp = ' ';
        } 
        cp++;
      }

      /*
       * TO DO: This white space removal is too aggresive, it will remove
       * spaces before italics.
       */

      /* Remove trailing white space */
      cp--;
      while (*cp == ' ')
      {
        *cp = '\0'; 
        cp--;
      }

      /* Skip over initial white space */
      cp = buff;
      while (*cp == ' ')
      {
        cp++;
      }
      /*
       * TO DO: Should escape JSON special characters, espcially double quote
       */ 

      escape_xml_json(cp, 0);
    }
  }
}

void parse_message(xmlNode *node)
{
xmlNode *current;
xmlChar *role;
int first;

  first = 1;
  for (current = node; current; current = current->next)
  {
    if (current->type == XML_ELEMENT_NODE)
    {
      if (strcmp(current->name, "msg") != 0)
      {
        fwprintf(stderr, L"Expected <msg>, found <%s>.\n", current->name);
        exit(-1);
      }
      role = xmlGetProp(current, "role");
      if (first == 0)
      {
        wprintf(L", ");
      }
      else
      {
        first = 0;
      }
      wprintf(L"{\"role\":\"%s\", \"content\":\"", role);
      walk_tree(current->children);
      wprintf(L"\"}");
    }
  }
}  

void parse_session(xmlNode *node)
{
xmlNode *current;

  for (current = node; current; current = current->next)
  {
    if (current->type == XML_ELEMENT_NODE)
    {
      if (strcmp(current->name, "session") != 0)
      {
        fwprintf(stderr, L"Expected <session>, found <%s>.\n", current->name);
        exit(-1);
      }
      wprintf(L"{\"messages\":[");
      parse_message(current->children);
      wprintf(L"]}");
    }
  }
}

void workfnx()
{
xmlDoc *doc; 
xmlNode *root_element;

  doc = xmlReadFile("iliad.xml", NULL, 0);
  if (doc == NULL)
  {
    fprintf(stderr, "xmlReadFile failed\n");
    exit(-1);
  }

  root_element = xmlDocGetRootElement(doc);
  parse_session(root_element);
  wprintf(L"\n");

}

int main(int argc, char **argv)
{
  setlocale(LC_ALL, getenv("LANG"));

  LIBXML_TEST_VERSION

  workfnx();

  xmlCleanupParser();

  return 0;
}
