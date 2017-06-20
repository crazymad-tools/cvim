/*********************************************************
 * Author           : crazy_mad
 * Last modified    : 2017-06-20 00:28
 * Filename         : main.cc
 * Description      : 
 *********************************************************/

#include <Draw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <crazy/net/Socket.h>

class Text {
private:
	int x;			// 光标行数
	int y;			// 光标列数
	Draw edit;		// 文字编辑
	
public:
	Text() : x(1), y(1) { }
	void print(char ch) {
		//aedit.print(x, y++, &ch, 1);
		if (ch == 13) {
			x++;
			y = 1;
			edit.print(x, y, " ", 1);
			move();
		} else if (ch == 127) {
			x = (--y < 1 ? x-- : x);
			x = (x < 1 ? 1 : x);
			if (x != 1 && y == 0) {
				y = edit.getCol();
			} else if (x == 1 && y == 0) {
				y = 1;
			}
			edit.print(x, y, " ", 1);
			move();
		} else if (ch == 9) {
			y += 8;
			x = (y > edit.getCol() ? x+1 : x);
			edit.print(x, y-1, &ch, 1);
			move();
		} else if (ch == 046) {						// UP
			x = (x+1 < 1 ? 1 : x);
			move();
		} else if (ch == 050) {						// DOWN
			x --;
			move();
		} else if (ch == 045) {						// LEFT
			x = (--y < 1 ? x-- : x);
			x = (x < 1 ? 1 : x);
			if (x != 1 && y == 0) {
				y = edit.getCol();
			} else if (x == 1 && y == 0) {
				y = 1;
			}
			move();
		} else if (ch == 047) {						// RIGHT
			y++;
			x = (y > edit.getCol() ? x+1 : x);
			y = (y > edit.getCol() ? 1 : y);
			move();
		} else {
			x = (++y > edit.getCol() ? x+1 : x);
			edit.print(x, y-1, &ch, 1);
			move();
		} 
	}
	void print(char* buf) {
		
	}
	void move() {
		edit.move(x, y);
	}
};

Text text;
Draw draw;					// 界面绘制器
int epfd;					// epoll描述符
struct termios old;			
//Socket server("127.0.0.1", 8002);

char comm[1024];			// 命令行参数
int mode = 0;					// 当前模式 0: normal, 1: commd, 2: insert

void set_echo(int op) {		// 设置是够显示回显
	struct termios ters;
	tcgetattr(STDIN_FILENO, &ters);
	old = ters;
	if (op == 1) {
		ters.c_lflag |= ECHO | ECHOE | ECHOK | ECHONL;
	} else {
		ters.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
		cfmakeraw(&ters);
	}
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &ters);
}
void recover_echo() {
	tcsetattr(STDIN_FILENO, 0, &old);
}
void init_signal() {				// 初始化信号
	;
}
void init_read() {
	epfd = epoll_create(5);			// 5是随便取的数字，其实只要监听一个文件描述符
	struct epoll_event ev;			// 事件结构体
	ev.data.fd = STDIN_FILENO;		// 监听的是标准输入的描述符
	ev.events = EPOLLIN;			// 只监听可读事件就好了
	epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev);			// 将需要监听的事件加入到epoll中
}
void init() {						// 初始化程序
	system("clear");
	draw.line(1, 1, draw.getRow(), 1, '~');
	draw.move(1, 1);
	set_echo(0);
	init_signal();					// 初始化信号
	init_read();					// 初始化输入设置
	//server.Connect();
}

void clear() {						// 程序结束前的收尾工作
	//set_echo(1);
	recover_echo();
	draw.reset(1, 1, draw.getRow(), draw.getCol());
	draw.move(1, 1);
}
void quit() {
	clear();
	exit(0);
}

char get_from_epoll() {
	char buf[1];
	struct epoll_event events[1];
	epoll_wait(epfd, events, 5, -1);
	read(STDIN_FILENO, buf, 1);
	//server.Write(buf, 1);
	return buf[0];
}
void insert() {						// insert模式:2
	draw.reset(draw.getRow(), 1, draw.getRow(), draw.getCol());
	draw.print(draw.getRow(), 1, "-- Insert --");
	text.move();
	while (true) {
		char ch = get_from_epoll();
		if (ch == 27) {				// ESC
			mode = 0;
			break;
		} 
		//draw.nextCol(ch);
		text.print(ch);
	}
}
void normal() {						// normal模式:0
	while (true) {
		char ch = get_from_epoll();
		if (ch == ':') {				// 转入末行模式
			mode = 1;
			break;
		} else if (ch == 'i') {
			mode = 2;
			break;
		}
	}
}
void command() {					// 末行模式:1
	draw.reset(draw.getRow(), 1, draw.getRow(), draw.getCol());
	draw.print(draw.getRow(), 1, ":");
	char buf[1024] = { 0 };
	for (int i = 0; i < 80; i++) {
		buf[i] = get_from_epoll();
		draw.nextCol(buf[i]);
		if (buf[i] == 13) {			// 回车
			break;
		}
	}
	if (!strncmp(buf, "wq", 2)) {
		quit();
	}
	mode = 0;
}

void start() {
	while (true) {
		//char ch = get_from_epoll();
		//draw.print(1, 1, &ch, 1);
		switch (mode) {
		case 0:
			normal();
			break;
		case 1:
			command();
			break;
		case 2:
			insert();
			break;
		}
	}
}

int main() {
	init();
	start();
	clear();

	return 0;
}
