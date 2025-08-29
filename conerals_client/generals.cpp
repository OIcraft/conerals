// generals
// by lzj

// headers
#include "gameheader.h"

// variables
mt19937 randNumber(time(nullptr) ^ 451844);

const int clickTimeDelay = 30;

const int deltaX[9] = {0, -1, 0, 1, -1, -1, 1, 1, 0};
const int deltaY[9] = {1, 0, -1, 0, 1, -1, -1, 1, 0};

const int maxPlayers = 21;
string playerName[maxPlayers + 5];
int playerTotal, playerTotalOfFFA;

int AItotal;
vector<string> AInames;

// generals variables

const int maxMapSize = 50;

const int minMapRows = 1, maxMapRows = 15;
const int minMapCols = 1, maxMapCols = 40;
const int minPlayerTotal = 2, maxPlayerTotal = 16;

struct block{
	int army, belongTo;
	char type;
	// K : king
	// C : castle
	// L : land
	// M : mountain
};
int mapRows, mapCols, mapSize, tmr, tmc;
block generalsMap[maxMapSize][maxMapSize];
int kingsX[maxPlayers], kingsY[maxPlayers];

struct movement{
	int x, y, dir;
	// x, y : position
	// dir : direction
};

// for setting kings
bool possessedByKings[maxMapSize][maxMapSize];

// for setting mountains
bool floodAccessible[maxMapSize][maxMapSize];

// for showing players
const int standardPlayerColors[maxPlayers] = {8, 4, 1, 2, 3, 7, 5, 6, 13, 10, 11, 12, 9, 14, 8, 0, 15};
int playerColors[maxPlayers];

// for playing generals
int generalsTurns;

// for players
bool alreadyDead[maxPlayers];
bool playerFailed[maxPlayers];
map<string, bool> AIfailed;

// for watching blocks
int currentAsking;

// for messages
int messagex, messagey;
void putMessage(string s){
    Place(messagex, messagey);
    puts(s.c_str());
    messagey++;
}

// for movements
movement playerMovement[maxPlayers];

// for human
int humanID, humanChooseX, humanChooseY;
bool keepMovingWhileHumanPress;
bool humanParticipation;

// for turns
int theTimeBetweenHalfTurns;
double theStartTimeOfTheCurrentTurn;

// statistic
int armyTotal[maxPlayers], landTotal[maxPlayers];

// for files
string prefix;

// get AIs

block Ask(int askX, int askY);

#include "getAIs.h"

// functions
void InitGame();
void ShowMainPage();
void SettingsPage();
void RandomChoosePlayers();
void Play1v1Page();
void PlayFFAPage();
void Generals(string s);
bool OutOfTheMap(int askX, int askY);
void FloodFillTheMap(int currentX, int currentY);
void MakeGeneralsMap();
void ShowPlayers();
void ShowMapEdges();
void PrintBlockType(char currentType, int currentBelong, char whichSide);
void ShowBlockTop(block currentBlock);
void ShowBlockBottom(block currentBlock);
void ShowGeneralsMap();
void ShowGeneralsMapOfHumanView();
bool CheckVisible(block checkBlock);
string customMap();
void readMap(string s);

// init game
void InitGame(){
	// read AI
	ifstream AIs(prefix + "AIs.txt");
	AIs >> AItotal;
	AInames.clear();
	for (int i = 1; i <= AItotal; ++i){
		string AIname;
		AIs >> AIname;
		AInames.push_back(AIname);
	}
	AIs.close();
	// read settings
	ifstream Settings(prefix + "Settings.txt");
	Settings >> mapRows;
	Settings >> mapCols;
	Settings >> playerTotalOfFFA;
	Settings >> theTimeBetweenHalfTurns;
	Settings >> keepMovingWhileHumanPress;
	for (int i = 1; i <= 16; ++i){
		Settings >> playerColors[i];
	}
	Settings.close();
	// init window
	Noedit();
	output_color;
	HideCursor();
	system("mode con cols=101 lines=35");
	return;
}
// the center : X = 50

