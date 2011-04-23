// Netzwerkkonvertierung Int usw einbauen


int handle_loginrequest(int fd, char *puffer, struct HEADER * header) {
	int ret;

	// Ab hier entscheiden was tun SWITCH-Block des Todes
	ret = read(fd, puffer, (header->length) - 3);

	// Struct LOGINREQUEST erstellen
	struct LOGINREQUEST login;

	// Kopieren der Header-Daten in neu erstelltes Struct
	memcpy(&login, header, sizeof(struct HEADER));
	
	// Kopiere Daten in den Namen
	memcpy(&login.name, puffer, login.header.length - 3);
	
	// String nullterminieren
	login.name[login.header.length - 3] = 0;

	// Testweise Ausgabe:
	printf("Type: %d, Length: %d, Name: %s \n", login.header.type, login.header.length, login.name);

	// Sind die Daten korrekt?

	// Überprüfung ob Name schon vorhanden
	// Rückgabe: true false blah kaffee
	return 1;
}





static void clientthread(int fd) {

	int ret;
	char buf[1024];

	struct HEADER header;

	int run = 1;

	while(run) {

		ret = read(fd, buf, sizeof(struct HEADER));

		if (ret == 0) {
			close(fd);
			return;
		}

		if (ret < 0) {
			close(fd);
			perror("read");
			return;

		} else {

			memcpy(&header, buf, sizeof(struct HEADER));

			// Debug
			// printf("Type: %d, Length: %d \n", header.type, header.length);

			// Switch-Block des Todes
			switch(header.type) {

				case 1: run = handle_loginrequest(fd, buf, &header);
					break;

				default:
					// Random stuff
					break;

			}





			
		}


	}

// Aufräumen
close(fd);

}










