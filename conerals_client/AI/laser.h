// laser
namespace laser{
	mt19937 rnd(time(0));
	const int dx[] = {0, -1, 0, 1};
	const int dy[] = {1, 0, -1, 0};
	int kingx, kingy, tposx, tposy;
	int myid, round, hatepos;
	block m[50][50];
	bool vis[50][50], noking[50][50], noneed[50][50], geted[50][50], attack[50][50];
	int armyposx, armyposy, gotox, gotoy;
	stack<movement> toking;
	vector<pair<int, int> > otherking, otherarmy, kingdom_edge;
	void Init(){
		otherking.clear(); otherarmy.clear();
		armyposx = armyposy = kingx = kingy = tposx = tposy = myid = round = hatepos = gotox = gotoy = 0;
		memset(m, 0, sizeof m); memset(vis, 0, sizeof vis); memset(attack, 0, sizeof attack);
		memset(noking, 0, sizeof noking); memset(noneed, 0, sizeof noneed); memset(geted, 0, sizeof geted);
		while(!toking.empty()) toking.pop();
	}
	movement Move(){
		kingdom_edge.clear();
		round++; bool flag = 0, needarmy = 0;
		for (int i = 1; i <= mapRows; i++)
			for (int j = 1; j <= mapCols; j++)
				m[i][j] = Ask(i, j);
		if(round == 1) for (int i = 1; i <= mapRows; ++i)
			for (int j = 1; j <= mapCols; ++j) if(m[i][j].type == 'K') myid = m[i][j].belongTo, armyposx = kingx = i, armyposy = kingy = j;
		if(m[tposx][tposy].belongTo == myid) armyposx = tposx, armyposy = tposy;
		int maxposx = 0, maxposy = 0, maxarmytot = 0;
        for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
            if(m[i][j].belongTo == myid && m[i][j].army > m[maxposx][maxposy].army) maxposx = i, maxposy = j;
		if(m[armyposx][armyposy].army <= 1 || m[armyposx][armyposy].belongTo != myid) armyposx = maxposx, armyposy = maxposy, needarmy = 1;
		for (int i = 1; i <= mapRows; ++i)
			for (int j = 1; j <= mapCols; ++j)
                    if(m[i][j].belongTo == myid) attack[i][j] = 0;
		for (int i = 1; i <= mapRows; ++i)
			for (int j = 1; j <= mapCols; ++j)
				if(m[i][j].belongTo != myid && m[i][j].type != 'M' && m[i][j].belongTo > 0){
					if(m[i][j].type == 'K') otherking.push_back({i, j});
					maxarmytot = max(maxarmytot, armyTotal[m[i][j].belongTo]);
					attack[i][j] = 1;
				}
		for(int i = 1; i <= mapRows; ++i) for (int j = 1; j <= mapCols; ++j) if(attack[i][j]) flag = 1;
		if(flag) for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
			if((m[i][j].belongTo == myid) && ((m[i - 1][j].belongTo > 0 && m[i - 1][j].belongTo != myid) || (m[i + 1][j].belongTo > 0 && m[i + 1][j].belongTo != myid) || (m[i][j - 1].belongTo > 0 && m[i][j - 1].belongTo != myid) || (m[i][j + 1].belongTo > 0 && m[i][j + 1].belongTo != myid))) kingdom_edge.push_back({i, j});
		if(!flag) for(int i = 1; i <= mapRows; i++) for(int j = 1; j <= mapCols; j++)
			if((m[i][j].belongTo == myid) && ((m[i - 1][j].type != 'M' && m[i - 1][j].belongTo >= 0 && m[i - 1][j].belongTo != myid) || (m[i + 1][j].type != 'M' && m[i + 1][j].belongTo >= 0 && m[i + 1][j].belongTo != myid) || (m[i][j - 1].type != 'M' && m[i][j - 1].belongTo >= 0 && m[i][j - 1].belongTo != myid) || (m[i][j + 1].type != 'M' && m[i][j + 1].belongTo >= 0 && m[i][j + 1].belongTo != myid))) kingdom_edge.push_back({i, j});
        if(!toking.empty()){
            movement tmpmove = toking.top(); toking.pop();
            while(!toking.empty() && (m[tmpmove.x][tmpmove.y].army <= 1 || m[tmpmove.x][tmpmove.y].belongTo != myid)) tmpmove = toking.top(), toking.pop();
            if(toking.empty()) armyposx = tposx = gotox, armyposy = tposy = gotoy, needarmy = 0;
            return tmpmove;
        }
		while(!otherking.empty()){
			int x = otherking.back().first, y = otherking.back().second;
			if(m[x][y].type == 'C' || m[x][y].type == 'B') otherking.pop_back();
			else break;
		}
        if(flag && needarmy && round > 50){
            memset(vis, 0, sizeof vis); pair<int, int> gotowhere = kingdom_edge[rnd() % kingdom_edge.size()];
			queue<movement> q; gotox = gotowhere.first, gotoy = gotowhere.second;
			if(!otherking.empty()) gotox = otherking.back().first, gotoy = otherking.back().second;
			q.push({gotox, gotoy, 4}); vis[gotox][gotoy] = 1; while(!q.empty()){
				int x = q.front().x, y = q.front().y; q.pop();
				for(int i = 0; i < 4; i++){
					int nx = x + dx[i], ny = y + dy[i], nf = (i + 2) % 4;
					if(m[nx][ny].type == 'B' || (m[nx][ny].army >= 40 && m[nx][ny].type != 'K' && m[nx][ny].belongTo != myid) || m[nx][ny].type == 'M' || vis[nx][ny] || nx < 1 || mapRows < nx || ny < 1 || mapCols < ny) continue;
					q.push({nx, ny, nf}); vis[nx][ny] = 1; toking.push({nx, ny, nf});
				}
			}
        }
		if(!otherking.empty()){
			memset(vis, 0, sizeof vis);
			queue<movement> q; int sx = otherking.back().first, sy = otherking.back().second;
			q.push({sx, sy, 4}); vis[sx][sy] = 1; while(!q.empty()){
				int x = q.front().x, y = q.front().y; q.pop();
				for(int i = 0; i < 4; i++){
					int nx = x + dx[i], ny = y + dy[i], nf = (i + 2) % 4;
					if((m[nx][ny].type == 'C' && m[nx][ny].belongTo == 0 && m[nx][ny].army > 10) || m[nx][ny].type == 'M' || vis[nx][ny] || nx < 1 || mapRows < nx || ny < 1 || mapCols < ny) continue;
					if(m[nx][ny].army >= m[armyposx][armyposy].army && m[nx][ny].belongTo == myid){
						tposx = nx + dx[nf]; tposy = ny + dy[nf];
						armyposx = nx; armyposy = ny;
						return {nx, ny, nf};
					}
					q.push({nx, ny, nf}); vis[nx][ny] = 1;
				}
			}
		}
		if(flag){
			queue<movement> q; int sx = armyposx, sy = armyposy;
			memset(vis, 0, sizeof vis); vis[sx][sy] = 1;
			for(int tmp = 0; tmp < 10; tmp++){
				int i = rand() % 4;
				int nx = sx + dx[i], ny = sy + dy[i];
				if((m[nx][ny].type == 'C' && m[nx][ny].belongTo == 0 && ((m[nx][ny].army > 10 && flag) || (m[nx][ny].army >= m[sx][sy].army - 5 && !flag))) || m[nx][ny].type == 'M' || vis[nx][ny] || nx < 1 || mapRows < nx || ny < 1 || mapCols < ny) continue;
				if((m[nx][ny].belongTo != myid && !flag) || (attack[nx][ny] && flag)){
					tposx = sx + dx[i], tposy = sy + dy[i];
					return {sx, sy, i};
				}
				q.push({nx, ny, i}); vis[nx][ny] = 1;
			}
			while(!q.empty()){
				int x = q.front().x, y = q.front().y, f = q.front().dir; q.pop();
				for(int i = 0; i < 4; i++){
					int nx = x + dx[i], ny = y + dy[i];
					if((m[nx][ny].type == 'C' && m[nx][ny].belongTo == 0 && ((m[nx][ny].army > 10 && flag) || (m[nx][ny].army >= m[sx][sy].army - 5 && !flag))) || m[nx][ny].type == 'M' || vis[nx][ny] || nx < 1 || mapRows < nx || ny < 1 || mapCols < ny) continue;
					if((m[nx][ny].belongTo != myid && !flag) || (attack[nx][ny] && flag)){
						tposx = sx + dx[f], tposy = sy + dy[f];
						return {sx, sy, f};
					}
					q.push({nx, ny, f}); vis[nx][ny] = 1;
				}
			}
		}else{
            memset(vis, 0, sizeof vis); queue<movement> q;
			do gotox = rnd() % mapCols + 1, gotoy = rnd() % mapCols + 1; while(m[gotox][gotoy].type == 'C' || m[gotox][gotoy].type == 'M' || m[gotox][gotoy].type == 'B' || m[gotox][gotoy].belongTo == myid);
			q.push({gotox, gotoy, 4}); vis[gotox][gotoy] = 1; while(!q.empty()){
				int x = q.front().x, y = q.front().y; q.pop();
				for(int i = 0; i < 4; i++){
					int nx = x + dx[i], ny = y + dy[i], nf = (i + 2) % 4;
					if(vis[nx][ny] || (m[nx][ny].type == 'C' && m[nx][ny].belongTo != myid) || m[nx][ny].type == 'M' || m[nx][ny].type == 'B' || nx < 1 || mapRows < nx || ny < 1 || mapCols < ny) continue;
					q.push({nx, ny, nf}); vis[nx][ny] = 1; toking.push({nx, ny, nf});
				}
			}
		}
		return {-1, -1, -1};
	}
}