// the start
void ShowMainPage(){
	system("cls");
	// set bottons
	button _1v1_ = New_button(49, 6, 0, 2, "1v1");
	button _FFA_ = New_button(49, 13, 0, 2, "FFA");
	button _Settings_ = New_button(47, 20, 0, 2, "Settings");
	button _Exit_ = New_button(49, 27, 0, 4, "Exit");
	bool needRefresh = false;
	// options
	while (true){
		if (needRefresh){
            AIfailed.erase("human");
			_1v1_.lighted = 2;
			_FFA_.lighted = 2;
			_Settings_.lighted = 2;
			_Exit_.lighted = 2;
			needRefresh = false;
		}
		Sleep(clickTimeDelay);
		if (Click_button(_Exit_)){
			break;
		}
		if (Click_button(_1v1_)){
			Play1v1Page();
			system("cls");
			needRefresh = true;
		}
		if (Click_button(_FFA_)){
			PlayFFAPage();
			system("cls");
			needRefresh = true;
		}
		if (Click_button(_Settings_)){
			SettingsPage();
			system("cls");
			needRefresh = true;
		}
	}
	return;
}

// settings
// .....
// -   +
button _playerColor_[maxPlayers];
void SettingsPage(){
	// init
	system("cls");
	output_color;
	// title
	Place(45, 5);printf("Settings");
	// reset
	button _reset_ = New_button(70, 16, 0, 4, "Reset");
	// map rows
	Place(33, 8);printf("The rows of the map :");
	button _mapRowsDel_ = New_button(55, 9, 0, 2, "-");
	button _mapRowsAdd_ = New_button(60, 9, 0, 2, "+");
	// map cols
	Place(30, 11);printf("The columns of the map :");
	button _mapColsDel_ = New_button(55, 12, 0, 2, "-");
	button _mapColsAdd_ = New_button(60, 12, 0, 2, "+");
	// FFA players
	Place(32, 14);printf("The players in a FFA :");
	button _playerTotalOfFFADel_ = New_button(55, 15, 0, 2, "-");
	button _playerTotalOfFFAAdd_ = New_button(60, 15, 0, 2, "+");
	// time between turns
	Place(25, 17);printf("The time between half turns :      ms");
	button _timeDel_ = New_button(55, 18, 0, 2, "-");
	button _timeAdd_ = New_button(60, 18, 0, 2, "+");
	// special time
	Place(20, 19);printf("Or click to set the time :");
	button _timeSet500_ = New_button(48, 19, 0, 2, "500");
	button _timeSet250_ = New_button(54, 19, 0, 2, "250");
	button _timeSet125_ = New_button(60, 19, 0, 2, "125");
	button _timeSet50_ = New_button(66, 19, 0, 2, "50");
	// keep moving while human press
	Place(28, 21);printf("Move mode : While human press keys,");
	button _keepMoving_ = New_button(45, 23, 0, 2, "Keep moving");
	button _moveOnce_ = New_button(46, 24, 0, 2, "Move once");
	// players' color
	Place(42, 26);printf("Players' color :");
	button _colorDel_ = New_button(9, 27, 0, 2, "<");
	button _colorAdd_ = New_button(13, 27, 0, 2, ">");
	int currentClick = 1;
	for (int i = 1; i <= 16; ++i){
		string currentPlayerKingString = "$";
		currentPlayerKingString += (char)(i + 'A' - 1);
		currentPlayerKingString += '$';
		_playerColor_[i] = New_button(5 + i * 5, 27, playerColors[i], 0, currentPlayerKingString);
	}
	// back
	button _Back_ = New_button(48, 29, 0, 4, "Back");
	// options
	while (true){
		// show
		Place(57, 8);printf("%2d", mapRows);
		Place(57, 11);printf("%2d", mapCols);
		Place(57, 14);printf("%2d", playerTotalOfFFA);
		Place(56, 17);printf("%3d", theTimeBetweenHalfTurns);
		if (keepMovingWhileHumanPress){
			Place(41, 23);printf("-->");
			Place(57, 23);printf("<--");
			Place(41, 24);printf("   ");
			Place(57, 24);printf("   ");
		}
		else{
			Place(41, 23);printf("   ");
			Place(57, 23);printf("   ");
			Place(41, 24);printf("-->");
			Place(57, 24);printf("<--");
		}
		// delay
		Sleep(clickTimeDelay * 2);
		// back
		if (Click_button(_Back_)){
			// check
			if (playerTotalOfFFA * 25 > mapRows * mapCols){
				// set color
				Get_color(4);
				// put warning
				Place(40, 28);printf("The map is too small.");
				Sleep(1000);
				Place(40, 28);printf("                     ");
				// reset color
				output_color;
				continue;
			}
			break;
		}
		// reset
		if (Click_button(_reset_)){
			mapRows = 9;
			mapCols = 24;
			playerTotalOfFFA = 8;
			theTimeBetweenHalfTurns = 500;
			keepMovingWhileHumanPress = true;
			for (int i = 1; i <= 16; ++i){
				playerColors[i] = standardPlayerColors[i];
				_playerColor_[i].dark_color = playerColors[i];
				_playerColor_[i].lighted = 2;
			}
		}
		// map rows
		if (mapRows > minMapRows && Click_button(_mapRowsDel_)){
			--mapRows;
		}
		if (mapRows < maxMapRows && Click_button(_mapRowsAdd_)){
			++mapRows;
		}
		// map cols
		if (mapCols > minMapCols && Click_button(_mapColsDel_)){
			--mapCols;
		}
		if (mapCols < maxMapCols && Click_button(_mapColsAdd_)){
			++mapCols;
		}
		// player total in a FFA
		if (playerTotalOfFFA > minPlayerTotal && Click_button(_playerTotalOfFFADel_)){
			--playerTotalOfFFA;
		}
		if (playerTotalOfFFA < maxPlayerTotal && Click_button(_playerTotalOfFFAAdd_)){
			++playerTotalOfFFA;
		}
		// the time between half turns
		if (theTimeBetweenHalfTurns > 50 && Click_button(_timeDel_)){
			--theTimeBetweenHalfTurns;
		}
		if (theTimeBetweenHalfTurns < 500 && Click_button(_timeAdd_)){
			++theTimeBetweenHalfTurns;
		}
		if (Click_button(_timeSet500_)){
			theTimeBetweenHalfTurns = 500;
		}
		if (Click_button(_timeSet250_)){
			theTimeBetweenHalfTurns = 250;
		}
		if (Click_button(_timeSet125_)){
			theTimeBetweenHalfTurns = 125;
		}
		if (Click_button(_timeSet50_)){
			theTimeBetweenHalfTurns = 50;
		}
		// keep moving while human press
		if ((!keepMovingWhileHumanPress) && Click_button(_keepMoving_)){
			keepMovingWhileHumanPress = true;
		}
		if (keepMovingWhileHumanPress && Click_button(_moveOnce_)){
			keepMovingWhileHumanPress = false;
		}
		// players' color
		for (int i = 1; i <= 16; ++i){
			if (Click_button(_playerColor_[i])){
				currentClick = i;
				Place(_colorDel_.x, 27);printf(" ");
				Place(_colorAdd_.x, 27);printf(" ");
				_colorDel_.x = 4 + i * 5;
				_colorAdd_.x = 8 + i * 5;
				_colorDel_.lighted = 2;
				_colorAdd_.lighted = 2;
			}
		}
		if (Click_button(_colorDel_)){
			playerColors[currentClick] = (playerColors[currentClick] + 15) % 16;
			_playerColor_[currentClick].dark_color = playerColors[currentClick];
			_playerColor_[currentClick].lighted = 2;
		}
		if (Click_button(_colorAdd_)){
			playerColors[currentClick] = (playerColors[currentClick] + 1) % 16;
			_playerColor_[currentClick].dark_color = playerColors[currentClick];
			_playerColor_[currentClick].lighted = 2;
		}
	}
	// rewrite
	ofstream Settings(prefix + "Settings.txt");
	Settings << mapRows << '\n';
	Settings << mapCols << '\n';
	Settings << playerTotalOfFFA << '\n';
	Settings << theTimeBetweenHalfTurns << '\n';
	Settings << keepMovingWhileHumanPress << '\n';
	for (int i = 1; i <= 16; ++i){
		Settings << playerColors[i] << ' ';
	}
	Settings << '\n';
	Settings.close();
	return;
}

