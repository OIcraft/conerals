// join
#include <bits/stdc++.h>
using namespace std;
vector<string> AInames;
string AIname;
int AI_total;
int main(){
	// read AI
	ifstream AIsR("AIs.txt");
	AIsR >> AI_total;
	for (int i = 1; i <= AI_total; ++i){
		AIsR >> AIname;
		AInames.push_back(AIname);
	}
	AIsR.close();
	// add new AI
	cout << "\nInput your AI's name :\n";
	cin >> AIname;
	if (AIname == "human" || AIname == "humanOptions"){
		cout << "Name illegal.\n";
		return 0;
	}
	bool exists = false;
	for (string AI : AInames){
		if (AI == AIname){
			cout << "This AI already exists.\n";
			exists = true;
			break;
		}
	}
	if (!exists){
		cout << "Join successfully !\n";
		AInames.push_back(AIname);
		++AI_total;
	}
	// write again
	ofstream AIsW("AIs.txt");
	AIsW << AI_total << '\n';
	for (string AI : AInames){
		AIsW << AI << '\n';
	}
	AIsW.close();
	// make "getAIs.h"
	ofstream getAIs("getAIs.h");
	getAIs << "// get AIs\n";
	getAIs << "#include \"humanOptions.h\"\n";
	for (string AI : AInames){
		getAIs << "#include \".\\AI\\";
		getAIs << AI;
		getAIs << ".h\"\n";
	}
	getAIs << "// initialize\n";
	getAIs << "void Initialize(string currentAIname){\n";
	getAIs << "    if (currentAIname == \"human\"){\n";
	getAIs << "        humanOptions::Init();\n";
	getAIs << "        return;\n";
	getAIs << "    }\n";
	for (string AI : AInames){
		getAIs << "    else if (currentAIname == \"" << AI << "\"){\n";
		getAIs << "        " << AI << "::" << "Init();\n";
		getAIs << "        return;\n";
		getAIs << "    }\n";
	}
	getAIs << "    return;\n";
	getAIs << "}\n";
	getAIs << "// get moves\n";
	getAIs << "movement GetMove(string currentAIname){\n";
	getAIs << "    if (currentAIname == \"human\"){\n";
	getAIs << "        humanOptions::GetTheTime(theStartTimeOfTheCurrentTurn);\n";
	getAIs << "        return humanOptions::Move();\n";
	getAIs << "    }\n";
	for (string AI : AInames){
		getAIs << "    else if (currentAIname == \"" << AI << "\"){\n";
		getAIs << "        return " << AI << "::Move();\n";
		getAIs << "    }\n";
	}
	getAIs << "    return (movement){-1, -1, 0};\n";
	getAIs << "}\n";
	getAIs.close();
	return 0;
}
