
// FILE *sound = fopen("Sleep_Away.wav", "rb");
// fseek(sound, 0, SEEK_END);
// int soundSize = ftell(sound);
// fseek(sound, 0, SEEK_SET);
// void *soundData = malloc(soundSize);
// fread(soundData, 1, soundSize, sound);
// Sound sleepAway = LoadSoundFromMemory(soundData, soundSize);
// fclose(sound);

// sound = fopen("shoot2.wav", "rb");
// fseek(sound, 0, SEEK_END);
// soundSize = ftell(sound);
// fseek(sound, 0, SEEK_SET);
// soundData = malloc(soundSize);
// fread(soundData, 1, soundSize, sound);
// Sound shootSound = LoadSoundFromMemory(soundData, soundSize);
// fclose(sound);

// SoundPlay(sleepAway);
//SoundPlay(shootSound);
// PlaySound("Sleep_Away.wav", NULL, SND_ASYNC);

#include "w:/softwarerenderer/platform.c"

void Test () {
	FILE *sound = fopen("w:/miscassets/aurora.ogg", "rb");
	fseek(sound, 0, SEEK_END);
	int size = ftell(sound);
	fseek(sound, 0, SEEK_SET);
	void *data = malloc(size);
	fread(data, 1, size, sound);
	{
		int x = 0;
	}
	fclose(sound);
}

Sound LoadSound (char *file) {
	FILE *sound = fopen(file, "rb");
	fseek(sound, 0, SEEK_END);
	int soundSize = ftell(sound);
	fseek(sound, 0, SEEK_SET);
	void *soundData = malloc(soundSize);
	fread(soundData, 1, soundSize, sound);
	Sound s = LoadSoundFromMemory(soundData, soundSize);
	fclose(sound);
	return s;
}

uint AudioThread (void *udata) {
	OSState *os = udata;
	InitSound(os);
	while (true) {
		UpdateSound(os);
		Sleep(16);
	}
	return 0;
}

int main (int argc, char**argv) {
	Test();

	Sound shootSound = LoadSound("w:/miscassets/shoot2.wav");

	Sound music[7];
	music[0] = LoadSound("w:/miscassets/Sleep_Away.wav");
	music[1] = LoadSound("w:/miscassets/Kalimba.wav");
	music[2] = LoadSound("w:/miscassets/Dunka_Dunka.wav");
	music[3] = LoadSound("w:/miscassets/aurora.wav");
	music[4] = LoadSound("w:/miscassets/tron.wav");
	music[5] = LoadSound("w:/miscassets/A-8.wav");
	music[6] = LoadSound("w:/miscassets/MegaMan.wav");

	// SoundPlay(&sleepAway);
	// SoundPlay(shootSound);

	OSState os = {0};
	InitOpenglVideo(&os, 1920, 1080);

	CreateThread(NULL, 0, AudioThread, &os, 0, NULL);

	// InitSound(&os);
	while (os.windowOpen) {
		PollEvents(&os);

		if (KeyPressed(&os, KEYBOARD_LEFT)) {
			SoundPlay(&shootSound, 0);
		}
		if (KeyPressed(&os, KEYBOARD_RIGHT)) {
			// SoundPlay(&sleepAway, SOUND_FADE_IN);
		}

		if (KeyPressed(&os, KEYBOARD_1)) FadeOutSounds(),SoundPlay(&music[0], SOUND_FADE_IN);
		if (KeyPressed(&os, KEYBOARD_2)) FadeOutSounds(),SoundPlay(&music[1], SOUND_FADE_IN);
		if (KeyPressed(&os, KEYBOARD_3)) FadeOutSounds(),SoundPlay(&music[2], SOUND_FADE_IN);
		if (KeyPressed(&os, KEYBOARD_4)) FadeOutSounds(),SoundPlay(&music[3], SOUND_FADE_IN);
		if (KeyPressed(&os, KEYBOARD_5)) FadeOutSounds(),SoundPlay(&music[4], SOUND_FADE_IN);
		if (KeyPressed(&os, KEYBOARD_6)) FadeOutSounds(),SoundPlay(&music[5], SOUND_FADE_IN);
		if (KeyPressed(&os, KEYBOARD_7)) FadeOutSounds(),SoundPlay(&music[6], SOUND_FADE_IN);

		if (KeyPressed(&os, KEYBOARD_ESCAPE)) FadeOutSounds();

		// glAccum(GL_ACCUM, 0.05f);
		// glAccum(GL_RETURN, 1.0f);
		// glAccum(GL_LOAD, 0.95f);
		glAccum(GL_ACCUM, 0.075f);
		glAccum(GL_RETURN, 1.0f);
		glAccum(GL_LOAD, 0.9f);

		// static bool paused = false;
		if (KeyPressed(&os, KEYBOARD_SPACE)) {
			// if (!paused) paused = true;
			// else paused = false;
			TogglePauseSound();
		}

		// UpdateSound(&os);

		// glClear(GL_COLOR_BUFFER_BIT);

#if 0
		{
			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			glBegin(GL_LINE_STRIP);
			// glVertex2f(0.0f, 0.0f);
			// glVertex2f(0.1f, 0.0f);
			// glVertex2f(0.1f, 0.1f);
			for (int i = 0; i < paintSize; ++i) {
				float x = -1.0f + (2.0f / (float)paintSize)*i;
				float y = (float)buffer[i].left / (float)0x7FFF;
				glVertex2f(x, y / volume);
			}
			glEnd();
		}
#else
		int size = SOUND_SAMPLES_PER_SEC/10;

#if 1
		glEnable(GL_BLEND);
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendFunc(GL_ONE, GL_ONE);

		int cursor = visualCursor;
		glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < size; ++i) {
			float x = 1.0f - (2.0f / (float)size)*i;
			float y = (float)visualSamples[cursor].left / (float)0x7FFF;
			glVertex2f(x, y / volume);

			--cursor;
			if (cursor < 0) cursor += SOUND_SAMPLES_PER_SEC;
		}
		glEnd();

		cursor = visualCursor;
		glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < size; ++i) {
			float x = 1.0f - (2.0f / (float)size)*i;
			float y = (float)visualSamples[cursor].right / (float)0x7FFF;
			glVertex2f(x, y / volume);

			--cursor;
			if (cursor < 0) cursor += SOUND_SAMPLES_PER_SEC;
		}
		glEnd();
#endif
#endif

/*		glBegin(GL_QUADS);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glVertex2f(-1.0f, -1.0f);
		glVertex2f(1.0f, -1.0f);
		glVertex2f(1.0f, 1.0f);
		glVertex2f(-1.0f, 1.0f);
		glEnd();*/


		FinishVideo(&os);
	}
}