#include <iostream>
#include <SFML/Network.hpp>

const unsigned short PORT = 5000;
const std::string IPADDRESS("192.168.1.66");

std::string sendMsg;

sf::TcpSocket socket;
sf::Mutex globalMutex;
bool quit = false;

void clChat(void) {
    static std::string lastMsg;
    while(!quit) {
        sf::Packet packetSend;
        globalMutex.lock();
        packetSend << sendMsg;
        globalMutex.unlock();

        socket.send(packetSend);

        std::string msg;
        sf::Packet packetReceive;

        socket.receive(packetReceive);
        if ((packetReceive >> msg) && lastMsg != msg && !msg.empty()) {
            std::cout << msg << std::endl;
            lastMsg = msg;
        }
    }
}

void server(void) {
    sf::TcpListener listener;
    listener.listen(PORT);
    listener.accept(socket);
    std::cout << "New client connected: " << socket.getRemoteAddress() << std::endl;
}


bool client(void) {
    if(socket.connect(IPADDRESS, PORT) == sf::Socket::Done) {
        std::cout << "Connected" << std::endl;
        return true;
    }
}

void getInput(void) {
    std::string s;
    std::cout << "\nMessage (\"exit\" to quit): ";
    getline(std::cin, s);
    if(s == "exit") {
        quit = true;
    }
    globalMutex.lock();
    msgSend = s;
    globalMutex = unlock();
}


int main(int argc, char* argv[]) {
    sf::Thread* thread = 0;

    int role = 0; // client by default

    for(int i = 1; i < argc; i++) {

        // this is the server
        if(strcmp(argv[i], "-s") == 0) {
            role = 1;
        }
    }

    if(role == 1) {
        server();
    }
    else {
        client();
    }

    thread = new sf::Thread(&clChat);
    thread->launch();

    while(!quit) {
        getInput();
    }

    if(thread) {
        thread->wait();
        delete thread;
    }

    return 0;
}


