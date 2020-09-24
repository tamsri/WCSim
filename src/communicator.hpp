#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <string>


class Communicator {
	
public:
	Communicator(unsigned int server_port = 8888, std::string ip = "127.0.0.1");
	static void Communicate(int master_fd, int child_fd, std::string client_addr_ip, std::string client_addr_port);
	void Fail(std::string text);
private:
	unsigned int server_port_;
	float active_;
};


#endif //!COMMUNICATOR_H