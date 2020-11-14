#ifndef SERVER_H
#define SERVER_H

#include <glm/glm.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace asio = boost::asio;
namespace ip = boost::asio::ip;

class Engine;
class TCPConnection : public boost::enable_shared_from_this<TCPConnection>{
public:
    typedef boost::shared_ptr<TCPConnection> pointer;
    static pointer Create(asio::io_context& io_context, Engine * engine);

    ip::tcp::socket & Socket();

    void Start();

private:
    TCPConnection(asio::io_context & io_context, Engine * engine);

    void HandleWrite(const boost::system::error_code& error_code,
                      size_t transfer_bytes);

    ip::tcp::socket socket_;
    std::string message_;
    Engine * engine_;
};

class Server{
public:
    Server(asio::io_context & io_context, unsigned int port_number ,Engine * engine);

    bool AddTransmitter(glm::vec3 position, glm::vec3 rotation, float frequency);
    bool AddReceiver(glm::vec3 position);
    bool RemoveTransmitter(unsigned int transmitter_id);
    bool RemoveReceiver(unsigned int receiver_id);
    bool ConnectReceiverToTransmitter(unsigned int tx_id, unsigned int rx_id);
    bool DisconnectReceiverFromTransmitter(unsigned int tx_id, unsigned int rx_id);
    bool MoveTransmitterTo(unsigned int rx_id, glm::vec3 position, glm::vec3 rotation);
    bool MoveReceiverTo(unsigned int rx_id, glm::vec3 position);

    void StartAccept();
    void HandleAccept(TCPConnection::pointer new_connection,
                      const boost::system::error_code & error);
private:
    asio::io_context & io_context_;
    asio::ip::tcp::acceptor acceptor_;
    Engine * engine_;
};
#endif