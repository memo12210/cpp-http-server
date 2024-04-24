#include "http_operations.hpp"

request::request(char* body) 
{
  std::string req_body(body);
  parse_request(req_body);
}

std::string request::set_path(const std::string &request) 
{
  // path is contained in the first line of the request
  std::string line = request.substr(0, request.find(CRLF));

  // if path does not contain GET, return empty string
  if (line.find("GET") == std::string::npos) { return ""; }

  return line.substr(line.find("GET") + 4, line.find("HTTP") - 5);
}

std::string request::set_host(const std::string &request) 
{
  // host is contained in the second line of the request
  std::string line = request.substr(request.find(CRLF) + 2, request.find(CRLF, request.find(CRLF) + 2) - request.find(CRLF) - 2);

  // if host does not contain Host, return empty string
  if (line.find("Host") == std::string::npos) { return ""; }
  
  return line.substr(line.find("Host") + 6);
}

std::string request::set_user_agent(const std::string &request) 
{
  // return the substring after "User-Agent: " until the next CRLF
  std::string user_agent = request.substr(request.find("User-Agent: ") + 12, request.find(CRLF, request.find("User-Agent: ")) - request.find("User-Agent: ") - 12);

  // trim the user agent string
  while (user_agent.back() == ' ') { user_agent.pop_back(); }
  return user_agent;
}

// parse 'GET /echo/<a-random-string>' into 'a-random-string' 
std::string request::set_message(const std::string &path) 
{
  if(path.find("echo/") == std::string::npos) { return ""; }
  
  std::string msg = path.substr(path.find("echo/") + 5);
  return msg;
}

std::string request::get_method() 
{
  return method;
}

std::string request::get_path() 
{
  return path;
}

std::string request::get_host() 
{
  return host;
}

std::string request::get_user_agent() 
{
  return user_agent;
}

std::string request::get_message() 
{
  return message;
}

void request::parse_request(const std::string &request) 
{
  method = "GET";
  path = set_path(request);
  host = set_host(request);
  user_agent = set_user_agent(request);
  message = set_message(path);
}

std::string response::get_status() 
{
  return status;
}

void response::set_status(std::string status) 
{
  this->status = status;
}

std::string response::get_content_type() 
{
  return content_type;
}

void response::set_content_type(std::string content_type) 
{
  this->content_type = content_type;
}

std::string response::get_content_length() 
{
  return content_length;
}

void response::set_content_length(std::string content_length) 
{
  this->content_length = content_length;
}

std::string response::get_message() 
{
  return message;
}

void response::set_message(std::string message) 
{
  this->message = message;
}

std::string response::get_response_body() 
{
  return body;
}

void response::set_response_body() 
{
  body += HTTP " " + status + CRLF;
  body += "Content-Type: " + content_type + CRLF;
  body += "Content-Length: " + content_length + CRLF + CRLF;
  body += message;
}