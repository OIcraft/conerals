// aaa1145141919810
namespace aaa1145141919810 {
	const int Explore = 0, Merge = 1, Attack = 2, City = 3, Guard = 4, MergeFinish = 5;
	bool FinishMerge;
	int Mode, id, ax, ay;
	stack<movement> ops;
	block mp[55][55];
	const int mdx[4] = {0, -1, 0, 1};
	const int mdy[4] = {1, 0, -1, 0};
	bool vis[55][55];
	struct poi {
		int x, y;
		poi(int xx = 0, int yy = 0) : x(xx), y(yy) {
		}
		friend bool operator < (poi a, poi b) {
			if(a.x != b.x)return a.x < b.x;
			return a.y < b.y;
		}
	}Kings[15];
	int to_int(int x, int y) {
		return (x-1)*mapCols+y;
	}
	poi to_poi(int x) {
		return {(x+mapCols-1)/mapCols, (x+mapCols-1)%mapCols+1};
	}
	
	void Init() {
		while(!ops.empty())ops.pop();
		FinishMerge = false;
		ax = ay = 0;
		mp[0][0].army = 0;
	}
	
	bool CanSeeOthers() {
		for(int i = 1; i <= mapRows; i++)
			for(int j = 1; j <= mapCols; j++)
				if(mp[i][j].type != 'M' && mp[i][j].type != 'x' && mp[i][j].type != '?' && mp[i][j].type != 'B' && mp[i][j].belongTo != id && mp[i][j].belongTo != 0)
					return true;
		return false;
	}
	bool TooLeast() {
		for(int i = 1; i <= mapRows; i++)
			for(int j = 1; j <= mapCols; j++)
				if(mp[i][j].type != 'M' && mp[i][j].type != 'x' && mp[i][j].type != '?' && mp[i][j].type != 'B' && mp[i][j].belongTo != id && mp[i][j].belongTo != 0 && abs(i-Kings[id].x)+abs(j-Kings[id].y) <= 5) {
					//printf("Too least! %d %d ", i, j);
					return true;
				}
		return false;
	}
	bool CanSeeCity() {
		for(int i = 1; i <= mapRows; i++)
			for(int j = 1; j <= mapCols; j++)
				if(mp[i][j].belongTo != id && mp[i][j].type == 'C')
					return true;
		return false;
	}
	int LeastCityArmy() {
		queue<int> q;
		q.push(to_int(Kings[id].x, Kings[id].y));
		memset(vis, false, sizeof vis);
		vis[Kings[id].x][Kings[id].y] = true;
		while(!q.empty()) {
			int ux = to_poi(q.front()).x, uy = to_poi(q.front()).y;
			q.pop();
			
			for(int i = 0; i < 4; i++) {
				int nx = ux+mdx[i], ny = uy+mdy[i];
				if(1 <= nx && nx <= mapRows && 1 <= ny && ny <= mapCols && !vis[nx][ny] && mp[nx][ny].type != 'B' && mp[nx][ny].type != 'M') {
					if(mp[nx][ny].type == 'C' && mp[nx][ny].belongTo != id)
						return mp[nx][ny].army;
					int pu = to_int(nx, ny);
					q.push(pu);
					vis[nx][ny] = true;
				}
			}
		}
		return 0;
	}
	void Push(int x, int y, int x2, int y2) {
		for(int i = 0; i < 4; i++)
			if(x2+mdx[i] == x && y2+mdy[i] == y) {
				//printf(" %d %d %d %d %d", x2, y2, i, x, y);
				movement pu;
				pu.x = x2, pu.y = y2, pu.dir = i;
				ops.push(pu);
			}
	}
	
