// generals client
// by zzn
// web connecter is powered by httplib
// game engine is powered by szh
// multiplayer only
// the singleplayer version is by lzj
// some functions is from singleplayer version

// headers
#include "gameheader.h"
#include "httplib.h"
#define multiplayers true
using namespace httplib;

// for user
string name;

// for server
string serverIp;
Client server("www.msftconnecttest.com");

// from singleplayers version
mt19937 randNumber(time(nullptr) ^ 451844);

const int clickTimeDelay = 30;

const int deltaX[9] = {0, -1, 0, 1, -1, -1, 1, 1, 0};
const int deltaY[9] = {1, 0, -1, 0, 1, -1, -1, 1, 0};

const int maxPlayers = 21, playerTotal = 8;

const int mapRows = 9;
const int mapCols = 28;
const int maxMapSize = 50;

const int minMapRows = 1, maxMapRows = 15;
const int minMapCols = 1, maxMapCols = 40;
const int minPlayerTotal = 2, maxPlayerTotal = 16;

// for showing players
const int playerColors[maxPlayers] = {8, 4, 1, 2, 3, 7, 5, 6, 13, 10, 11, 12, 9, 14, 8, 0, 15};

struct block{
	int army, belongTo;
	char type;
	// K : king
	// C : castle
	// L : land
	// M : mountain
};

struct movement{
	int x, y, dir;
	// x, y : position
	// dir : direction
};

int mapSize;

int currentAsking;

string playerName[maxPlayers + 5];
bool alreadyDead[maxPlayers];
int armyTotal[maxPlayers];
int landTotal[maxPlayers];
int humanID;

block generalsMap[maxMapSize][maxMapSize];

// out of the map
bool OutOfTheMap(int askX, int askY){
	return (askX < 1 || mapRows < askX || askY < 1 || mapCols < askY);
}

// ask
block Ask(int askX, int askY){
	block answerBlock;
	answerBlock.type = ' ';
	answerBlock.belongTo = -1;
	answerBlock.army = -1;
	// This block doesn't exist.
	if (OutOfTheMap(askX, askY)){
		answerBlock.type = 'x';
		return answerBlock;
	}
	// visible ?
	for (int currentDirection = 0; currentDirection < 9; ++currentDirection){
		int nextX = askX + deltaX[currentDirection];
		int nextY = askY + deltaY[currentDirection];
		if (OutOfTheMap(nextX, nextY)){
			continue;
		}
		if (generalsMap[nextX][nextY].belongTo == currentAsking){
			// visible
			answerBlock = generalsMap[askX][askY];
			return answerBlock;
		}
	}
	// not visible
	if (generalsMap[askX][askY].type == 'M' || generalsMap[askX][askY].type == 'C'){
		answerBlock.type = 'B';
	}
	else{
		answerBlock.type = '?';
	}
	return answerBlock;
}
// show the players
/*
| X : xxxxx... |
| Army : xxxxx |
| Land : xxxxx |
*/
void ShowPlayers(){
	// show "|"
	output_color;
	Place(0, 1);printf("|");
	Place(0, 2);printf("|");
	Place(0, 3);printf("|");
	// set X position
	int currentShowX = 2;
	for (int currentPlayer = 1; currentPlayer <= playerTotal; ++currentPlayer){
		// get color
		Get_color(playerColors[currentPlayer]);
		// show name
		Place(currentShowX, 1);
		printf("%c : ", (char)(currentPlayer | 64));
		string currentName = playerName[currentPlayer];
		int currentLength = (int)(currentName.length());
		if (currentLength <= 8){
			printf("%s", currentName.c_str());
		}
		else{
			for (int currentChar = 0; currentChar < 5; ++currentChar){
				printf("%c", currentName[currentChar]);
			}
			printf("...");
		}
		// show army and land
		Place(currentShowX, 2);
		printf("Army : %5d", armyTotal[currentPlayer]);
		Place(currentShowX, 3);
		printf("Land : %5d", landTotal[currentPlayer]);
		// show "|"
		output_color;
		Place(currentShowX + 13, 1);printf("|");
		Place(currentShowX + 13, 2);printf("|");
		Place(currentShowX + 13, 3);printf("|");
		// update "currentShowX"
		currentShowX += 15;
	}
	// reset color
	output_color;
	return;
}
// show the map
/*
+---+
|   |
|   |
+---+
*/
void ShowMapEdges(){
	Place(0, 4);
	Get_color(8);
	for (int j = 1; j <= mapCols; ++j){
		printf("+---");
	}
	printf("+");
	for (int i = 1; i <= mapRows; ++i){
		printf("\n");
		for (int j = 1; j <= mapCols; ++j){
			printf("|   ");
		}
		printf("|\n");
		for (int j = 1; j <= mapCols; ++j){
			printf("|   ");
		}
		printf("|\n+");
		for (int j = 1; j <= mapCols; ++j){
			printf("---+");
		}
	}
	output_color;
	Place(0, 0);
	printf("Turns :");
	return;
}

