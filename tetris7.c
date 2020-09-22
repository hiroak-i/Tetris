#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<sys/types.h>
#include<signal.h>
#include<errno.h>
#include<termios.h>
#include<unistd.h>

extern int errno;
struct termios otty,ntty;
int kbhit(void); //キー入力の確認
int getch(void); //キー入力一文字読み込み
int tinit(void); //端末初期化

#define clearScreen() printf("\033[2J")
#define setPosition(x,y) printf("\033[%d;%dH",(y)+1,(x)*2+1)
#define cursolOn() printf("\033[?25h") //カーソル表示
#define cursolOff() printf("\033[?25l") //カーソル非表示
#define WIDTH 10
#define HEIGHT 24

#define setCharColor(n) printf("\033[3%dm",(n))
#define setBackColor(n) printf("\033[4%dm",(n))
#define BLACK 0
#define RED 0
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENDA 5
#define CYAN 6
#define WHITE 7
#define DEFAULT 9

#define setAttribute(n) printf("\033[%dm",(n))
#define NORMAL 0 //通常
#define BLIGHT 1 //明るく
#define DIM 2 //暗く
#define UNDERBAR 4 //下線
#define BLINK 5 //点滅
#define REVERSE 7 // 明暗反転
#define HIDE 8 //隠れ（見えない）
#define STRIKE 9 //取り消し線

#define BLOCK_SIZE 4
#define BLOCK_NUM 7


typedef struct cell {
	char c;
	int charcolor;
	int backcolor;
	int attribute;
} Cell;


int wait(int msec); //待ち関数
void initialize(void); //画面の初期化
void reset(void); //画面の復元
int checkRenge(Cell a, int x, int y); //座標範囲チェック
int printCell(Cell c, int x, int y); //セル表示
int clearCell(Cell c, int x, int y); //セル削除
void copyBlock(Cell src[BLOCK_SIZE][BLOCK_SIZE],Cell dst[BLOCK_SIZE][BLOCK_SIZE]);
int printBlock(Cell block[BLOCK_SIZE][BLOCK_SIZE],int x, int y);
int clearBlock(Cell block[BLOCK_SIZE][BLOCK_SIZE],int x, int y);

Cell block_type[BLOCK_NUM][BLOCK_SIZE][BLOCK_SIZE] = {
	'\0',RED,BLACK,NORMAL,
	'\0',RED,BLACK,NORMAL,
	' ' ,RED,BLACK,REVERSE,
	'\0',RED,BLACK,NORMAL,
	'\0',RED,BLACK,NORMAL,
	' ' ,RED,BLACK,REVERSE,
	' ' ,RED,BLACK,REVERSE,
	'\0',RED,BLACK,NORMAL,
	'\0',RED,BLACK,NORMAL,
	' ' ,RED,BLACK,REVERSE,
	'\0',RED,BLACK,NORMAL,
	'\0',RED,BLACK,NORMAL,
	'\0',RED,BLACK,NORMAL,
	'\0',RED,BLACK,NORMAL,
	'\0',RED,BLACK,NORMAL,
	'\0',RED,BLACK,NORMAL,

	'\0',BLUE,BLACK,NORMAL,
	'\0',BLUE,BLACK,NORMAL,
	' ' ,BLUE,BLACK,REVERSE,
	'\0',BLUE,BLACK,NORMAL,
	'\0',BLUE,BLACK,NORMAL,
	' ' ,BLUE,BLACK,REVERSE,
	' ' ,BLUE,BLACK,REVERSE,
	'\0',BLUE,BLACK,NORMAL,
	'\0',BLUE,BLACK,NORMAL,
	' ' ,BLUE,BLACK,REVERSE,
	'\0',BLUE,BLACK,NORMAL,
	'\0',BLUE,BLACK,NORMAL,
	'\0',BLUE,BLACK,NORMAL,
	'\0',BLUE,BLACK,NORMAL,
	'\0',BLUE,BLACK,NORMAL,
	'\0',BLUE,BLACK,NORMAL,

	'\0',WHITE,BLACK,NORMAL,
	'\0',WHITE,BLACK,NORMAL,
	' ' ,WHITE,BLACK,REVERSE,
	'\0',WHITE,BLACK,NORMAL,
	'\0',WHITE,BLACK,NORMAL,
	' ' ,WHITE,BLACK,REVERSE,
	' ' ,WHITE,BLACK,REVERSE,
	'\0',WHITE,BLACK,NORMAL,
	'\0',WHITE,BLACK,NORMAL,
	' ' ,WHITE,BLACK,REVERSE,
	'\0',WHITE,BLACK,NORMAL,
	'\0',WHITE,BLACK,NORMAL,
	'\0',WHITE,BLACK,NORMAL,
	'\0',WHITE,BLACK,NORMAL,
	'\0',WHITE,BLACK,NORMAL,
	'\0',WHITE,BLACK,NORMAL,
};


