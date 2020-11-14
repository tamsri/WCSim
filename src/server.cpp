#include "server.hpp"
#include "engine.hpp"

#include <iostream>
#include <string>

TCPConnection::TCPConnection(asio::io_context &io_context,
                             Engine * engine) : socket_(io_context), engine_(engine) {

}

void TCPConnection::HandleWrite(const boost::system::error_code &error_code, size_t transfer_bytes) {

}

void TCPConnection::Start() {
    // long run here!
}


ip::tcp::socket &TCPConnection::Socket() {
    return socket_;
}


TCPConnection::pointer TCPConnection::Create(asio::io_context &io_context, Engine * engine) {
    return pointer(new TCPConnection(io_context, engine));
}



Server::Server(asio::io_context & io_context, unsigned int port_number, Engine * engine)
        :io_context_(io_context),
        acceptor_(io_context, ip::tcp::endpoint(ip::tcp::v4(), port_number)),
        engine_(engine){
    StartAccept();
}

void Server::StartAccept(){
    TCPConnection::pointer new_connection = TCPConnection::Create(io_context_, engine_);

    acceptor_.async_accept(new_connection->Socket(),
                           boost::bind(&Server::HandleAccept, this,
                                       new_connection,
                                       asio::placeholders::error));
}
void Server::HandleAccept(TCPConnection::pointer new_connection,
                          const boost::system::error_code & error) {
    if (!error)
    {
        new_connection->Start();
    }
    StartAccept();
}

