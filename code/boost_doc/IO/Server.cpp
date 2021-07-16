#include <iostream>
using namespace std;
#include <boost/asio.hpp>
//using namespace boost::asio;

int main()
{
    boost::asio::io_service io_service;
    boost::asio::ip::udp::socket udp_socket(io_service);
    boost::asio::ip::udp::endpoint local_add(boost::asio::ip::address::from_string("127.0.0.1"), 2000);

    udp_socket.open(local_add.protocol());
    udp_socket.bind(local_add);

    char receive_str[1024] = { 0 };
    while (1)
    {
        boost::asio::ip::udp::endpoint  sendpoint;
        udp_socket.receive_from(boost::asio::buffer(receive_str, 1024), sendpoint);
        cout << "收到" << sendpoint.address().to_string() <<":"<< receive_str << endl;
        udp_socket.send_to(boost::asio::buffer("服务端返回success"), sendpoint);
        memset(receive_str, 0, 1024);

    }
    return 0;
}
