/** @file
    @brief class Actor
*/
#pragma once
#include <string>

class Actor{
private:
	std::string _id;
	std::string _name;
	std::string _surname;
	std::string _mostPopularFilm;
public:
/**
	@brief Actor constructor
*/
	Actor(std::string id, std::string fullName, std::string film);

/**
	@brief getting id on ImDB of actor
	@return string with id
*/
	std::string id();
/**
	@brief getting name of actor
	@return string with name
*/
	std::string name();
/**
	@brief getting surname actor
	@return string with surname
*/
	std::string surname();
/**
	@brief getting most popular film with actor
	@return string with most popular film
*/
	std::string mostPopularFilm();
};