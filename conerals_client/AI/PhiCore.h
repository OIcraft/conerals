// PhiCore
namespace PhiCore {
	mt19937 exrd(time(nullptr)+274080);
	const int PhirainEX_dx[4] = {0, -1, 0, 1};
	const int PhirainEX_dy[4] = {1, 0, -1, 0};
	int PhirainEX_id,PhirainEX_x,PhirainEX_y;
	int now_x,now_y,now_max,now_dis,now_val;
	int PhirainEX_list[1005][3],PhirainEX_head,PhirainEX_tail;
	block PhirainEX_map[105][105];
	int PhirainEX_army[105][105];
	int PhirainEX_vi[105][105],PhirainEX_op[105][105],PhirainEX_dis[105][105],PhirainEX_num[105][105],PhirainEX_val[105][105];
	int PhirainEX_mode=0;//0:explore,1:fight,2:get_army
	int total_land=0;
	int total_army=0;
	// add by lzj
	void Init() {
		PhirainEX_id = 0;
		PhirainEX_x = 0;
		PhirainEX_y = 0;
		now_x = 0;
		now_y = 0;
		now_max = 0;
		now_dis = 0;
		now_val = 0;
		for (int i = 0; i < 105; ++i) {
			PhirainEX_list[i][0] = 0;
			PhirainEX_list[i][1] = 0;
			PhirainEX_list[i][2] = 0;
		}
		PhirainEX_head = 0;
		PhirainEX_tail = 0;
		for (int i = 0; i < 105; ++i) {
			for (int j = 0; j < 105; ++j) {
				PhirainEX_map[i][j].type = '?';
				PhirainEX_map[i][j].belongTo = 0;
				PhirainEX_map[i][j].army = 0;
				PhirainEX_army[i][j] = 0;
				PhirainEX_vi[i][j] = 0;
				PhirainEX_op[i][j] = 0;
				PhirainEX_num[i][j] = 0;
				PhirainEX_dis[i][j] = 0;
				PhirainEX_val[i][j] = 0;
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
		total_land=0;
		total_army=0;
		for (int i = 1; i <= mapRows; ++i) {
			for (int j = 1; j <= mapCols; ++j) {
				block new_map=Ask(i, j);
				if(PhirainEX_map[i][j].type != '?' && PhirainEX_map[i][j].type != 'B' && PhirainEX_map[i][j].belongTo != PhirainEX_id) {
					if(new_map.type != '?' && new_map.type != 'B') {
						PhirainEX_map[i][j]=new_map;
					}
				} else {
					PhirainEX_map[i][j]=new_map;
				}
				if(PhirainEX_map[i][j].type == 'B' || PhirainEX_map[i][j].type == 'M') {
					PhirainEX_army[i][j] = -99999;
					continue;
				}
				if(PhirainEX_map[i][j].type == '?') {
					PhirainEX_army[i][j] = 0;
					continue;
				}
				if(PhirainEX_map[i][j].belongTo == PhirainEX_id) {
					total_land++;
					PhirainEX_army[i][j]=PhirainEX_map[i][j].army;
					total_army+=PhirainEX_army[i][j];
				} else {
					if(PhirainEX_map[i][j].type=='C'){
						PhirainEX_army[i][j]=-PhirainEX_map[i][j].army*3/2;
					}
					else{
						PhirainEX_army[i][j]=-PhirainEX_map[i][j].army;
					}
				}
			}
		}
		return;
	}
	int checkBlock(int x,int y) {
		if(x<1||x>mapRows||y<1||y>mapCols) {
			return 0;
		}
		if(PhirainEX_map[x][y].type == 'B' || PhirainEX_map[x][y].type == 'M') {
			return 0;
		}
		return 1;
	}
	int moveable(int x,int y) {
		if(checkBlock(x,y) == 0) {
			return 0;
		}
		if(PhirainEX_map[x][y].belongTo != PhirainEX_id || PhirainEX_map[x][y].army <= 1) {
			return 0;
		}
		return 1;
	}
	int isinvis(int x,int y){
		if(x<1||x>mapRows||y<1||y>mapCols) {
			return 0;
		}
		if(PhirainEX_map[x][y].type == '?'){
			return 2;
		}
		if(PhirainEX_map[x][y].type == 'B'){
			return 1;
		}
		return 0;
	}
	int inviscnt(int x,int y){
		int ans=0;
		ans+=isinvis(x-1,y-1);
		ans+=isinvis(x-1,y);
		ans+=isinvis(x-1,y+1);
		ans+=isinvis(x,y-1);
		ans+=isinvis(x,y);
		ans+=isinvis(x,y+1);
		ans+=isinvis(x+1,y-1);
		ans+=isinvis(x+1,y);
		ans+=isinvis(x+1,y+1);
		return ans;
	}
	struct PhirainEX_node {
		int x,y;
	};
	queue<PhirainEX_node>PhirainEX_dl;
	void get_dis(int x,int y,int mode=0) {
		for (int i = 1; i <= mapRows; ++i) {
			for (int j = 1; j <= mapCols; ++j) {
				PhirainEX_vi[i][j]=0;
				PhirainEX_op[i][j]=-1;
				PhirainEX_dis[i][j]=99999;
				PhirainEX_num[i][j]=-99999;
			}
		}
		PhirainEX_node aa;
		aa.x=x;
		aa.y=y;
		PhirainEX_dis[x][y]=0;
		PhirainEX_num[x][y]=PhirainEX_army[x][y];
		PhirainEX_vi[x][y]=1;
		PhirainEX_val[x][y]=99999;
		PhirainEX_dl.push(aa);
		while(!PhirainEX_dl.empty()) {
			PhirainEX_node aa=PhirainEX_dl.front();
			PhirainEX_dl.pop();
			if(PhirainEX_dis[aa.x][aa.y]>0&&PhirainEX_map[aa.x][aa.y].type=='C'&&PhirainEX_map[aa.x][aa.y].belongTo != PhirainEX_id){
				continue;
			}
			for(int i=0; i<4; i++) {
				int next_x=aa.x+PhirainEX_dx[i];
				int next_y=aa.y+PhirainEX_dy[i];
				if(checkBlock(next_x,next_y)) {
					if(PhirainEX_vi[next_x][next_y]==0) {
						if(mode==1&&PhirainEX_map[next_x][next_y].belongTo != PhirainEX_id){
							continue;
						}
						PhirainEX_vi[next_x][next_y]=1;
						PhirainEX_op[next_x][next_y]=i^2;
						PhirainEX_dis[next_x][next_y]=PhirainEX_dis[aa.x][aa.y]+1;
						PhirainEX_val[next_x][next_y]=PhirainEX_val[aa.x][aa.y]-10;
						if(mode==2){
							if(isinvis(next_x,next_y))
								PhirainEX_val[next_x][next_y]=-99999;
							else{
								PhirainEX_val[next_x][next_y]+=inviscnt(next_x,next_y)*8;
							}
						}
						PhirainEX_num[next_x][next_y]=PhirainEX_num[aa.x][aa.y]+PhirainEX_army[next_x][next_y]-1;
						PhirainEX_node bb;
						bb.x=next_x;
						bb.y=next_y;
						PhirainEX_dl.push(bb);
					}
				}
			}
		}
		return;
	}
	bool PlanA(int end_x,int end_y,int start_x=0,int start_y=0) {
		get_dis(end_x,end_y);
		int min_dis=99999,max_army=0;
		if(start_x==0&&start_y==0) {
			for (int i = 1; i <= mapRows; ++i) {
				for (int j = 1; j <= mapCols; ++j) {
					if((PhirainEX_dis[i][j]<min_dis&&PhirainEX_num[i][j]>=1)||
					    (PhirainEX_dis[i][j]==min_dis&&PhirainEX_num[i][j]>max_army)) {
						min_dis=PhirainEX_dis[i][j];
						max_army=PhirainEX_num[i][j];
						start_x=i;
						start_y=j;
					}
				}
			}
			if(start_x==0&&start_y==0) {
				return 0;
			}
		}
		PhirainEX_head=1;
		PhirainEX_tail=1;
		int temp_x=start_x;
		int temp_y=start_y;
		while(temp_x!=end_x||temp_y!=end_y) {
			int op=PhirainEX_op[temp_x][temp_y];
			PhirainEX_list[PhirainEX_tail][0]=temp_x;
			PhirainEX_list[PhirainEX_tail][1]=temp_y;
			PhirainEX_list[PhirainEX_tail][2]=op;
			temp_x+=PhirainEX_dx[op];
			temp_y+=PhirainEX_dy[op];
			PhirainEX_tail++;
		}
		return 1;
	}
	bool PlanB(int start_x,int start_y) {
		get_dis(start_x,start_y);
		int max_army=0,max_val=0;
		int end_x=0,end_y=0;
		for (int i = 1; i <= mapRows; ++i) {
			for (int j = 1; j <= mapCols; ++j) {
				int PhirainEX_val=0;
				if(PhirainEX_map[i][j].belongTo == PhirainEX_id||PhirainEX_num[i][j]<=0) continue;
				if(PhirainEX_map[i][j].belongTo == 0||isinvis(i,j)) {
					PhirainEX_val=20;
					if(PhirainEX_map[i][j].type == 'C') PhirainEX_val=30;
				} else {
					PhirainEX_val=50;
					if(PhirainEX_map[i][j].type == 'C') PhirainEX_val=60;
					if(PhirainEX_map[i][j].type == 'K') PhirainEX_val=70;
				}
				PhirainEX_val-=PhirainEX_dis[i][j]*2;
				if(isinvis(i,j)==0){
					PhirainEX_val+=inviscnt(i,j)*3;
				}
				PhirainEX_val+=100;
				if((PhirainEX_val>max_val)||
					(PhirainEX_val==max_val&&PhirainEX_num[i][j]>max_army)) {
					max_army=PhirainEX_num[i][j];
					max_val=PhirainEX_val;
					end_x=i;
					end_y=j;
				}
			}
		}
		if(end_x==0&&end_y==0) {
			return 0;
		}
		if(PlanA(end_x,end_y,start_x,start_y)) {
			return 1;
		}
		return 0;
	}
	void PlanC() {
		get_dis(PhirainEX_x,PhirainEX_y,1);
		PhirainEX_head=1;
		PhirainEX_tail=1;
		for(int di = mapRows + mapCols; di>=1; di--) {
			for (int i = 1; i <= mapRows; ++i) {
				for (int j = 1; j <= mapCols; ++j) {
					if(PhirainEX_map[i][j].belongTo == PhirainEX_id && PhirainEX_dis[i][j] == di && PhirainEX_op[i][j] != -1) {
						int op=PhirainEX_op[i][j];
						PhirainEX_list[PhirainEX_tail][0]=i;
						PhirainEX_list[PhirainEX_tail][1]=j;
						PhirainEX_list[PhirainEX_tail][2]=op;
						PhirainEX_tail++;
					}
				}
			}
		}
		return;
	}
	bool PlanD(){
		get_dis(PhirainEX_x,PhirainEX_y,2);
		int min_dis=99999,max_army=0,max_val=0;
		int end_x=0,end_y=0;
		for (int i = 1; i <= mapRows; ++i) {
			for (int j = 1; j <= mapCols; ++j) {
				if(PhirainEX_map[i][j].belongTo == PhirainEX_id||PhirainEX_num[i][j]<=0) continue;
				if((PhirainEX_val[i][j]>max_val)||
					(PhirainEX_val[i][j]==max_val&&PhirainEX_dis[i][j]<min_dis)||
				    (PhirainEX_val[i][j]==max_val&&PhirainEX_dis[i][j]==min_dis&&PhirainEX_num[i][j]>max_army)) {
					max_val=PhirainEX_val[i][j];
					min_dis=PhirainEX_dis[i][j];
					max_army=PhirainEX_num[i][j];
					end_x=i;
					end_y=j;
				}
			}
		}
		if(end_x==0&&end_y==0) {
			return 0;
		}
		if(PlanA(end_x,end_y)) {
			return 1;
		}
		return 0;
	}
	movement Move() {
		if(PhirainEX_id == 0) {
			find_id();
		}
		getMap();
		if(generalsTurns < 0) {
			return (movement) {0, 0, 0};
		} else {
			if(PhirainEX_head >= PhirainEX_tail) {
				if(PhirainEX_mode==0&&generalsTurns>=50&&total_army>=total_land*2) {
					PhirainEX_mode=2;
					PlanC();
				}
				else{
					if(PhirainEX_mode==2){
						PhirainEX_mode=1;
						now_x=PhirainEX_x;
						now_y=PhirainEX_y;
					}
					if(PhirainEX_mode==1){
						if(moveable(now_x,now_y)==0){
							PhirainEX_mode=0;
						}
						else{
							if(PlanB(now_x,now_y)==0){
								PhirainEX_mode=0;
							}
						}
					}
					if(PhirainEX_mode==0){
						if(PlanD()==0){
							return (movement) {0, 0, 0};
						}
					}
				}
			}
			int x=PhirainEX_list[PhirainEX_head][0];
			int y=PhirainEX_list[PhirainEX_head][1];
			int op=PhirainEX_list[PhirainEX_head][2];
			PhirainEX_head++;
			while(moveable(x,y)==0&&PhirainEX_head < PhirainEX_tail){
				x=PhirainEX_list[PhirainEX_head][0];
				y=PhirainEX_list[PhirainEX_head][1];
				op=PhirainEX_list[PhirainEX_head][2];
				PhirainEX_head++;
			}
			if(moveable(x,y)==0){
				return (movement) {0, 0, 0};
			}
			now_x+=PhirainEX_dx[op];
			now_y+=PhirainEX_dy[op];
			return (movement) {x, y, op};
		}
	}
}
