#include <stdio.h>
#include <string.h>

#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <regex.h>


#include "jsmn.h"
#include "comm.h"
#include "gpio-h3.h"


#define  MAX_TOK 1024
#define  MAX_ROW 20
#define  MAX_COL 20

#define MAX_OPT   20

static char ** cmd_parse(const char * str, int * size)
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


static char* read_file (const char* filename, size_t* length)
{
	int fd;
	struct stat file_info;
	char* buffer;

	/* Open the file.  */
	fd = open (filename, O_RDONLY);

	/* Get information about the file.  */
	fstat (fd, &file_info);
	*length = file_info.st_size;
	/* Make sure the file is an ordinary file.  */
	if (!S_ISREG (file_info.st_mode)) {
		/* It's not, so give up.  */
		close (fd);
		return NULL;
	}

	/* Allocate a buffer large enough to hold the file's contents.  */
	buffer = (char*) malloc (*length);
	/* Read the file into the buffer.  */
	read (fd, buffer, *length);

	/* Finish up.  */
	close (fd);
	return buffer;
}


static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

int main(int argc, char **argv) {
	char *cfg_file = NULL;
	char * file_content = NULL;
	char  char_tmp[1024] = {0};
	int  file_length = 0;
	int c, r, j, k;

	struct gpio_t  gpio_row[MAX_ROW] = {0};
	struct gpio_t  gpio_col[MAX_COL] = {0};

	uint32_t  row_size = 0;
	uint32_t  col_size = 0;

	char *** cmd = calloc(MAX_ROW * MAX_COL, sizeof(const char**));

	jsmn_parser p;
	jsmntok_t t[MAX_TOK];

	jsmn_init(&p);
	gpio_system_init();

	while ((c = getopt (argc, argv, "c:")) != -1) 
		switch (c)
		{
			case 'c':
				cfg_file = optarg;
				break;
			default:
				abort ();
		}
	
	if ( cfg_file == NULL ) {
		FATAL_ERRORF("no config file specified");
		return -1;
	}

	file_content = read_file(cfg_file, &file_length);

	r = jsmn_parse(&p, file_content, file_length, t, sizeof(t)/sizeof(t[0]));
	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

	/* Assume the top-level element is an object */
	if (r < 1 || t[0].type != JSMN_OBJECT) {
		printf("Object expected\n");
		return 1;
	}

	/* Loop over all keys of the root object */
	for (int i = 1; i < r; i++) {
		if (jsoneq(file_content, &t[i], "row") == 0) {
			DBG_MSG("- ROW:");
			if (t[i+1].type != JSMN_ARRAY) {
				continue;
			}
			row_size = t[i+1].size;
			for (j = 0; j < t[i+1].size; j++) {
				jsmntok_t *g = &t[i+j+2];
				DBG_MSG("  * %.*s", g->end - g->start, file_content + g->start);
				strncpy(char_tmp, file_content + g->start, g->end - g->start);
				gpio_init(&gpio_row[j], char_tmp);
			}
			i += t[i+1].size + 1;
		} else if (jsoneq(file_content, &t[i], "col") == 0) {
			DBG_MSG("- COL:");
			if (t[i+1].type != JSMN_ARRAY) {
				continue;
			}
			col_size = t[i+1].size;
			for (j = 0; j < t[i+1].size; j++) {
				jsmntok_t *g = &t[i+j+2];
				DBG_MSG("  * %.*s", g->end - g->start, file_content + g->start);
				strncpy(char_tmp, file_content + g->start, g->end - g->start);
				gpio_init(&gpio_col[j], char_tmp);
			}
			i += t[i+1].size + 1;
		} else if (jsoneq(file_content, &t[i], "cmd") == 0) {
			k = 0;
			DBG_MSG("- CMD:");
			if ( 0 == row_size || 0 == col_size ) {
				FATAL_ERRORF("Please put row and col before the cmd ");
			}
			if (t[i+1].type != JSMN_ARRAY) {
				FATAL_ERRORF("the cmd must be an array");
				continue; 
			}
			col_size = t[i+1].size;
			for (j = 0; j < t[i+1].size; j++) {
				if ( k > MAX_ROW * MAX_COL ) {
					FATAL_ERRORF("cmd is too bigger");
					return 0;
				}
				int s;
				jsmntok_t *g = &t[i+j+2];
				DBG_MSG("  * %.*s", g->end - g->start, file_content + g->start);
				strncpy(char_tmp, file_content + g->start, g->end - g->start);
				char ** scmd = cmd_parse(char_tmp, &s);
				cmd[k++] = scmd;
			}
			i += t[i+1].size + 1;
		}
	}
	return 0;
}
