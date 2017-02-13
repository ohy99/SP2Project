#include "LoadTextData.h"

#include <iostream>
#include <fstream>

void LoadTextData(const char *file_path, float CharData[256])
{
	std::ifstream fileStream(file_path, std::ios::binary);
	if (!fileStream.is_open()) {
		std::cout << "Impossible to open " << file_path << ". Are you in the right directory ?\n";
		return (void)0;
	}
	int charWidth = 0;
	while (!fileStream.eof()) {
		char buf[256];
		fileStream.getline(buf, 256);
		if (strncmp("Cell Width,", buf, 11) == 0)
		{
			sscanf_s((buf + 11), "%d", &charWidth);
		}
		if (strncmp("Char", buf, 4) == 0)
		{
			unsigned int Char;
			char NumofDigits = 0;
			int value = 0;
			sscanf_s((buf + 5), "%d", &Char);
			if (Char < 10)
				NumofDigits = 1;
			else if (Char < 100)
				NumofDigits = 2;
			else
				NumofDigits = 3;

			//Char 0 Base Width,
			sscanf_s((buf + 17 + NumofDigits), "%d", &value);

			CharData[Char] = (float)((float)value / (float)charWidth);

			//Just need to store witdh values for now. there is no offsets
			if (Char == 255)
				break;
		}
	}
}