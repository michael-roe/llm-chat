#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

static wchar_t buff[8*1024];

void walk_tree(xmlNode *node)
{
xmlNode *current;
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
  walk_tree(root_element);
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
