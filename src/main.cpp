#include <iostream>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

// 根据配置定义一个基于 Asio 的 WebSocket 服务器类型
typedef websocketpp::server<websocketpp::config::asio> server;

    bool operator==(const websocketpp::connection_hdl& lhs,
                    const websocketpp::connection_hdl& rhs) {
      return lhs.lock().get() == rhs.lock().get();
    };

int main() {
  std::string ReplyMessage;
  try {
    server endpoint;
    endpoint.init_asio();
    // 关闭日志消息输出
    endpoint.clear_access_channels(websocketpp::log::alevel::all);

    // 定义连接列表
    std::vector<websocketpp::connection_hdl> connections;

    // 注册用于处理传入连接的处理程序
    endpoint.set_open_handler([&connections](websocketpp::connection_hdl hdl) {
      connections.push_back(hdl);  // 将新连接添加到连接列表中
      std::cout << "有新连接加入，当前连接数：" << connections.size()
                << std::endl;
    });

     // 注册用于处理断开连接的处理程序
    endpoint.set_close_handler([&connections](websocketpp::connection_hdl hdl) {
      auto it = std::find_if(connections.begin(), connections.end(),
                             [&hdl](const websocketpp::connection_hdl& elem) {
                               return hdl.lock().get() == elem.lock().get();
                             });
      if (it != connections.end()) {
        connections.erase(it);  // 从连接列表中移除断开的连接
        std::cout << "有连接断开，当前连接数：" << connections.size()
                  << std::endl;
      }
    });

    // 注册用于处理传入连接的处理程序
    endpoint.set_message_handler(
        [&endpoint, &ReplyMessage](websocketpp::connection_hdl hdl,
                                   server::message_ptr msg) {
          std::string message = msg->get_payload();
          std::cout << "接收到的消息: " << message << std::endl;
          std::cout << "请回复消息: ";
          std::cin >> ReplyMessage;

          try {
            // 发消息到客户端
            endpoint.send(hdl, ReplyMessage, msg->get_opcode());
          } catch (const websocketpp::lib::error_code& e) {
            std::cout << "发送消息失败: " << e.message() << std::endl;
          }
        });

    // 启动服务器&开始监听
    endpoint.listen(9002);
    endpoint.start_accept();

    // 运行服务器
    endpoint.run();

  } catch (const std::exception& e) {
    std::cout << "异常: " << e.what() << std::endl;
  }

  return 0;
}
