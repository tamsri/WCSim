#ifndef SERVER_H
#define SERVER_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
namespace asio = boost::asio;

class Engine;

class Server{
public:
    Server(unsigned int port_number, Engine * engine_);

    void Run(const boost::system::error_code err);
private:

    asio::io_context io_context_;
    asio::ip::tcp::acceptor acceptor;
    Engine * engine_;
};
#endif