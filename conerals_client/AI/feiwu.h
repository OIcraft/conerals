// zhengpie
namespace feiwu{
	mt19937 exrd(time(nullptr));
	const int mydx[4] = {0, -1, 0, 1};
	const int mydy[4] = {1, 0, -1, 0};
	int myId, myKingX, myKingY,landd;
	map<int,pair<int,int> > pos;
	block now;
	void Init(){
		myKingX = 0;
		myKingY = 0;
		myId = 0;
		landd = 0;
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
		return;
	}
	movement Move(){
		if (myId == 0){
			getKing();landd = 1;
			pair<int,int> sssbbb;sssbbb.first = myKingX;sssbbb.second = myKingY;
			pos[landd] = sssbbb;
		}
		int tmp = exrd() % landd + 1,dir = exrd() & 3,cnt = 0;
		if(Ask(pos[tmp].first + mydx[dir],pos[tmp].second + mydy[dir]).type == 'M' || Ask(pos[tmp].first + mydx[dir],pos[tmp].second + mydy[dir]).army >= Ask(pos[tmp].first,pos[tmp].second).army)
		{
			int xxxxxxx = exrd() % 3;
			if(cnt < 40) tmp = ((xxxxxxx == 2) ? (exrd() % landd + 1) : landd),dir = exrd() % 4,cnt++;
			else return (movement){2,2,2};
		}
		pair<int,int> sssbbb;sssbbb.first = pos[tmp].first + mydx[dir];sssbbb.second = pos[tmp].second + mydy[dir];
		pos[++landd] = sssbbb;
		return (movement){pos[tmp].first,pos[tmp].second,dir};
	}
}
