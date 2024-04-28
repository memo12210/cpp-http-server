#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>

#include "http_operations.hpp"

#define HTTP "HTTP/1.1"
#define CRLF "\r\n"
#define OK "200 OK"
#define CREATED "201 Created"
#define NOT_FOUND "404 Not Found"

void handle_client(int client_fd, char* argv[])
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

  // if path == '/' or path == '/echo/<message>'
  if(req.get_path() == "/" || req.get_path().find("echo/") != std::string::npos) 
  {
    res.set_status(OK);
    res.set_content_type("text/plain");
    res.set_content_length(std::to_string(req.get_message().size()));
    res.set_message(req.get_message());
    res.set_response_body();
  }

  // for the case when the path is user-agent
  else if(req.get_path() == "/user-agent") 
  {
    res.set_status(OK);
    res.set_content_type("text/plain");
    res.set_content_length(std::to_string(req.get_user_agent().size()));
    res.set_message(req.get_user_agent());
    res.set_response_body();
  }
  
  // for the case when the path is /files/<filename>
  // If <filename> exists in <directory>, respond with a 200 OK response
  else if(strcmp(argv[2],"--directory") == 0 && req.get_path().find("/files/") != std::string::npos)
  {
    // TODO: if request mode is POST, handle the request and return CREATE response
    // if request mode is GET, handle the request and return OK response

    std::cout << "File request,";

    std::string file_path = argv[2] + req.get_path().substr(7);
    struct stat sb;

    // check if the file does exist
    if(stat(file_path.c_str(), &sb) == 0 && !(sb.st_mode & S_IFDIR)) 
    {
      if(req.get_method() == "POST")
      {
        std::cout << "POST request\n";
        std::ofstream file(file_path);
        file << req.get_message();
        file.close();

        res.set_status(CREATED);
        res.set_content_type("");
        res.set_content_length("");
        res.set_message("");
        res.set_response_body();
      }

      else
      {
        std::cout << "GET request\n";
        res.set_status(OK);
        res.set_content_type("application/octet-stream"); // binary file
        res.set_content_length(std::to_string(sb.st_size));

        // response should contain the contents of the file
        std::ifstream file(file_path, std::ios::binary);
        std::string file_contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        res.set_message(file_contents);
        res.set_response_body();
      }
    }

    else 
    {
      res.set_status(NOT_FOUND);
      res.set_content_type("");
      res.set_content_length("");
      res.set_message("");
      res.set_response_body();
    }
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

int main(int argc, char* argv[]) 
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

    threads.push_back(std::thread(handle_client, client_fd,argv)); // create a new thread for the client
    threads.back().detach(); // detach the thread so that it can run independently
  }

  close(server_fd);
  return 0;
}