#include "communicator.hpp"
#include <iostream>

Communicator::Communicator(unsigned int server_port, std::string ip):server_port_(server_port)
{
	int server_socket;

    active_ = true;
}

void Communicator::Communicate(int master_fd, int child_fd, std::string client_addr_ip, std::string client_addr_port)
{
    std::string input;

}

void Communicator::Fail(std::string text)
{
    std::cerr << "Server Error: " << text << std::endl;
}
