#pragma once
#include <string>
class Client
{
public:
	Client();
	~Client();
	void Connect_To_Server(std::string IPADD, int PORT);
	void sendMessage(std::string msg_);
	std::string recvMessage(char buf_[4096]);
	void graceDisconnect();
	char buf[4096];
};

