#include <stdio.h>
#include <string.h>

#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "jsmn.h"



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
	char * file_content;
	int  file_length;
	int c;
	int r;

	jsmn_parser p;
	jsmntok_t t[128]; /* We expect no more than 128 tokens */

	jsmn_init(&p);

	while ((c = getopt (argc, argv, "c:")) != -1) 
		switch (c)
		{
			case 'c':
				cfg_file = optarg;
				break;
			default:
				abort ();
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
			int j;
			printf("- ROW:\n");
			if (t[i+1].type != JSMN_ARRAY) {
				continue; /* We expect groups to be an array of strings */
			}
			for (j = 0; j < t[i+1].size; j++) {
				jsmntok_t *g = &t[i+j+2];
				printf("  * %.*s\n", g->end - g->start, file_content + g->start);
			}
			i += t[i+1].size + 1;
		} 
	}
	return 0;
}