int main(int argc, char *argv[])
{
	int x,y,c,count;
	Cell block[BLOCK_SIZE][BLOCK_SIZE];
	copyBlock(block_type[1],block);
	initialize();
	x=5;
	y=10;
	printBlock(block,x,y);
	count=0;

	for(count=0;count<10 ; )
	{
		if(kbhit())
		{
			clearBlock(block,x,y);
			c=getch();
			if(c==0x1b)
			{
				c = getch();
				if(c==0x5b)
				{
					c=getch();
					switch(c)
					{
						case 0x41: //UP
							break;
						case 0x42:
							break; //DOWN
						case 0x43: //RIGHT
							x++;
							break;
						case 0x44: //LEFT
							x--;
							break;
					}
				}
			}
			else
			{
				reset();
				exit(1);
			}
			count++;
			printBlock(block,x,y);
		}
	}
	reset();
}

int wait(int msec)
{
	struct timespec r = {0,msec*1000L*1000L};
	return nanosleep(&r,NULL);
}

void initialize(void){
	tinit();
	setAttribute(NORMAL);
	setBackColor(BLACK);
	setCharColor(WHITE);
	clearScreen();
	cursolOff();
}

void reset(void)
{
	setBackColor(BLACK);
	setCharColor(WHITE);
	setAttribute(NORMAL);
	clearScreen();
	cursolOn();
	tcsetattr(1,TCSADRAIN,&otty);
	write(1,"\n",1);
}

int checkRange(Cell a, int x, int y)
{
	if(a.c=='\0' || x<0 || y<0 || x>=WIDTH || y>=HEIGHT)
		return -1;
	else
		return 0;
}

int printCell(Cell c, int x, int y)
{
	if(checkRange(c,x,y)==-1)
		return -1;
	setPosition(x,y);
	setAttribute(c.attribute);
	setBackColor(c.backcolor);
	setCharColor(c.charcolor);
	printf("%c%c",c.c,c.c);
	fflush(stdout);
	return 0;
}

int clearCell(Cell c, int x, int y)
{
	if(checkRange(c,x,y)==-1)
		return -1;

	setPosition(x,y);
	setAttribute(NORMAL);
	setBackColor(BLACK);
	setCharColor(BLACK);
	printf("%c%c",c.c,c.c);
	printf("  ");
	fflush(stdout);
	return 0;
}

void copyBlock(Cell src[BLOCK_SIZE][BLOCK_SIZE], Cell dst[BLOCK_SIZE][BLOCK_SIZE])
{
	int i,j;
	for(j=0;j<BLOCK_SIZE;j++)
		for(i=0;i<BLOCK_SIZE;i++)
			dst[j][i]=src[j][i];
}


int printBlock(Cell block[BLOCK_SIZE][BLOCK_SIZE],int x, int y)
{
	int i,j;
	for(j=0;j<BLOCK_SIZE;j++)
		for(i=0;i<BLOCK_SIZE;i++)
			printCell(block[j][i], i+x,j+y);
	return 0;
}

int clearBlock(Cell block[BLOCK_SIZE][BLOCK_SIZE],int x, int y)
{
	int i,j;
	for(j=0;j<BLOCK_SIZE;j++)
		for(i=0;i<BLOCK_SIZE;i++)
			clearCell(block[j][i], i+x,j+y);
	return 0;
}

int kbhit(void)
{
	int ret;
	fd_set rfd;
	struct timeval timeout = {0,0};
	FD_ZERO(&rfd);
	FD_SET(0,&rfd);
	ret = select(1,&rfd,NULL,NULL,&timeout);
	if (ret==1)
		return 1;
	else
		return 0;
}

int getch(void)
{
	unsigned char c;
	int n;
	while((n=read(0,&c,1)) < 0 && errno == EINTR);

	if (n==0)
		return -1;
	else
		return (int)c;
}

static void onsignal(int sig)
{
	signal(sig,SIG_IGN);
	switch(sig){
		case SIGINT:
		case SIGQUIT:
		case SIGTERM:
		case SIGHUP:
			exit(1);
			break;
	}
}

int tinit(void)
{
	if(tcgetattr(1,&otty) < 0)
		return -1;
	ntty = otty;
	ntty.c_iflag &= ~(INLCR|ICRNL|IXON|IXOFF|ISTRIP);
	ntty.c_oflag &= ~OPOST;
	ntty.c_lflag &= ~(ICANON|ECHO);
	ntty.c_cc[VMIN] = 1;
	ntty.c_cc[VTIME] = 0;
	tcsetattr(1,TCSADRAIN,&ntty);
	signal(SIGINT,onsignal);
	signal(SIGQUIT,onsignal);
	signal(SIGTERM,onsignal);
	signal(SIGHUP,onsignal);
}