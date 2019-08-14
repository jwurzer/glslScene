#include <gs/context.h>
#include <gs/system/log.h>
#include <stdlib.h>


namespace {
	int startGlslScene()
	{
		gs::Context game;
		if (game.isError()) {
			// error happend at init and/or loading resources
			return 1;
		}
		if (!game.run()) {
			return 1;
		}
		return 0;
	}
}

int main(int argc, char* args[])
{
	int rv = startGlslScene();

#ifdef _WIN32
	if (rv) {
		LOGI("Error happend at loading. Press any key to say Good bye!\n");
		getchar();
	}
	LOGI("Good bye!\n");
#else
	LOGI("Good bye!\n");
#endif
	return rv;
}

