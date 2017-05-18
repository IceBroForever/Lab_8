#include "../include/server.h"
#include <ctime>
#include <jansson.h>
#include <fstream>

enum RequestType{
	ServerInformation = 0,
	AllActors,
	ActorsByKey,
	ActorsByID,
	File,
	FileData,
	NonValidate
};

struct Request{
	RequestType type;
	std::string key;
	std::string value;
	int id;
};

Server::Server(int port){
	_title = std::string("Server for Lab 8");
	_developer = std::string("IceBro");

	_port = port;
	_message = new NetMessage(1024);

	_listener = new TcpListener();
	try{
		_listener->bind(IpAddress("127.0.0.1", _port));
		_listener->start();
	} catch(NetException const & exc){
		std::cout << exc.what() << std::endl;
	}

	ActorsLoader * loader = new ActorsLoader();
	list = loader->getListOfActors();
	delete loader;
}

Server::~Server(){
	delete _message;
	delete _listener;
	for(auto a : list) delete a;
}

void Server::work(){
	while(true){
		TcpClient * client = _listener->accept();
		client->receive(*_message);
		
		Request request = parseRequestStr(_message->dataAsString());
		_message->setDataString("");
		std::string answer = createAnswer(request);

		for(int i = 0, n = answer.length(); i < n; i+=1000){
			_message->setDataString(answer.substr(i, 1000));
			client->send(*_message);
		}

		delete client;
	}
}

Request Server::parseRequestStr(std::string requestStr){
	Request request;

	int posOfSpace = requestStr.find(' ', 0);
	std::string method = requestStr.substr(0, posOfSpace);

	if(method != std::string("GET")){
		request.type = RequestType::NonValidate;
		return request;
	}

	int secondPosOfSpace = requestStr.find(' ', posOfSpace + 1);
	std::string path = requestStr.substr(posOfSpace + 1, secondPosOfSpace - posOfSpace - 1);

	if(path == std::string("/")) request.type = RequestType::ServerInformation;
	else if(path == std::string("/file/data")) request.type = RequestType::FileData;
	else if(path == std::string("/file")) request.type = RequestType::File;
	else if(path == std::string("/favorites")) request.type = RequestType::AllActors;
	else if(path.find("/favorites/", 0) != std::string::npos){
		request.type = RequestType::ActorsByID;
		request.id = std::atoi(path.c_str() + 11);
	}
	else{
		unsigned int pos = path.find("/favorites?", 0);
		unsigned int posOfEq = path.find('=', 0);
		if(pos != std::string::npos && posOfEq != std::string::npos){
			request.type = RequestType::ActorsByKey;
			request.key = path.substr(11, posOfEq - 11);
			request.value = path.substr(posOfEq + 1);
		}
		else request.type = RequestType::NonValidate;
	}
	return request;
}

std::string Server::createAnswer(Request request){
	std::string answer;
	std::string goodHeader("HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n");
	switch(request.type){
		case RequestType::ServerInformation:{
			answer = goodHeader + serverInformation();
		}break;
		case RequestType::FileData:{
			answer = goodHeader + fileDataJSON();
		}break;
		case RequestType::File:{
			answer = goodHeader + fileJSON();
		}break;
		case RequestType::AllActors:{
			answer = goodHeader + actorsJSON();
		}break;
		case RequestType::ActorsByID:{
			if(request.id >= (int)list.size() || request.id < 0) answer = std::string("HTTP/1.1 404 Not Found\r\n\r\n");
			else{
				std::list<Actor *>::iterator it = list.begin();
				std::advance(it, request.id);
				Actor * actor = *it;
				answer = goodHeader + actorJSON(actor);
			}
		}break;
		case RequestType::ActorsByKey:{
			answer = goodHeader + actorsJSONByKey(request.key, request.value);
		}break;
		case RequestType::NonValidate:{
			answer = std::string("HTTP/1.1 404 Not Found\r\n\r\n");
		}break;
	}
	return answer;
}

std::string Server::serverInformation(){
	json_t * json = json_object();

	json_object_set_new(json, "title", json_string(_title.c_str()));
	json_object_set_new(json, "developer", json_string(_developer.c_str()));
	json_object_set_new(json, "time", json_string(serverTime().c_str()));

	char * jsonString = json_dumps(json, JSON_INDENT(2));

	json_decref(json);

	return std::string(jsonString);
}

