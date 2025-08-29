// human
namespace humanOptions{
	const int keyDirectionIndex[4] = {39, 38, 37, 40};
	const int keyAlphabetIndex[4] = {68, 87, 65, 83};
	double theStartTime, lastMoveTime;
	queue<movement> options;
	movement topOption;
	bool pressed[4];
	block topBlock;
	POINT mousePT;
	int d, dir;
#ifdef multiplayers
    bool keepMovingWhileHumanPress = 0;
    int humanChooseX, humanChooseY;
    const int theTimeBetweenHalfTurns = 50;
#endif // multiplayers
	void Init(){
		lastMoveTime = 0.0;
		humanChooseX = 0;
		humanChooseY = 0;
		for (d = 0; d < 4; ++d){
			pressed[d] = false;
		}
		return;
	}
	void GetTheTime(double currentStartTime){
		theStartTime = currentStartTime;
		return;
	}
	void PrintHumanBlock(int printColor){
		if (humanChooseX){
			// set color
			Get_color(printColor);
			// print the edges
			Place(humanChooseY * 4 - 4, humanChooseX * 3 + 1);printf("+---+");
			Place(humanChooseY * 4 - 4, humanChooseX * 3 + 4);printf("+---+");
			Place(humanChooseY * 4 - 4, humanChooseX * 3 + 2);printf("|");
			Place(humanChooseY * 4 - 4, humanChooseX * 3 + 3);printf("|");
			Place(humanChooseY * 4, humanChooseX * 3 + 2);printf("|");
			Place(humanChooseY * 4, humanChooseX * 3 + 3);printf("|");
			// reset color
			output_color;
		}
		return;
	}
	movement Move(){
		while (((double)(clock()) - theStartTime) / CLOCKS_PER_SEC < (double)(theTimeBetweenHalfTurns) / 1000 - 0.01){
#ifndef multiplayers
            if (kd(90)) throw runtime_error("114514");
#endif
			if (click(l_mouse)){
				PrintHumanBlock(8);
				Get_pos(mousePT);
				int mouX = (int)(mousePT.x), mouY = (int)(mousePT.y);
				humanChooseX = (mouY - 4) / 3 + 1;
				humanChooseY = mouX / 4 + 1;
				if (humanChooseX < 1 || humanChooseX > mapRows || humanChooseY < 1 || humanChooseY > mapCols){
					humanChooseX = 0;
					humanChooseY = 0;
				}
				PrintHumanBlock(playerColors[humanID]);
			}
#ifndef multiplayers
			if (kd(72)){
				PrintHumanBlock(8);
				humanChooseX = kingsX[humanID];
				humanChooseY = kingsY[humanID];
				PrintHumanBlock(playerColors[humanID]);
			}
#endif
			if (humanChooseX){
				dir = -1;
				if (keepMovingWhileHumanPress){
					if (((double)(clock()) - lastMoveTime) / CLOCKS_PER_SEC >= (double)(theTimeBetweenHalfTurns) / 3000.0){
						for (d = 0; d < 4; ++d){
							if (kd(keyDirectionIndex[d]) || kd(keyAlphabetIndex[d])){
								dir = (dir == -1 ? d : dir);
							}
						}
					}
				}
				else{
					for (d = 0; d < 4; ++d){
						if (kd(keyDirectionIndex[d]) || kd(keyAlphabetIndex[d])){
							if (!pressed[d]){
								pressed[d] = true;
								dir = (dir == -1 ? d : dir);
							}
						}
						else{
							pressed[d] = false;
						}
					}
				}
				if (dir != -1){
					PrintHumanBlock(8);
					humanChooseX += deltaX[dir];
					humanChooseY += deltaY[dir];
					if (humanChooseX < 1 || mapRows < humanChooseX || humanChooseY < 1 || mapCols < humanChooseY || Ask(humanChooseX, humanChooseY).type == 'M'){
						humanChooseX -= deltaX[dir];
						humanChooseY -= deltaY[dir];
					}
					else{
						options.push((movement){humanChooseX - deltaX[dir], humanChooseY - deltaY[dir], dir});
						lastMoveTime = (double)(clock());
					}
					PrintHumanBlock(playerColors[humanID]);
				}
			}
			if (kd(81)){
				while (!options.empty()){
					options.pop();
				}
			}
		}
		do{
			if (options.empty()){
				topOption = (movement){-1, -1, -1};
				break;
			}
			topOption = options.front();
			options.pop();
			topBlock = generalsMap[topOption.x][topOption.y];
		}while(topBlock.army <= 1 || topBlock.belongTo != humanID);
		return topOption;
	}
}
