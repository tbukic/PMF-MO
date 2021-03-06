#ifndef __CLIENT_COMD_COUNT_H_
#define __CLIENT_COMD_COUNT_H_

#include "h_client_command.h"
#include "h_tokenizer.h"

using namespace std;

class client_command_count : public client_command {
	public:
		client_command_count();
		string* getDescription();
		bool execute(int socket, tokenizer tokens);
};

#endif
