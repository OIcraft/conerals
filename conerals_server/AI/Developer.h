// Developer
namespace Developer{
	mt19937 rnd(time(0));
	const int dx[] = {0, -1, 0, 1};
	const int dy[] = {1, 0, -1, 0};
	int round, kingx, kingy, myid;
	int armyx, armyy, gotox, gotoy;
	block m[50][50];
	vector<pair<int, int> > kingdom_edge;
	stack<movement> movestack;
	bool fighting, collecting;
	bool vis[50][50], attack[50][50];
	bool attacklist[50][50], isking[50][50];
	queue<pair<int, int>> kings;
	void Init(){
		round = kingx = kingy = gotox = gotoy = myid = 0;
		while(!kings.empty()) kings.pop();
		while(!movestack.empty())
			movestack.pop();
		memset(attack, 0, sizeof attack);
		memset(attacklist, 255, sizeof attacklist);
		memset(isking, 0, sizeof isking);
	}
	inline void readmap(){
		for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++) m[i][j] = Ask(i, j);
	}
	inline bool OutOfTheMap(int askX, int askY){
		return (askX < 1 || mapRows < askX || askY < 1 || mapCols < askY);
	}
	inline void getking(){
		for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
			if(m[i][j].type == 'K') kingx = i, kingy = j, myid = m[i][j].belongTo, isking[i][j] = 1;
		armyx = gotox = kingx, armyy = gotoy = kingy;
	}
	inline void getedge(){
		fighting = 0; kingdom_edge.clear();
		for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
			if(m[i][j].belongTo != myid && m[i][j].belongTo > 0){
				attack[i][j] = 1; for(int f = 0; f < 4; f++){
					int nx = i + dx[f], ny = j + dy[f];
					if(m[nx][ny].belongTo == myid) kingdom_edge.push_back({nx, ny});
				}
			}else if(m[i][j].belongTo == myid) attack[i][j] = 0;
		for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++) fighting |= attack[i][j];
		if(kingdom_edge.empty()) for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
			if(m[i][j].belongTo == 0) for(int f = 0; f < 4; f++){
					int nx = i + dx[f], ny = j + dy[f];
					if(m[nx][ny].belongTo == myid) kingdom_edge.push_back({nx, ny});
				}
	}
	inline void collectarmy(){
	    collecting = 1;
		queue<pair<int, int> > q; memset(vis, 0, sizeof vis);
		auto where = kingdom_edge[rnd() % kingdom_edge.size()];
		gotox =	where.first, gotoy = where.second;
		q.push({gotox, gotoy}); vis[gotox][gotoy] = 1;
		while(!q.empty()){
			auto [x, y] = q.front(); q.pop();
			for(int i = 0; i < 4; i++){
				int nx = x + dx[i], ny = y + dy[i];
				if(!OutOfTheMap(nx, ny) && !vis[nx][ny] && m[nx][ny].type != 'M' && m[nx][ny].type != 'B' && (m[nx][ny].type != 'C' || m[nx][ny].army < 20 || m[nx][ny].belongTo == myid))
					vis[nx][ny] = 1, q.push({nx, ny}), movestack.push({nx, ny, (i + 2) % 4});
			}
		}
	}
	inline void getarmypos(){
		if(m[armyx][armyy].army < 2 || m[armyx][armyy].belongTo != myid) collectarmy();
	}
	inline void getkings(){
		for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
			if(!isking[i][j] && m[i][j].type == 'K') kings.push({i, j}), isking[i][j] = 1;
		for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
			if(m[i][j].belongTo >= 0 && (m[i][j].type != 'K' || m[i][j].belongTo == myid)) attacklist[i][j] = 0;
	}
	movement Move(){
		round++; readmap(); if(!myid) getking(); getkings(); getedge();
		while(!movestack.empty()){
			auto [x, y, f] = movestack.top();
			if(m[x][y].army < 2 || m[x][y].belongTo != myid) movestack.pop();
			else break;
		}
		if(!movestack.empty()){
			movement t = movestack.top();
			movestack.pop();
			return t;
		}
        if(collecting && movestack.empty()) collecting = 0, armyx = gotox, armyy = gotoy;
		if(m[gotox][gotoy].army > 1 && m[gotox][gotoy].belongTo == myid) armyx = gotox, armyy = gotoy;
		while(!kings.empty()){
			auto [x, y] = kings.front();
			if(m[x][y].type == 'C' || m[x][y].type == 'B') kings.pop(), isking[x][y] = 0;
			else break;
		}
		if(!kings.empty() && (m[armyx][armyy].army < 2 || m[armyx][armyy].belongTo != myid)){
			queue<pair<int, int> > q; collecting = 1; memset(vis, 0, sizeof vis);
			gotox =	kings.front().first, gotoy = kings.front().second;
			q.push({gotox, gotoy}); vis[gotox][gotoy] = 1;
			while(!q.empty()){
				auto [x, y] = q.front(); q.pop();
				for(int i = 0; i < 4; i++){
					int nx = x + dx[i], ny = y + dy[i];
					if(!OutOfTheMap(nx, ny) && !vis[nx][ny] && m[nx][ny].type != 'M' && m[nx][ny].type != 'B' && (m[nx][ny].type != 'C' || m[nx][ny].army < 20 || m[nx][ny].belongTo == myid))
						vis[nx][ny] = 1, q.push({nx, ny}), movestack.push({nx, ny, (i + 2) % 4});
				}
			}
		}else if(kings.empty()) getarmypos();
		memset(vis, 0, sizeof vis); queue<movement> q; vis[armyx][armyy] = 1;
		for(int t = 0; t <= 30; t++){
            int i = rnd() % 4;
			int nx = armyx + dx[i], ny = armyy + dy[i];
			if(!OutOfTheMap(nx, ny) && !vis[nx][ny] && m[nx][ny].type != 'M' && m[nx][ny].type != 'B' && (m[nx][ny].type != 'C' || m[nx][ny].army + 15 < m[armyx][armyy].army || m[nx][ny].belongTo == myid))
				vis[nx][ny] = 1, q.push({nx, ny, i});
		}
		while(!q.empty()){
			auto [x, y, f] = q.front(); q.pop();
			if(kings.empty() ? (fighting ? attack[x][y] : attacklist[x][y]) : isking[x][y]){
				if(!collecting) gotox = armyx + dx[f], gotoy = armyy + dy[f];
				return {armyx, armyy, f};
			}
			for(int i = 0; i < 4; i++){
				int nx = x + dx[i], ny = y + dy[i];
				if(!OutOfTheMap(nx, ny) && !vis[nx][ny] && m[nx][ny].type != 'M' && m[nx][ny].type != 'B' && (m[nx][ny].type != 'C' || m[nx][ny].army + 15 < m[armyx][armyy].army || m[nx][ny].belongTo == myid))
					vis[nx][ny] = 1, q.push({nx, ny, f});
			}
		}
		return {-1, -1, -1};
	}
}