void PrintBlockType(char currentType, int currentBelong, char whichSide){
	if (currentType == 'K'){
		printf("$");// king
	}
	else if (currentType == 'C'){
		if (currentBelong != 0){
			// possessed castle
			if (whichSide == 'L'){
				printf("(");
			}
			else{
				printf(")");
			}
		}
		else{
			// empty castle
			if (whichSide == 'L'){
				printf("<");
			}
			else{
				printf(">");
			}
		}
	}
	else if (currentType == 'L'){
		printf(" ");// normal land
	}
	return;
}
// show the top of the block
void ShowBlockTop(block currentBlock){
	char currentType = currentBlock.type;
	// special type
	if (currentType == 'M'){
		printf("###");
		return;
	}
	if (currentType == 'B'){
		printf("???");
		return;
	}
	if (currentBlock.type == '?'){
		printf("   ");
		return;
	}
	// belong
	int currentBelong = currentBlock.belongTo;
	Get_color(playerColors[currentBelong]);
	PrintBlockType(currentType, currentBelong, 'L');
	if (currentBelong != 0){
		printf("%c", (char)(currentBelong | 64));
	}
	else if (currentType == 'C'){
		printf("M");
	}
	else{
		printf(" ");
	}
	PrintBlockType(currentType, currentBelong, 'R');
	output_color;
	return;
}
// show the bottom of the block
void ShowBlockBottom(block currentBlock){
	// special type
	if (currentBlock.type == 'M'){
		printf("###");
		return;
	}
	if (currentBlock.type == 'B'){
		printf("???");
		return;
	}
	if (currentBlock.type == '?'){
		printf("   ");
		return;
	}
	// army
	int currentArmy = currentBlock.army;
	if (currentArmy != 0){
		// set color
		Get_color(playerColors[currentBlock.belongTo]);
		// too large
		if (currentArmy > 999){
			// calc digits
			int theDigitsOfArmy = 0;
			while (currentArmy > 9){
				++theDigitsOfArmy;
				currentArmy /= 10;
			}
			// special output
			printf("%de%d", currentArmy, theDigitsOfArmy);
		}
		else{
			// normal output
			printf("%3d", currentArmy);
		}
		// reset color
		output_color;
	}
	else{
		printf("   ");
	}
	return;
}
// check visible
bool CheckVisible(block checkBlock){
	return (checkBlock.type == 'K' || checkBlock.type == 'M' || checkBlock.type == 'C' || checkBlock.type == 'L');
}
// human view
void ShowGeneralsMapOfHumanView(){
	currentAsking = humanID;
	// set color
	Get_color(8);
	for (int i = 1; i <= mapRows; ++i){
		for (int j = 1; j <= mapCols; ++j){
			Place(j * 4 - 3, i * 3 + 2);
			ShowBlockTop(generalsMap[i][j]);
		}
		for (int j = 1; j <= mapCols; ++j){
			Place(j * 4 - 3, i * 3 + 3);
			ShowBlockBottom(generalsMap[i][j]);
		}
	}
	// reset color
	output_color;
	return;
}
#include"humanOptions.h"
// end
// multiplayer version only
button b;
Client gameServer("www.msftconnecttest.com");
bool st = 0;
bool gamestart(){
	server.Get("/play");
	Result s = server.Get("/port/" + name);
	if(s->body == "") return 0;
	gameServer = Client(serverIp + ":" + (s->body));
	return st = 1;
}
void fs(){
    while(!st){
        if(Click_button(b)){
            server.Get("/game");
            break;
        }
        Sleep(clickTimeDelay);
    }
}
void Init(){
    cout << "Please input your name:\n";
    cin >> name;
    cout << "Please input server's address:\n";
    cin >> serverIp;
    server = Client(serverIp + ":3232");
	server.Get("/join/" + name);
    system("cls");
	system("mode con cols=101 lines=35");
    HideCursor(); Noedit();
}
void GetStatus(movement themove){
    string t = to_string(themove.x);
    t += '+' + to_string(themove.y);
    t += '+' + to_string(themove.dir);
    Result res = gameServer.Get("/" + name + "/" + t);
    stringstream ss; ss << (res->body);
    for(int i = 1; i <= mapRows; i++)
        for(int j = 1; j <= mapCols; j++)
            ss >> generalsMap[i][j].type
                >> generalsMap[i][j].army
            >> generalsMap[i][j].belongTo;
    for(int i = 1; i <= playerTotal; i++) ss >> playerName[i] >> armyTotal[i] >> landTotal[i] >> alreadyDead[i];
    ss >> humanID;
}
void Game(){
	b = New_button(46, 18, 0, 2, "Force Start");
    Place(46, 10); printf("Waiting");
	thread(fs).detach();
    while(!gamestart());
	system("mode con cols=125 lines=32");
    system("cls");
    ShowMapEdges();
    humanOptions::Init();
    int turns = 0;
    GetStatus({-1, -1, -1});
    do{
        Sleep(10); Place(0, 0); printf("Turns : %d%c", turns / 2, " ."[turns & 1]);
        humanOptions::theStartTime = clock();
        movement tm;
        tm = humanOptions::Move();
        GetStatus(tm);
        ShowGeneralsMapOfHumanView();
        ShowPlayers(); turns++;
    }while(!alreadyDead[humanID]);
    server.Get("/exit/" + name);
}
// end
// main
int main(){
    Init();
    Game();
    return 0;
}
