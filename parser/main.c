
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
	TOKEN_UNKNOWN,
	TOKEN_TEXT,
	TOKEN_SIGIL,
	TOKEN_NODE,
	TOKEN_COLON,
	TOKEN_EQUALS,
	TOKEN_OPEN_CURLY_BRACE,
	TOKEN_CLOSE_CURLY_BRACE,
	TOKEN_NEWLINE,
	TOKEN_SPACE,
	TOKEN_EOF,
} TokenType;
typedef struct {
	int type;
	char *str;
} Token;

char *parse_ptr;
char buffer[256];

Token get_token() {
	Token tkn = {0};
	if (*parse_ptr) {
		int len = 0;
		if ((*parse_ptr >= 'a' && *parse_ptr <= 'z') || (*parse_ptr >= 'A' && *parse_ptr <= 'Z')) {
			buffer[len++] = *parse_ptr++;
			while ((*parse_ptr >= 'a' && *parse_ptr <= 'z') ||
				   (*parse_ptr >= 'A' && *parse_ptr <= 'Z') ||
				   (*parse_ptr >= '0' && *parse_ptr <= '9')) {
				buffer[len++] = *parse_ptr++;
			}
			buffer[len] = 0;
			tkn.type = TOKEN_TEXT;
			tkn.str = buffer;
			return tkn;
		} else if (*parse_ptr == '@') {
			buffer[len++] = *parse_ptr++;
			while ((*parse_ptr >= 'a' && *parse_ptr <= 'z') ||
				   (*parse_ptr >= 'A' && *parse_ptr <= 'Z') ||
				   (*parse_ptr >= '0' && *parse_ptr <= '9')) {
				buffer[len++] = *parse_ptr++;
			}
			buffer[len] = 0;
			tkn.type = TOKEN_NODE;
			tkn.str = buffer;
			return tkn;
		} else if (*parse_ptr == ':') {
			buffer[0] = *parse_ptr++;
			buffer[1] = 0;
			tkn.type = TOKEN_COLON;
			tkn.str = buffer;
			return tkn;
		} else if (*parse_ptr == '\n') {
			buffer[0] = *parse_ptr++;
			buffer[1] = 0;
			tkn.type = TOKEN_NEWLINE;
			tkn.str = buffer;
			return tkn;
		} else if (*parse_ptr == ' ') {
			buffer[0] = *parse_ptr++;
			buffer[1] = 0;
			tkn.type = TOKEN_SPACE;
			tkn.str = buffer;
			return tkn;
		} else if (*parse_ptr == '=') {
			buffer[0] = *parse_ptr++;
			buffer[1] = 0;
			tkn.type = TOKEN_EQUALS;
			tkn.str = buffer;
			return tkn;
		} else if (*parse_ptr == '{') {
			buffer[0] = *parse_ptr++;
			buffer[1] = 0;
			tkn.type = TOKEN_OPEN_CURLY_BRACE;
			tkn.str = buffer;
			return tkn;
		} else if (*parse_ptr == '}') {
			buffer[0] = *parse_ptr++;
			buffer[1] = 0;
			tkn.type = TOKEN_CLOSE_CURLY_BRACE;
			tkn.str = buffer;
			return tkn;
		} else {
			buffer[0] = *parse_ptr++;
			buffer[1] = 0;
			tkn.type = TOKEN_TEXT;
			tkn.str = buffer;
			return tkn;
		}

		// char *s = malloc(2);
		// s[0] = *parse_ptr++;
		// s[1] = 0;
		// return s;
	} else {
		tkn.type = TOKEN_EOF;
		return tkn;
	}
}

typedef struct {
	char node[32];
	void *parent;
	void *child;
	void *next;
} ContentBlock;

ContentBlock blocks[256];
int block_count = 0;

int main() {
	printf("test\n");

	FILE *f = fopen("test.txt", "rb");
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);

	char *str = malloc(size+1);
	fread(str, 1, size, f);
	str[size] = 0;
	fclose(f);

	printf("size %i\n", size);
	// printf("%s\n", str);

	parse_ptr = str;
	Token t;
	t = get_token();
	ContentBlock *b = NULL;
	while (1) {
		
		if (t.type == TOKEN_EOF) break;

		while (t.type != TOKEN_NODE && t.type != TOKEN_EOF) {
			if (!b || strcmp(b->node, "paragraph")!=0) {
				b = &blocks[block_count++];
				// b->node = "paragraph";
				strcpy(b->node, "paragraph");
			}
			printf("%s", t.str);
			t = get_token();
		}
		// need to have saved the beginning of the string and set the end to 0
		// should return the ptr in token so it can set to 0

		if (t.type == TOKEN_NODE) {
			t = get_token();
			if (t.type == TOKEN_COLON) {
				printf("_");
				t = get_token();
				while (t.type != TOKEN_NEWLINE && t.type != TOKEN_EOF) {
					printf("%s", t.str);
					t = get_token();
				}
				printf("_");
			}
			if (t.type == TOKEN_EQUALS) {
				printf("_");
				t = get_token();
				while (t.type != TOKEN_SPACE && t.type != TOKEN_NEWLINE && t.type != TOKEN_EOF) {
					printf("%s", t.str);
					t = get_token();
				}
				printf("_");
			}
			if (t.type == TOKEN_OPEN_CURLY_BRACE) {
				printf("_");
				t = get_token();
				int brace_indent = 0;
				while ((t.type != TOKEN_CLOSE_CURLY_BRACE || brace_indent>0) && t.type != TOKEN_EOF) {
					printf("%s", t.str);
					if (t.type == TOKEN_OPEN_CURLY_BRACE) ++brace_indent;
					if (t.type == TOKEN_CLOSE_CURLY_BRACE) --brace_indent;
					t = get_token();
				}
				t = get_token();
				printf("_");
			}
		}
		// printf("%i %s\n", t.type, t.str);
	}
}