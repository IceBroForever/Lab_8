/** @file
    @brief class to get actors information from ImDB API
*/
#pragma once
#include <progbase-cpp/net.h>
#include "actor.h"
#include <list>

using namespace progbase::net;

class ActorsLoader{
private:
	TcpClient * client;
	NetMessage * message;
	IpAddress * ip;

/**
	@brief getting responce from ImDB with information about actor by name
	@param fullName - name of actor
	@returns string with JSON document about actor
*/
	std::string getResponceFromIMDBByName(std::string fullName);
/**
	@brief parsing responce from ImDB with information about actor by name
	@param responce - string with responce from ImDB
	@param fullName - name of actor
	@returns
*/
	Actor * parseResponce(std::string responce, std::string fullName);

public:
/**
	@brief ActorsLoader constructor
*/
	ActorsLoader();
/**
	@brief ActorsLoader destructor 
*/
	~ActorsLoader();

/**
	@brief connecting to ImDB API
*/
	void connectToIMDB();

/**
	@brief getting list of actors
	@returns list of actors
*/
	std::list<Actor *> getListOfActors();
};