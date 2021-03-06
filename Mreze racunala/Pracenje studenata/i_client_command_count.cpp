#include "h_client_command_count.h"

client_command_count::client_command_count() : client_command() {

};

bool client_command_count::execute(int socket, tokenizer tokens) {
	
	
	//citanje jmbaga i broja dvorane:
		
	if (!tokens.hasNext()) {
		cout << *getDescription() << endl;
		return false;
	}
	const char *dvorana_str = tokens.next().c_str();
	
	if (tokens.hasNext()) {
		cout << *getDescription() << endl;
		return false;
	}
	
	
	//provjera broja dvorane:
	
	if (!isNumber(dvorana_str)) {
		cout << "Pogresan unos! Dvorana se oznacava brojem!" << endl;
		return false;
	}
	
	
	//pise poruku:
	
	char	message[strlen(dvorana_str) + 1];
	sprintf(message, "%s", dvorana_str);
	
	
	//salje poruku:

	cout << "Trazim broj studenata u dvorani " << dvorana_str << "." << endl;
	send_message(socket, COUNT, message);
	
	int	messageType,
		messageLength;
	char	*reply;
	
	if (
		receive_message(
			socket,
			&messageType,
			&messageLength,
			&reply
		)
		!= SUCESSFULL) {
		cout << "Greska u komunikaciji sa serverom!" << endl;
		return false;
	}
	
	if (
		(messageType != COUNT && messageType != ERROR)
		|| messageLength == 0
		|| !isNumber(reply)
	) {
		cout << "Bug na serveru!" << endl;
		cout << "Server je poslao krivi odgovor!" << endl;
		return false;
	}
	
	if (
		messageType == ERROR
	) {
		cout << "Server javlja gresku u podacima, pokusajte ponovo!" << endl;
		return false;
	}
	
	int no_dvorane;
	sscanf(reply, "%d", &no_dvorane);
	if (DATA_DOESNT_EXIST == no_dvorane) {
		cout << "U dvorani " << dvorana_str << " nema studenata ili dvorana ne postoji." << endl;
	} else {
		cout << "U dvorani " << dvorana_str << " ima " << reply << " studenata." << endl;
	}
	
	return false;
};

string* client_command_count::getDescription() {
	return new string("Upotreba: count <broj_dvorane>");
};
