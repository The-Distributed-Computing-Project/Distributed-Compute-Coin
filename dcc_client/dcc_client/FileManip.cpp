
#include "FileManip.h"

//#include <dirent.h>

int ExtractZip(std::string path, std::string saveAs)
{
	//try
	//{
	//    elz::extractZip(path, saveAs);
	//}
	//catch (const std::exception&)
	//{
	//    return 1;
	//}
	return 0;
}


int FileCount(std::string dir)
{
	auto dirIter = std::filesystem::directory_iterator(dir);
	int fileCount = 0;

	for (auto& entry : dirIter)
	{
		if (entry.is_regular_file())
		{
			++fileCount;
		}
	}

	return fileCount;
}

