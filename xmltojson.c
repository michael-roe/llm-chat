#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <locale.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

static wchar_t buff[8*1024];

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
      wprintf(L"%ls", cp);
    }
  }
}

void parse_message(xmlNode *node)
{
xmlNode *current;
xmlChar *role;

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
      wprintf(L"<msg role=\"%s\">", role);
      walk_tree(current->children);
      wprintf(L"</msg>");
    }
  }
}  

void parse_session(xmlNode *node)
{
xmlNode *current;

  wprintf(L"<?xml version=\"1.0\"?>");
  for (current = node; current; current = current->next)
  {
    if (current->type == XML_ELEMENT_NODE)
    {
      if (strcmp(current->name, "session") != 0)
      {
        fwprintf(stderr, L"Expected <session>, found <%s>.\n", current->name);
        exit(-1);
      }
      wprintf(L"<session>");
      parse_message(current->children);
      wprintf(L"</session>");
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
