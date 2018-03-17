#pragma once

#include "Poco/Runnable.h"  
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/ServerSocket.h"

using namespace Poco::Net;

class CTCPServer : public Poco::Runnable
{
public:
	CTCPServer(const std::string & ip, Poco::UInt16 port);
	~CTCPServer(void);

public:
	void setQuit();

public:
	virtual void run();

private:
	ServerSocket _serverSocket;

	bool _bQuit;
};