	void Explore_ops() {
		//puts("explore start");
		for(int i = 1; i <= mapRows; i++)
			for(int j = 1; j <= mapCols; j++)
				if(mp[i][j].type != 'x' && mp[i][j].type != 'B' && mp[i][j].type != '?' && mp[i][j].belongTo == id && mp[i][j].army > mp[ax][ay].army)
					ax = i, ay = j;
		queue<int> q;
		q.push(to_int(ax, ay));
		memset(vis, 0, sizeof vis);
		vis[ax][ay] = true;
		int last[3005];
		//puts("bfs start");
		while(!q.empty()) {
			int ux = to_poi(q.front()).x, uy = to_poi(q.front()).y;
			//printf(" %d %d %d %d ", ux, uy, to_poi(last[to_int(ux, uy)]).x, to_poi(last[to_int(ux, uy)]).y);
			q.pop();
			
			for(int i = 0; i < 4; i++) {
				int nx = ux+mdx[i], ny = uy+mdy[i];
				if(1 <= nx && nx <= mapRows && 1 <= ny && ny <= mapCols && !vis[nx][ny] && mp[nx][ny].type != 'B' && mp[nx][ny].type != 'M') {
					int pu = to_int(nx, ny);
					last[pu] = to_int(ux, uy);
					if(mp[nx][ny].type != 'C' && mp[nx][ny].belongTo != id) {
						//printf("%d %d %d %d ", nx, ny, to_poi(last[to_int(nx, ny)]).x, to_poi(last[to_int(nx, ny)]).y);
						while(nx != ax || ny != ay) {
							int pu2 = to_int(nx, ny);
							Push(nx, ny, to_poi(last[pu2]).x, to_poi(last[pu2]).y);
							nx = to_poi(last[pu2]).x, ny = to_poi(last[pu2]).y;
						}
						return;
					}
					if(mp[nx][ny].type == 'C' && mp[nx][ny].belongTo != id)continue;
					vis[nx][ny] = true;
					q.push(pu);
				}
			}
		}
	}
	void Merge_ops() {
		ax = Kings[id].x, ay = Kings[id].y;
		FinishMerge = true;
		
		queue<int> q;
		memset(vis, false, sizeof vis);
		q.push(to_int(ax, ay));
		vis[ax][ay] = true;
		
		while(!q.empty()) {
			int ux = to_poi(q.front()).x, uy = to_poi(q.front()).y;
			q.pop();
			
			for(int i = 0; i < 4; i++) {
				int nx = ux+mdx[i], ny = uy+mdy[i];
				if(1 <= nx && nx <= mapRows && 1 <= ny && ny <= mapCols && !vis[nx][ny] && mp[nx][ny].type != 'B' && mp[nx][ny].type != 'M' && mp[nx][ny].belongTo == id) {
					Push(ux, uy, nx, ny);
					vis[nx][ny] = true;
					q.push(to_int(nx, ny));
				}
			}
		}
	}
	void Attack_ops() {
		//puts("start attack");
		int ex = 0, ey = 0;
		if(!ax || !ay || mp[ax][ay].belongTo != id || mp[ax][ay].army == 1)ax = Kings[id].x, ay = Kings[id].y;
		for(int i = 1; i <= playerTotal; i++) {
			if(i != id && Kings[i].x != 0 && Kings[i].y != 0 && !alreadyDead[i]) {
				ex = Kings[i].x, ey = Kings[i].y;
				break;
			}
		}
		if(!ex || !ey) {
			queue<int> q;
			q.push(to_int(ax, ay));
			memset(vis, 0, sizeof vis);
			vis[ax][ay] = true;
			int last[3005];
			//puts("bfs start");
			while(!q.empty() && (!ex || !ey)) {
				int ux = to_poi(q.front()).x, uy = to_poi(q.front()).y;
				//printf("%d %d %d %d ", ux, uy, to_poi(last[to_int(ux, uy)]).x, to_poi(last[to_int(ux, uy)]).y);
				q.pop();
				
				for(int i = 0; i < 4; i++) {
					int nx = ux+mdx[i], ny = uy+mdy[i];
					if(1 <= nx && nx <= mapRows && 1 <= ny && ny <= mapCols && !vis[nx][ny] && mp[nx][ny].type != 'B' && mp[nx][ny].type != 'M') {
						int pu = to_int(nx, ny);
						last[pu] = to_int(ux, uy);
						if(mp[nx][ny].belongTo != id && mp[nx][ny].belongTo != 0) {
							ex = nx, ey = ny;
							break;
						}
						vis[nx][ny] = true;
						q.push(pu);
					}
				}
			}
		}
		//puts("finish choose end");
		queue<int> q;
		q.push(to_int(ax, ay));
		memset(vis, 0, sizeof vis);
		vis[ax][ay] = true;
		int last[3005];
		//puts("bfs start");
		//system("pause");
		while(!q.empty()) {
			int ux = to_poi(q.front()).x, uy = to_poi(q.front()).y;
			//printf("%d %d %d %d ", ux, uy, to_poi(last[to_int(ux, uy)]).x, to_poi(last[to_int(ux, uy)]).y);
			q.pop();
			
			for(int i = 0; i < 4; i++) {
				int nx = ux+mdx[i], ny = uy+mdy[i];
				if(1 <= nx && nx <= mapRows && 1 <= ny && ny <= mapCols && !vis[nx][ny] && mp[nx][ny].type != 'B' && mp[nx][ny].type != 'M') {
					int pu = to_int(nx, ny);
					last[pu] = to_int(ux, uy);
					if(nx == ex && ny == ey) {
						//puts("push start");
						while(nx != ax || ny != ay) {
							Push(nx, ny, to_poi(last[to_int(nx, ny)]).x, to_poi(last[to_int(nx, ny)]).y);
							int pu2 = to_int(nx, ny);
							nx = to_poi(last[pu2]).x, ny = to_poi(last[pu2]).y;
						}
						ax = ex, ay = ey;
						//puts("finish push");
						return;
					}
					vis[nx][ny] = true;
					q.push(pu);
				}
			}
		}
	}
	void City_ops() {
		//puts("start city");
		queue<int> q;
		ax = Kings[id].x, ay = Kings[id].y;
		q.push(to_int(Kings[id].x, Kings[id].y));
		memset(vis, false, sizeof vis);
		vis[Kings[id].x][Kings[id].y] = true;
		int last[3005];
		//puts("start bfs");
		while(!q.empty()) {
			int ux = to_poi(q.front()).x, uy = to_poi(q.front()).y;
			q.pop();
			
			for(int i = 0; i < 4; i++) {
				int nx = ux+mdx[i], ny = uy+mdy[i];
				if(1 <= nx && nx <= mapRows && 1 <= ny && ny <= mapCols && !vis[nx][ny] && mp[nx][ny].type != 'B' && mp[nx][ny].type != 'M') {
					last[to_int(nx, ny)] = to_int(ux, uy);
					if(mp[nx][ny].type == 'C' && mp[nx][ny].belongTo != id) {
						//puts("start push");
						int tx = nx, ty = ny;
						while(nx != ax || ny != ay) {
							int pu = to_int(nx, ny);
							Push(nx, ny, to_poi(last[pu]).x, to_poi(last[pu]).y);
							nx = to_poi(last[pu]).x, ny = to_poi(last[pu]).y;
						}
						//puts("push finish");
						ax = tx, ay = ty;
						return;
					}
					int pu = to_int(nx, ny);
					q.push(pu);
					vis[nx][ny] = true;
				}
			}
		}
	}
	
