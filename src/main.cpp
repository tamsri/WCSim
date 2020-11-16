#include<iostream>
#include <string>

#define GLM_FORCE_CUDA

#include "window.hpp"
#include "engine.hpp"
#include <glm/gtx/string_cast.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

namespace ip = boost::asio::ip;

void TCPServer(Engine * engine){
    try{
        boost::asio::io_context io_context;
        ip::tcp::acceptor acceptor(io_context, ip::tcp::endpoint(ip::tcp::v4(), 8877));
        std::cout << "Server is initialized, Waiting for incoming the client\n";
        for (;;) {
            ip::tcp::socket socket(io_context);
            boost::array<char, 1024> data_buffer{};
            acceptor.accept(socket); // if not accepted, continue the loop

            boost::system::error_code ign_err; // param for ignoring the error.

            // After the connection, the server says hello for testing the connection
            std::cout << "The client has connected" << std::endl;
            boost::asio::write(socket, boost::asio::buffer("Server: Hello Client"), ign_err);
            // Then, the server waits for client to reply
            size_t len = socket.read_some(boost::asio::buffer(data_buffer), ign_err);
            std::string rec_data = std::string(data_buffer.begin(), data_buffer.begin() + len);
            std::cout << rec_data << std::endl;
            std::cout << "Starting Engine & Communication" << std::endl;
            // Initialize Window and Engine for the client.
            // Communications
            while (true) {
                len = socket.read_some(boost::asio::buffer(data_buffer), ign_err);
                rec_data = std::string(data_buffer.begin(), data_buffer.begin() + len);

                if (rec_data == "e") {
                    std::cout << "Server: the client disconnected to the server.\n";
                    //delete engine; // Engine automatically delete the window if window exists.
                    boost::asio::write(socket, boost::asio::buffer("eok"), ign_err);
                    socket.close();
                    break;
                }

                switch (rec_data[0]) {
                    case 'q': {
                        engine->ExecuteQuestion(socket, ign_err, rec_data);
                    }break;
                    case 'c': {
                        engine->ExecuteCommand(socket, ign_err, rec_data);
                    }break;
                    case 'r': {
                        std::cout << "Server: the client commands to reset the environment.\n";
                        engine->Reset();
                        boost::asio::write(socket, boost::asio::buffer("rok"), ign_err);
                    }break;
                    default: {
                        std::cout << "Server: " << rec_data << "Unknown command, Disconnecting the client.\n";
                        goto end_connect;
                    } break;
                }

            }
            end_connect:;
        }
    }
    catch(std::exception & err){
        std::cerr << err.what() << std::endl;
    }
}

int main(int argc, char *argv[]){

    std::cout 
        << "            ___           ___           ___                       ___     \n"
        << "           /\\__\\         /\\  \\         /\\  \\          ___        /\\__\\    \n"
        << "          /:/ _/_       /::\\  \\       /::\\  \\        /\\  \\      /::|  |   \n"
        << "         /:/ /\\__\\     /:/\\:\\  \\     /:/\\ \\  \\       \\:\\  \\    /:|:|  |   \n"
        << "        /:/ /:/ _/_   /:/  \\:\\  \\   _\\:\\~\\ \\  \\      /::\\__\\  /:/|:|__|__ \n"
        << "       /:/_/:/ /\\__\\ /:/__/ \\:\\__\\ /\\ \\:\\ \\ \\__\\  __/:/\\/__/ /:/ |::::\\__\\\n"
        << "       \\:\\/:/ /:/  / \\:\\  \\  \\/__/ \\:\\ \\:\\ \\/__/ /\\/:/  /    \\/__/~~/:/  /\n"
        << "        \\::/_/:/  /   \\:\\  \\        \\:\\ \\:\\__\\   \\::/__/           /:/  / \n"
        << "         \\:\\/:/  /     \\:\\  \\        \\:\\/:/  /    \\:\\__\\          /:/  /  \n"
        << "          \\::/  /       \\:\\__\\        \\::/  /      \\/__/         /:/  /   \n"
        << "           \\/__/         \\/__/         \\/__/                     \\/__/    \n"
        << "                                                                By Supawat Tamsri\n"
        << "                                                                   <meep@supawat.dev>\n";
    std::cout << "Welcome to WCSim, the Wireless Communication Simulator\n";
    
    // Question 1: Turn on TCP Server?
    std::cout << "Please select the mode.\n";
    char tcp_answer; bool is_tcp_on = false;
    std::cout << "Would you like to turn on TCP server? [Y/N]\n";
    do {
        std::cout << "[y/n]:";
        std::cin >> tcp_answer;
    } while (tcp_answer != 'y' && tcp_answer != 'n');
    if (tcp_answer == 'y') is_tcp_on = true;
    
    // Question 2: Turn on Window?
    
     bool is_window_on = true;
     if (is_tcp_on) {
         std::cout << "Would you like to turn on Visualization? [Y/N]\n";
         char window_answer;
         do {
             std::cout << "[y/n]:";
             std::cin >> window_answer;
         } while (window_answer != 'y' && window_answer != 'n');
         if (window_answer == 'n') is_window_on = false;
     }


    Window * window;
    Engine* engine;

    if (is_tcp_on) {
        // Run as TCP Server
        if(is_window_on){
            window = new Window(800, 600);
            engine = new Engine(window);
            engine->InitializeWithWindow();
            std::thread ServerThread(TCPServer, engine);
            engine->RunWithWindow();
            ServerThread.join();
        }else{
            engine = new Engine();
            engine->InitializeWithoutWindow();
            TCPServer(engine);
        }
    }else{
        // Run as Local Simulator
        window = new Window(800, 600);
        engine = new Engine(window);
        engine->InitializeWithWindow();
        engine->RunWithWindow();
    }

    return 0;
}