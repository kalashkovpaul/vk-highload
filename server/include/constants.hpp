#pragma once

const int POOL_SIZE = 50;
const int PORT = 8080;
const int CONN_COUNT = 400;
const int REQ_SIZE = 1024;
const std::string HTTP_1_1 = "HTTP/1.1\r";
const std::string HTTP_1_0 = "HTTP/1.0\r";
const std::string ROOT_FILE = "index.html";
const std::string CONTENT_LENGTH = "Content-Length:";
const std::string CONTENT_TYPE = "Content-Type:";
const std::string HEAD = "HEAD";
const std::string GET = "GET";

// return codes
const int OK = 0;
const int SERVER_ERROR = -1;
const int FILE_OPEN_ERROR = -2;
