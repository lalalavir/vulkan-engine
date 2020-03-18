#include "example.h"

int main(int argc, char* argv[])
{
	try
	{
		Example example;
		example.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what();
	}
	system("pause");
	return EXIT_SUCCESS;
}