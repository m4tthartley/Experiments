
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

char *file_str;

void first_test() {
	parse_ptr = file_str;
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

typedef struct {} ParserState;
typedef struct {
	enum {
		NODE_START,
		NODE_END,
		TEXT,
	} type;
	char str[256];
	int len;
} Segment;
bool parser_get_segment(ParserState *ps, Segment *seg) {
	Token t;
	t = get_token();

	seg->len = 0;

	if (t.type == TOKEN_EOF) /*break;*/ return false;

	while (t.type != TOKEN_NODE && t.type != TOKEN_EOF) {
		// if (!b || strcmp(b->node, "paragraph")!=0) {
		// 	b = &blocks[block_count++];
		// 	strcpy(b->node, "paragraph");
		// }
		// printf("%s", t.str);

		/*todo: check if last node was equals, if it was and END flag isn't set, return text.
				If it was and END flag is set, return the node end.*/

		seg->type = TEXT;
		strcpy(seg->str+seg->len, t.str);
		seg->len += strlen(t.str);
		return true;

		// t = get_token();
	}
	// need to have saved the beginning of the string and set the end to 0
	// should return the ptr in token so it can set to 0

	if (t.type == TOKEN_NODE) {
		t = get_token();
		// if (t.type == TOKEN_COLON) {
		// 	printf("_");
		// 	t = get_token();
		// 	while (t.type != TOKEN_NEWLINE && t.type != TOKEN_EOF) {
		// 		printf("%s", t.str);
		// 		t = get_token();
		// 	}
		// 	printf("_");
		// }
		// if (t.type == TOKEN_EQUALS) {
		// 	printf("_");
		// 	t = get_token();
		// 	while (t.type != TOKEN_SPACE && t.type != TOKEN_NEWLINE && t.type != TOKEN_EOF) {
		// 		printf("%s", t.str);
		// 		t = get_token();
		// 	}
		// 	printf("_");
		// }
		if (t.type == TOKEN_OPEN_CURLY_BRACE) {
			// printf("_");
			// t = get_token();
			// int brace_indent = 0;
			// while ((t.type != TOKEN_CLOSE_CURLY_BRACE || brace_indent>0) && t.type != TOKEN_EOF) {
			// 	printf("%s", t.str);
			// 	if (t.type == TOKEN_OPEN_CURLY_BRACE) ++brace_indent;
			// 	if (t.type == TOKEN_CLOSE_CURLY_BRACE) --brace_indent;
			// 	t = get_token();
			// }
			// t = get_token();
			// printf("_");

			seg->type = NODE_START;
			strcpy(seg->str+seg->len, t.str);
			seg->len += strlen(t.str);
		}
	}
	/*todo: When end of line or close brace, return the end for the last node started of the correct type*/
	if (t.type == TOKEN_CLOSE_CURLY_BRACE) {
		seg->type = NODE_END;
		strcpy(seg->str+seg->len, t.str);
		seg->len += strlen(t.str);
	}

	return true;
}

void second_test() {
	// Usage code
	ParserState ps;
	parse_ptr = file_str;
	// parser_init(&ps, "test.txt");
	Segment seg;
	while (parser_get_segment(&ps, &seg)) {
		// parser_segment(&ps, &seg);
		if (seg.type == NODE_START) {
			if (strcmp(seg.str, "italic")) {
				printf("<i>");
			}
		}
		if (seg.type == NODE_END) {
			if (strcmp(seg.str, "italic")) {
				printf("</i>");
			}
		}
		if (seg.type == TEXT) {
			printf(seg.str);
		}
	}
}

int main() {
	printf("test\n");

	FILE *f = fopen("test.txt", "rb");
	fseek(f, 0, SEEK_END);
	int size = ftell(f);
	fseek(f, 0, SEEK_SET);

	file_str = malloc(size+1);
	fread(file_str, 1, size, f);
	file_str[size] = 0;
	fclose(f);

	printf("size %i\n", size);
	// printf("%s\n", str);


	// first_test();

	second_test();
}