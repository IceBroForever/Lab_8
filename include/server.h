/** @file
    @brief Server class
*/
#pragma once
#include <progbase-cpp/net.h>
#include "actorsLoader.h"
#include "actor.h"

using namespace progbase::net;

/**
    @struct Request
    @brief struct, whitch used in privates methods of class Server to get type of request
*/
struct Request;

class Server{
private:
	int _port;
	TcpListener * _listener;
	NetMessage * _message;

	std::string _title;
	std::string _developer;

	std::list<Actor *> list;

/**
	@brief parse Request string to get type of Request
	@param requestStr - string to e parsing
	@returns struct Request with type of request
*/
	Request parseRequestStr(std::string requestStr);
/**
	@brief creating string with responce
	@param request - type of request
	@returns string with responce
*/
	std::string createAnswer(Request request);
/**
	@brief creating JSON document with information about server
	@returns string, whitch contains JSON document
*/
	std::string serverInformation();
/**
	@brief getting time on server
	@returns string, whitch contains server time
*/
	std::string serverTime();
/**
	@brief creating JSON document with information about all actors
	@returns string with JSON document
*/
	std::string actorsJSON();
/**
	@brief creating JSON document with information about one actor
	@param actor - actor, whitch information should be documented
	@returns string with JSON document
*/
	std::string actorJSON(Actor * actor);
/**
	@brief creating JSON document with information about all actors by key
	@param key - param, by whitch actors must be choosed
	@param value - value of key
	@returns string with JSON document
*/
	std::string actorsJSONByKey(std::string key, std::string value);
/**
	@brief read data from file "data.txt"
	@returns string with data from file "data.txt"
*/
	std::string readDataFile();
/**
	@brief calculating size of file "data.txt"
	@returns size of file "data.txt"
*/
	int dataFileSize();
/**
	@brief creating JSON document about file "data.txt"
	@returns string with JSON document
*/
	std::string fileJSON();
/**
	@brief creating JSON document with all digits on paired position file "data.txt"
	@returns string with JSON document
*/
	std::string fileDataJSON();

public:
/**
	@brief Server construction
*/
	Server(int port);
/**
	@brief Server destructor
*/
	~Server();

/**
	@brief starting of server and handling clients
*/
	void work();
};