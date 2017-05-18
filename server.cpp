#include <iostream>
#include <progbase-cpp/net.h>
#include "include/actorsLoader.h"
#include "include/server.h"
#include <list>

using namespace std;
using namespace progbase::net;

int main(void) {

    Server * server = new Server(7777);
    server->work();

    delete server;

    return 0;
}