// example1
namespace example1{
	mt19937 exrd(time(nullptr));
	const int mydx[4] = {0, -1, 0, 1};
	const int mydy[4] = {1, 0, -1, 0};
	int myId, myKingX, myKingY;
	int maxX, maxY, maxArmy;
	vector<int> doh, dsh;
	int dir, lsd;
	block now;
	void Init(){
		myKingX = 0;
		myKingY = 0;
		myId = 0;
		return;
	}
	void getKing(){
		for (int i = 1; i <= mapRows; ++i){
			for (int j = 1; j <= mapCols; ++j){
				now = Ask(i, j);
				if (now.type == 'K'){
					myKingX = i;
					myKingY = j;
					myId = now.belongTo;
					break;
				}
			}
		}
		lsd = -1;
		return;
	}
	bool checkMove(int d){
		int nx = maxX + mydx[d], ny = maxY + mydy[d];
		if (nx < 1 || mapRows < nx || ny < 1 || mapCols < ny){
			return false;
		}
		now = Ask(nx, ny);
//		printf("%d %d %c %2d %2d\n", nx, ny, now.type, now.belongTo, now.army);
		if (now.type == 'M'){
			return false;
		}
		return true;
	}
	movement Move(){
		if (myId == 0){
			getKing();
		}
		maxX = 0;
		maxY = 0;
		maxArmy = -114514;
		for (int i = 1; i <= mapRows; ++i){
			for (int j = 1; j <= mapCols; ++j){
				now = Ask(i, j);
				if (now.belongTo == myId){
					int ay = now.army - 1;
					if (now.type == 'K'){
						ay /= 2;
					}
					if (ay > maxArmy){
						maxArmy = ay;
						maxX = i;
						maxY = j;
					}
				}
			}
		}
		doh.clear();
		dsh.clear();
		for (int d = 0; d < 4; ++d){
			if (!checkMove(d)){
				continue;
			}
//			printf("\n%d", d);
			dsh.push_back(d);
			if (now.belongTo != myId){
				doh.push_back(d);
			}
		}
		dir = -1;
		if ((int)(doh.size()) > 0){
			int pos = exrd() % (int)(doh.size());
			dir = doh[pos];
		}
		if (dir == -1){
			int mx = -1;
			for (int d : dsh){
				checkMove(d);
				if (now.army > mx){
					mx = now.army;
				}
			}
			doh.clear();
			for (int d : dsh){
				checkMove(d);
				if (now.army == mx){
					doh.push_back(d);
				}
			}
			if (lsd != -1 && checkMove(lsd) && (exrd() & 127) < 111){
				dir = lsd;
			}
			else{
				int pos = exrd() % (int)(doh.size());
				dir = doh[pos];
			}
		}
		lsd = dir;
		return (movement){maxX, maxY, dir};
	}
}
