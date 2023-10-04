#include <iostream>
#include "server.h"

void run()
{
    std::shared_ptr<Server> server = std::make_shared<Server>();

    if(server->connectToDB("127.0.0.1", 4000)){
        std::cout << "start" << std::endl;
        server->start(2000, 3000);
        int a;
        std::cin >> a;
    }
};

int main(){
    run();
    return 0;
};