#include <gs/context.h>
#include <gs/system/log.h>
#include <stdlib.h>


namespace {
	int startGlslScene(const std::string& progname)
	{
		gs::Context game(progname);
		if (game.isError()) {
			// error happend at init and/or loading resources
			LOGE("Error happend at context creation. Press any key to say Good bye!\n");
			getchar();
			return 1;
		}
		if (!game.run()) {
			LOGE("Error happend at loading. Press any key to say Good bye!\n");
			getchar();
			return 1;
		}
		return 0;
	}
}

int main(int argc, char* args[])
{
	std::string progname;
	if (argc >= 1) {
		// should be always the case...
		progname = args[0];
	}

	int rv = startGlslScene(progname);

	LOGI("Good bye!\n");
	return rv;
}