// random choose players
void RandomChoosePlayers(){
	for (int currentPlayer = 1; currentPlayer <= playerTotal; ++currentPlayer){
		bool checkUnqiue = false;
		while (!checkUnqiue){
			// random choose
			playerName[currentPlayer] = AInames[randNumber() % AItotal];
			// check
			checkUnqiue = !AIfailed.count(playerName[currentPlayer]);
			for (int lastPlayer = 1; lastPlayer < currentPlayer; ++lastPlayer){
				if (playerName[currentPlayer] == playerName[lastPlayer]){
					checkUnqiue = false;
					break;
				}
			}
		}
	}
	return;
}

// 1v1
void Play1v1Page(){
	// init
	system("cls");
	humanParticipation = false;
	humanID = 0;
	// screen
	Get_color(8);
	Place(46, 8);printf("1v1");
	Place(46, 12);Get_color(4);printf("V ");Get_color(1);printf("S");Get_color(8);
	// set buttons
	button _Lpre_ = New_button(10, 10, 0, 2, "^");
	button _Lnxt_ = New_button(10, 14, 0, 2, "v");
	button _Rpre_ = New_button(90, 10, 0, 2, "^");
	button _Rnxt_ = New_button(90, 14, 0, 2, "v");
	button _Play_ = New_button(46, 18, 0, 2, "Play");
	button _Back_ = New_button(46, 22, 0, 4, "Back");
	int Lplayer = 0, Rplayer = 0;
	string Lname = AInames[0], Rname = AInames[0];
	// options
	while (true){
		// print player
		Place(10, 12);Get_color(4);printf("%s                    ", Lname.c_str());
		Place(71 - (int)(Rname.length()), 12);Get_color(1);printf("                    %s", Rname.c_str());
		Get_color(8);
		// check button
		Sleep(clickTimeDelay * 3);
		if (Click_button(_Back_)){
			return;
		}
		if (Lplayer != Rplayer && Click_button(_Play_)){
			playerTotal = 2;
			playerName[1] = Lname;
			playerName[2] = Rname;
			if (Lname == "human"){
				humanParticipation = true;
				humanID = 1;
			}
			if (Rname == "human"){
				humanParticipation = true;
				humanID = 2;
			}
			break;
		}
		if (Click_button(_Lpre_)){
			Lplayer = (Lplayer + AItotal) % (AItotal + 1);
			Lname = (Lplayer >= AItotal ? "human" : AInames[Lplayer]);
		}
		if (Click_button(_Rpre_)){
			Rplayer = (Rplayer + AItotal) % (AItotal + 1);
			Rname = (Rplayer >= AItotal ? "human" : AInames[Rplayer]);
		}
		if (Click_button(_Lnxt_)){
			Lplayer = (Lplayer + 1) % (AItotal + 1);
			Lname = (Lplayer >= AItotal ? "human" : AInames[Lplayer]);
		}
		if (Click_button(_Rnxt_)){
			Rplayer = (Rplayer + 1) % (AItotal + 1);
			Rname = (Rplayer >= AItotal ? "human" : AInames[Rplayer]);
		}
	}
	// play generals
	Generals("");
	return;
}

