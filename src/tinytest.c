#define CUTE_SOUND_IMPLEMENTATION
#include "cute_sound.h"
#include <stdio.h>

int main()
{
	cs_context_t *soundctx;
	cs_loaded_sound_t sound;
	cs_play_sound_def_t def;
	
	soundctx = cs_make_context(0, 44000, 8192, 5, NULL);
	cs_spawn_mix_thread(soundctx);
	cs_thread_sleep_delay(soundctx, 10);

	sound = cs_load_wav("test.wav");
	def = cs_make_def(&sound);
	cs_play_sound(soundctx, def);
	getchar();

	cs_shutdown_context(soundctx);
}
