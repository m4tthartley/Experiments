
#ifdef _WIN32
#	define WINDOWS
#endif
#ifdef __APPLE__
#	define MACOS
#endif
#ifdef __linux__
#	define LINUX
#endif
#ifdef __unix__
#	define UNIX
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#ifdef WINDOWS
#	include <windows.h>
#endif
#ifdef MACOS
#	include <mach/clock.h>
#	include <mach/mach.h>
#endif

typedef int64_t int64;

#define array_size(arr) (sizeof(arr)/sizeof(arr[0]))

#ifdef WINDOWS
LARGE_INTEGER _performance_frequency = {0};
int64 start_time() {
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	return time.QuadPart;
}
int64 end_time(int64 start) {
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);
	if (!_performance_frequency.QuadPart) QueryPerformanceFrequency(&_performance_frequency);
	int64 result = time.QuadPart - start;
	result *= 1000000000;
	result /= _performance_frequency.QuadPart;
	return result;
}
#endif
#ifdef MACOS
int64 mac_get_nanoseconds() {
	clock_serv_t clock_service;
	mach_timespec_t time;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &clock_service);
	clock_get_time(clock_service, &time);
	mach_port_deallocate(mach_task_self(), clock_service);
	// todo: seems like this is allocating and deallocating everytime, bad?

	return time.tv_nsec;
}
int64 start_time() {
	return mac_get_nanoseconds();
}
int64 end_time(int64 start) {
	int64 time = mac_get_nanoseconds() - start;
	return time;
}
#endif

typedef struct {
	int key;
	char *str;
} Item;
Item items[] = {
	/*1, "Ruby Rose",
	2, "Weiss Schnee",
	3, "Blake Belladonna",
	4, "Yang Xiao Long",
	5, "Jaune Arc",
	6, "Pyrrha Nikos",
	7, "Ozpin",
	8, "Glynda Goodwitch",
	9, "Lie Ren",
	10, "Nora Valkyrie",
	11, "Sun Wukong",
	12, "Cinder Fall",
	13, "Roman Torchwick",
	14, "Penny Polendina",
	15, "Neptune Vasilias",
	16, "James Ironwood",
	17, "Peter Port",
	18, "Bartholomew Oobleck",
	19, "Emerald Sustrai",
	20, "Mercury Black",
	21, "Qrow Branwen",
	22, "Mysterious Narrator",
	23, "Taiyang Xiao Long",*/

	19,	"Emerald Sustrai",
	22,	"Mysterious Narrator",
	10,	"Nora Valkyrie",
	5,	"Jaune Arc",
	11,	"Sun Wukong",
	16,	"James Ironwood",
	2,	"Weiss Schnee",
	7,	"Ozpin",
	17,	"Peter Port",
	6,	"Pyrrha Nikos",
	15,	"Neptune Vasilias",
	3,	"Blake Belladonna",
	9,	"Lie Ren",
	21,	"Qrow Branwen",
	8,	"Glynda Goodwitch",
	23,	"Taiyang Xiao Long",
	18,	"Bartholomew Oobleck",
	1,	"Ruby Rose",
	20,	"Mercury Black",
	13,	"Roman Torchwick",
	12,	"Cinder Fall",
	14,	"Penny Polendina",
	4,	"Yang Xiao Long",
};

// note: had to change this to be weird so gcc didn't complain
#define assert(exp) {if (!exp) {volatile int *p = (int*)0; *p = 0;}}

/*	----------------------------------------------------------------
	Dynamic arrays
*/
#define DYN_ARRAY_CHUNK_SIZE 64

typedef struct {
	int len;
	int max;
	int stride;
} DynArrayHead;

#define dyn_array_add(array, num) ((_dyn_array_add(&(array), sizeof(array[0]), num)) + array)
#define dyn_array_push(array, item) {\
	*dyn_array_add(array, 1) = item;\
}
#define dyn_array_len(array) (((DynArrayHead*)array-1)->len)

// todo: increase malloc size for big len, and realloc when needed
// 0000 0000
int _dyn_array_add(void **ptr, int stride, int len) {
	// printf("*ptr %p\n", *ptr);
	if (len > 0) {
		if (!*ptr) {
			// int asd = len & (DYN_ARRAY_CHUNK_SIZE-1);
			// if (asd > 0) asd = DYN_ARRAY_CHUNK_SIZE-asd;
			// int len2 = len + asd;
			int len2 = (len+64) & ~(DYN_ARRAY_CHUNK_SIZE-1);
			*ptr = (char*)malloc(sizeof(DynArrayHead)+(len2*stride)) + sizeof(DynArrayHead);
			DynArrayHead *head = (DynArrayHead*)*ptr - 1;
			head->len = 0;
			head->max = len2;
			head->stride = stride;
		}

		DynArrayHead *head = (DynArrayHead*)*ptr - 1;
		head->len += len;
		if (head->len > head->max) {
			int len2 = (head->len+64) & ~(DYN_ARRAY_CHUNK_SIZE-1);
			void *tmp = realloc(head, sizeof(DynArrayHead)+(len2*stride));
			printf("realloc %i\n", len2);
			if (!tmp) {
				// free(head);
				assert(false);
			}
			head = tmp;
			head->max = len2;
			*ptr = head+1;
		}
		// printf("*ptr %p\n", *ptr);
		// printf("array_add return %i\n", head->len-len);
		return head->len-len;
	} else {
		assert(false);
		return 0;
	}
}

