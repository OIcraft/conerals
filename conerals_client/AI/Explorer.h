// Explorer ( Easy Version )
namespace Explorer{
	void Init();
	movement Move();

	mt19937 rd(time(nullptr) ^ 513458);

	typedef pair<int, int> Pii;
	typedef pair<int, Pii> PiPii;

	const int N = 100, inf = 1e9;
	const int dx[8] = {0, -1, 0, 1, 1, 1, -1, -1};
	const int dy[8] = {1, 0, -1, 0, 1, -1, -1, 1};

	int i, j, d;

	const Pii emptyPair = make_pair(0, 0);
	const block emptyBlock = (block){-1, -1, '$'};
	const movement emptyMovement = (movement){-1, -1, -1};

	bool InMap(int askX, int askY){
		return (1 <= askX && askX <= mapRows && 1 <= askY && askY <= mapCols);
	}
	bool Visible(char blockType){
		return (blockType == 'M' || blockType == 'C' || blockType == 'K' || blockType == 'L');
	}

	// initialize
	char mapType[N][N];
	block currentMap[N][N];
	void GetMap(){
		for (i = 1; i <= mapRows; ++i){
			for (j = 1; j <= mapCols; ++j){
				currentMap[i][j] = Ask(i, j);
				if (Visible(currentMap[i][j].type)){
					mapType[i][j] = currentMap[i][j].type;
				}
				else{
					if (currentMap[i][j].type == 'B'){
						currentMap[i][j].army = 50;
					}
					else if (currentMap[i][j].type == '?'){
						currentMap[i][j].army = 0;
					}
				}
			}
		}
		return;
	}

	int myID;
	Pii myKing;
	bool gotKing;
	void GetKing(){
		if (gotKing){
			return;
		}
		for (i = 1; i <= mapRows; ++i){
			for (j = 1; j <= mapCols; ++j){
				if (currentMap[i][j].type == 'K'){
					myKing = make_pair(i, j);
					myID = currentMap[i][j].belongTo;
				}
			}
		}
		gotKing = true;
		return;
	}

	queue<movement> movementQueue;
	movement currentMovement;
	bool gotCurrentMovementSuccessfully;
	void GetCurrentMovement(){
		gotCurrentMovementSuccessfully = false;
		currentMovement = emptyMovement;
		while (!movementQueue.empty()){
			currentMovement = movementQueue.front();
			movementQueue.pop();
			if (currentMap[currentMovement.x][currentMovement.y].belongTo == myID && currentMap[currentMovement.x][currentMovement.y].army > 1){
				gotCurrentMovementSuccessfully = true;
				return;
			}
		}
		currentMovement = emptyMovement;
		return;
	}
	void ClearMovementQueue(){
		while (!movementQueue.empty()){
			movementQueue.pop();
		}
		return;
	}

	// gather army
	Pii currentPosition;
	queue<Pii> makePathQueue;
	int currentX, currentY, nextX, nextY, preDirection[N][N], distanceToTarget[N][N];
	bool visitedInMakingPaths[N][N];
	void MakePaths(Pii target){
		while (!makePathQueue.empty()){
			makePathQueue.pop();
		}
		for (i = 1; i <= mapRows; ++i){
			for (j = 1; j <= mapCols; ++j){
				preDirection[i][j] = -1;
				distanceToTarget[i][j] = inf;
				visitedInMakingPaths[i][j] = false;
			}
		}
		distanceToTarget[target.first][target.second] = 0;
		visitedInMakingPaths[target.first][target.second] = true;
		makePathQueue.push(target);
		while (!makePathQueue.empty()){
			currentPosition = makePathQueue.front();
			makePathQueue.pop();
			currentX = currentPosition.first;
			currentY = currentPosition.second;
			for (d = 0; d < 4; ++d){
				nextX = currentX + dx[d];
				nextY = currentY + dy[d];
				if ((!InMap(nextX, nextY)) || visitedInMakingPaths[nextX][nextY] || mapType[nextX][nextY] == 'M' || currentMap[nextX][nextY].type == 'B' || (currentMap[nextX][nextY].type == 'C' && currentMap[nextX][nextY].belongTo != myID)){
					continue;
				}
				preDirection[nextX][nextY] = d;
				distanceToTarget[nextX][nextY] = distanceToTarget[currentX][currentY] + 1;
				makePathQueue.push(make_pair(nextX, nextY));
				visitedInMakingPaths[nextX][nextY] = true;
			}
		}
		return;
	}

