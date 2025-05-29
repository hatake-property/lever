#define SYSTEM SYSTEM_WINDOWS

#include<stdio.h>
#include<string.h>

#define SYSTEM_LINUX	0
#define SYSTEM_MACOS	1
#define SYSTEM_WINDOWS	2

#if SYSTEM==SYSTEM_WINDOWS
#	include<stdlib.h>
#	include<windows.h>
#elif SYSTEM==SYSTEM_UNIX
#	include<stdlib.h>
#	include<sys/ioctl.h>
#	include<termios.h>
#	include<unistd.h>
#else
#	err it is an operating system that is not supported
#endif

enum program_lang{
	PL_C,
	PL_TEXT_ENGLISH,
};

enum token_type{
	T_WORD,
	T_NUMBER,
	T_SYMBOL,
	T_BLANK,
	T_NEWLINE,
	T_NONE,
};

struct token{
	char*content;
	int size;
	enum token_type type;
};

struct file{
	char*path;
	enum program_lang lang;
	struct token*token;
	int size;
};

void free_file(struct file file);

void canonical(void);
void clear_term(void);
void get_term_size(void);
void invert_color(void);
void noncanonical(void);
void reset_color(void);

static int backcolor;
static int forecolor;
int term_row;
int term_col;

int main(int argc,char**argv){
	struct file file;
	FILE*fp;
	int c;
	switch(argc){
	case 2:
		file.path=argv[1];
		char*extension=strrchr(file.path,'.');
		if(!extension){
			file.lang=PL_TEXT_ENGLISH;
		}else if(strcmp(extension,"c")==0||strcmp(extension,"h")==0){
			file.lang=PL_C;
		}else if(strcmp(extension,"txt")==0){
			file.lang=PL_TEXT_ENGLISH;
		}else{
			file.lang=PL_TEXT_ENGLISH;
		}
		break;
	case 3:
		file.path=argv[1];
		if(strcmp(argv[2],"C")==0){
			file.lang=PL_C;
		}else if(strcmp(argv[2],"Text")==0){
			file.lang=PL_TEXT_ENGLISH;
		}else{
			file.lang=PL_TEXT_ENGLISH;
		}
		break;
	default:
		break;
	}

	fp=fopen(file.path,"r");
	file.token=malloc(sizeof(struct token)*0x10000);
	if(!file.token){
		free_file(file);
		fputs("error: no get memory\n",stderr);
		return -1;
	}
	file.size=0x10000;
	for(int i=0;i<file.size;i++){
		file.token[i].content=malloc(sizeof(char)*0x8);
		if(!file.token[i].content){
			free_file(file);
			fputs("error: no get memory\n",stderr);
			return -1;
		}
		file.token[i].size=0x8;
		file.token[i].type=T_NONE;
	}
	if(fp){
		int file_char;
		int i;
		int content_indent=0;
		while{
			if(i>=file.size-1){
				file.size+=0x10000;
				void*p=realloc(file.token,sizeof(struct token)*file.size);
				if(!p){
					free_file(file);
					fputs("error: no get memory\n",stderr);
					return -1;
				}
				for(int i=file.size-0x10000;i<file.size;i++){
					file.token[i].content=malloc(sizeof(char)*0x8);
					if(!file.token[i].content){
						free_file(file);
						fputs("error: no get memory\n",stderr);
						return -1;
					}
					file.token[i].size=0x8;
					file.token[i].type=T_NONE;
				}
			}
			if(content_indent>=file.token[i].size-1){
				file.token[i].size*=2;
				void*p=realloc(file.token[i].content,sizeof(char)*file.token[i].size);
				if(!p){
					free_file(file);
					fputs("error: no get memory\n",stderr);
					return -1;
				}
			}
			file_char=fgetc(fp);
			if(file_char==EOF){
				file.token[i].content[content_indent]='\0';
				i++;
				file.token[i].content[0]='\0';
				file.token[i].type=T_NONE;
				break;
			}else if(file_char=='\n'||file_char=='\r'){
				if(file.token[i].type!=T_NEWLINE){
					file.token[i].content[content_indent]='\0';
					content_indent=0;
					i++;
					file.token[i].type=T_NEWLINE;
				}
			}else if('0'<=file_char&&file_char<='9'){
				if(file.token[i].type!=T_NUMBER){
					file.token[i].content[content_indent]='\0';
					content_indent=0;
					i++;
					file.token[i].type=T_NUMBER;
				}
			}else if(('!'<=file_char&&file_char<='~')^(('A'<=file_char&&file_char'Z')||('a'<=file_char&&file_char'z'))){
				if(file.token[i].type!=T_SYMBOL){
					file.token[i].content[content_indent]='\0';
					content_indent=0;
					i++;
					file.token[i].type=T_SYMBOL;
				}
			}else{
				if(file.token[i].type!=T_WORD){
					file.token[i].content[content_indent]='\0';
					content_indent=0;
					i++;
					file.token[i].type=T_WORD;
				}
			}
			file.token[i].content[content_indent]=c;
			content_indent++;
		}
	}else{
		file.token[0].content[0]='\0';
		file.token[0].type=T_NONE;
	}

	get_term_size();
	reset_color();
	while(1){
		clear_term();
		invert_color();
		for(int i=0;i<term_col;i++){
			printf(" ");
		}
		invert_color();
		for(int i=0;i<term_row-3;i++){
			printf("%2d\n",i);
		}
		invert_color();
		for(int i=0;i<term_col;i++){
			printf(" ");
		}
		invert_color();
		noncanonical();
		c=getchar();
		canonical();
		if(c=='q'){
			break;
		}
	}
	canonical();
	return 0;
}

