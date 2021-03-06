#include "h_server_command_bye.h"

server_command_bye::server_command_bye(
		data *studentsAndClassroomData,
		pthread_mutex_t *dataLock,
		pthread_mutex_t *outputLock
	) : server_command(
		studentsAndClassroomData,
		dataLock,
		outputLock) {

};
bool server_command_bye::execute(int socket, char *message) {

	pthread_mutex_lock(outputLock);
	cout << "Klijent br. " << socket << " je poslao zahtjev za odspajanjem." << endl;
	pthread_mutex_unlock(outputLock);
	if (strlen(message) != 0) {
		send_message(socket, ERROR, "");
	} else {
		send_message(socket, BYE, "");
	}
	
	return true;
};