// FFA
void PlayFFAPage(){
	system("cls");
	tmr = mapRows;
	tmc = mapCols;
    string s = customMap();
	Place(46, 10);printf("FFA");
	// set buttons
	button _Back_ = New_button(46, 22, 0, 4, "Back");
	button _AIs_VS_AIs_ = New_button(46, 18, 0, 2, "AIs VS AIs");
	button _Human_VS_AIs_ = New_button(46, 14, 0, 2, "Human VS AIs");
	// options
	while (true){
		Sleep(clickTimeDelay);
		if (Click_button(_Back_)){
			return;
		}
		if (Click_button(_AIs_VS_AIs_)){
			// varibles
			humanParticipation = false;
			if(s == "") playerTotal = min(playerTotalOfFFA, AItotal);
			else readMap(s);
			// players
			RandomChoosePlayers();
			break;
		}
		if (Click_button(_Human_VS_AIs_)){
			// variables
			humanParticipation = true;
			if(s == "") playerTotal = min(playerTotalOfFFA - 1, AItotal);
			else readMap(s);
			// players
			RandomChoosePlayers();
			// human
			if(s == ""){
                ++playerTotal;
                playerName[playerTotal] = "human";
                humanID = randNumber() % playerTotal + 1;
                if (humanID != playerTotal){
                    playerName[playerTotal] = playerName[humanID];
                    playerName[humanID] = "human";
                }
			}else playerName[humanID = (randNumber() % playerTotal + 1)] = "human";
			break;
		}
	}
	// play generals
	Generals(s);
	mapRows = tmr;
	mapCols = tmc;
	return;
}