	struct armyBlock{
		int army, distance;
		Pii position;
	};
	int myCastlesTotal;
	vector<Pii> placesWhereArmyFrom;
	vector<armyBlock> myCastles, temporyArmyPlaces;
	bool ArmyBlockCompare(armyBlock armyBlockX, armyBlock armyBlockY){
		return 1.0 * armyBlockX.army / sqrt(armyBlockX.distance) > 1.0 * armyBlockY.army / sqrt(armyBlockY.distance);
	}
	bool ArmyBlockCompareByDistance(armyBlock armyBlockX, armyBlock armyBlockY){
		return armyBlockX.distance > armyBlockY.distance;
	}
	void ChoosePlacesWhereArmyFrom(int expectation){
		// Easy Version
		myCastles.clear();
		myCastles.shrink_to_fit();
		for (i = 1; i <= mapRows; ++i){
			for (j = 1; j <= mapCols; ++j){
				if (currentMap[i][j].belongTo == myID && currentMap[i][j].army > 1 && ((currentMap[i][j].type == 'L' && currentMap[i][j].army > 2) || currentMap[i][j].type == 'K' || currentMap[i][j].type == 'C')){
					myCastles.push_back((armyBlock){currentMap[i][j].army, distanceToTarget[i][j], make_pair(i, j)});
				}
			}
		}
		sort(myCastles.begin(), myCastles.end(), ArmyBlockCompare);
		myCastlesTotal = (int)(myCastles.size());
		temporyArmyPlaces.clear();
		temporyArmyPlaces.shrink_to_fit();
		for (i = 0; i < myCastlesTotal && expectation > 0; ++i){
			expectation -= myCastles[i].army;
			temporyArmyPlaces.push_back(myCastles[i]);
		}
		sort(temporyArmyPlaces.begin(), temporyArmyPlaces.end(), ArmyBlockCompareByDistance);
		placesWhereArmyFrom.clear();
		placesWhereArmyFrom.shrink_to_fit();
		for (i = 0; i < (int)(temporyArmyPlaces.size()); ++i){
			placesWhereArmyFrom.push_back(temporyArmyPlaces[i].position);
		}
		return;
	}

	vector<Pii> armyPlaces;
	bool armyWillStart[N][N], goToTheSamePath;
	int directionToNextPlace[N][N], restArmyCount[N][N];
	void MakeGatherMovements(Pii target){
		armyPlaces.clear();
		armyPlaces.shrink_to_fit();
		for (i = 1; i <= mapRows; ++i){
			for (j = 1; j <= mapCols; ++j){
				directionToNextPlace[i][j] = -1;
				restArmyCount[i][j] = 0;
			}
		}
		for (i = 0; i < (int)(placesWhereArmyFrom.size()); ++i){
			currentPosition = placesWhereArmyFrom[i];
			goToTheSamePath = false;
			while (currentPosition != target){
				d = (preDirection[currentPosition.first][currentPosition.second] ^ 2);
				directionToNextPlace[currentPosition.first][currentPosition.second] = d;
				currentPosition.first += dx[d];
				currentPosition.second += dy[d];
			}
		}
		for (i = 1; i <= mapRows; ++i){
			for (j = 1; j <= mapCols; ++j){
				if (directionToNextPlace[i][j] != -1){
					d = directionToNextPlace[i][j];
					++restArmyCount[i + dx[d]][j + dy[d]];
				}
			}
		}
		for (i = 0; i < (int)(placesWhereArmyFrom.size()); ++i){
			currentPosition = placesWhereArmyFrom[i];
			if (restArmyCount[currentPosition.first][currentPosition.second] == 0){
				armyPlaces.push_back(currentPosition);
			}
		}
		while (!armyPlaces.empty()){
			currentPosition = armyPlaces.back();
			armyPlaces.pop_back();
			d = directionToNextPlace[currentPosition.first][currentPosition.second];
			if (d == -1){
				continue;
			}
			movementQueue.push((movement){currentPosition.first, currentPosition.second, d});
			nextX = currentPosition.first + dx[d];
			nextY = currentPosition.second + dy[d];
			--restArmyCount[nextX][nextY];
			if (restArmyCount[nextX][nextY] == 0){
				armyPlaces.push_back(make_pair(nextX, nextY));
			}
		}
		return;
	}

