#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#define HTTP "HTTP/1.1"
#define CRLF "\r\n"
#define OK "200 OK"
#define NOT_FOUND "404 Not Found"

std::string get_path_from_request(const std::string &request) 
{
  // path is contained in the first line of the request
  std::string line = request.substr(0, request.find(CRLF));

  // if path does not contain GET, return empty string
  if (line.find("GET") == std::string::npos) { return ""; }

  return line.substr(line.find("GET") + 2, line.find("HTTP") - 5);
}


int main(int argc, char **argv) 
{
  // You can use print statements as follows for debugging, they'll be visible when running tests.
  std::cout << "Logs from your program will appear here!\n";

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) 
  {
    std::cerr << "Failed to create server socket\n";
    return 1;
  }
  
  // Since the tester restarts your program quite often, setting REUSE_PORT
  // ensures that we don't run into 'Address already in use' errors
  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) 
  {
    std::cerr << "setsockopt failed\n";
    return 1;
  }
  
  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(4221);
  
  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) 
  {
    std::cerr << "Failed to bind to port 4221\n";
    return 1;
  }
  
  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) 
  {
    std::cerr << "listen failed\n";
    return 1;
  }

  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);
  
  std::cout << "Waiting for a client to connect...\n";

  int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
  if(client_fd < 0) 
  {
    std::cerr << "accept failed\n";
    return 1;
  }

  std::cout << "Client connected\n";

  char buffer[1024] = {0};
  int valread = read(client_fd, buffer, 1023);
  if (valread < 0) 
  {
    std::cerr << "read failed\n";
    return 1;
  }

  std::string request(buffer);

  std::string path = get_path_from_request(request);

  std::string response = HTTP " ";

  if(path.empty()) 
  {
    response += NOT_FOUND CRLF CRLF;
  }

  else
  {
    response += OK CRLF CRLF;
  }

  send(client_fd, response.c_str(), response.size(), 0);
  
  close(client_fd);
  close(server_fd);

  return 0;
}
