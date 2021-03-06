#include "h_client_command.h"

client_command::client_command() {
};
bool client_command::execute(int socket, tokenizer tokens) {
	cout << "Osnovna komanda!" << endl << "Implementiraj ostale!" << endl;
	if (tokens.hasNext()) {
		cout << "Unio si jos ove rijeci nakon komande:" << endl;
	}
	while (tokens.hasNext()) {
		cout << "	" << tokens.next() << endl;
	}
	return true;
};
bool client_command::isNumber(const char *s) {
	while (*s != '\0') {
	    	if (!isdigit(*s))
		return false;
		s++;
	}
	return true;
};
string* client_command::getDescription() {
	return new string("Default command, shouldn't be mapped and available.");
};