// todo: dyn_array_remove_and_shift();

// void *_dyn_array_push(void **ptr, void *item, int stride) {
// 	if (!*ptr) {
// 		*ptr = malloc(sizeof(DynArrayHead)+(DYN_ARRAY_CHUNK_SIZE*stride));
// 	}
// 	return NULL;
// }

// bool int_compare_proc(void *a, void *b) {
// 	int aa = *(int*)a;
// 	int bb = *(int*)b;
// 	if (bb <= aa) {
// 		return true;
// 	} else {
// 		return false;
// 	}
// }

/*	----------------------------------------------------------------
	Sorting
*/
int compare_ints(void *a, void *b) {
	int aa = *(int*)a;
	int bb = *(int*)b;
	return aa - bb;
}

void bubble_sort(void *array, int len, int stride, int (*compare)(void*, void*)) {
	char *arr = array;
	for (int i = 0; i < len-1; ++i) {
		bool repeat = false;
		for (int j = 0; j < len-1; ++j) {
			char *a = arr+(stride*j);
			char *b = arr+(stride*(j+1));
			if (compare(a, b)>0) {
				repeat = true;
				for (int k = 0; k < stride; ++k) {
					char swap = *(a+k);
					*(a+k) = *(b+k);
					*(b+k) = swap;
				}
			}
		}
		if (!repeat) break;
	}
}

/*
	Insertion sort is the same except instead of swapping
	you insert in the correct place and shift
	the rest of the array up to make room.
*/
void selection_sort(void *array, int len, int stride, int (*compare)(void*, void*)) {
	char *arr = array;
	for (int i = 0; i < len-1; ++i) {
		char *min = arr+(i*stride);
		for (int j = i+1; j < len; ++j) {
			char *b = arr+(j*stride);
			if (compare(min, b)>0) {
				for (int k = 0; k < stride; ++k) {
					char swap = *(min+k);
					*(min+k) = *(b+k);
					*(b+k) = swap;
				}
			}
		}
	}
}

// todo: Heap sort?

#define copy_sort_item(dest, src)\
	{for (int ii = 0; ii < stride; ++ii)\
		*(dest+ii) = *(src+ii);}

void inner_merge_sort(char *array, int len, int stride, int (*compare)(void*, void*), char *scratch) {
	int l = len/2;
	if (l > 0) {
		inner_merge_sort(array, l, stride, compare, scratch);
		inner_merge_sort(array+(l*stride), len-l, stride, compare, scratch+(l*stride));

		char *a = scratch;
		char *b = scratch+(l*stride);
		int ai = 0;
		int bi = 0;
		for (int k = 0; k < len; ++k) {
			if (ai < l && (bi >= len-l || compare(a+(ai*stride), b+(bi*stride))<1)) {
				copy_sort_item(array+(k*stride), a+(ai*stride));
				++ai;
			} else {
				copy_sort_item(array+(k*stride), b+(bi*stride));
				++bi;
			}
		}
		memcpy(scratch, array, len*stride);
	}
}
void merge_sort (void *array, int len, int stride, int (*compare)(void*, void*)) {
	void *scratch = malloc(len*stride);
	memcpy(scratch, array, len*stride);
	inner_merge_sort(array, len, stride, compare, scratch);
	free(scratch);
}

void quick_sort (void *array, int len, int stride, int (*compare)(void*, void*)) {
	char *pivot = (char*)array+((len-1)*stride);
	int swap_index = 0;
	for (int i = 0; i < len; ++i) {
		if (compare((char*)array+(i*stride), pivot) < 1) {
			for (int k = 0; k < stride; ++k) {
				char swap = *((char*)array+(i*stride)+k);
				*((char*)array+(i*stride)+k) = *((char*)array+(swap_index*stride)+k);
				*((char*)array+(swap_index*stride)+k) = swap;
			}
			++swap_index;
		}
	}

	if (swap_index > 0) {
		quick_sort(array, swap_index-1, stride, compare);
	}
	if (len-swap_index > 0) {
		quick_sort((char*)array+((swap_index-1)*stride), len-(swap_index-1), stride, compare);
	}
}

// todo: In place merge sort, hehe

/*
	================================================================
	Searching
	================================================================
*/
// should key be void* ?
void *linear_search(int key, void *array, int len, int stride) {
	char *ptr = array;
	for (int i = 0; i < len; ++i) {
		int k = *(int*)ptr;
		if (k == key) return ptr;
		ptr += stride;
	}

	return NULL;
}