string customMap(){
    puts("Will you use custom map?");
    button yes = New_button(0, 1, 0, 2, "Yes");
    button no = New_button(4, 1, 0, 4, "No");
    while(true){
        Sleep(clickTimeDelay);
        if(Click_button(yes)){
            Place(0, 2);
            puts("Please input the map's name:");
            string s; cin >> s;
            system("cls");
            return s;
        }
        if(Click_button(no)){
            system("cls");
            return "";
        }
    }
}

void readMap(string s){
    s += ".cmap"; ifstream in(s); memset(generalsMap, 0, sizeof generalsMap);
    in >> mapRows >> mapCols >> playerTotal;
    for(int i = 1; i <= playerTotal; i++)
        (in >> kingsX[i] >> kingsY[i]), generalsMap[kingsX[i]][kingsY[i]].belongTo = i;
    for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
        (in >> generalsMap[i][j].type >> generalsMap[i][j].army);
    in.close();
}

// generals

// out of the map
bool OutOfTheMap(int askX, int askY){
	return (askX < 1 || mapRows < askX || askY < 1 || mapCols < askY);
}
// make the map
void FloodFillTheMap(int currentX, int currentY){
	// mark
	floodAccessible[currentX][currentY] = true;
	// fill block
	for (int currentDirection = 0; currentDirection < 4; ++currentDirection){
		// next block
		int nextX = currentX + deltaX[currentDirection];
		int nextY = currentY + deltaY[currentDirection];
		// check position
		if (OutOfTheMap(nextX, nextY)){
			continue;
		}
		// check visited
		if (floodAccessible[nextX][nextY] || generalsMap[nextX][nextY].type == 'M' || generalsMap[nextX][nextY].type == 'C'){
			continue;
		}
		// fill
		FloodFillTheMap(nextX, nextY);
	}
	return;
}
void MakeGeneralsMap(){
	// size
	mapSize = mapRows * mapCols;
	// reset
	for (int i = 1; i <= mapRows; ++i){
		for (int j = 1; j <= mapCols; ++j){
			generalsMap[i][j].army = 0;
			generalsMap[i][j].belongTo = 0;
			generalsMap[i][j].type = 'L';
		}
	}
	// set kings
	int minDistance = (int)(sqrt(mapSize / playerTotal));
	int tryPutKingsCounts = 0;
	bool checkKings = false;
	while (!checkKings){
		checkKings = true;
		// reset kings
		for (int i = 1; i <= mapRows; ++i){
			for (int j = 1; j <= mapCols; ++j){
				possessedByKings[i][j] = false;
				if (generalsMap[i][j].type == 'K'){
					generalsMap[i][j].type = 'L';
					generalsMap[i][j].belongTo = 0;
					generalsMap[i][j].army = 0;
				}
			}
		}
		// set kings
		for (int currentPlayer = 1; currentPlayer <= playerTotal; ++currentPlayer){
			// try to set
			int tryCounts = 0;
			kingsX[currentPlayer] = 0;
			kingsY[currentPlayer] = 0;
			do{
				kingsX[currentPlayer] = randNumber() % mapRows + 1;
				kingsY[currentPlayer] = randNumber() % mapCols + 1;
				++tryCounts;
			}while (tryCounts <= 100 && possessedByKings[kingsX[currentPlayer]][kingsY[currentPlayer]]);
			// check
			if (possessedByKings[kingsX[currentPlayer]][kingsY[currentPlayer]]){
				checkKings = false;
				break;
			}
			// set
			generalsMap[kingsX[currentPlayer]][kingsY[currentPlayer]].belongTo = currentPlayer;
			generalsMap[kingsX[currentPlayer]][kingsY[currentPlayer]].type = 'K';
			generalsMap[kingsX[currentPlayer]][kingsY[currentPlayer]].army = 1;
			// possess
			for (int kingDeltaX = -minDistance; kingDeltaX <= minDistance; ++kingDeltaX){
				for (int kingDeltaY = -minDistance; kingDeltaY <= minDistance; ++kingDeltaY){
					// check distance
					int distanceToTheKing = kingDeltaX + kingDeltaY;
					if (distanceToTheKing < 0){
						distanceToTheKing = -distanceToTheKing;
					}
					if (distanceToTheKing <= minDistance){
						// possessed
						int possessedX = kingsX[currentPlayer] + kingDeltaX;
						int possessedY = kingsY[currentPlayer] + kingDeltaY;
						// check position
						if (OutOfTheMap(possessedX, possessedY)){
							continue;
						}
						// possess
						possessedByKings[kingsX[currentPlayer] + kingDeltaX][kingsY[currentPlayer] + kingDeltaY] = true;
					}
				}
			}
		}
		// trys
		++tryPutKingsCounts;
		if (tryPutKingsCounts >= 100){
			if (minDistance > 3){
				--minDistance;
			}
			tryPutKingsCounts = 0;
		}
	}
	// try to put mountains and castles
	int standardMountainsAndCastles = (int)((double)(mapSize) * 0.2);
	if (playerTotal + standardMountainsAndCastles > mapSize){
		standardMountainsAndCastles = mapSize - playerTotal;
	}
	bool checkFill = false;
	while (!checkFill){
		// reset the map
		for (int i = 1; i <= mapRows; ++i){
			for (int j = 1; j <= mapCols; ++j){
				floodAccessible[i][j] = false;
				if (generalsMap[i][j].type == 'C' || generalsMap[i][j].type == 'M'){
					generalsMap[i][j].type = 'L';
					generalsMap[i][j].army = 0;
				}
			}
		}
		// generate the number of mountains and castles
		int mountainsAndCastles = standardMountainsAndCastles + randNumber() % 9 - 4;
		if (playerTotal + mountainsAndCastles > mapSize){
			mountainsAndCastles = mapSize - playerTotal;
		}
		// put mountains and castles
		while (mountainsAndCastles--){
			int currentX = 0, currentY = 0;
			do{
				currentX = randNumber() % mapRows + 1;
				currentY = randNumber() % mapCols + 1;
			}while (generalsMap[currentX][currentY].type != 'L');
			int someRandNumber = randNumber() % 100;
			if (someRandNumber < 30){
				generalsMap[currentX][currentY].type = 'C';
				generalsMap[currentX][currentY].army = 40 + randNumber() % 10;
			}
			else{
				generalsMap[currentX][currentY].type = 'M';
			}
		}
		// check the map
		FloodFillTheMap(kingsX[1], kingsY[1]);
		checkFill = true;
		for (int currentPlayer = 1; currentPlayer <= playerTotal; ++currentPlayer){
			if (!floodAccessible[kingsX[currentPlayer]][kingsY[currentPlayer]]){
				checkFill = false;
				break;
			}
		}
	}
	return;
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
		// calc army and land
		armyTotal[currentPlayer] = 0;
		landTotal[currentPlayer] = 0;
		for (int i = 1; i <= mapRows; ++i){
			for (int j = 1; j <= mapCols; ++j){
				if (generalsMap[i][j].belongTo == currentPlayer){
					armyTotal[currentPlayer] += generalsMap[i][j].army;
					++landTotal[currentPlayer];
				}
			}
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
void ShowGeneralsMap(){
	// set color
	Get_color(8);
	for (int i = 1; i <= mapRows; ++i){
		// show type
		for (int j = 1; j <= mapCols; ++j){
			Place(j * 4 - 3, i * 3 + 2);
			ShowBlockTop(generalsMap[i][j]);
		}
		// show army
		for (int j = 1; j <= mapCols; ++j){
			Place(j * 4 - 3, i * 3 + 3);
			ShowBlockBottom(generalsMap[i][j]);
		}
	}
	output_color;
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
			ShowBlockTop(Ask(i, j));
		}
		for (int j = 1; j <= mapCols; ++j){
			Place(j * 4 - 3, i * 3 + 3);
			ShowBlockBottom(Ask(i, j));
		}
	}
	// reset color
	output_color;
	return;
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
// check
bool CheckGame(){
	int numberOfPlayersStillAlive = 0; int winner = 0;
	for (int currentPlayer = 1; currentPlayer <= playerTotal; ++currentPlayer){
		if (!alreadyDead[currentPlayer]){
			++numberOfPlayersStillAlive;
			winner = currentPlayer;
		}
	}
	if (numberOfPlayersStillAlive == 1){
        // broadcast
        Get_color(playerColors[winner]);
        putMessage(playerName[winner] + " wins!");
        output_color;
		return true;
	}
	return false;
}
// play
// 1. moves
// 2. check alive
// 3. add by turns
// 4. add by kings and castles
void Generals(string s){
	// init
	// init window
	system("cls");
	int windowsRows = max(5 + mapRows * 3, playerTotal * 4 + 2);
	int windowsCols = max(playerTotal * 15 + 1, mapCols * 4 + 21);
	messagex = mapCols * 4 + 2, messagey = 5;
	system(("mode con cols=" + to_string(windowsCols + 1) + " lines=" + to_string(windowsRows + 1)).c_str());
	HideCursor();
	Sleep(100);
	// init player
	for (int currentPlayer = 1; currentPlayer <= playerTotal; ++currentPlayer){
		// init AI
		Initialize(playerName[currentPlayer]);
		// set the state of live
		alreadyDead[currentPlayer] = false;
		// statistic
		armyTotal[currentPlayer] = 1;
		landTotal[currentPlayer] = 1;
	}
	// make the map
	if(s == "") MakeGeneralsMap();
	// show edges
	ShowMapEdges();
	// play
	generalsTurns = 0;
	while (!CheckGame()){
		// print Turns
		Place(8, 0);
		printf("%d", generalsTurns / 2);
		if (generalsTurns % 2 == 0){
			printf(" ");
		}
		else{
			printf(".");
		}
		// get the start time of the current turn
		theStartTimeOfTheCurrentTurn = clock();
		// moves
		// get movements
		for (int currentPlayer = 1; currentPlayer <= playerTotal; ++currentPlayer){
			// check alive
			if (alreadyDead[currentPlayer] || playerFailed[currentPlayer]){
				continue;
			}
			// set asking
			currentAsking = currentPlayer;
			// get the movement
			try{
                playerMovement[currentPlayer] = GetMove(playerName[currentPlayer]);
			}catch(...){
			    //failed
			    playerFailed[currentPlayer] = 1;
			    playerMovement[currentPlayer] = {-1, -1, -1};
			    generalsMap[kingsX[currentPlayer]][kingsY[currentPlayer]].belongTo = 0;
			    AIfailed[playerName[currentPlayer]] = 1;
			}
		}
		// make movements
		for (int currentPlayer = 1; currentPlayer <= playerTotal; ++currentPlayer){
			// check alive
			if (alreadyDead[currentPlayer]){
				continue;
			}
			// get the movement
			movement currentMove = playerMovement[currentPlayer];
			// check the block
			int currentX = currentMove.x, currentY = currentMove.y;
			if (OutOfTheMap(currentX, currentY)){
				continue;
			}
			if (generalsMap[currentX][currentY].belongTo != currentPlayer){
				continue;
			}
			// check the direction
			int currentDirection = currentMove.dir;
			if (currentDirection < 0 || 3 < currentDirection){
				continue;
			}
			// check the next block
			int nextX = currentX + deltaX[currentDirection];
			int nextY = currentY + deltaY[currentDirection];
			if (OutOfTheMap(nextX, nextY)){
				continue;
			}
			if (generalsMap[nextX][nextY].type == 'M'){
				continue;
			}
			// check the army
			if (generalsMap[currentX][currentY].army <= 1){
				continue;
			}
			// make the movement
			int currentArmy = generalsMap[currentX][currentY].army;
			int nextArmy = generalsMap[nextX][nextY].army;
			int moveArmy = currentArmy - 1;
			if (generalsMap[currentX][currentY].belongTo == generalsMap[nextX][nextY].belongTo){
				// add army
				generalsMap[nextX][nextY].army = moveArmy + nextArmy;
			}
			else{
				// recude army
				if (moveArmy > nextArmy){
					generalsMap[nextX][nextY].army = moveArmy - nextArmy;
					generalsMap[nextX][nextY].belongTo = currentPlayer;
				}
				else{
					generalsMap[nextX][nextY].army = nextArmy - moveArmy;
				}
			}
			// clear the block
			generalsMap[currentX][currentY].army = 1;
		}
		// check alive
		for (int currentPlayer = 1; currentPlayer <= playerTotal; ++currentPlayer){
			// check alive
			if (alreadyDead[currentPlayer]){
				continue;
			}
			if (generalsMap[kingsX[currentPlayer]][kingsY[currentPlayer]].belongTo != currentPlayer){
				// dead
				alreadyDead[currentPlayer] = true;
				// captured by
				int capturedBy = generalsMap[kingsX[currentPlayer]][kingsY[currentPlayer]].belongTo;
				// update type
				generalsMap[kingsX[currentPlayer]][kingsY[currentPlayer]].type = 'C';
				// update army
				for (int i = 1; i <= mapRows; ++i){
					for (int j = 1; j <= mapCols; ++j){
						if (generalsMap[i][j].belongTo == currentPlayer){
							generalsMap[i][j].belongTo = capturedBy;
							generalsMap[i][j].army = (generalsMap[i][j].army + 1) / 2;
						}
					}
				}
				// broadcast
                if(capturedBy)
                    Get_color(playerColors[capturedBy]), putMessage(playerName[capturedBy]),
                    output_color, putMessage("captured"),
                    Get_color(playerColors[currentPlayer]), putMessage(playerName[currentPlayer]),
                    output_color, putMessage("");
				else Get_color(playerColors[currentPlayer]), putMessage(playerName[currentPlayer]),
                    output_color, putMessage("failed!"), putMessage("");
			}
		}
		// add armys by turns
		if (generalsTurns % 50 == 0 && generalsTurns != 0){
			for (int i = 1; i <= mapRows; ++i){
				for (int j = 1; j <= mapCols; ++j){
					if (generalsMap[i][j].belongTo != 0){
						++generalsMap[i][j].army;
					}
				}
			}
		}
		// add armys by kings and castles
		if (generalsTurns % 2 == 0 && generalsTurns != 0){
			for (int i = 1; i <= mapRows; ++i){
				for (int j = 1; j <= mapCols; ++j){
					if (generalsMap[i][j].belongTo != 0){
						char currentType = generalsMap[i][j].type;
						if (currentType == 'K' || currentType == 'C'){
							++generalsMap[i][j].army;
						}
					}
				}
			}
		}
		// show
		ShowPlayers();
		if (humanParticipation && !alreadyDead[humanID]){
			ShowGeneralsMapOfHumanView();
		}
		else{
			ShowGeneralsMap();
		}
		// add turns
		++generalsTurns;
		// wait for the turn done
		while (((double)(clock()) - theStartTimeOfTheCurrentTurn) / CLOCKS_PER_SEC < (double)(theTimeBetweenHalfTurns) / 1000);
	}
	Sleep(1500);
	// reset window
	system("cls");
	system("mode con cols=101 lines=35");
	HideCursor();
	return;
}

// main
int main(int argc, char* argv[]){
    string t = argv[0];
    prefix = t.substr(0, t.size() - 12);
	InitGame();
	ShowMainPage();
	return 0;
}
