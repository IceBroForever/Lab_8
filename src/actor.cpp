#include "../include/actor.h"

Actor::Actor(std::string id, std::string fullName, std::string film){
	_id = id;
	_mostPopularFilm = film;

	int pos = fullName.find(' ', 0);
	_name = fullName.substr(0, pos);
	_surname = fullName.substr(pos + 1);
}

std::string Actor::id(){ return _id; }

std::string Actor::name(){ return _name; }

std::string Actor::surname(){ return _surname; }

std::string Actor::mostPopularFilm(){ return _mostPopularFilm; }