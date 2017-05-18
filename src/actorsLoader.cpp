#include "../include/actorsLoader.h"
#include <algorithm>
#include <jansson.h>

ActorsLoader::ActorsLoader(){
	ip = new IpAddress(Ip::resolveHostname(std::string("www.imdb.com")), 80);

	message = new NetMessage(1024);
}

void ActorsLoader::connectToIMDB(){
	if(client != NULL) delete client;

	client = new TcpClient();
	client->connect(*ip);
}

std::string ActorsLoader::getResponceFromIMDBByName(std::string fullName){
	std::transform(fullName.begin(), fullName.end(), fullName.begin(), ::tolower);

	int i = 0;
	for(int n = fullName.length(); i < n && fullName[i] != ' '; i++);

	std::string name = fullName.substr(0, i);
	std::string surname = fullName.substr(i + 1);

	std::string path = std::string("/xml/find?json=1&nr=1&nm=on&q=") + name + "+" + surname;

	message->setDataString(std::string("GET ") + path + std::string(" HTML/1.0\r\n\r\n"));

	client->send(*message);

	std::string responce;
	client->receive(*message);
	while(message->dataAsString().length() != 0){
		responce += message->dataAsString();
		message->setDataString("");
		client->receive(*message);
	}

	int pos = responce.find("\r\n\r\n", 0);
	responce = responce.substr(pos + 4);

	return responce;
}

Actor * ActorsLoader::parseResponce(std::string responce, std::string fullName){
	json_error_t err;
	json_t * json = json_loads(responce.c_str(), 0, &err);

	json_t * actors = json_object_get(json, "name_popular");
	int i = 0;
	json_t * value = NULL;
	json_array_foreach(actors, i, value){
		std::string name((char *)json_string_value(json_object_get(value, "name")));
		if(name == fullName){
			std::string id((char *)json_string_value(json_object_get(value, "id")));
			std::string description((char *)json_string_value(json_object_get(value, "description")));
			std::string film = description.substr(description.find(',', 0) + 2);

			json_decref(json);

			return new Actor(id, name, film);
		}
	}

	json_decref(json);

	return NULL;
}

std::list<Actor *> ActorsLoader::getListOfActors(){
	std::list<Actor *> list;

	std::string actors[] = {
		"Matthew McConaughey",
		"Keanu Reeves",
		"Johnny Depp",
		"Chris Pratt",
		"Brad Pitt",
		"Bradley Cooper",
		"Michael Rooker",
		"Jason Statham",
		"Paul Walker",
		"Zac Efron"
	};
	int numOfActors = 10;

	for(int i = 0; i < numOfActors; i++){
		connectToIMDB();
		list.push_back(parseResponce(getResponceFromIMDBByName(actors[i]), actors[i]));
	}

	return list;
}

ActorsLoader::~ActorsLoader(){
	delete client;
	delete message;
	delete ip;
}
