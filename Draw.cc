/*********************************************************
 * Author           : crazy_mad
 * Last modified    : 2017-06-09 16:44
 * Filename         : Draw.cc
 * Description      : 
 *********************************************************/

#include <Draw.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/ioctl.h>

void Draw::move(int x, int y) {
	char comm[10] = { 0 };
	sprintf(comm, "\033[%d;%dH", x, y);
	write(STDOUT_FILENO, comm, strlen(comm));
}

Draw::Draw() {
	struct winsize size_;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &size_);
	col = size_.ws_col;
	row = size_.ws_row;
}

void Draw::reset(int x1, int y1, int x2, int y2) {
	if (x1 > x2) {
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	int y = (y1 < y2 ? y1 : y2);
	int len = abs(y2-y1);
	char *cover = new char[len];
	memset(cover, ' ', len);
	for (; x1 <= x2; x1++) {
		print(x1, y, cover, len);
	}
	delete cover;
}
void Draw::line(int x1, int y1, int x2, int y2, char ch) {
	if (x1 == x2) {
		int y = (y1 < y2 ? y1 : y2);
		int len = abs(y2 -y1+1);
		char *buf = new char[len];
		memset(buf, ch, len);
		print(x1, y, buf, len);
		delete buf;
	} else if (y1 == y2) {
		char buf[1] = { ch };
		int x = (x1 < x2 ? x1 : x2);
		int len = abs(x2 - x1 + 1);
		for (int i = 0; i < len; i++, x++) {
			print(x, y1, buf, 1);
		}
	} 
}
void Draw::print(int x, int y, char* str, int len) {
	move(x, y);
	write(STDIN_FILENO, str, len);
}
void Draw::print(int x, int y, const char* format, ...) {
	char buf[256] = { 0 };
	va_list args;
	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);
	print(x, y, buf, strlen(buf));
}
int Draw::getCol() {
	return col;
}
int Draw::getRow() {
	return row;
}
void Draw::border() {
	border(1, 1, row, col);
}
void Draw::border(int x1, int y1, int x2, int y2) {
	if (x1 > x2) {
		int tmp = x1;
		x1 = x2;
		x2 = tmp;
	}
	if (y1 > y2) {
		int tmp = y1;
		y1 = y2;
		y2 = tmp;
	}
	line(x1, y1, x1, y2, '_');
	line(x2, y1, x2, y2, '_');
	line(x1+1, y1, x2, y1, '|');
	line(x1+1, y2, x2, y2, '|');
}
void Draw::nextLine() {
	write(STDIN_FILENO, "\n", 1);
}
void Draw::nextLine(const char* format, ...) {
	nextLine();
	char buf[256] = { 0 };
	va_list args;
	va_start(args, format);
	vsprintf(buf, format, args);
	va_end(args);
	write(STDOUT_FILENO, buf, strlen(buf));
}
void Draw::nextCol(char ch) {
	printf("\033[1");
	write(STDOUT_FILENO, &ch, 1);
}
