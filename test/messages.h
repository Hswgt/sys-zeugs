#pragma pack(1)


// 3 byte
struct HEADER{
	uint8_t type;
	uint16_t length;
};

//3 byte + strlen(name)
//type 1
struct LOGINREQUEST{
	struct HEADER header;
	char name[32];
};

//4 byte
//type 2
struct LOGINRESPONSEOK{
	struct HEADER header;
	uint8_t clientid;
};

//3 byte + 37*player byte
//type 3
struct PLAYERLIST{
	struct HEADER header;
	//zu erweitern	
};

// 37 byte
struct PLAYER{
	char name[32];
	uint32_t score;
	uint8_t clientid;
};

// 3 byte 
// type 4
struct CATALOGREQUEST{			//rausschmeissen?
	struct HEADER header;
};

//type 5
struct CATALOGRESPONSE{
	struct HEADER header;
	char filename[0];
};

//type 6
struct CATALOGCHANGE{
	struct HEADER header;
	char filename[0];
};

//type 7
struct STARTGAMECLIENT{
	struct HEADER header;
	char filename[0];
};

//type 7
struct STARTGAMESERVER{			//rausschmeissen?
	struct HEADER header;
};

//type 8
struct QUESTIONREQUEST{			//rausschmeissen?
	struct HEADER header;
};

//type 9
struct QUESTION{
	struct HEADER header;
	char question[770]; // schon fertig als struktur vorhanden (projektgerüst (common/question.h)
};

//type 10
struct QUESTIONANSWERED{
	struct HEADER header;
	uint8_t selection;
};

//type 11
struct QUESTIONRESULT{
	struct HEADER header;
	uint8_t selection;
	uint8_t correct;
};

//type 12
struct GAMEOVER{
	struct HEADER header;
	uint8_t rank;
};

//type 255
//subtype 0-> fataler Fehler
//subtype 1-> Warnung
struct ERRORWARNING{
	struct HEADER header;
	uint8_t subtype;
	char message[];
};

#pragma pack(0)
