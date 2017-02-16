#include "LoadATOM.h"
#include <fstream>
#include <string>

/*************************************************************
*	@param:													 *
*	const char*: GETS FILE PATH FOR .ATOM					 *
*	MS*: GET MODEL OBJ TO ANIMATE							 *
*	double: TIME ELAPSED FOR ANIMATION						 *
*	const string: SETTING OBJ PART NAME						 *
*															 *
*	@return: bool											 *
*	RETURNS TRUE ONCE DONE WITH LOADING OF .ATOM AT FILEPATH *
*	RETURNS FALSE IF CANNOT LOAD .ATOM AT FILEPATH			 *
*	
 Formula for reseting time in render
*if (timeelapsed >= ((double)FRAMELIMIT * (double)((double)1 / (double)NUMBER OF FRAMEPERSEC)))
*		timeelapsed = 0;
**************************************************************/
bool LoadAtom(const char *file_path, MS* modelStack, double* timeElapsed, const std::string& dagNode)
{
	unsigned fps = 0;//INITIALIZING FPS

	std::ifstream fileStream(file_path, std::ios::binary); //OPENING OF .ATOM FILE
	if (!fileStream.is_open()) {//CHECKING IF THERE IS SUCH FILE NAME
		std::cout << "Impossible to open " << file_path << ". Are you in the right directory ?\n";
		return 0;
	}


	std::string atfSTR;
	char const *atf;
	unsigned atThisFrame;

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
	TransformTypes atTransformation = count;//SETTING TRANSFROM TYPE
	
	bool isCheckingTransform = false;//CHECKING FOR TRANSFORMATION
	float transformValues[count] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };//STORING OF TX, TY, TZ, RX, RY, RZ, SX, SY, SZ TRANSFORMATION VALUES
	bool isCheckingKey = false;
	bool isCheckingDagNode = false;
	bool atDagNode = false;
	char previousLine[256];
	char dagNodeStr[128];
	unsigned checkTransform = 0;
	unsigned endTimeFrame = 0;

	while (!fileStream.eof()) {//CHECKING FOR END OF FILE
		char buf[256];
		fileStream.getline(buf, 256);//GETTING THE FIRST LINE
		if (strncmp("timeUnit ", buf, 9) == 0)//COMPARING IF BUF STRING IS SAME AS timeUnit
		{
			char FPSType[10];
			sscanf_s((buf + 9), "%s", FPSType, (unsigned)_countof(FPSType));//READS DATA IN TEXT FROM BUFFER FOR FPS TYPE AND SETTING IT INTO FPSType

			//FPSType == "ntsc;"
			if (strncmp(FPSType, "ntsc;", 5) == 0)
				fps = 30;//SETTING OF FPS FOR REQUIRED ANIMATION SPEED
			//Add more types if needed

		}

		if (strncmp("endTime ", buf, 8) == 0)//get the endTime Frame
		{
			sscanf_s((buf + 8), "%d", &endTimeFrame);
			
			if (*timeElapsed > (double)((double)endTimeFrame / (double)fps))//IF TIME ELAPSED IS MORE THAN THE LAST FRAME, RESET
				*timeElapsed = 0.0;

			atThisFrame = (unsigned)(*timeElapsed / ((double)((double)1 / (double)fps)));//GETTING THE REQUIRED FRAME NUMBER FOR TRANSFORMATION USING TIME
			atfSTR = std::to_string(atThisFrame);//CONVERTING FRAME STARTING NUMBER TO STRING
			if (atfSTR.size() < 2)
				atfSTR += " ";//ADD A SPACE IF IT IS LESS THAN 2 DIGITS
			atf = atfSTR.c_str();//CONVERTING STRING TO CHAR POINTER

		}

		if (strncmp("dagNode {", buf, 9) == 0 && !isCheckingDagNode)//CHECKING IF BUF STRING IS SAME AS dagNode
		{
			isCheckingDagNode = true;
			continue;
		}
		if (isCheckingDagNode && !atDagNode)
		{

			sscanf_s((buf + 2), "%s", dagNodeStr, (unsigned)_countof(dagNodeStr));
			if (dagNodeStr == dagNode)
				atDagNode = true;
			else
				isCheckingDagNode = false;
		}

		if (atDagNode == true)
		{
			if (strncmp("  anim translate.translateX", buf, 27) == 0) //COMPARING STRING AND BUF TO SEE IF THEYRE THE SAME
			{
				atTransformation = tx;//SET TRANSFORMATION TYPE
				isCheckingTransform = true;
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
			else if (strncmp("  anim scale.scaleX", buf, 19) == 0)
			{
				atTransformation = sx;
				isCheckingTransform = true;
				continue;
			}
			else if (strncmp("  anim scale.scaleY", buf, 19) == 0)
			{
				atTransformation = sy;
				isCheckingTransform = true;
				continue;
			}
			else if (strncmp("  anim scale.scaleZ", buf, 19) == 0)
			{
				atTransformation = sz;
				isCheckingTransform = true;
				continue;
			}

			if (checkTransform == 9)
				break;

			if (isCheckingTransform)
			{
				if (strncmp("    keys {", buf, 10) == 0)//CHECKING IF BUF IS AT THE KEYS SECTION BY COMPARING STRING
				{
					isCheckingKey = true;
					continue;
				}
				if (isCheckingKey == true)
				{
					unsigned int frameNum = 0;
					//static float defaultValue = 0;//default value no need for now
					sscanf_s((buf + 6), "%d", &frameNum);//SETTING THE CURRENT FRAME NUMBER TO framNum


					//if (strncmp("0", buf + 6, 1) == 0)//6 whitespaces, checking the default value
					//	sscanf_s((buf + 8), "%f", &defaultValue);

					if (strncmp(atf, buf + 6, 2) == 0)//6 whitespaces,  CHECKING IF buf LOCATION IS AT FRAME
					{
						float value;
						bool numOfBuf = 0;
						if (atThisFrame >= 10)//IF FRAME INPUT IS MORE THEN OR EQUALS THAN 10
							numOfBuf = 1;
						sscanf_s((buf + 8 + numOfBuf), "%f", &value);//GETTING VALUE FOR TRANSFORMATION
						transformValues[atTransformation] = (value); //SETTING VALUE INTO TRANSFORMATION
				
						isCheckingTransform = false;
						isCheckingKey = false;
						//defaultValue = 0.0f;
						checkTransform++;
						continue;
					}
					if (atThisFrame < frameNum)//if frame needed is less than current frame, INTERPOLATION
					{
						float value;
						bool numOfBuf = 0;
						if (frameNum >= 10)
							numOfBuf = 1;
						sscanf_s((buf + 8 + numOfBuf), "%f", &value);

						unsigned int prevKeyFrame = 0;
						bool numOfprevKFBuf = 0;
						float prevKeyFrameValue = 0.0f;
						sscanf_s((previousLine + 6), "%d", &prevKeyFrame);//SETTING PREV KEY FRAME
						if (prevKeyFrame >= 10)
							numOfprevKFBuf = 1;
						sscanf_s((previousLine + 8 + numOfprevKFBuf), "%f", &prevKeyFrameValue);//SETTING PREV KEY FRAME VALUE
						//PASSING TRANSFORMATION VALUE FROM PREV KEY FRAME INTO transformValues

						transformValues[atTransformation] = prevKeyFrameValue + (float)((float)((float)(atThisFrame - prevKeyFrame) / (float)(frameNum - prevKeyFrame)) * (float)(value - prevKeyFrameValue));// -defaultValue;

						isCheckingTransform = false;
						isCheckingKey = false;
						//defaultValue = 0.0f;
						checkTransform++;
						continue;
					}
					strncpy(previousLine, buf, 256);//COPYING buf INTO previousLine, ALL 256 POKEMO.. OF THEM. FOR INTERPOLATION USE
				}
			}
		}
	}
	if (atDagNode == false)//IF CANNOT FIND OBJECT PIECE
		return 0;

	modelStack->Translate(transformValues[tx], transformValues[ty], transformValues[tz]);
	modelStack->Rotate(transformValues[rz], 0, 0, 1);
	modelStack->Rotate(transformValues[ry], 0, 1, 0);
	modelStack->Rotate(transformValues[rx], 1, 0, 0);
	modelStack->Scale(transformValues[sx], transformValues[sy], transformValues[sz]);

	return 1;
}