	void Gather(Pii target, int expectation){
		if (currentMap[target.first][target.second].belongTo == myID && currentMap[target.first][target.second].army >= expectation){
			return;
		}
		MakePaths(target);
		ChoosePlacesWhereArmyFrom(expectation);
		MakeGatherMovements(target);
		return;
	}

	// expand
	block askBlock;
	double CalcExpandValue(int askX, int askY, int distance){
		askBlock = currentMap[askX][askY];
		if (askBlock.belongTo == myID || askBlock.type == 'M'){
			return -1e8;
		}
		double p = 0;
		if (askBlock.type == 'K'){
			p = 1e8;
		}
		else if (askBlock.type == 'C'){
			if (askBlock.belongTo != 0){
				p = 1e4;
			}
			else{
				p = 50;
				if (armyTotal[myID] < askBlock.army * 3 || landTotal[myID] < 75){
					p = -1e4;
				}
			}
		}
		else if (askBlock.type == 'L'){
			if (askBlock.belongTo != 0){
				p = 100;
			}
			else{
				p = 5;
			}
		}
//		for (d = 0; d < 4; ++d){
//			nextX = askX + dx[d];
//			nextY = askY + dy[d];
//			if (InMap(nextX, nextY) && currentMap[nextX][nextY].belongTo == myID){
//				p += currentMap[nextX][nextY].army * currentMap[nextX][nextY].army;
//			}
//		}
		return (p * askBlock.army + 1) / (double)(distance + 5);
	}

	struct valueBlock{
		double value;
		Pii position;
	};
	int armyTotalAroundTarget;
	vector<valueBlock> expandTargets;
	bool CompareValueBlock(valueBlock valueBlockX, valueBlock valueBlockY){
		return valueBlockX.value > valueBlockY.value;
	}
	Pii GetExpandTargets(){
		expandTargets.clear();
		expandTargets.shrink_to_fit();
		for (i = 1; i <= mapRows; ++i){
			for (j = 1; j <= mapCols; ++j){
				if (currentMap[i][j].belongTo == myID || (!Visible(currentMap[i][j].type)) || currentMap[i][j].type == 'M'){
					continue;
				}
				armyTotalAroundTarget = 0;
				for (d = 0; d < 8; ++d){
					if (d >= 4 && armyTotalAroundTarget == 0){
						break;
					}
					nextX = i + dx[d];
					nextY = j + dy[d];
					if ((!InMap(nextX, nextY)) || (currentMap[nextX][nextY].belongTo != myID) || (currentMap[nextX][nextY].type == 'M')){
						continue;
					}
					armyTotalAroundTarget += currentMap[nextX][nextY].army;
				}
				if (armyTotalAroundTarget > 0){
					expandTargets.push_back((valueBlock){(CalcExpandValue(i, j, abs(i - myKing.first) + abs(j - myKing.second)) + currentMap[i][j].army + armyTotalAroundTarget) / (abs(i - myKing.first) + abs(j - myKing.second)), make_pair(i, j)});
				}
			}
		}
		if (expandTargets.empty()){
			return emptyPair;
		}
		sort(expandTargets.begin(), expandTargets.end(), CompareValueBlock);
		return expandTargets[0].position;
	}

