#include<iostream>

#define GLM_FORCE_CUDA

#include "window.hpp"
#include "engine.hpp"

#include <boost/array.hpp>
#include <boost/asio.hpp>
namespace ip = boost::asio::ip;

int main(){

    //try {
    //    boost::asio::io_context io_context;
    //    ip::tcp::acceptor acceptor(io_context, ip::tcp::endpoint(ip::tcp::v4(), 8877));
    //    for (;;) {
    //        ip::tcp::socket socket(io_context);
    //        boost::array<char, 1024> buf;
    //        acceptor.accept(socket);
    //        boost::system::error_code ignored_error;
    //        boost::asio::write(socket, boost::asio::buffer("Server: Hello Client~"), ignored_error);
    //        size_t len = socket.read_some(boost::asio::buffer(buf), ignored_error);
    //        std::string data(buf.begin(), buf.end());
    //        while(data != 'end){

    //            }
    //        std::cout.write(buf.data(), len) << std::endl;
    //        boost::asio::write(socket, boost::asio::buffer("Server: Hello Back~"), ignored_error);
    //    }
    //}
    //catch(std::exception & err){
    //    std::cerr << err.what() << std::endl;
    //}

    //return 0;
    Window * window = new Window(800, 600);
    Engine * engine = new Engine(window);
    //Engine* engine = new Engine(window);
    engine->LoadComponents();
    window->AssignEngine(engine);
    engine->Trace();

    window->Run();
    delete window;
    delete engine;
    return 0;
}