	movement Move() {
		//Get map
		for(int i = 1; i <= mapRows; i++)
			for(int j = 1; j <= mapCols; j++) {
				mp[i][j] = Ask(i, j);
				if(generalsTurns == 0 && mp[i][j].type == 'K')id = mp[i][j].belongTo, Kings[id].x = i, Kings[id].y = j;
				if(mp[i][j].type == 'K')Kings[mp[i][j].belongTo] = {i, j};
			}
		//puts("get map success");
		
		//Finish ops
		int ox, oy;
		if(!ops.empty()) {
			ox = ops.top().x, oy = ops.top().y;
			while(!ops.empty() && (mp[ox][oy].belongTo != id || mp[ox][oy].army == 1)) {
				ops.pop();
				if(!ops.empty())ox = ops.top().x, oy = ops.top().y;
			}
			if(!ops.empty()) {
				movement res = ops.top();
				ops.pop();
				return res;
			}	
		}
		
		//puts("finish ops success");
		
		//Set mode
		if(mp[ax][ay].army == 1)ax = ay = 0, FinishMerge = false;
		if(FinishMerge) {
			//if(CanSeeOthers() && TooLeast())Mode = Guard;
			if(CanSeeOthers())Mode = Attack;
			else if(CanSeeCity() && LeastCityArmy()*4 <= armyTotal[id]) Mode = City;
			else 
				Mode = Explore;
		}
		else if((CanSeeOthers() || (CanSeeCity() && LeastCityArmy()*4 <= armyTotal[id])) && (ax && ay && mp[ax][ay].army * 4 < armyTotal[id])) {
			Mode = Merge;
		}
		else Mode = Explore;
		//puts("set mode success");
		//printf("%d", Mode);
		
		//Set ops
		switch(Mode) {
			case Explore: Explore_ops();break;
			case Merge: Merge_ops();break;
			case Attack: Attack_ops(); break;
			case City: City_ops();break;
			//case Guard: Guard_ops();break;
		}
		//printf(" %d %d", Mode, ops.size());
		if(!ops.empty()) {
			ox = ops.top().x, oy = ops.top().y;
			while(!ops.empty() && (mp[ox][oy].belongTo != id || mp[ox][oy].army == 1)) {
				//printf(" %d %d %d", ops.top().x, ops.top().y, ops.top().dir);
				ops.pop();
				if(!ops.empty())ox = ops.top().x, oy = ops.top().y;
			}
			if(!ops.empty()) {
				movement res = ops.top();
				ops.pop();
				return res;
			}	
		}
		return {Kings[id].x, Kings[id].y, 0};
	}
}
