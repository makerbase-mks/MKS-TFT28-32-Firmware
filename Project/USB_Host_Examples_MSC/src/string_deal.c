#include "string_deal.h"


void cutFileName(char *path, int len, int bytePerLine,  char *outStr)
{
	char *tmpFile;
	char *strIndex1 = 0, *strIndex2 = 0, *beginIndex;
	char secSeg[10] = {0};
	
	if(path == 0 || len <= 3 || outStr == 0)
	{
		return;
	}
	
	tmpFile = path;
	strIndex1 = (char *)strrchr(tmpFile, '/');
	strIndex2 = (char *)strrchr(tmpFile, '.');

	if(strIndex1 != 0)
	{
		beginIndex = strIndex1 + 1;	
	}
	/*if((strIndex1 != 0) && (strIndex2 != 0) && (strIndex1 < strIndex2))
	{
		beginIndex = strIndex1 + 1;			
	}*/
	else
	{
		beginIndex = tmpFile;
	}

	if(strIndex2 == 0 ||  (strIndex1 > strIndex2))	//not gcode file
	{
		if(strlen(beginIndex) > len)
		{
			strncpy(outStr, beginIndex, len);
		
		}
		else
		{
			strcpy(outStr, beginIndex);
		}
	}
	else //gcode file
	{
		if(strIndex2 - beginIndex > (len - 2))
		{
			//tan0929 有中文需要2的倍数
			//strncpy(outStr, beginIndex, len - 3);
			strncpy(outStr, beginIndex, len - 4);
			strcat(outStr, "~.g");
		
		}
		else
		{
			strncpy(outStr, beginIndex, strIndex2 - beginIndex + 1);
			strcat(outStr, "g");
		}
		
	}
	if(strlen(outStr) > bytePerLine)
	{
		
		strcpy(secSeg, &outStr[bytePerLine]);
		outStr[bytePerLine] = '\n';
		outStr[bytePerLine + 1] = '\0';
		strcat(outStr, secSeg);
		
	}
}
	

