// by szh !
#include<bits/stdc++.h>
#include"server.h"
#include<Windows.h>
#define kd(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000)?1:0)
#define click(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1:0)
#define l_mouse VK_LBUTTON
#define r_mouse VK_RBUTTON
#define output_color SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0F)
using namespace std;
struct button{
	int x,y,dark_color,light_color,lighted,len;
	string name;
};
int output_speed;
void Slow_out(string s){
	for (int i=0;i<(int)(s.size());i++){
		printf("%c",s[i]);
		Sleep(output_speed);
	}
	return;
}
/*
#define      FOREGROUND_BLUE 0x1
#define     FOREGROUND_GREEN 0x2
#define       FOREGROUND_RED 0x4
#define FOREGROUND_INTENSITY 0x8
*/
void Get_color(int a){
	/* light white */
	if(a== 0) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0F);
	/* cyan */
	if(a== 1) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0B);
	/* green */
	if(a== 2) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0A);
	/* pink(win7) or purple(win10) */
	if(a== 3) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0D);
	/* red */
	if(a== 4) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0C);
	/* yellow */
	if(a== 5) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x0E);
	/* blue(win10) */
	if(a== 6) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x09);
	/* dark yellow */
	if(a== 7) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x06);
	/* white */
	if(a== 8) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x07);
	/* grey */
	if(a== 9) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x08);
	/* dark blue(win10) */
	if(a==10) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x01);
	/* dark green */
	if(a==11) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x02);
	/* dark cyan */
	if(a==12) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x03);
	/* dark red */
	if(a==13) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x04);
	/* purple(win7) dark purple(win10) */
	if(a==14) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x05);
	/* black */
	if(a==15) SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),0x00);
}
void Place(const int x, const int y) {//先列后行 
	COORD PlaceCursorHere;
	PlaceCursorHere.X = x;
	PlaceCursorHere.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), PlaceCursorHere);
	return;
}
void Noedit(){
	HANDLE hStdin=GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode;
	GetConsoleMode(hStdin,&mode);
	mode&=~ENABLE_QUICK_EDIT_MODE;
	mode&=~ENABLE_INSERT_MODE;
	mode&=~ENABLE_MOUSE_INPUT;
	SetConsoleMode(hStdin,mode);
}
void HideCursor(){
	CONSOLE_CURSOR_INFO cur={1,0};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE),&cur);
}
void Get_pos(POINT &pt){
    HWND hwnd=GetForegroundWindow();
    GetCursorPos(&pt);
    ScreenToClient(hwnd,&pt);
	pt.y=pt.y/16,pt.x=pt.x/8;
	return;
}
button New_button(int X,int Y,int Dark_Color,int Light_Color,string str){
	button b;
	b.x=X,b.y=Y;
	b.dark_color=Dark_Color;
	b.light_color=Light_Color;
	b.name=str;
	b.len=str.length();
	b.lighted=2;
	Place(X,Y);
	Get_color(Dark_Color);
	printf("%s",str.c_str());
	output_color;
	return b;
}
bool Click_button(button &btn){
    POINT pt;
    Get_pos(pt);
	if((pt.x>btn.x+btn.len-0.75||pt.x<btn.x-0.25||pt.y<btn.y-0.25||pt.y>btn.y+0.25)&&(btn.lighted==1||btn.lighted==2)){
	    Get_color(btn.dark_color); 
		Place(btn.x,btn.y),printf("%s",btn.name.c_str());
		output_color;
		btn.lighted=0;
	}
    if((pt.x<=btn.x+btn.len-0.75&&pt.x>=btn.x-0.25&&pt.y<=btn.y+0.25&&pt.y>=btn.y-0.25)){
    	if(btn.lighted==0||btn.lighted==2){
	    	Get_color(btn.light_color); 
	    	Place(btn.x,btn.y),printf("%s",btn.name.c_str());
	    	output_color;
    		btn.lighted=1;
		}
		if(click(l_mouse)){
			return 1;
		}
	}
	return 0;
}
// by lzj
bool Click_out(int rectEside, int rectNside, int rectWside, int rectSside){
	POINT pt;
	Get_pos(pt);
	return (click(l_mouse) && ((pt.x < rectWside) || (pt.x > rectEside) || (pt.y < rectNside) || (pt.y > rectSside)));
}
