#pragma once

#include <exception>
using namespace std;

class CNetworkException : public exception
{
public:
	CNetworkException(void);
	~CNetworkException(void);
};

