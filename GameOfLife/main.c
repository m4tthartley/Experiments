
#include <platform.c>

#define GRIDX 100
#define GRIDY 100
#define GRID_SIZE (100*100)

typedef struct {
	bool live;
	bool new_state;
} Cell;

#define grid_pos_index(x, y) (((y<0?y+GRIDY:y)%GRIDY)*GRIDX + ((x<0?x+GRIDX:x)%GRIDX))

void spawn_blinker(Cell *cells, int x, int y) {
	cells[grid_pos_index(x, y)].live = true;
	cells[grid_pos_index(x+1, y)].live = true;
	cells[grid_pos_index(x-1, y)].live = true;
}

void spawn_glider(Cell *cells, int x, int y) {
	cells[grid_pos_index(x,   y)].live = true;
	cells[grid_pos_index(x+1, y)].live = true;
	cells[grid_pos_index(x-1, y)].live = true;
	cells[grid_pos_index(x+1, y-1)].live = true;
	cells[grid_pos_index(x,   y-2)].live = true;
}

int main(int argc, char **argv) {
	OSState os;
	InitSoftwareVideo(&os, 1000, 1000, GRIDX, GRIDY);

	uint *video = malloc(100*100*sizeof(uint));

	Cell cells[GRID_SIZE] = {0};

	for (int i = 0; i < GRID_SIZE; ++i) {
		cells[i].live = rand()%2;
	}

	/*for (int i = 0; i < 100; ++i) {
		spawn_glider(cells, rand()%GRIDX, rand()%GRIDY);
	}*/

	while (os.windowOpen) {
		int64 start_time = GetTime();
		PollEvents(&os);
		// for (int i = 0; i < 100*100; ++i) {
		// 	video[i] = rand();
		// }

		for (int y = 0; y < GRIDY; ++y)
		for (int x = 0; x < GRIDX; ++x) {
			Cell *c = &cells[grid_pos_index(x, y)];
			c->new_state = c->live;

			int live_neighbours = 0;
			if (cells[grid_pos_index(x-1, y-1)].live) ++live_neighbours;
			if (cells[grid_pos_index(x-0, y-1)].live) ++live_neighbours;
			if (cells[grid_pos_index(x+1, y-1)].live) ++live_neighbours;
			if (cells[grid_pos_index(x+1, y-0)].live) ++live_neighbours;
			if (cells[grid_pos_index(x+1, y+1)].live) ++live_neighbours;
			if (cells[grid_pos_index(x-0, y+1)].live) ++live_neighbours;
			if (cells[grid_pos_index(x-1, y+1)].live) ++live_neighbours;
			if (cells[grid_pos_index(x-1, y-0)].live) ++live_neighbours;

			if (c->live) {
				if (live_neighbours < 2) c->new_state = false;
				else if (live_neighbours > 3) c->new_state = false;
			} else {
				if (live_neighbours == 3) c->new_state = true;
			}
		}

		for (int i = 0; i < GRID_SIZE; ++i) {
			cells[i].live = cells[i].new_state;
			if (cells[i].live) {
				video[i] = 0x000000FF;
			} else {
				video[i] = 0xFFFFFFFF;
			}
		}

		DisplaySoftwareGraphics(&os, video, PIXEL_FORMAT_UBYTE, 4);

		int64 time = GetTime() - start_time;
		float secs = ConvertToSeconds(time);
		if (secs < 0.1f) {
			int ms = 100.0f - (1000.0f*secs);
			Sleep(ms);
		}
	}
}