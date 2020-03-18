#include "base.h"

int main()
{
	try
	{
		Transfer transfer;
		Set_Base base(true);
		base.transferValue(transfer);
		base.cleanup();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what();
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
