#pragma once
#include <string>
class Client
{
public:
	Client();
	~Client();
	void Connect_To_Server(std::string IPADD, int PORT);
};

