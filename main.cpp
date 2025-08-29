#include"gameheader.h"
using namespace std;
button play, server[2], mapcreator, ext;
button singleplayer, multiplayer, bck, smap, cmap;
bool ssed; const int clickTimeDelay = 30;
void ServerSC(bool t){
    if(t) system("start .\\conerals_server\\main_server.exe");
    else system("taskkill /im main_server.exe /f"), system("taskkill /im generals_server.exe /f");
}
void PlayGame(){
    bool needReflush = true;
    while(true){
        if (needReflush){
            system("cls");
            system("mode con cols=101 lines=35");
            Place(46, 6);
            printf("Play");
			singleplayer.lighted = 2;
			multiplayer.lighted = 2;
			bck.lighted = 2;
			needReflush = false;
		}
		Sleep(clickTimeDelay);
		if(Click_button(bck)) break;
		if(Click_button(singleplayer)) system("cls"), system(".\\conerals_client\\main.exe"), needReflush = 1;
        if(Click_button(multiplayer)) system("cls"), system(".\\conerals_client\\client.exe"), needReflush = 1;
    }
    Sleep(80);
}
inline string input(string s){
    cout << s << '\n';
    string res;
    cin >> res;
    return res;
}
void CreateMap(){
    bool needReflush = true;
    string name;
    while(true){
        if (needReflush){
            system("cls");
            system("mode con cols=101 lines=35");
            Place(42, 6);
            printf("Map Creator");
			smap.lighted = 2;
			cmap.lighted = 2;
			bck.lighted = 2;
			needReflush = false;
		}
		Sleep(clickTimeDelay);
		if(Click_button(bck)) break;
		if(Click_button(smap)) system("cls"), system(("mapcreator.exe " + input("Please input the map's name:")).c_str()), needReflush = 1;
        if(Click_button(cmap)) system("cls"), system("mapcreator.exe"), needReflush = 1;
    }
    Sleep(80);
}
int main(){
    system("title Generals");
	Noedit(); HideCursor();
	system("mode con cols=101 lines=35");
	singleplayer = New_button(42, 13, 0, 2, "Singleplayer");
	multiplayer = New_button(42, 20, 0, 2, "Multiplayer");
	bck = New_button(46, 27, 0, 4, "Back");
	smap = New_button(43, 13, 0, 2, "Edit Map");
	cmap = New_button(42, 20, 0, 2, "Create Map");
	system("cls");
	play = New_button(46, 6, 0, 2, "Play");
	server[1] = New_button(42, 13, 0, 2, "Close Server");
	server[0] = New_button(42, 13, 0, 2, "Start Server");
	mapcreator = New_button(42, 20, 0, 2, "Map Creator");
	ext = New_button(46, 27, 0, 4, "Exit");
	bool needReflush = false;
	while (true){
		if (needReflush){
            system("cls");
            system("mode con cols=101 lines=35");
			play.lighted = 2;
			server[ssed].lighted = 2;
			mapcreator.lighted = 2;
			ext.lighted = 2;
			needReflush = false;
		}
		Sleep(clickTimeDelay);
		if(Click_button(ext)) break;
		if(Click_button(play)) PlayGame(), needReflush = true;
		if(Click_button(mapcreator)) CreateMap(), needReflush = true;
		if(Click_button(server[ssed])) ssed ^= 1, ServerSC(ssed), needReflush = true;
	}
    return 0;
}
