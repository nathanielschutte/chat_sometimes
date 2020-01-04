#include <iostream>
#include <SFML/Network.hpp>
#include <SFML/System.hpp>

const unsigned short PORT = 50001;
const sf::IpAddress IPADDRESS("192.168.1.66");

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
    return false;
}

void getInput(void) {
    std::string s;
    std::cout << "\nMessage (\"exit\" to quit): ";
    getline(std::cin, s);
    if(s == "exit") {
        quit = true;
    }
    globalMutex.lock();
    sendMsg = s;
    globalMutex.unlock();
}


int main(int argc, char* argv[]) {
    sf::Thread* thread = 0;

    char role = 'c'; // client by default
    std::cout << "Are you a server (s) or a client (c)? ";
    std::cin >> role;


    if(role == 's') {
        server();
    }
    else {
        if(!client()) {
            std::cout << "Failed to connect." << std::endl;
        }
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


