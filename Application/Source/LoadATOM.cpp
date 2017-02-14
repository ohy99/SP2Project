#include "LoadATOM.h"
#include <fstream>
#include <string>


//Capable of doing animation for 1 object only
bool LoadAtom(const char *file_path, MS* modelStack, unsigned atThisFrame)
{
	std::ifstream fileStream(file_path, std::ios::binary);
	if (!fileStream.is_open()) {
		std::cout << "Impossible to open " << file_path << ". Are you in the right directory ?\n";
		return 0;
	}

	unsigned fps = 0;
	std::string linearUnit = "cm";
	std::string angularUnit = "deg";
	//unsigned startTime = 0;//start frame
	//unsigned endTime = 30;//end frame animation

	std::string atfSTR = std::to_string(atThisFrame);
	if (atfSTR.size() < 2)
		atfSTR += " ";
	char const *atf = atfSTR.c_str();

	enum TransformTypes
	{
		tx,
		ty,
		tz,
		rx,
		ry,
		rz,
		sx,
		sy,
		sz,
		count
	};
	TransformTypes atTransformation = count;
	//bool transform[count] = { 0, };
	bool isCheckingTransform = false;
	float transformValues[count] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
	bool isCheckingKey = false;
	char previousLine[256];

	while (!fileStream.eof()) {
		char buf[256];
		fileStream.getline(buf, 256);
		if (strncmp("timeUnit ", buf, 9) == 0)
		{
			char FPSType[10];
			sscanf_s((buf + 9), "%s", FPSType, (unsigned)_countof(FPSType));

			if (FPSType == "ntsc")
				fps = 30;
			//Add more types if needed
		}
		//if (strncmp("dagNode {", buf, 9) == 0) Ignore dagnode for now

		if (strncmp("  anim translate.translateX", buf, 27) == 0)
		{
			atTransformation = tx;
			isCheckingTransform = true;
			//transform[tx] = true;
			continue;
		}
		else if (strncmp("  anim translate.translateY", buf, 27) == 0)
		{
			atTransformation = ty;
			isCheckingTransform = true;
			continue;
		}
		else if (strncmp("  anim translate.translateZ", buf, 27) == 0)
		{
			atTransformation = tz;
			isCheckingTransform = true;
			continue;
		}
		else if (strncmp("  anim rotate.rotateX", buf, 21) == 0)
		{
			atTransformation = rx;
			isCheckingTransform = true;
			continue;
		}
		else if (strncmp("  anim rotate.rotateY", buf, 21) == 0)
		{
			atTransformation = ry;
			isCheckingTransform = true;
			continue;
		}
		else if (strncmp("  anim rotate.rotateZ", buf, 21) == 0)
		{
			atTransformation = rz;
			isCheckingTransform = true;
			continue;
		}
		else if (strncmp("  anim scale.scaleX", buf, 21) == 0)
		{
			atTransformation = sx;
			isCheckingTransform = true;
			continue;
		}
		else if (strncmp("  anim scale.scaleY", buf, 21) == 0)
		{
			atTransformation = sy;
			isCheckingTransform = true;
			continue;
		}
		else if (strncmp("  anim scale.scaleZ", buf, 21) == 0)
		{
			atTransformation = sz;
			isCheckingTransform = true;
			continue;
		}


		if (isCheckingTransform)
		{
			if (strncmp("    keys {", buf, 10) == 0)
			{
				isCheckingKey = true;
				continue;
			}
			if (isCheckingKey == true)
			{
				unsigned int frameNum = 0;
				static float defaultValue = 0;
				sscanf_s((buf + 6), "%d", &frameNum);
				if (strncmp("0", buf + 6, 1) == 0)//6 whitespaces, checking the default value
					sscanf_s((buf + 8), "%f", &defaultValue);
				
				if (strncmp(atf, buf + 6, 2) == 0)//6 whitespaces
				{
					float value;
					bool numOfBuf = 0;
					if (atThisFrame >= 10)
						numOfBuf = 1;
					sscanf_s((buf + 8 + numOfBuf), "%f", &value);
					transformValues[atTransformation] = (value);// -defaultValue);
					isCheckingTransform = false;
					isCheckingKey = false;
					defaultValue = 0.0f;
					continue;
				}
				if (atThisFrame < frameNum)//if frame needed is less than current frame
				{
					float value;
					bool numOfBuf = 0;
					if (frameNum >= 10)
						numOfBuf = 1;
					sscanf_s((buf + 8 + numOfBuf), "%f", &value);

					unsigned int prevKeyFrame = 0;
					bool numOfprevKFBuf = 0;
					float prevKeyFrameValue = 0.0f;
					sscanf_s((previousLine + 6), "%d", &prevKeyFrame);
					if (prevKeyFrame >= 10)
						numOfprevKFBuf = 1;
					sscanf_s((previousLine + 8 + numOfprevKFBuf), "%f", &prevKeyFrameValue);

					transformValues[atTransformation] = prevKeyFrameValue + (float)((float)((float)(atThisFrame - prevKeyFrame) / (float)(frameNum - prevKeyFrame)) * (float)(value - prevKeyFrameValue));// -defaultValue;

					isCheckingTransform = false;
					isCheckingKey = false;
					defaultValue = 0.0f;
					continue;
				}
				strncpy(previousLine, buf, 256);
			}
		}
	}
	modelStack->Translate(transformValues[tx], transformValues[ty], transformValues[tz]);
	modelStack->Rotate(transformValues[rz], 0, 0, 1);
	modelStack->Rotate(transformValues[ry], 0, 1, 0);
	modelStack->Rotate(transformValues[rx], 1, 0, 0);
	modelStack->Scale(transformValues[sx], transformValues[sy], transformValues[sz]);

	return 1;
}