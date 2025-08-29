// PhirainEX
namespace PhirainEX {
	mt19937 exrd(time(nullptr));
	const int PhirainEX_dx[4] = {0, -1, 0, 1};
	const int PhirainEX_dy[4] = {1, 0, -1, 0};
	int PhirainEX_id,PhirainEX_x,PhirainEX_y;
	int now_x,now_y,now_max;
	int PhirainEX_list[105],PhirainEX_head,PhirainEX_tail;
	block PhirainEX_map[105][105];
	int PhirainEX_army[105][105];
	int PhirainEX_vi[105][105],PhirainEX_op[105][105],PhirainEX_num[105][105];
	int PhirainEX_mode=0;
	// add by lzj
	void Init(){
		PhirainEX_id = 0;
		PhirainEX_x = 0;
		PhirainEX_y = 0;
		now_x = 0;
		now_y = 0;
		now_max = 0;
		for (int i = 0; i < 105; ++i){
			PhirainEX_list[i] = 0;
		}
		PhirainEX_head = 0;
		PhirainEX_tail = 0;
		for (int i = 0; i < 105; ++i){
			for (int j = 0; j < 105; ++j){
				PhirainEX_map[i][j].type = '\0';
				PhirainEX_map[i][j].belongTo = 0;
				PhirainEX_map[i][j].army = 0;
				PhirainEX_army[i][j] = 0;
				PhirainEX_vi[i][j] = 0;
				PhirainEX_op[i][j] = 0;
				PhirainEX_num[i][j] = 0;
			}
		}
		PhirainEX_mode = 0;
		return;
	}
	// end
	void find_id() {
		for (int i = 1; i <= mapRows; ++i) {
			for (int j = 1; j <= mapCols; ++j) {
				block now = Ask(i, j);
				if (now.type == 'K') {
					PhirainEX_x = i;
					PhirainEX_y = j;
					PhirainEX_id = now.belongTo;
					break;
				}
			}
		}
		return;
	}
	void getMap() {
		for (int i = 1; i <= mapRows; ++i) {
			for (int j = 1; j <= mapCols; ++j) {
				PhirainEX_map[i][j] = Ask(i, j);
				if(PhirainEX_map[i][j].type == 'B' || PhirainEX_map[i][j].type == 'M'){
					PhirainEX_army[i][j] = -99999;
					continue;
				}
				if(PhirainEX_map[i][j].type == '?'){
					PhirainEX_army[i][j] = -1;
					continue;
				}
				if(PhirainEX_map[i][j].belongTo == PhirainEX_id){
					PhirainEX_army[i][j]=PhirainEX_map[i][j].army;
				}
				else{
					PhirainEX_army[i][j]=-PhirainEX_map[i][j].army;
				}
				PhirainEX_army[i][j]--;
			}
		}
		return;
	}
	int checkBlock(int x,int y){
		if(x<1||x>mapRows||y<1||y>mapCols){
			return 0;
		}
		if(PhirainEX_map[x][y].type == 'B' || PhirainEX_map[x][y].type == 'M'){
			return 0;
		}
		return 1;
	}
	int moveable(int x,int y){
		if(checkBlock(x,y) == 0){
			return 0;
		}
		if(PhirainEX_map[x][y].belongTo != PhirainEX_id || PhirainEX_map[x][y].army <= 0){
			return 0;
		}
		return 1;
	}
	int calcValue(int x,int y){
		if(checkBlock(x,y) == 0){
			return 0;
		}
		if(PhirainEX_map[x][y].belongTo == PhirainEX_id){
			return 0;
		}
		return 1;
	}
	struct PhirainEX_node{
		int x,y,z;
	};
	bool operator < (PhirainEX_node a,PhirainEX_node b){
		return a.z<b.z;
	}
	priority_queue<PhirainEX_node>PhirainEX_dl;
	int getMaxArmy(int x,int y){
		for (int i = 1; i <= mapRows; ++i) {
			for (int j = 1; j <= mapCols; ++j) {
				PhirainEX_vi[i][j]=0;
				PhirainEX_op[i][j]=-1;
				PhirainEX_num[i][j]=-99999;
			}
		}
		PhirainEX_node aa;
		aa.x=x;
		aa.y=y;
		aa.z=PhirainEX_army[x][y];
		PhirainEX_num[x][y]=aa.z;
		PhirainEX_dl.push(aa);
		while(!PhirainEX_dl.empty()){
			PhirainEX_node aa=PhirainEX_dl.top();
			PhirainEX_dl.pop();
			if(PhirainEX_vi[aa.x][aa.y]==1){
				continue;
			}
			PhirainEX_vi[aa.x][aa.y]=1;
			for(int i=0;i<4;i++){
				int next_x=aa.x+PhirainEX_dx[i];
				int next_y=aa.y+PhirainEX_dy[i];
				if(checkBlock(next_x,next_y)){
					if(PhirainEX_vi[next_x][next_y]==0&&PhirainEX_num[next_x][next_y]<PhirainEX_num[aa.x][aa.y]+PhirainEX_army[next_x][next_y]){
						PhirainEX_num[next_x][next_y]=PhirainEX_num[aa.x][aa.y]+PhirainEX_army[next_x][next_y];
						PhirainEX_op[next_x][next_y]=i^2;
						PhirainEX_node bb;
						bb.x=next_x;
						bb.y=next_y;
						bb.z=PhirainEX_num[next_x][next_y];
						PhirainEX_dl.push(bb);
					}
				}
			}
		}
		now_max=-99999;
		for (int i = 1; i <= mapRows; ++i) {
			for (int j = 1; j <= mapCols; ++j) {
				if(PhirainEX_num[i][j]>now_max){
					now_max=PhirainEX_num[i][j];
					now_x=i;
					now_y=j;
				}
			}
		}
		PhirainEX_head=1;
		PhirainEX_tail=1;
		int temp_x=now_x;
		int temp_y=now_y;
		while(temp_x!=x&&temp_y!=y){
			int op=PhirainEX_op[temp_x][temp_y];
			PhirainEX_list[PhirainEX_tail]=op;
			temp_x+=PhirainEX_dx[op];
			temp_y+=PhirainEX_dy[op];
			PhirainEX_tail++;
		}
		return now_max;
	}
	movement Move() {
		if(PhirainEX_id == 0) {
			find_id();
		}
		getMap();
		if(generalsTurns < 25) {
			return (movement){0, 0, 0};
		}
		else{
			if(PhirainEX_head == PhirainEX_tail || moveable(now_x,now_y) == 0){
				int _=500;
				while(_--){
					int x = exrd() % mapRows + 1;
					int y = exrd() % mapCols + 1;
					if(calcValue(x,y)==1&&getMaxArmy(x,y)>0){
						break;
					}
				}
				if(_<=0){
					return (movement){0, 0, 0};
				}
			}
			int x=now_x;
			int y=now_y;
			int op=PhirainEX_list[PhirainEX_head];
			now_x+=PhirainEX_dx[op];
			now_y+=PhirainEX_dy[op];
			PhirainEX_head++;
			return (movement){x, y, op};
		}
	}
}