	Pii expandTarget, bestPosition;
	int needArmyToTarget, maxArmyOnThatBlock, bestDirection;
	void Expand(){
		expandTarget = GetExpandTargets();
		if (expandTarget == emptyPair){
			return;
		}
		needArmyToTarget = currentMap[expandTarget.first][expandTarget.second].army + 2;
		bestDirection = -1;
		maxArmyOnThatBlock = 0;
		bestPosition = emptyPair;
		for (d = 0; d < 4; ++d){
			nextX = expandTarget.first + dx[d];
			nextY = expandTarget.second + dy[d];
			if ((!InMap(nextX, nextY)) || (currentMap[nextX][nextY].belongTo != myID)){
				continue;
			}
			if (currentMap[nextX][nextY].army > maxArmyOnThatBlock){
				maxArmyOnThatBlock = currentMap[nextX][nextY].army;
				bestPosition = make_pair(nextX, nextY);
				bestDirection = (d ^ 2);
			}
		}
		if (currentMap[expandTarget.first][expandTarget.second].belongTo != myID && currentMap[expandTarget.first][expandTarget.second].belongTo != 0){
			Gather(bestPosition, 0.75 * armyTotal[myID]);
		}
		else{
			Gather(bestPosition, needArmyToTarget);
		}
		movementQueue.push((movement){bestPosition.first, bestPosition.second, bestDirection});
		return;
	}

	// attack
	string attackState;
	Pii currentEnemyKing, currentLargestArmy;
	int minValue, currentEnemy, belongToMySee[N][N], tryToGatherOnEnemyKing;

	bool CheckAttackOtherKing(){
		for (i = 1; i <= mapRows; ++i){
			for (j = 1; j <= mapCols; ++j){
				if (currentMap[i][j].type == 'K' && currentMap[i][j].belongTo != myID){
					if (currentMap[i][j].belongTo == currentEnemy && attackState != "attack"){
						currentEnemyKing = make_pair(i, j);
						ClearMovementQueue();
						attackState = "attack";
					}
					for (d = 0; d < 4; ++d){
						nextX = i + dx[d];
						nextY = j + dy[d];
						if (InMap(nextX, nextY) && currentMap[nextX][nextY].belongTo == myID){
							if (currentMap[nextX][nextY].army > currentMap[i][j].army){
								movementQueue.push((movement){nextX, nextY, (d ^ 2)});
								currentEnemyKing = emptyPair;
								return true;
							}
						}
					}
				}
			}
		}
		return false;
	}

	void CheckEnemy(){
		if (alreadyDead[currentEnemy]){
			currentEnemyKing = emptyPair;
			currentEnemy = 0;
			attackState = "none";
		}
		if (currentEnemy > 0){
			return;
		}
		currentEnemyKing = emptyPair;
		for (i = 1; i <= mapRows; ++i){
			for (j = 1; j <= mapCols; ++j){
				if (Visible(currentMap[i][j].type)){
					belongToMySee[i][j] = currentMap[i][j].belongTo;
				}
				if (belongToMySee[i][j] != myID && belongToMySee[i][j] != 0){
					if ((currentEnemy == 0) || (armyTotal[belongToMySee[i][j]] <= 1.2 * armyTotal[myID] && landTotal[belongToMySee[i][j]] >= 3)){
						currentEnemy = belongToMySee[i][j];
					}
				}
			}
		}
		if (currentEnemy > 0){
			attackState = "find";
			ClearMovementQueue();
			tryToGatherOnEnemyKing = 0;
		}
		return;
	}

	Pii temporyAttackTarget;
	vector<movement> temporyMovements;
	void MakeAttackPathFromMyLargestArmyToTarget(Pii attackTarget){
		temporyAttackTarget = attackTarget;
		while (attackTarget != currentLargestArmy){
			d = preDirection[attackTarget.first][attackTarget.second];
			attackTarget.first += dx[d ^ 2];
			attackTarget.second += dy[d ^ 2];
			temporyMovements.push_back((movement){attackTarget.first, attackTarget.second, d});
		}
		while (!temporyMovements.empty()){
			movementQueue.push(temporyMovements.back());
			temporyMovements.pop_back();
		}
		currentLargestArmy = temporyAttackTarget;
		return;
	}

