#pragma once
#include <iostream>

#define HTTP "HTTP/1.1"
#define CRLF "\r\n"
#define OK "200 OK"
#define NOT_FOUND "404 Not Found"

class request 
{
private:
    std::string method; // ex: GET, POST, PUT, DELETE
    std::string path; // ex: /echo/abc
    std::string host; // ex: localhost:8080
    std::string user_agent; // ex: curl/7.68.0
    std::string message; // ex: abc

    // helper functions
    std::string set_path(const std::string &request);
    std::string set_host(const std::string &request);
    std::string set_user_agent(const std::string &request);
    std::string set_message(const std::string &path);
public:
    request(char* body);
    void parse_request(const std::string &request);

    // getters
    std::string get_method();
    std::string get_path();
    std::string get_host();
    std::string get_user_agent();
    std::string get_message();
};

class response 
{
private:
    std::string body;
    std::string status; // ex: 200 OK, 404 Not Found
    std::string content_type; // ex: text/plain
    std::string content_length; // ex: 3
    std::string message; // ex: abc
public:
    // getter-setter functions
    std::string get_status();
    void set_status(std::string status);
    std::string get_content_type();
    void set_content_type(std::string content_type);
    std::string get_content_length();
    void set_content_length(std::string content_length);
    std::string get_message();
    void set_message(std::string message);
    std::string get_response_body();
    void set_response_body();
};