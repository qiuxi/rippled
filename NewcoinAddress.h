#ifndef __NEWCOIN_ADDRESS__
#define __NEWCOIN_ADDRESS__

#include "base58.h"


// TODO: https://en.bitcoin.it/wiki/Address
// TODO: Why do we need steps 5 and 6? why not just use a checksum function to get the checksum?

class NewcoinAddress : public CBase58Data
{
public:
	NewcoinAddress();
	NewcoinAddress(uint160& hash160In);
	NewcoinAddress(const std::vector<unsigned char>& vchPubKey);
	NewcoinAddress(const std::string& strAddress);
	NewcoinAddress(const char* pszAddress);

	bool SetHash160(const uint160& hash160);
	bool SetPubKey(const std::vector<unsigned char>& vchPubKey);

	bool IsValid();
	uint160 GetHash160() const;
	std::string GetString() const;
};

#endif
