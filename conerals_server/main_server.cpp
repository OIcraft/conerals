#include<bits/stdc++.h>
#include"server.h"
#include<windows.h>
using namespace std;
mt19937 rnd(time(0));
vector<string> nowPlayers;
int startCnt;
int AItotal;
vector<string> AInames;
map<string, int> ntp;
string prefix;
void RandomChoosePlayers(){
	for (int currentPlayer = nowPlayers.size(); currentPlayer < 8; ++currentPlayer){
		bool checkUnique = false; string str = "";
		while (!checkUnique){
			// random choose
			str = AInames[rnd() % AItotal];
			// check
			checkUnique = true;
			for(string s: nowPlayers) if(s == str){
				checkUnique = false;
				break;
			}
		}
		nowPlayers.push_back(str);
	}
	return;
}
void InitGame(){
	// read AI
	ifstream AIs(prefix + "AIs.txt");
	AIs >> AItotal;
	AInames.clear();
	for (int i = 1; i <= AItotal; ++i){
		string AIname;
		AIs >> AIname;
		AInames.push_back(AIname);
	}
	AIs.close();
}
string workWithRequest(const string& request, const string& cip){
    try {
    	if(request.size() < 5) return "404 Not Found";
    	if(request.substr(0, 5) == "game/"){
    		startCnt++;
    		return "404 Not Found";
		}
    	if(request.substr(0, 5) == "exit/"){
    		string name = request.substr(5, request.size() - 6);
    		ntp.erase(name);
    		return "404 Not Found";
		}
    	if(request.substr(0, 5) == "port/"){
    		string name = request.substr(5, request.size() - 6);
    		if(!ntp.count(name)) return "";
    		return to_string(ntp[name]);
		}
    	if(request.substr(0, 5) == "play/" && (startCnt == nowPlayers.size() || nowPlayers.size() == 8)){
    		int port = rnd() & 65535;
    		for(string s: nowPlayers) ntp[s] = port;
    		int ai = 1 << (8 - nowPlayers.size()); ai--;
    		RandomChoosePlayers();
    		stringstream ss; ss << "start " << prefix << "generals_server.exe";
    		for(string s: nowPlayers) ss << " " << s;
    		ss << " " << port << " " << ai << " 9 28";
    		string str = ss.str(); system(str.c_str());
    		startCnt = 0; nowPlayers.clear();
    	}
    	if(request.substr(0, 5) == "join/") nowPlayers.push_back(request.substr(5, request.size() - 6));
        return "404 Not Found";
    } catch (string s) {
        return "ERROR:\n- " + s;
    }
}
int main(int argc, char* argv[]){
    string t = argv[0];
    prefix = t.substr(0, t.size() - 15);
    InitGame();
	start_server(0xCA0);
    return 0;
}