std::string Server::serverTime(){
	time_t rawtime;
	struct tm * timeinfo;
	char buffer[100];

	time (&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer,sizeof(buffer),"%d-%m-%Y %I:%M:%S",timeinfo);
	return std::string(buffer);
}

std::string Server::actorsJSON(){
	json_t * json = json_object();

	json_t * actors = json_array();
	for(auto a : list){
		json_t * actor = json_object();
		json_object_set_new(actor, "id", json_string(a->id().c_str()));
		json_object_set_new(actor, "name", json_string(a->name().c_str()));
		json_object_set_new(actor, "surname", json_string(a->surname().c_str()));
		json_object_set_new(actor, "film", json_string(a->mostPopularFilm().c_str()));
		json_array_append_new(actors, actor);
	}
	json_object_set_new(json, "actors", actors);

	char * jsonString = json_dumps(json, JSON_INDENT(2));

	json_decref(json);
	return std::string(jsonString);
}

std::string Server::actorJSON(Actor * actor){
	json_t * actorJSON = json_object();

	json_object_set_new(actorJSON, "id", json_string(actor->id().c_str()));
	json_object_set_new(actorJSON, "name", json_string(actor->name().c_str()));
	json_object_set_new(actorJSON, "surname", json_string(actor->surname().c_str()));
	json_object_set_new(actorJSON, "film", json_string(actor->mostPopularFilm().c_str()));

	char * jsonString = json_dumps(actorJSON, JSON_INDENT(2));

	json_decref(actorJSON);
	return std::string(jsonString);
}

std::string Server::actorsJSONByKey(std::string key, std::string value){
	json_t * json = json_object();

	json_t * actors = json_array();
	for(auto a : list){
		bool add = false;
		if(key == std::string("id") && a->id() == value) add = true;
		else if(key == std::string("name") && a->name() == value) add = true;
		else if(key == std::string("surname") && a->surname() == value) add = true;
		else if(key == std::string("film") && a->mostPopularFilm() == value) add = true;
		if(!add) continue;
		json_t * actor = json_object();
		json_object_set_new(actor, "id", json_string(a->id().c_str()));
		json_object_set_new(actor, "name", json_string(a->name().c_str()));
		json_object_set_new(actor, "surname", json_string(a->surname().c_str()));
		json_object_set_new(actor, "film", json_string(a->mostPopularFilm().c_str()));
		json_array_append_new(actors, actor);
	}
	json_object_set_new(json, "actors", actors);

	char * jsonString = json_dumps(json, JSON_INDENT(2));

	json_decref(json);
	return std::string(jsonString);
}

std::string Server::readDataFile(){
	std::ifstream data("data/data.txt");
	std::filebuf* dataBuf = data.rdbuf();

	std::size_t size = dataBuf->pubseekoff (0,data.end,data.in);
	dataBuf->pubseekpos (0,data.in);

	char* buffer = new char[size];
	dataBuf->sgetn (buffer,size);

	data.close();

	std::string dataStr(buffer);
	delete[] buffer;
	return dataStr;
}

int Server::dataFileSize(){

	std::ifstream data("data/data.txt");
	std::filebuf* dataBuf = data.rdbuf();

	int size = (int)dataBuf->pubseekoff (0,data.end,data.in);
	dataBuf->pubseekpos (0,data.in);

	data.close();

	return size;
}

std::string Server::fileJSON(){
	json_t * fileJSON = json_object();

	json_object_set_new(fileJSON, "name", json_string("data.txt"));
	json_object_set_new(fileJSON, "size", json_integer(dataFileSize()));
	json_object_set_new(fileJSON, "content", json_string(readDataFile().c_str()));

	char * jsonString = json_dumps(fileJSON, JSON_INDENT(2));

	json_decref(fileJSON);
	return std::string(jsonString);
}

std::string Server::fileDataJSON(){
	std::string data = readDataFile();
	int size = dataFileSize();

	json_t * fileJSON = json_object();
	json_t * digits = json_array();
	for(int i = 0; i < size; i += 2){
		if(data[i] < '0' || data[i] > '9') continue;
		json_t * digit = json_object();
		json_object_set_new(digit, "digit", json_string(data.substr(i, 1).c_str()));
		json_array_append_new(digits, digit);
	}
	json_object_set_new(fileJSON, "digits", digits);

	char * jsonString = json_dumps(fileJSON, JSON_INDENT(2));

	json_decref(fileJSON);
	return std::string(jsonString);
}