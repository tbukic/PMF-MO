#include "h_server_command_locate.h"

server_command_locate::server_command_locate(
		data *studentsAndClassroomData,
		pthread_mutex_t *dataLock,
		pthread_mutex_t *outputLock
	) : server_command(
		studentsAndClassroomData,
		dataLock,
		outputLock) {

};
bool server_command_locate::execute(int socket, char *message) {
	
	cout << message << endl;
	tokenizer *tokens = new tokenizer(string(message), ' ');
	
	string	jmbag		= getJMBAGstring(tokens, SC_LAST);

	if (jmbag.empty()) {
		send_message(socket, ERROR, "");
		return false;
	}
	
	pthread_mutex_lock(outputLock);
	pthread_mutex_lock(dataLock);
	
	int classroomID = studentsAndClassroomData->getClassroomIDfor(jmbag);
	pthread_mutex_unlock(dataLock);
	cout << socket << ":LOCATE (klijent br. " << socket << "): student "<< jmbag;
	if (classroomID < DATA_MIN_CLASSROOM_NO) {
		cout << " nije registriran." << endl;
	} else {
		cout << " u dvorani " << classroomID << "." << endl;
	}
	pthread_mutex_unlock(outputLock);
	
	int toAllocate = (classroomID > 0) ? ((int) floor(log10(classroomID)) + 2) : 2;
	char to_send[toAllocate];
	sprintf(to_send, "%d", classroomID);
	send_message(socket, LOCATE, to_send);
	
	return false;
};