void free_file(struct file file){
	for(int i=0;i<file.size;i++){
		free(file.token[i].content);
	}
	free(file.token);
	return;
}

void canonical(void){
#if SYSTEM==SYSTEM_LINUX||SYSTEM==SYSTEM_MACOS
	struct termios tio;
	tcgetattr(STDIN_FILENO,&tio);
	tio.c_lflag|=(ICANON|ECHO);
	tcsetattr(STDIN_FILENO,TCSANOW,&tio);
#elif SYSTEM==SYSTEM_WINDOWS
	HANDLE terminal=GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(terminal,&mode);
	mode|=(ENABLE_LINE_INPUT|ENABLE_ECHO_INPUT);
	SetConsoleMode(terminal,mode);
#endif
	return;
}

void clear_term(void){
#if SYSTEM==SYSTEM_LINUX||SYSTEM==SYSTEM_MACOS
	system("clear");
#elif SYSTEM==SYSTEM_WINDOWS
	system("cls");
#endif
	return;
}

void get_term_size(void){
#if SYSTEM==SYSTEM_LINUX||SYSTEM==SYSTEM_MACOS
	struct winsize w;
	ioctl(STDOUT_FILENO,TIOCGWINSZ,&w);
	term_col=w.ws_col;
	term_row=w.ws_row;
#elif SYSTEM==SYSTEM_WINDOWS
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
	term_col=csbi.srWindow.Right-csbi.srWindow.Left+1;
	term_row=csbi.srWindow.Bottom-csbi.srWindow.Top+1;
#endif
	return;
}

void invert_color(void){
#if SYSTEM==SYSTEM_LINUX||SYSTEM==SYSTEM_MACOS
	int num=backcolor;
	backcolor=40+forecolor%10;
	forecolor=30+num%10;
	printf("\e[%dm\e[%dm",backcolor,forecolor);
#elif SYSTEM==SYSTEM_WINDOWS
	int num=backcolor;
	backcolor=forecolor*0x10;
	forecolor=num/0x10;
	HANDLE terminal=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(terminal,forecolor|backcolor);
#endif
	return;
}

void noncanonical(void){
#if SYSTEM==SYSTEM_LINUX||SYSTEM==SYSTEM_MACOS
	struct termios tio;
	tcgetattr(STDIN_FILENO,&tio);
	tio.c_lflag&=~(ICANON|ECHO);
	tcsetattr(STDIN_FILENO,TCSANOW,&tio);
#elif SYSTEM==SYSTEM_WINDOWS
	HANDLE terminal=GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(terminal,&mode);
	mode&=~(ENABLE_LINE_INPUT|ENABLE_ECHO_INPUT);
	SetConsoleMode(terminal,mode);
#endif
	return;
}

void reset_color(void){
#if SYSTEM==SYSTEM_LINUX||SYSTEM==SYSTEM_MACOS
	backcolor=40;
	forecolor=37;
	printf("\e[%dm\e[%dm",backcolor,forecolor);
#elif SYSTEM==SYSTEM_WINDOWS
	HANDLE terminal=GetStdHandle(STD_OUTPUT_HANDLE);
	backcolor=0;
	forecolor=FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED;
	SetConsoleTextAttribute(terminal,backcolor|forecolor);
#endif
	return;
}
