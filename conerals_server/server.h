#include <bits/stdc++.h>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
using namespace std;

string Type;
sockaddr_in server_address, client_address;
int addrLen = sizeof(client_address);
map<string, time_t> mp;

// URL decoding function
string decode_url(const string& url) {
    string decoded_url;
    for (size_t i = 0; i < url.length(); ++i)
        if (url[i] == '%')
            if (i + 2 < url.length()) {
                int value = 0;
                stringstream ss;
                ss << hex << url.substr(i + 1, 2);
                ss >> value;
                decoded_url += static_cast<char>(value);
                i += 2; // Skip the encoded characters
            }else;
        else if (url[i] == '+') decoded_url += ' '; // Convert '+' to space
        else decoded_url += url[i]; // Directly add unencoded characters
    if(decoded_url.back() != '/') decoded_url += '/';
    return decoded_url;
}

// Simulated database
map<int, string> database;

// Process request
string workWithRequest(const string& request, const string& cip);

// Handle client request
void handle_request(SOCKET client_socket) {
	Type = "plain";
    char buffer[1024];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);

    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        string request(buffer);

        string method;
        string uri;
        istringstream request_stream(request);
        request_stream >> method;
        request_stream >> uri;

        if (method == "GET") {
            char clientIP[1024]; DWORD strLen = INET_ADDRSTRLEN;
            WSAAddressToString((sockaddr*)&(client_address), sizeof(client_address), nullptr, clientIP, (LPDWORD)&strLen);
            string cip(clientIP);
            string port = cip.substr(cip.find(":") + 1);
            cip = cip.substr(0, cip.find(":"));
            string decoded_uri = decode_url(uri.substr(1));
            string response_body = workWithRequest(decoded_uri, cip);
            string response_header = "HTTP/1.1 200 OK\r\n"
                                     "Content-Type: text/" + Type + "\r\n"
                                     "Content-Length: " + to_string(response_body.size()) + "\r\n\r\n";
            string full_response = response_header + response_body;
            send(client_socket, full_response.c_str(), full_response.size(), 0);
        }
    }
    closesocket(client_socket);
}

// Start the server
void start_server(int port) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    listen(server_socket, SOMAXCONN);

    while (true) {
        SOCKET client_socket = accept(server_socket, (struct sockaddr*)&client_address, &addrLen);
        thread(handle_request, client_socket).detach();
    }

    closesocket(server_socket);
    WSACleanup();
}
