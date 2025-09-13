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
bool isAI[maxPlayers + 5];

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
int mapRows, mapCols, mapSize;
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

// for playing generals
int generalsTurns;
bool gameStarted;

// for players
bool alreadyDead[maxPlayers];
bool playerFailed[maxPlayers];
bool playerDone[maxPlayers];
map<string, bool> AIfailed;
bool waitingForWebPlayers;

// for watching blocks
int currentAsking;

// for movements
movement playerMovement[maxPlayers];

// for turns
int theTimeBetweenHalfTurns;
double theStartTimeOfTheCurrentTurn;

// statistic
int armyTotal[maxPlayers], landTotal[maxPlayers];

// get AIs

block Ask(int askX, int askY);

#include "getAIs.h"

// functions
void Generals();
bool OutOfTheMap(int askX, int askY);
void FloodFillTheMap(int currentX, int currentY);
void MakeGeneralsMap();
void WaitForWebPlayersDone();

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
void WaitForWebPlayersDone(){
	bool flag; memset(playerDone, 0, sizeof playerDone);
	waitingForWebPlayers = 1;
	do{
		Sleep(5); flag = 0;
		for(int i = 1; i <= playerTotal; i++)
			if(!isAI[i] && !playerDone[i] && !alreadyDead[i]) flag = 1;
	}while(flag);
	waitingForWebPlayers = 0;
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
inline block AskNet(int askX, int askY, int askid){
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
		if (generalsMap[nextX][nextY].belongTo == askid){
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
	if (numberOfPlayersStillAlive == 1) return true;
	return false;
}
// play
// 1. moves
// 2. check alive
// 3. add by turns
// 4. add by kings and castles
void Generals(){
	// init player
	for (int currentPlayer = 1; currentPlayer <= playerTotal; ++currentPlayer){
		// init AI
		if(isAI[currentPlayer]) Initialize(playerName[currentPlayer]);
		// set the state of live
		alreadyDead[currentPlayer] = false;
		// statistic
		armyTotal[currentPlayer] = 1;
		landTotal[currentPlayer] = 1;
	}
	// make the map
	MakeGeneralsMap();
	// play
	generalsTurns = 0;
	waitingForWebPlayers = 1;
	Sleep(10);
	waitingForWebPlayers = 0;
	Sleep(20);
	while (!CheckGame()){
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
                if(isAI[currentPlayer]) playerMovement[currentPlayer] = GetMove(playerName[currentPlayer]);
			}catch(...){
			    //failed
			    playerFailed[currentPlayer] = 1;
			    playerMovement[currentPlayer] = {-1, -1, -1};
			    generalsMap[kingsX[currentPlayer]][kingsY[currentPlayer]].belongTo = 0;
			    AIfailed[playerName[currentPlayer]] = 1;
			}
		}
		WaitForWebPlayersDone();
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
		// add turns
		++generalsTurns;
		// calc army and land
		for (int currentPlayer = 1; currentPlayer <= playerTotal; ++currentPlayer){
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
		}
		Sleep(clickTimeDelay);
	}
	return;
}

// main
string workWithRequest(const string& request, const string& cip){
    try {
    	if(waitingForWebPlayers){
    		stringstream ss(request.substr(request.find('/') + 1));
			string name = request.substr(0, request.find('/'));
    		int x, y, dir, id; ss >> x >> y >> dir;
    		for(id = 1; id <= playerTotal; id++) if(playerName[id] == name){
                playerMovement[id] = {x, y, dir}, playerDone[id] = 1; break;
    		}
    		while(waitingForWebPlayers) Sleep(2); Sleep(5);
    		stringstream mapstr("");
    		for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
    			mapstr << AskNet(i, j, id).type << " " << AskNet(i, j, id).army << " " << AskNet(i, j, id).belongTo << " ";
            for(int i = 1; i <= playerTotal; i++) mapstr << playerName[i] << " " << armyTotal[i] << " " << landTotal[i] << " " << alreadyDead[i] << " ";
            mapstr << id;
    		return mapstr.str();
		}
        return "404 Not Found";
    } catch (string s) {
        return "ERROR:\n- " + s;
    }
}
int main(int argc, char* argv[]){
	playerTotal = argc - 5;
	for(int i = 1; i < argc - 4; i++) playerName[i] = argv[i];
	int port, ai;
	sscanf(argv[argc - 4], "%d", &port);
	sscanf(argv[argc - 3], "%d", &ai);
	sscanf(argv[argc - 2], "%d", &mapRows);
	sscanf(argv[argc - 1], "%d", &mapCols);
	for(int i = 8; i; i--) isAI[i] = ai & 1, ai >>= 1;
	thread(start_server, port).detach(); Generals();
	return 0;
}
