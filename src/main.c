#include<stdio.h>
#include<string.h>

enum program_lang
{
	PL_C,
	PL_TEXT_ENGLISH,
};

struct file
{
	char*path;
	enum program_lang lang;
};

int main(int argc,char**argv)
{
	struct file file;
	switch(argc)
	{
	case 2:
		file.path=argv[1];
		char*extension=strrchr(file.path,'.');
		if(!extension)
		{
			file.lang=PL_TEXT_ENGLISH;
		}
		else if(strcmp(extension,"c")==0||strcmp(extension,"h")==0)
		{
			file.lang=PL_C;
		}
		else if(strcmp(extension,"txt")==0)
		{
			file.lang=PL_TEXT_ENGLISH;
		}
		else
		{
			file.lang=PL_TEXT_ENGLISH;
		}
		break;
	case 3:
		file.path=argv[1];
		if(strcmp(argv[2],"C")==0)
		{
			file.lang=PL_C;
		}
		else if(strcmp(argv[2],"Text")==0)
		{
			file.lang=PL_TEXT_ENGLISH;
		}
		else
		{
			file.lang=PL_TEXT_ENGLISH;
		}
		break;
	default:
		break;
	}
}
