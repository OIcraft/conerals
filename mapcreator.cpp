#include"gameheader.h"

struct block{
	int army, belongTo;
	char type;
	// K : king
	// C : castle
	// L : land
	// M : mountain
};
string name;
POINT mousePT;
int mapRows, mapCols, playerTotal;
int humanChooseX, humanChooseY;
block generalsMap[50][50];
const int playerColors[21] = {8, 4, 1, 2, 3, 7, 5, 6, 13, 10, 11, 12, 9, 14, 8, 0, 15};
const int clickTimeDelay = 30;
const char types[] = {'L', 'M', 'K', 'C'};
int mptype[50][50];
int kx[21], ky[21];
button mnstype, addtype, mnsarmy, addarmy;
button mnsrows, addrows, mnscols, addcols;
int lastClickTime = clock();
button bck;
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
void changeHumanChoose(){
    if (click(l_mouse) || click(r_mouse)){
        int tx = humanChooseX, ty = humanChooseY;
        PrintHumanBlock(8);
		Get_pos(mousePT);
		int mouX = (int)(mousePT.x), mouY = (int)(mousePT.y);
		humanChooseX = ((mouY - 4) / 3) + 1;
		humanChooseY = (mouX / 4) + 1;
		if (humanChooseX < 1 || humanChooseX > mapRows || humanChooseY < 1 || humanChooseY > mapCols || mouY < 5)
            humanChooseX = tx, humanChooseY = ty, lastClickTime = clock();
        if(click(r_mouse)){
            int lstbl = generalsMap[humanChooseX][humanChooseY].belongTo;
            mptype[humanChooseX][humanChooseY] = mptype[tx][ty];
            generalsMap[humanChooseX][humanChooseY].type = generalsMap[tx][ty].type;
            generalsMap[humanChooseX][humanChooseY].army = generalsMap[tx][ty].army;
            if(generalsMap[humanChooseX][humanChooseY].type == 'K' && lstbl == 0)
                generalsMap[humanChooseX][humanChooseY].belongTo = ++playerTotal;
            if(generalsMap[humanChooseX][humanChooseY].type != 'K' && lstbl > 0){
                for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
                    if(generalsMap[i][j].belongTo > lstbl) generalsMap[i][j].belongTo--;
                generalsMap[humanChooseX][humanChooseY].belongTo = 0; playerTotal--;
            }
        }
		PrintHumanBlock(14);
    }
}
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
void ResetWindow(){
    int windowsRows, windowsCols;
    windowsRows = 5 + mapRows * 3;
	windowsCols = max(mapCols * 4 + 1, 35);
	system(("mode con cols=" + to_string(windowsCols + 1) + " lines=" + to_string(windowsRows + 1)).c_str());
	HideCursor();
    system("cls");
    ShowMapEdges();
    Place(0, 0); printf("Gird Settings:\nType:\nArmy:");
    mnstype = New_button(6, 1, 0, 2, "-");
    addtype = New_button(12, 1, 0, 2, "+");
    mnsarmy = New_button(6, 2, 0, 2, "-");
    addarmy = New_button(12, 2, 0, 2, "+");
    Place(15, 0); printf("Map Settings:");
    Place(15, 1); printf("Rows:");
    Place(15, 2); printf("Cols:");
    mnsrows = New_button(21, 1, 0, 2, "-");
    addrows = New_button(27, 1, 0, 2, "+");
    mnscols = New_button(21, 2, 0, 2, "-");
    addcols = New_button(27, 2, 0, 2, "+");
    bck = New_button(windowsCols - 4, 0, 0, 4, "Back");
}
int main(int argc, char* argv[]){
    system("cls");
    if(argc > 1){
        name = argv[1], name += ".cmap";
        ifstream in(name);
        int tmp[128] = {};
        tmp['L'] = 0; tmp['M'] = 1;
        tmp['K'] = 2; tmp['C'] = 3;
        in >> mapRows >> mapCols >> playerTotal;
        for(int i = 1, x, y; i <= playerTotal; i++)
            (in >> x >> y), generalsMap[x][y].belongTo = i;
        for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
            (in >> generalsMap[i][j].type >> generalsMap[i][j].army), mptype[i][j] = tmp[generalsMap[i][j].type];
        in.close();
    }
    else{
        mapRows = 9, mapCols = 24;
        printf("Please input the map's name:\n");
        cin >> name; name += ".cmap";
        for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++) generalsMap[i][j].type = 'L';
    }
    ResetWindow();
    while(true){
        PrintHumanBlock(14);
		Place(9, 1); printf("%c", generalsMap[humanChooseX][humanChooseY].type);
        Place(8, 2);
        int currentArmy = generalsMap[humanChooseX][humanChooseY].army;
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
		else printf("%3d", currentArmy);
        Place(24, 1); printf("%d", mapRows);
        Place(24, 2); printf("%d", mapCols);
		if(Click_button(bck)) break;
		if(Click_button(mnsrows) && mapRows > 1){
            for(int x = 1; x <= mapCols; x++) if(mptype[mapRows][x] == 2){
                int tbl = generalsMap[mapRows][x].belongTo;
                for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
                    if(generalsMap[i][j].belongTo > tbl) generalsMap[i][j].belongTo--;
                generalsMap[mapRows][x].belongTo = 0; playerTotal--;
            }
            mapRows--;
            lastClickTime = clock();
            ResetWindow();
		}
		if(Click_button(addrows) && mapRows <= 40){
            mapRows++;
            for(int x = 1; x <= mapCols; x++) mptype[mapRows][x] = 0, generalsMap[mapRows][x] = {0, 0, 'L'};
            lastClickTime = clock();
            ResetWindow();
		}
		if(Click_button(mnscols) && mapCols > 1){
            for(int x = 1; x <= mapRows; x++) if(mptype[x][mapCols] == 2){
                int tbl = generalsMap[x][mapCols].belongTo;
                for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
                    if(generalsMap[i][j].belongTo > tbl) generalsMap[i][j].belongTo--;
                generalsMap[x][mapCols].belongTo = 0; playerTotal--;
            }
            mapCols--;
            lastClickTime = clock();
            ResetWindow();
		}
		if(Click_button(addcols) && mapCols <= 40){
            mapCols++;
            for(int x = 1; x <= mapRows; x++) mptype[x][mapCols] = 0, generalsMap[x][mapCols] = {0, 0, 'L'};
            lastClickTime = clock();
            ResetWindow();
		}
		if(Click_button(mnstype) && humanChooseX){
            if(mptype[humanChooseX][humanChooseY] == 2){
                int tbl = generalsMap[humanChooseX][humanChooseY].belongTo;
                for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
                    if(generalsMap[i][j].belongTo > tbl) generalsMap[i][j].belongTo--;
                generalsMap[humanChooseX][humanChooseY].belongTo = 0; playerTotal--;
            }
            mptype[humanChooseX][humanChooseY] = (mptype[humanChooseX][humanChooseY] + 3) % 4;
            generalsMap[humanChooseX][humanChooseY].type = types[mptype[humanChooseX][humanChooseY]];
            if(mptype[humanChooseX][humanChooseY] == 2) generalsMap[humanChooseX][humanChooseY].belongTo = ++playerTotal;
            lastClickTime = clock();
		}
		if(Click_button(addtype) && humanChooseX){
            if(mptype[humanChooseX][humanChooseY] == 2){
                int tbl = generalsMap[humanChooseX][humanChooseY].belongTo;
                for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
                    if(generalsMap[i][j].belongTo > tbl) generalsMap[i][j].belongTo--;
                generalsMap[humanChooseX][humanChooseY].belongTo = 0; playerTotal--;
            }
            mptype[humanChooseX][humanChooseY] = (mptype[humanChooseX][humanChooseY] + 1) % 4;
            generalsMap[humanChooseX][humanChooseY].type = types[mptype[humanChooseX][humanChooseY]];
            if(mptype[humanChooseX][humanChooseY] == 2) generalsMap[humanChooseX][humanChooseY].belongTo = ++playerTotal;
            lastClickTime = clock();
		}
		if(Click_button(mnsarmy) && humanChooseX) generalsMap[humanChooseX][humanChooseY].army--, lastClickTime = clock();
		if(Click_button(addarmy) && humanChooseX) generalsMap[humanChooseX][humanChooseY].army++, lastClickTime = clock();
        ShowGeneralsMap();
        if(clock() - lastClickTime > 100) changeHumanChoose();
        Sleep(clickTimeDelay);
    }
    FILE* out = fopen(name.c_str(), "w");
    fprintf(out, "%d %d %d\n", mapRows, mapCols, playerTotal);
    for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
        kx[generalsMap[i][j].belongTo] = i, ky[generalsMap[i][j].belongTo] = j;
    for(int i = 1; i <= playerTotal; i++) fprintf(out, "%d %d\n", kx[i], ky[i]);
    for(int i = 1; i <= mapRows; i++, fprintf(out, "\n")) for(int j = 1; j <= mapCols; j++)
        fprintf(out, "%c %d ", generalsMap[i][j].type, generalsMap[i][j].army);
    fclose(out);
    return 0;
}
