#include "json.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>


using json = nlohmann::json;

struct receive_msg {
  std::string command {};
  std::string params {};
};

struct person {
  uint64_t id;
  std::string name;
  std::string adress;
};

uint64_t socket_create () {
  uint64_t listening = socket(AF_INET, SOCK_STREAM, 0);

  if (listening == -1) {
    std::cerr << "Can't create a socket";
    return -1;
  }
  return listening;
}
int bind (uint64_t listening) {
  sockaddr_in hint;
  hint.sin_family = AF_INET;
  hint.sin_port = htons(8005);
  inet_pton(AF_INET, "0.0.0.0", &hint.sin_addr);

  if (bind(listening, (sockaddr*)&hint, sizeof(hint)) == -1) {
    std::cerr << "Can't bind to IP/port";
    return -2;
  }
  if (listen(listening, SOMAXCONN) == -1) {
    std::cerr << "Can't listen";
    return -3;
  }
}
int accept_call (uint64_t listening) {
  sockaddr_in client;
  socklen_t clien_size = sizeof(client);
  char host[NI_MAXHOST];
  char svc[NI_MAXSERV];

  int client_socket = accept(listening, (sockaddr*)&client, &clien_size);
  if (client_socket == -1) {
    std::cerr << "Problem with client connecting";
  }
  close(listening);

  memset(host, 0, NI_MAXHOST);
  memset(svc, 0, NI_MAXSERV);
  
  int result = getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, svc, NI_MAXSERV, 0);
  
  if (result) {
    std::cout << host << " connected on " << svc << std::endl;
  } else {
    inet_ntop(listening, &client.sin_addr, host, NI_MAXHOST);
    std::cout << host << " connected on " << ntohs(client.sin_port) << std::endl;
  }
  return client_socket;
}


void to_json (json& j, const person &p) {
  j = json{{"id", p.id},
  {"name", p.name},
  {"address", p.adress}};
}
void from_json (const json& j, receive_msg &msg) {
  j.at("command").get_to(msg.command);
  j.at("params").get_to(msg.params);
}

int main () {
  uint64_t listening = socket_create();
  bind(listening);
  int client_socket = accept_call(listening);

  char buff[4096];
  std::ofstream otf;
  otf.open("db.json");
  receive_msg msg {};
  person p{};
  while (true) {

    memset(buff, 0, 4096);

    int bytesRecv = recv(client_socket, buff, sizeof(buff), 0);

    if (bytesRecv == -1) {
      std::cerr << "There was a connection issue";
      break;
    }
    if (bytesRecv == 0) {
      std::cout << "Client disconnected\n";
      break;
    }
    auto serv_j = json::parse(buff);
    auto conv_json_to_msg = serv_j.get<receive_msg>();
    msg = conv_json_to_msg;
    if (msg.command == "insert") {
      std::istringstream ist (msg.params);
      std::vector<std::string> splitted {std::istream_iterator<std::string>{ist},
                                          std::istream_iterator<std::string>{}};
      std::string pid = splitted.at(0);
      p.id = std::stoi(pid);
      p.name = splitted.at(1);   
      p.adress = splitted.at(2);
    }
    
    json to_write = p;
    std::cout << to_write;

    otf << to_write; 

    //std::cout << "Received " << std::string(buff, 0, bytesRecv) << std::endl;

    //send(client_socket, buff, bytesRecv + 1, 0); 
   
  }
  close(client_socket);

}

