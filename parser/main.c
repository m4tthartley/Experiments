
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

char *last_parse_ptr;
char *parse_ptr;
char buffer[256];

void revert_token () {
	parse_ptr = last_parse_ptr;
}

Token get_token() {
	Token tkn = {0};
	last_parse_ptr = parse_ptr;
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
		PARA_START,
		PARA_END,
	} type;
	char str[256];
	int len;
} Segment;
typedef struct {
	char name[16];
	enum {
		NODE_EQUALS,
		NODE_COLON,
		NODE_BRACES,
	} type;
	bool end;
	int indent;
} Node;
Node node_stack[16];
int node_count = 0;
int brace_indent = 0;
bool paragraph_open = false;
void remove_node_from_stack(ParserState *ps, int index) {
	printf("removing node %s\n", node_stack[index].name);
	if (index == node_count-1) {
		--node_count;
		return;
	}
	for (int i = index+1; i < node_count; ++i) {
		node_stack[i-1] = node_stack[i];
	}
	--node_count;
}
Token t;
Token last_token;
bool parser_get_segment(ParserState *ps, Segment *seg) {
	last_token = t;
	t = get_token();

	seg->len = 0;

	if (t.type == TOKEN_EOF) {
		printf("node stack %i\n", node_count);
		for (int i = node_count-1; i >= 0; --i) {
			if (node_stack[i].type == NODE_COLON) {
				seg->type = NODE_END;
				strcpy(seg->str+seg->len, node_stack[i].name);
				seg->len += strlen(node_stack[i].name);

				printf("node %s %i\n", node_stack[i].name, node_stack[i].indent);

				remove_node_from_stack(ps, i);
				return true;
			}
		}

		if (paragraph_open) {
			seg->type = PARA_END;
			seg->len = 0;
			revert_token();
			paragraph_open = false;
			return true;
		}

		return false;
	}

	while (t.type != TOKEN_NODE && t.type != TOKEN_EOF) {
		// if (!b || strcmp(b->node, "paragraph")!=0) {
		// 	b = &blocks[block_count++];
		// 	strcpy(b->node, "paragraph");
		// }
		// printf("%s", t.str);

		if (t.type == TOKEN_OPEN_CURLY_BRACE) {
			++brace_indent;
		}
		if (t.type == TOKEN_CLOSE_CURLY_BRACE) {
			--brace_indent;
			for (int i = node_count-1; i >= 0; --i) {
				if (node_stack[i].type == NODE_BRACES) {
					if (brace_indent <= node_stack[i].indent) {
						goto end_curly_brace;
					} else {
						break;
					}
				}
			}
		}

		/*todo: check if last node was equals, if it was and END flag isn't set, return text.
				If it was and END flag is set, return the node end.*/
		if (node_count > 0 && node_stack[node_count-1].type == NODE_EQUALS) {
			if (node_stack[node_count-1].end) {
				seg->type = NODE_END;
				strcpy(seg->str+seg->len, node_stack[node_count-1].name);
				seg->len += strlen(t.str);

				remove_node_from_stack(ps, node_count-1);
				revert_token();
				return true;
			} else {
				node_stack[node_count-1].end = true;
			}
		}

		if (t.type == TOKEN_NEWLINE) {
			for (int i = node_count-1; i >= 0; --i) {
				if (node_stack[i].type == NODE_COLON) {
					seg->type = NODE_END;
					strcpy(seg->str+seg->len, node_stack[i].name);
					seg->len += strlen(node_stack[i].name);

					remove_node_from_stack(ps, i);
					revert_token();
					return true;
				}
			}

			if (paragraph_open) {
				seg->type = PARA_END;
				seg->len = 0;
				revert_token();
				paragraph_open = false;
				return true;
			}

			seg->type = TEXT;
			seg->str[0] = '\n';
			seg->str[1] = 0;
			seg->len = 1;
			return true;
		}

		if (last_token.type == TOKEN_NEWLINE && node_count < 1) {
			seg->type = PARA_START;
			seg->len = 0;
			revert_token();
			paragraph_open = true;
			return true;
		}
		seg->type = TEXT;
		strcpy(seg->str+seg->len, t.str);
		seg->len += strlen(t.str);
		return true;

		// t = get_token();
	}
	// need to have saved the beginning of the string and set the end to 0
	// should return the ptr in token so it can set to 0

	if (t.type == TOKEN_NODE) {
		strcpy(node_stack[node_count].name, t.str+1);
		printf("adding node %s\n", t.str);
		strcpy(seg->str, t.str+1);
		seg->len += strlen(t.str)-1;
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
		if (t.type == TOKEN_COLON) {
			node_stack[node_count].type = NODE_COLON;
			node_stack[node_count].end = false;
			++node_count;
			// printf("adding node %s\n", t.str);

			seg->type = NODE_START;
			return true;
		}
		if (t.type == TOKEN_EQUALS) {
			node_stack[node_count].type = NODE_EQUALS;
			node_stack[node_count].end = false;
			++node_count;
			// printf("adding node %s\n", t.str);

			seg->type = NODE_START;
			return true;
		}
		if (t.type == TOKEN_OPEN_CURLY_BRACE) {
			node_stack[node_count].indent = brace_indent;
			++brace_indent;
			node_stack[node_count].type = NODE_BRACES;
			node_stack[node_count].end = false;
			++node_count;
			// printf("adding node %s\n", t.str);

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
			return true;
		}
	}
	/*todo: When end of line or close brace, return the end for the last node started of the correct type*/
end_curly_brace:
	if (t.type == TOKEN_CLOSE_CURLY_BRACE) {
		for (int i = node_count-1; i >= 0; --i) {
			if (node_stack[i].type == NODE_BRACES) {
				seg->type = NODE_END;
				strcpy(seg->str+seg->len, node_stack[i].name);
				seg->len += strlen(node_stack[i].name);

				remove_node_from_stack(ps, i);
				return true;
			}
		}
	}

	// note: Nothing should reach this.
	return false;
}

