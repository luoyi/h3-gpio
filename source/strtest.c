
#include <stdio.h>
#include <string.h>
#include <regex.h>

#include "comm.h"

#define MAX_OPT   20

static char ** str_parse(const char * str, int * size)
{
	char * regexString = "\\S+";
	regex_t regexCompiled;
	regmatch_t rt;
	const char * cursor = str;

	char ** ret = calloc( 20 , sizeof(char*) );
	int i = 0;

	if (regcomp(&regexCompiled, regexString, REG_EXTENDED))
	{
		printf("Could not compile regular expression.\n");
		return NULL;
	};

	while (0 == regexec(&regexCompiled, cursor, 1, &rt, 0)) {
		char * tmp = NULL;
		if ( i > MAX_OPT ) {
			FATAL_ERRORF("command opt is bigger than 20");
			return NULL;
		}
		unsigned int offset = rt.rm_eo - rt.rm_so;
		tmp = calloc( offset + 1, sizeof(char) );
		cursor += rt.rm_so;
		strncpy(tmp, cursor,  offset);
		ret[i++] = tmp;
		DBG_MSG("match : [%.*s], offset = %d", offset, cursor, offset);
		cursor += offset;
	}
	*size = i;
	regfree(&regexCompiled);
	return ret;
}


int main ()
{
  char * source = "___ abc123def ___ ghi456 ___";
  char * regexString = "[a-z]*([0-9]+)([a-z]*)";
  size_t maxMatches = 2;
  size_t maxGroups = 3;
  
  regex_t regexCompiled;
  regmatch_t groupArray[maxGroups];
  unsigned int m;
  char * cursor;

  if (regcomp(&regexCompiled, regexString, REG_EXTENDED))
    {
      printf("Could not compile regular expression.\n");
      return 1;
    };

  m = 0;
  cursor = source;
  for (m = 0; m < maxMatches; m ++)
    {
      if (regexec(&regexCompiled, cursor, maxGroups, groupArray, 0))
        break;  // No more matches

      unsigned int g = 0;
      unsigned int offset = 0;
      for (g = 0; g < maxGroups; g++)
        {
          if (groupArray[g].rm_so == (size_t)-1)
            break;  // No more groups

          if (g == 0)
            offset = groupArray[g].rm_eo;

          char cursorCopy[strlen(cursor) + 1];
          strcpy(cursorCopy, cursor);
          cursorCopy[groupArray[g].rm_eo] = 0;
          printf("Match %u, Group %u: [%2u-%2u]: %s\n",
                 m, g, groupArray[g].rm_so, groupArray[g].rm_eo,
                 cursorCopy + groupArray[g].rm_so);
        }
      cursor += offset;
    }

  regfree(&regexCompiled);

  str_parse("/usr/bin/mpc --foo a b c", &m);
  DBG_MSG("m = %d", m);
  return 0;
}
