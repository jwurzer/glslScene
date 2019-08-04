#include "context.h"
#include <system/log.h>
#include <stdlib.h>


namespace {
	int startGlslScene()
	{
		gs::Context game;
		if (game.isError()) {
			// error happend at init and/or loading resources
			return 1;
		}
		game.run();
		return 0;
	}
}

int main(int argc, char* args[])
{
	int rv = startGlslScene();

#ifdef _WIN32
	LOGI("Press any key to say Good bye!\n");
	getchar();
	LOGI("Good bye!\n");
#else
	LOGI("Good bye!\n");
#endif
	return rv;
}