	double attackValue;
	int cannotVisibleBlocksAround;
	void CalcAttackValue(int askX, int askY, int distance){
		cannotVisibleBlocksAround = 0;
		for (d = 0; d < 8; ++d){
			nextX = askX + dx[d];
			nextY = askY + dy[d];
			if ((!InMap(nextX, nextY)) || Visible(mapType[nextX][nextY])){
				continue;
			}
			++cannotVisibleBlocksAround;
		}
		attackValue = (double)(cannotVisibleBlocksAround) / sqrt(distance);
		if (currentMap[askX][askY].type == 'C'){
			attackValue /= 1e8;
		}
		return;
	}

	bool searchedInAttack[N][N];
	double maxValueToMyLargestArmy;
	Pii placeWhichToMyLargestArmyHasMaxValue;
	int distanceToMyLand[N][N], newDistanceToNextPlace;
	priority_queue<PiPii, vector<PiPii>, greater<PiPii> > getPathToBorderQueue;
	void GetTheShortestPathToEnemy(){
		while (!getPathToBorderQueue.empty()){
			getPathToBorderQueue.pop();
		}
		for (i = 1; i <= mapRows; ++i){
			for (j = 1; j <= mapCols; ++j){
				preDirection[i][j] = -1;
				distanceToMyLand[i][j] = inf;
				searchedInAttack[i][j] = false;
			}
		}
		distanceToMyLand[currentLargestArmy.first][currentLargestArmy.second] = 0;
		getPathToBorderQueue.push(make_pair(0, currentLargestArmy));
		while (!getPathToBorderQueue.empty()){
			currentPosition = getPathToBorderQueue.top().second;
			getPathToBorderQueue.pop();
			currentX = currentPosition.first;
			currentY = currentPosition.second;
			if (searchedInAttack[currentX][currentY]){
				continue;
			}
			searchedInAttack[currentX][currentY] = true;
			for (d = 0; d < 4; ++d){
				nextX = currentX + dx[d];
				nextY = currentY + dy[d];
				if ((!InMap(nextX, nextY)) || (mapType[nextX][nextY] == 'M') || (currentMap[nextX][nextY].type == 'B')){
					continue;
				}
				newDistanceToNextPlace = distanceToMyLand[currentX][currentY];
				if (currentMap[nextX][nextY].belongTo == myID){
					newDistanceToNextPlace += 100;
				}
				else if (currentMap[nextX][nextY].belongTo == 0){
					newDistanceToNextPlace += 1000000;
				}
				else{
					newDistanceToNextPlace += 1;
				}
				if (distanceToMyLand[nextX][nextY] > newDistanceToNextPlace){
					distanceToMyLand[nextX][nextY] = newDistanceToNextPlace;
					preDirection[nextX][nextY] = d;
					getPathToBorderQueue.push(make_pair(distanceToMyLand[nextX][nextY], make_pair(nextX, nextY)));
				}
			}
		}
		maxValueToMyLargestArmy = -inf;
		placeWhichToMyLargestArmyHasMaxValue = emptyPair;
		for (i = 1; i <= mapRows; ++i){
			for (j = 1; j <= mapCols; ++j){
				if ((!Visible(currentMap[i][j].type)) || mapType[i][j] == 'M' || currentMap[i][j].type == 'B' || currentMap[i][j].belongTo != currentEnemy){
					continue;
				}
				CalcAttackValue(i, j, distanceToMyLand[i][j]);
				if (attackValue > maxValueToMyLargestArmy){
					maxValueToMyLargestArmy = attackValue;
					placeWhichToMyLargestArmyHasMaxValue = make_pair(i, j);
				}
			}
		}
		MakeAttackPathFromMyLargestArmyToTarget(placeWhichToMyLargestArmyHasMaxValue);
		return;
	}

