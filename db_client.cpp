#include "json.hpp"
#include <iostream>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

using json = nlohmann::json;

struct send_data { 
  std::string command {};
  std::string userInput{};
};
uint64_t create_socket () {
  int sock = socket(AF_INET, SOCK_STREAM, 0);

  if(sock == -1) {
    return 1;
  }
  return sock;
}


void to_json (json& j, const send_data &sd) {
  j = json{{"command", sd.command}, {"params", sd.userInput}};
}
// void from_json (const json& j, person &p) {
//   j.at("command").get_to(p.command);
//   j.at("params").get_to(p.name);
// }
int main () {

  int sock = create_socket();

  int port = 8005;
  std::string ipAdress {"127.0.0.1"};

  sockaddr_in hint;
  hint.sin_family = AF_INET;
  hint.sin_port = htons(port);
  inet_pton(AF_INET, ipAdress.c_str(), &hint.sin_addr);
 
  int connectRes = connect(sock, (sockaddr*)&hint, sizeof(hint));

  if (connectRes == -1) {
    return 1;
  }
  char buff[4096];

  send_data data {};

  do {
    std::cout << "> ";
    std::cin >> data.command;
    getline(std::cin, data.userInput);

    json j = data;
    std::string string_to_send = j.dump();

    int sendRes = send(sock, string_to_send.c_str(), string_to_send.size() + 1, 0);

    if (sendRes == -1) {
      std::cout << "could not send to server! \n";
      continue;
    }


    memset(buff, 0, sizeof(buff));
    int bytesReceived = recv(sock, buff, 4096, 0);



    //std::cout << "Server> " << std::string(buff,bytesReceived) << std::endl;

  } while (true);
  close(sock);

}