void *binary_search(int key, void *array, int len, int stride) {
	char *arr = array;
	int low = 0;
	int high = len-1;
	int index = high/2;
	while (low <= high) {
		int k = *(int*)(arr+(stride*index));
		if (key < k) {
			high = index-1;
		} else if (key > k) {
			low = index+1;
		} else {
			return arr+(stride*index);
		}
		index = low + (high-low)/2;
	}

	return NULL;
}

// todo: Interpolated binary search

/*
	================================================================
	Strings
	================================================================
*/
typedef struct {
	// note: in chunks
	int offset;
	int size;
} StringDir;
typedef struct {
	void *mem;
	int mem_size;
	StringDir *dir;
	int dir_size;
} StringPool;
char *make_string(StringPool *strp, char *str) {
	return str;
}

void str_append(StringPool *strp, char *str, char *str2) {

}

/*
	make_dyn_string()
	dyn_string_append
*/

int main() {
#if 0
	bool used[array_size(items)] = {0};
	for (int i = 0; i < array_size(items); ++i) {
		int r = rand()%array_size(items);
		while (used[r]) {
			r = rand()%array_size(items);
		}

		printf("%i,\t\"%s\",\n", items[r].key, items[r].str);
		used[r] = true;
	}
#endif

	{
		StringPool strp = {0};
		strp.mem_size = 1024;
		strp.mem = malloc(strp.mem_size);
		strp.dir = malloc((strp.mem_size/64)*sizeof(StringDir));
		strp.dir_size = strp.mem_size/64;

		char *my_str = make_string(&strp, "Hello");
		str_append(&strp, my_str, " World");
		str_append(&strp, my_str, "!\n");
		printf("%s", my_str);
	}

	printf("%lu items\n", sizeof(items)/sizeof(Item));

	FILE *out = fopen("out.txt", "w");

	#define print_items()\
		fprintf(out, "{\n");\
		for (int i = 0; i < array_size(items); ++i) {\
			fprintf(out, "\t%i %s\n", items[i].key, items[i].str);\
		}\
		fprintf(out, "}\n");

	print_items();
	int64 t = start_time();
	// bubble_sort(items, array_size(items), sizeof(Item), int_compare_proc);
	// selection_sort(items, array_size(items), sizeof(Item), int_compare_proc);
	// merge_sort(items, array_size(items), sizeof(Item), int_compare_proc);
	quick_sort(items, array_size(items), sizeof(Item), compare_ints);
	int64 time = end_time(t);
	printf("sort time %llins\n", time);
	print_items();

	{
		#define print_nums()\
			fprintf(out, "{\n");\
			for (int i = 0; i < array_size(nums); ++i) {\
				fprintf(out, "\t%i\n", nums[i]);\
			}\
			fprintf(out, "}\n");
		int nums[1000];
		for (int i = 0; i < array_size(nums); ++i) {
			nums[i] = rand();
		}

		print_nums();
		int64 start = start_time();
		quick_sort(nums, array_size(nums), sizeof(nums[0]), compare_ints);
		int64 end = end_time(start);
		printf("nums sort time %lli\n", end);
		print_nums();

		bool fail = false;
		int last = nums[0];
		for (int i = 1; i < array_size(nums); ++i) {
			if (nums[i] < last) {
				fail = true;
				break;
			}
			last = nums[i];
		}
		if (fail) {
			printf("ITEMS ARE NOT IN ORDER\n");
		} else {
			printf("nums success\n");
		}
	}

	// Item *i = linear_search(57, items, array_size(items), sizeof(Item));
	int key = 1;
	Item *i = binary_search(key, items, array_size(items), sizeof(Item));
	if (i) {
		printf("item %i is '%s'\n", key, i->str);
	}

	// system("pause");

	bool fail = false;
	for (int i = 0; i < array_size(items); ++i) {
		if (items[i].key != i+1) {
			fail = true;
			break;
		}
	}
	if (fail) {
		printf("ITEMS ARE NOT IN ORDER\n");
	} else {
		printf("success\n");
	}

	int *numbers = NULL;
	// dyn_array_push(numbers, 53);
	volatile int *tmp = dyn_array_add(numbers, 3);
	tmp[0] = 53;
	tmp[1] = 7;
	tmp[2] = 92;
	dyn_array_push(numbers, 33);
	int len = dyn_array_len(numbers);
	for (int i = 0; i < len; ++i) fprintf(out, "%i: %i\n", i, numbers[i]);

	typedef struct {
		int i;
		char *s;
	} Thing;
	Thing *things = NULL;
	Thing tmp_thing = {12, "TESTIG 12312312"};
	dyn_array_push(things, tmp_thing);
	Thing tmp2 = {53, "Hello World!"};
	dyn_array_push(things, tmp2);
	for (int i = 0; i < dyn_array_len(things); ++i) fprintf(out, "%i: %s\n", things[i].i, things[i].s);

	// Thing *stuff = dyn_array_add(stuff, 1000);
	Thing *stuff = NULL;
	int *stuff2 = NULL;
	// dyn_array_add(stuff, 1000);
	for (int i = 0; i < 1000; ++i) {
		// Thing a = {rand(), "test"};
		dyn_array_push(stuff2, rand());
	}
}