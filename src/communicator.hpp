#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include <string>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>


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