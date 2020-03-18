#include "base.h"

int main(int argc, char* argv[])
{
	try
	{
		HelloTriangleApplication app;
		app.run();
	}
	catch (std::exception& e)
	{
		std::cerr << e.what();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}