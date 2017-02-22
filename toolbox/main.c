
#include <stdio.h>
#include <stdbool.h>

#define array_size(arr) (sizeof(arr)/sizeof(arr[0]))

typedef struct {
	int key;
	char *str;
} Item;
Item items[] = {
	5, "Hello, World!",
	13, "Jerry Maguire",
	1, "Jim Hawkins",
	73, "Link",
	52, "Kate Walker",
	1567, "Hans Voralberg",
	6, "Han Solo",
	176, "Fox Mulder",
	235, "Dana Scully",
	10, "Rick Grimes",
	7, "Luke Skywalker",
	57, "Ruby Rose",
	35, "Weiss Schnee",
	146, "Blake Belladonna",
	98, "Yang Xiao Long",
	679, "Jaune Arc",
	74, "Pyrrha Nikos",
	21, "Ozpin",
	832, "Glynda Goodwitch",
	1427, "Lie Ren",
	1395, "Nora Valkyrie",
	3, "Sun Wukong",
	12, "Cinder Fall",
};

bool int_compare_proc(void *a, void *b) {
	int aa = *(int*)a;
	int bb = *(int*)b;
	if (bb < aa) {
		return true;
	} else {
		return false;
	}
}

void bubble_sort(void *array, int len, int stride, bool (*compare)(void*, void*)) {
	for (int i = 0; i < len-1; ++i) {
		for (int j = 0; j < len-1; ++j) {
			char *a = array+(stride*j);
			char *b = array+(stride*(j+1));
			if (compare(array+(stride*j), array+(stride*(j+1)))) {
				for (int k = 0; k < stride; ++k) {
					char swap = *(a+k);
					*(a+k) = *(b+k);
					*(b+k) = swap;
				}
			}
		}
	}
}

// should key be void* ?
void *linear_search(int key, void *array, int len, int stride, bool (*compare)(void*, void*)) {
	char *ptr = array;
	for (int i = 0; i < len; ++i) {
		int k = *(int*)ptr;
		if (k == key) return ptr;
		ptr += stride;
	}

	return NULL;
}

/*void *binary_search(int key, void *array, int len, int stride) {

}*/

int main() {
	printf("%lu items\n", sizeof(items)/sizeof(Item));

	for (int i = 0; i < array_size(items); ++i) printf("%i %s\n", items[i].key, items[i].str);
	bubble_sort(items, array_size(items), sizeof(Item), int_compare_proc);
	for (int i = 0; i < array_size(items); ++i) printf("%i %s\n", items[i].key, items[i].str);

	Item *i = linear_search(57, items, array_size(items), sizeof(Item), int_compare_proc);
	if (i) {
		printf("item 57 is '%s'\n", i->str);
	}
}