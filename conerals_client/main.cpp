#include<bits/stdc++.h>
#include<windows.h>
using namespace std;
int main(int argc, char* argv[]){
    string prefix = argv[0];
    prefix = prefix.substr(0, prefix.size() - 8);
    int exitCode; restart_game:;
    system("title Generals");
    exitCode = system((prefix + "generals.exe").c_str());
    stringstream stream;
    stream << hex << exitCode;
    string hex(stream.str());
    if(exitCode && MessageBox(NULL, ("Game crashed! Exitcode: " + to_string(exitCode) + "(0x" + hex + ")").c_str(), "Oops!", MB_RETRYCANCEL | MB_ICONWARNING) == IDRETRY) goto restart_game;
    return 0;
}
