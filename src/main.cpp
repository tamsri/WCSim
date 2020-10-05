#include<iostream>
#include <string>

#define GLM_FORCE_CUDA

#include "window.hpp"
#include "engine.hpp"
#include <boost/array.hpp>
#include <boost/asio.hpp>
namespace ip = boost::asio::ip;

int main(){

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
        std::cout << "[Y/N]:";
        std::cin >> tcp_answer;
    } while (tcp_answer != 'Y' && tcp_answer != 'N');
    if (tcp_answer == 'Y') is_tcp_on = true;
    
    // Quesetion 2: Turn on Window?
    
     bool is_window_on = true;
     if (is_tcp_on) {
         std::cout << "Would you like to turn on Visualization? [Y/N]\n";
         char window_answer;
         do {
             std::cout << "[Y/N]:";
             std::cin >> window_answer;
         } while (window_answer != 'Y' && window_answer != 'N');
         if (window_answer == 'N') is_window_on = false;
     }
   

    // Program
    Window * window;
    Engine* engine;

    // Run as TCP Server
    if (is_tcp_on) {
        
        try{
            boost::asio::io_context io_context;
            ip::tcp::acceptor acceptor(io_context, ip::tcp::endpoint(ip::tcp::v4(), 8877));
            std::cout << "Server is initialized, wait for incoming the client\n";
            for (;;) {
                ip::tcp::socket socket(io_context);
                boost::array<char, 1024> data_buffer;
                acceptor.accept(socket); // if not accepted, continue the loop 
                
                boost::system::error_code ignored_error; // param for ignoring the error.

                // After the connection, the server says hello for testing the connection
                std::cout << "The client has connected" << std::endl;
                boost::asio::write(socket, boost::asio::buffer("Server: Hello Client~"), ignored_error);
                // Then, the server waits for client to reply 
                size_t len = socket.read_some(boost::asio::buffer(data_buffer), ignored_error);
                std::string rev_data = std::string(data_buffer.begin(), data_buffer.begin()+len);
                std::cout << rev_data << std::endl; 
                std::cout << "Starting Engine & Communication" << std::endl;

                // Initialize Window and Engine for the client.

                if (is_window_on) {
                    window = new Window(800, 600);
                    engine = new Engine(window);
                    engine->InitalizeWithWindow();
                }
                else {
                    engine = new Engine();
                    engine->InitializeWithoutWindow();
                }

                while (true) {
                    // Talk here
                    len = socket.read_some(boost::asio::buffer(data_buffer), ignored_error);
                    rev_data = std::string(data_buffer.begin(), data_buffer.begin() + len);
                    std::cout << rev_data << std::endl;
                    if (rev_data == "e") {
                        std::cout << "Server: the client commands to end the server.\n";
                        delete engine; // Engine automaitcally delete the window if window exists.
                        boost::asio::write(socket, boost::asio::buffer("eok"), ignored_error);
                        socket.close();
                        break;
                    }

                    switch(rev_data[0]){
                    case 'q': {
                        switch (rev_data[1]) {
                        case '1': {
                            // How many stations are in the environment, who are they?
                            std::cout << "Server: The Client asks How many transmitter?\n";
                            boost::asio::write(socket, boost::asio::buffer("qok"), ignored_error);
                            std::string answer = "a:" + std::to_string(engine->GetTransmittersNumber());
                            boost::asio::write(socket, boost::asio::buffer(answer), ignored_error);
                            break;
                        }
                        case '2': {
                            // How many users are in the environment, who are they?
                            std::cout << "Server: The Clients ask How many receivers?\n";
                            boost::asio::write(socket, boost::asio::buffer("qok"), ignored_error);
                            std::string answer = "a:" + std::to_string(engine->GetReceiversNumber());
                            boost::asio::write(socket, boost::asio::buffer(answer), ignored_error);
                            break;
                        }
                        case '3': {
                            // Give me info of the station id #..


                            break;
                        }
                        case '4': {
                            // Give me info mation of the user number #..


                            break;
                        }
                        }
                        break; 
                    }
                    case 'c': {
                        break;
                    }
                    case 'r': {
                        std::cout << "Server: the client commands to reset the environment.\n";
                        engine->Reset();
                        boost::asio::write(socket, boost::asio::buffer("rok"), ignored_error);
                        break;
                    }
                    default:
                        std::cout << "Server: " << rev_data << " command is unknown\n";
                        break;
                    };
                };

			}
        }
        catch(std::exception & err){
            std::cerr << err.what() << std::endl;
        }
        return 0;
    }
    
    // Run as Local Simulator
    if (is_window_on) {
        window = new Window(800, 600);
        engine = new Engine(window);
        engine->AssignWindow(window);
        engine->InitalizeWithWindow();
        engine->RunWithWindow();
    }

    return 0;
}