	vector<valueBlock> enemyBlocks;
	void MakeAttackPath(){
		if (currentMap[currentLargestArmy.first][currentLargestArmy.second].belongTo != myID){
			Gather(myKing, min(1.3 * armyTotal[currentEnemy], 0.75 * armyTotal[myID]));
			// "preDirection" and "distanceToTarget"
			enemyBlocks.clear();
			enemyBlocks.shrink_to_fit();
			for (i = 1; i <= mapRows; ++i){
				for (j = 1; j <= mapCols; ++j){
					if (currentMap[i][j].belongTo == currentEnemy){
						enemyBlocks.push_back((valueBlock){1.0 / currentMap[i][j].army / distanceToTarget[i][j], make_pair(i, j)});
					}
				}
			}
			if (enemyBlocks.empty()){
				for (i = 1; i <= mapRows; ++i){
					for (j = 1; j <= mapCols; ++j){
						if (currentMap[i][j].belongTo != myID && currentMap[i][j].belongTo > 0){
							enemyBlocks.push_back((valueBlock){1.0 / currentMap[i][j].army / distanceToTarget[i][j], make_pair(i, j)});
						}
					}
				}
			}
			sort(enemyBlocks.begin(), enemyBlocks.end(), CompareValueBlock);
			temporyMovements.clear();
			temporyMovements.shrink_to_fit();
			currentLargestArmy = myKing;
		}
		else{
			GetTheShortestPathToEnemy();
		}
		return;
	}

	void NuclearAttack(){
		if (tryToGatherOnEnemyKing < 2 && Visible(currentMap[currentEnemyKing.first][currentEnemyKing.second].type) && currentMap[currentEnemyKing.first][currentEnemyKing.second].army < currentMap[currentLargestArmy.first][currentLargestArmy.second].army){
			Gather(currentEnemyKing, currentMap[currentEnemyKing.first][currentEnemyKing.second].army);
			++tryToGatherOnEnemyKing;
		}
		else{
			Gather(myKing, max(0.85 * armyTotal[myID], 1.25 * currentMap[currentEnemyKing.first][currentEnemyKing.second].army));
			currentLargestArmy = myKing;
			MakeAttackPathFromMyLargestArmyToTarget(currentEnemyKing);
		}
		return;
	}

	void Attack(){
		if (attackState == "find"){
			MakeAttackPath();
		}
		else if (attackState == "attack"){
			NuclearAttack();
		}
		return;
	}

	// main
	void Solve(){
		GetMap();
		GetKing();
		CheckEnemy();
		if (CheckAttackOtherKing()){
			return;
		}
		if (!movementQueue.empty()){
			return;
		}
		if (currentEnemy > 0){
			Attack();
		}
		else{
			Expand();
		}
		return;
	}

	// system
	movement Move(){
		Solve();
		GetCurrentMovement();
		return currentMovement;
	}

	void Init(){
		attackState = "none";
		gotKing = gotCurrentMovementSuccessfully = false;
		i = j = d = myID = currentX = currentY = nextX = nextY = 0;
		needArmyToTarget = maxArmyOnThatBlock = bestDirection = currentEnemy = tryToGatherOnEnemyKing = 0;
		myKing = currentPosition = expandTarget = bestPosition = currentEnemyKing = currentLargestArmy = emptyPair;
		currentMovement = emptyMovement;
		askBlock = emptyBlock;
		placesWhereArmyFrom.clear();
		placesWhereArmyFrom.shrink_to_fit();
		expandTargets.clear();
		expandTargets.shrink_to_fit();
		armyPlaces.clear();
		armyPlaces.shrink_to_fit();
		enemyBlocks.clear();
		enemyBlocks.shrink_to_fit();
		while (!makePathQueue.empty()){
			makePathQueue.pop();
		}
		while (!getPathToBorderQueue.empty()){
			getPathToBorderQueue.pop();
		}
		memset(mapType, 0, sizeof(mapType));
		memset(currentMap, 0, sizeof(currentMap));
		memset(preDirection, 0, sizeof(preDirection));
		memset(restArmyCount, 0, sizeof(restArmyCount));
		memset(belongToMySee, 0, sizeof(belongToMySee));
		memset(armyWillStart, 0, sizeof(armyWillStart));
		memset(distanceToTarget, 0, sizeof(distanceToTarget));
		memset(distanceToMyLand, 0, sizeof(distanceToMyLand));
		memset(visitedInMakingPaths, 0, sizeof(visitedInMakingPaths));
		memset(directionToNextPlace, -1, sizeof(directionToNextPlace));
		return;
	}
}