void second_test() {
	// Usage code
	ParserState ps;
	parse_ptr = file_str;
	// parser_init(&ps, "test.txt");
	FILE *fileout = fopen("test.html", "w");
	char *nodes[][3] = {
		{"header", "<h1>", "</h1>"},
		{"italic", "<i>", "</i>"},
		{"bold", "<b>", "</b>"},
		{"code", "<pre>", "</pre>"},
		{"video", "<iframe src=\"", "\"></iframe>"},
	};
	Segment seg;
	while (parser_get_segment(&ps, &seg)) {
		// parser_segment(&ps, &seg);
		if (seg.type == PARA_START) {
			fprintf(fileout, "<p>");
		}
		if (seg.type == PARA_END) {
			fprintf(fileout, "</p>");
		}
		if (seg.type == NODE_START) {
			bool found = false;
			for (int i = 0; i < (sizeof(nodes)/sizeof(char*)/3); ++i) {
				if (strcmp(seg.str, nodes[i][0])==0) {
					fprintf(fileout, "%s", nodes[i][1]);
					found = true;
					break;
				}
			}
			if (!found) {
				fprintf(fileout, "</? %s>", seg.str);
			}
			// if (strcmp(seg.str, "header")==0) {
			// 	fprintf(fileout, "<h1>");
			// } else if (strcmp(seg.str, "italic")==0) {
			// 	fprintf(fileout, "<i>");
			// } else {
			// 	fprintf(fileout, "<? %s>", seg.str);
			// }
		}
		if (seg.type == NODE_END) {
			bool found = false;
			for (int i = 0; i < (sizeof(nodes)/sizeof(char*)/3); ++i) {
				if (strcmp(seg.str, nodes[i][0])==0) {
					fprintf(fileout, "%s", nodes[i][2]);
					found = true;
					break;
				}
			}
			if (!found) {
				fprintf(fileout, "</? %s>", seg.str);
			}
			// if (strcmp(seg.str, "header")==0) {
			// 	fprintf(fileout, "</h1>");
			// } else if (strcmp(seg.str, "italic")==0) {
			// 	fprintf(fileout, "</i>");
			// } else {
			// 	fprintf(fileout, "</? %s>", seg.str);
			// }
		}
		if (seg.type == TEXT) {
			fprintf(fileout, "%s", seg.str);
		}
	}
	fclose(fileout);
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