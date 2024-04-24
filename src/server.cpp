#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>

#include "http_operations.hpp"

#define HTTP "HTTP/1.1"
#define CRLF "\r\n"
#define OK "200 OK"
#define NOT_FOUND "404 Not Found"

void handle_client(int client_fd)
{
  std::cout << "Client connected, handling request...\n";

  char buffer[1024] = {0};
  int valread = read(client_fd, buffer, 1023);
  if (valread < 0) 
  {
    std::cerr << "read failed\n";
    close(client_fd);
    return;
  }

  request req(buffer);
  response res;

  // OK conditions; if path == '/' or path == '/echo/<message>'
  if (req.get_path() == "/" || req.get_path().find("echo/") != std::string::npos) 
  {
    res.set_status(OK);
    res.set_content_type("text/plain");
    res.set_content_length(std::to_string(req.get_message().size()));
    res.set_message(req.get_message());
    res.set_response_body();
  }

  else if(req.get_path() == "/user-agent") 
  {
    res.set_status(OK);
    res.set_content_type("text/plain");
    res.set_content_length(std::to_string(req.get_user_agent().size()));
    res.set_message(req.get_user_agent());
    res.set_response_body();
  }

  else 
  {
    res.set_status(NOT_FOUND);
    res.set_content_type("");
    res.set_content_length("");
    res.set_message("");
    res.set_response_body();
  }

  send(client_fd, res.get_response_body().c_str(), res.get_response_body().size(), 0);
  close(client_fd);
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

  std::cout << "Server started on port 4221, waiting for clients\n";

  std::vector<std::thread> threads;

  while(true)
  {
    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);

    std::cout << "Waiting for a client to connect...\n";
    int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
    if(client_fd < 0) 
    {
      std::cerr << "accept failed\n";
      continue; // continue waiting for new clients
    }

    threads.push_back(std::thread(handle_client, client_fd)); // create a new thread to handle the client
    threads.back().detach(); // detach the thread so that it can run independently
  }
  
  close(server_fd);
  return 0;
}