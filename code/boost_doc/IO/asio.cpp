#include <iostream>
#include <boost/asio.hpp>
#include <thread>

using namespace std;
using namespace boost::asio;

void server_thread() {
  while (1)
  {
    //        boost::asio::ip::udp::endpoint  sendpoint;//请求的IP以及端口
    string s;
    cin >> s;
    udp_socket.send_to(boost::asio::buffer(s.c_str(),s.size()), local_add);
    udp_socket.receive_from(boost::asio::buffer(receive_str, 1024), local_add);//收取
    cout << "收到" << receive_str << endl;
    memset(receive_str, 0, 1024);//清空字符串
  }
  return 0;
}

void client_thread() {
  udp_socket.bind(local_add);
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

int main()
{
  io_service io_service;
  ip::udp::socket udp_socket(io_service);
  ip::udp::endpoint local_add(boost::asio::ip::address::from_string("127.0.0.1"), 2000);
  udp_socket.open(local_add.protocol());
  char receive_str[1024] = { 0 };
  
  thread(server_thread).join();
  thread(client_thread).join();

  return 0;
}
