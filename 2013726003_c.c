#include<ncurses.h>
#include<string.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<unistd.h>

#define KEYBOARD_UP 65
#define KEYBOARD_DOWN 66
#define KEYBOARD_LEFT 68
#define KEYBOARD_RIGHT 67
#define KEYBOARD_ENTER 13
#define KEYBOARD_BACKSPACE 127

#define MENU_MAIN1 0
#define MENU_MAIN2 1
#define MENU_MAINS 2
#define MENU_LOGIN1 3
#define MENU_LOGIN2 4
#define MENU_SIGNIO 5
#define MENU_EXIT 6
#define MENU_SIGNIN 7
#define MENU_SIGNOUT 8
#define MENU_PLAY 9
#define MENU_INGAME 10
#define MENU_STATISTICS 11

#define MENU_ID 100
#define MENU_PASS 101

#define MENU_WIN 200
#define MENU_LOSE 201

#define BACKFLAG_MEMU 1
#define BACKFLAG_INGAME 2

#define INGAME_NEXTTURN 300
#define INGAME_REGAME 301

#define INGAME_HORSE 400

#define IDPASSLENGTH 15

#define PLAYER1 0
#define PLAYER2 1
#define PLAYER_START 2

struct loginInformation{
	bool login;
	char id[IDPASSLENGTH];
	char pass[IDPASSLENGTH];
	int win;
	int lose;
};


int _mainMenu(int);
int _loginMenu(int);
int _signioMenu();
int _signinMenu();
int _signoutMenu();
int _playMenu();
int _ingameMenu();
int _statisticsMenu();

int ingameEnterKeyPressInBoard(int, char);
int ingameTheNumberOfHorse(char);
int ingameSelectHorse(int, int);
char ingameIsGameEnded();
char ingameOppositeTurn(char);
void ingameDrawBoard();
void ingameInitBoard();
void ingameDrawOneHorse(int);

void init_updownWindow(WINDOW *, WINDOW *, int);
void init_scr();
void saveInfo();

/* Global Value */

char board[6][6];
struct loginInformation loginInfo[2];
int sock;
struct sockaddr_in serv_addr;

/****************/

int main(int argc, char **argv)
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
		perror("connect error");
		exit(1);
	}


	init_scr();
	loginInfo[PLAYER1].login = false;
	loginInfo[PLAYER2].login = false;

	int thisMenu = MENU_MAIN1;

	while(thisMenu != MENU_EXIT)
	{
		switch(thisMenu){
			case MENU_MAIN1 :
				thisMenu = _mainMenu(PLAYER1);
				break;
			case MENU_MAIN2 :
				thisMenu = _mainMenu(PLAYER2);
				break;
			case MENU_MAINS :
				thisMenu = _mainMenu(PLAYER_START);
				break;
			case MENU_LOGIN1 :
				thisMenu = _loginMenu(PLAYER1);
				break;
			case MENU_LOGIN2 :
				thisMenu = _loginMenu(PLAYER2);
				break;
			case MENU_SIGNIO :
				thisMenu = _signioMenu();
				break;
			case MENU_SIGNIN :
				thisMenu = _signinMenu();
				break;
			case MENU_SIGNOUT :
				thisMenu = _signoutMenu();
				break;
			case MENU_PLAY :
				thisMenu = _playMenu();
				break;
			case MENU_INGAME :
				thisMenu = _ingameMenu();
				break;
			case MENU_STATISTICS :
				thisMenu = _statisticsMenu();
				break;
		}
	}

	endwin();
	return 0;
}

int _mainMenu(int player){
	werase(stdscr); // Clear Window
	curs_set(0); // Not need Cursor pointer
	char key; // User Input
	int MENU_FLUIDIC;

	if(player == PLAYER1)
		MENU_FLUIDIC = MENU_LOGIN1;
	else if(player == PLAYER2)
		MENU_FLUIDIC = MENU_LOGIN2;
	else if(player == PLAYER_START)
		MENU_FLUIDIC = MENU_PLAY;
	else
		perror("main menu fluidic menu error");
		
	int selectingMenu = MENU_FLUIDIC;

	WINDOW *upMenu, *downMenu;
	init_updownWindow(upMenu, downMenu, BACKFLAG_MEMU);

	mvprintw(3, 17, "Software Practice 2");
	mvprintw(5, 23, "OTHELLO");
	mvprintw(7, 35, "2014123456");
	mvprintw(8, 35, "Hong Gil-Dong");

	while(1){
		attron(COLOR_PAIR(2));
		
		if(player == PLAYER1)
			mvprintw(11, 6, "1P LOGIN");
		else if(player == PLAYER2)
			mvprintw(11, 6, "2P LOGIN");
		else if(player == PLAYER_START)
			mvprintw(11, 5, "GAME START");
		else
			perror("main menu player number error : ");
		
		mvprintw(11, 21, "SIGN IN&OUT");
		mvprintw(11, 39, "EXIT");

		attron(A_STANDOUT | A_UNDERLINE); // selected effect

		// Overwriting menu font
		if(selectingMenu == MENU_FLUIDIC){
			if(player == PLAYER1)
				mvprintw(11, 6, "1P LOGIN");
			else if(player == PLAYER2)
				mvprintw(11, 6, "2P LOGIN");
			else if(player == PLAYER_START)
				mvprintw(11, 5, "GAME START");
		} else if(selectingMenu == MENU_SIGNIO)
			mvprintw(11, 21, "SIGN IN&OUT");
		else if(selectingMenu == MENU_EXIT)
			mvprintw(11, 39, "EXIT");

		attroff(A_STANDOUT | A_UNDERLINE);

		refresh();

		key = getchar();

		switch(key){
			case KEYBOARD_RIGHT :
				if(selectingMenu == MENU_FLUIDIC)
					selectingMenu = MENU_SIGNIO;
				else if(selectingMenu == MENU_SIGNIO)
					selectingMenu = MENU_EXIT;
				else if(selectingMenu == MENU_EXIT){
					selectingMenu = MENU_FLUIDIC;
				}
				break;
			case KEYBOARD_LEFT :
				if(selectingMenu == MENU_SIGNIO)
					selectingMenu = MENU_FLUIDIC;
				else if(selectingMenu == MENU_EXIT)
					selectingMenu = MENU_SIGNIO;
				else if(selectingMenu == MENU_FLUIDIC)
					selectingMenu = MENU_EXIT;
				break;
			case KEYBOARD_ENTER :
				delwin(upMenu);
				delwin(downMenu);
				return selectingMenu;
		}
	}
}


int _loginMenu(int player){
	char id[IDPASSLENGTH] = ""; // user input id
	char password[IDPASSLENGTH] = ""; // user input passwd
	int i, MENU_FLUIDIC1, MENU_FLUIDIC2;
	werase(stdscr); // Clear Window
	char key; // User Input
	int selectingMenu = MENU_ID;

	WINDOW *upMenu, *downMenu;
	init_updownWindow(upMenu, downMenu, BACKFLAG_MEMU);
	
	if(player == PLAYER1){
		MENU_FLUIDIC1 = MENU_MAIN2;
		MENU_FLUIDIC2 = MENU_MAIN1;
	}
	else if(player == PLAYER2){
		MENU_FLUIDIC1 = MENU_MAINS;
		MENU_FLUIDIC2 = MENU_MAIN1;
	}
	else
		perror("login menu fluidic instance error : ");

	if(player == PLAYER1)
		mvprintw(2, 22, "1P LOGIN");
	else
		mvprintw(2, 22, "2P LOGIN");
		
	mvprintw(4, 19, " : ");
	mvprintw(6, 19, " : ");

	while(1){
		attron(COLOR_PAIR(1));
		mvprintw(4, 17, "ID");
		mvprintw(6, 11, "PASSWORD");
		mvprintw(4, 22, "               ");
		mvprintw(4, 22, id);
		mvprintw(6, 22, "               ");
		for(i=0; i<strlen(password); i++)
			mvprintw(6, 22+i, "*");

		attron(COLOR_PAIR(2));

		mvprintw(11, 19, "LOGIN");
		mvprintw(11, 28, "MAIN MENU");

		attron(A_STANDOUT | A_UNDERLINE); // selected effect

		// Overwriting menu font
		if(selectingMenu == MENU_ID){
			curs_set(1);
			mvprintw(4, 17, "ID");
			move(4, 22 + strlen(id));
		} else if(selectingMenu == MENU_PASS){
			curs_set(1);
			mvprintw(6, 11, "PASSWORD");
			move(6, 22 + strlen(password));
		}else if(selectingMenu == MENU_FLUIDIC1){
			curs_set(0);
			mvprintw(11, 19, "LOGIN");
		}else if(selectingMenu == MENU_FLUIDIC2){
			curs_set(0);
			mvprintw(11, 28, "MAIN MENU");
		}

		attroff(A_STANDOUT | A_UNDERLINE);

		refresh();

		// Input key
		key = getchar();

		switch(key){
			case KEYBOARD_UP :
				if(selectingMenu == MENU_FLUIDIC1 || selectingMenu == MENU_FLUIDIC2)
					selectingMenu = MENU_PASS;
				else if(selectingMenu == MENU_PASS)
					selectingMenu = MENU_ID;
				break;
			case KEYBOARD_DOWN :
				if(selectingMenu == MENU_ID)
					selectingMenu = MENU_PASS;
				else if(selectingMenu == MENU_PASS)
					selectingMenu = MENU_FLUIDIC1;
				break;
			case KEYBOARD_RIGHT :
				if(selectingMenu == MENU_FLUIDIC1)
					selectingMenu = MENU_FLUIDIC2;
				else if(selectingMenu == MENU_FLUIDIC2)
					selectingMenu = MENU_FLUIDIC1;
				break;
			case KEYBOARD_LEFT :
				if(selectingMenu == MENU_FLUIDIC1)
					selectingMenu = MENU_FLUIDIC2;
				else if(selectingMenu == MENU_FLUIDIC2)
					selectingMenu = MENU_FLUIDIC1;
				break;
			case KEYBOARD_BACKSPACE :
				if(selectingMenu == MENU_ID)
						id[strlen(id) - 1] = '\0';
				else if(selectingMenu == MENU_PASS)
						password[strlen(password) - 1] = '\0';
				break;
			case KEYBOARD_ENTER :
				if(selectingMenu == MENU_ID)
					selectingMenu = MENU_PASS;
				else if(selectingMenu == MENU_PASS)
					selectingMenu = MENU_FLUIDIC1;
				else if(selectingMenu == MENU_FLUIDIC1)
				{
					/*int fd = open("info", O_RDWR);

					int bufLength = IDPASSLENGTH + 1 + IDPASSLENGTH + 1 + 3 + 1 + 3 + 1;
					char buf[bufLength]; // ID(15)  PASS(15)  WIN(3)  LOSE(3)
					char tempID[IDPASSLENGTH], tempPW[IDPASSLENGTH];
					int tempWIN, tempLOSE;

					while(read(fd, buf, bufLength) > 0)
					{
						sscanf(buf, "%s %s %d %d", tempID, tempPW, &tempWIN, &tempLOSE);
						if(strcmp(tempID, id) == 0)
						{
							if(strcmp(tempPW, password) == 0) // Login Success!
							{
								if(player == PLAYER2){ // if 1P = 2P
									if(strcmp(loginInfo[PLAYER1].id, id) == 0){
										mvprintw(LINES - 1, 0, "                                          ");
										mvprintw(LINES - 1, 0, "Already logined by 1P");
										close(fd);
										break;
									}
								}
								mvprintw(LINES - 1, 0, "                                          ");
								mvprintw(LINES - 1, 0, "Hello %s!", id);
								getch();
								loginInfo[player].login = true;
								strcpy(loginInfo[player].id, id);
								strcpy(loginInfo[player].pass, password);
								loginInfo[player].win = tempWIN;
								loginInfo[player].lose = tempLOSE;
								close(fd);
								delwin(upMenu);
								delwin(downMenu);
								return 
							}
							else{
								close(fd);
								mvprintw(LINES - 1, 0, "                                          ");
								mvprintw(LINES - 1, 0, "Login Failed");
							}
						} 
					}
					close(fd);
					mvprintw(LINES - 1, 0, "                                          ");
					mvprintw(LINES - 1, 0, "Login Failed");*/
					int mode = 2;
					char s_id[IDPASSLENGTH];
					char s_pw[IDPASSLENGTH];
					bool loginSuccess;
					sprintf(s_id,"%-15s",id);
					sprintf(s_pw,"%-15s",password);
					write(sock, &mode, sizeof(mode));
					write(sock, s_id, strlen(s_id));
					write(sock, s_pw, strlen(s_pw));
					read(sock, &loginSuccess, sizeof(loginSuccess));
					if(loginSuccess == true){
						mvprintw(LINES - 1, 0,"                                                   ");
						mvprintw(LINES - 1, 0,"HELLO %s",id);
						getch();
						delwin(upMenu);
						delwin(downMenu);
						return MENU_PLAY;
					}
					mvprintw(LINES - 1, 0, "                                                      ");
					mvprintw(LINES - 1, 0, "Login Failed");
				}
				if(selectingMenu == MENU_FLUIDIC2) {
					delwin(upMenu);
					delwin(downMenu);
					return selectingMenu;
				}
				break;
			default :
				if((key >= '0' && key <= '9') || (key >= 'a' && key <= 'z'))
				{
					char tempKey[2];
					tempKey[0] = key;
					tempKey[1] = '\0';
					if(selectingMenu == MENU_ID && strlen(id) < IDPASSLENGTH)
						strcat(id, tempKey);
					else if(selectingMenu == MENU_PASS && strlen(password) < IDPASSLENGTH)
						strcat(password, tempKey);
				}
				break;
		}
	}
}

int _signioMenu(){
	werase(stdscr); // Clear Window
	curs_set(0); // Not need Cursor pointer
	char key; // User Input
	int selectingMenu = MENU_SIGNIN;

	WINDOW *upMenu, *downMenu;
	init_updownWindow(upMenu, downMenu, BACKFLAG_MEMU);

	mvprintw(4, 20, "SIGN IN/OUT");

	while(1){
		attron(COLOR_PAIR(2));
		mvprintw(11, 4, "SIGN IN");
		mvprintw(11, 21, "SIGN OUT");
		mvprintw(11, 38, "BACK(MENU)");

		// mvprintw(1, 1, "%d", key);

		attron(A_STANDOUT | A_UNDERLINE); // selected effect

		// Overwriting menu font
		switch(selectingMenu){
			case MENU_SIGNIN :
				mvprintw(11, 4, "SIGN IN");
				break;
			case MENU_SIGNOUT :
				mvprintw(11, 21, "SIGN OUT");
				break;
			case MENU_MAIN1 :
				mvprintw(11, 38, "BACK(MENU)");
				break;
		}

		attroff(A_STANDOUT | A_UNDERLINE);

		refresh();

		key = getchar();

		switch(key){
			case KEYBOARD_RIGHT :
				if(selectingMenu == MENU_SIGNIN)
					selectingMenu = MENU_SIGNOUT;
				else if(selectingMenu == MENU_SIGNOUT)
					selectingMenu = MENU_MAIN1;
				else if(selectingMenu == MENU_MAIN1)
					selectingMenu = MENU_SIGNIN;
				break;
			case KEYBOARD_LEFT :
				if(selectingMenu == MENU_MAIN1)
					selectingMenu = MENU_SIGNOUT;
				else if(selectingMenu == MENU_SIGNOUT)
					selectingMenu = MENU_SIGNIN;
				else if(selectingMenu == MENU_SIGNIN)
					selectingMenu = MENU_MAIN1;
				break;
			case KEYBOARD_ENTER :
				delwin(upMenu);
				delwin(downMenu);
				return selectingMenu;
		}
	}
}

int _signinMenu(){
	char id[IDPASSLENGTH] = ""; // user input id
	char password[IDPASSLENGTH] = ""; // user input passwd
	int i;
	werase(stdscr); // Clear Window
	char key; // User Input
	int selectingMenu = MENU_ID;

	WINDOW *upMenu, *downMenu;
	init_updownWindow(upMenu, downMenu, BACKFLAG_MEMU);

	mvprintw(2, 23, "SIGN IN");
	mvprintw(4, 19, " : ");
	mvprintw(6, 19, " : ");

	while(1){
		attron(COLOR_PAIR(1));
		mvprintw(4, 17, "ID");
		mvprintw(6, 11, "PASSWORD");
		mvprintw(4, 22, "               ");
		mvprintw(4, 22, id);
		mvprintw(6, 22, "               ");
		for(i=0; i<strlen(password); i++)
			mvprintw(6, 22+i, "*");

		attron(COLOR_PAIR(2));
		mvprintw(11, 17, "SIGN IN");
		mvprintw(11, 29, "BACK");

		attron(A_STANDOUT | A_UNDERLINE); // selected effect

		// Overwriting menu font
		switch(selectingMenu){
			case MENU_ID :
				curs_set(1);
				mvprintw(4, 17, "ID");
				move(4, 22 + strlen(id));
				break;
			case MENU_PASS :
				curs_set(1);
				mvprintw(6, 11, "PASSWORD");
				move(6, 22 + strlen(password));
				break;
			case MENU_MAIN1 :
				curs_set(0);
				mvprintw(11, 17, "SIGN IN");
				break;
			case MENU_SIGNIO :
				curs_set(0);
				mvprintw(11, 29, "BACK");
				break;
		}

		attroff(A_STANDOUT | A_UNDERLINE);

		refresh();

		// Input key
		key = getchar();

		switch(key){
			case KEYBOARD_UP :
				if(selectingMenu == MENU_MAIN1 || selectingMenu == MENU_SIGNIO)
					selectingMenu = MENU_PASS;
				else if(selectingMenu == MENU_PASS)
					selectingMenu = MENU_ID;
				break;
			case KEYBOARD_DOWN :
				if(selectingMenu == MENU_ID)
					selectingMenu = MENU_PASS;
				else if(selectingMenu == MENU_PASS)
					selectingMenu = MENU_MAIN1;
				break;
			case KEYBOARD_RIGHT :
				if(selectingMenu == MENU_MAIN1)
					selectingMenu = MENU_SIGNIO;
				else if(selectingMenu == MENU_SIGNIO)
					selectingMenu = MENU_MAIN1;
				break;
			case KEYBOARD_LEFT :
				if(selectingMenu == MENU_SIGNIO)
					selectingMenu = MENU_MAIN1;
				else if(selectingMenu == MENU_MAIN1)
					selectingMenu = MENU_SIGNIO;
				break;
			case KEYBOARD_BACKSPACE :
				if(selectingMenu == MENU_ID)
						id[strlen(id) - 1] = '\0';
				else if(selectingMenu == MENU_PASS)
						password[strlen(password) - 1] = '\0';
				break;
			case KEYBOARD_ENTER :
				if(selectingMenu == MENU_ID)
					selectingMenu = MENU_PASS;
				else if(selectingMenu == MENU_PASS)
					selectingMenu = MENU_MAIN1;
				else if(selectingMenu == MENU_MAIN1)
				{
					bool signInSuccess = true;
					/*int fd = open("info", O_RDWR | O_CREAT, 0644);

					int bufLength = IDPASSLENGTH + 1 + IDPASSLENGTH + 1 + 3 + 1 + 3 + 1;
					char buf[bufLength]; // ID(15)  PASS(15)  WIN(3)  LOSE(3)
					char tempID[IDPASSLENGTH], tempPW[IDPASSLENGTH];
					int tempWIN, tempLOSE;
					bool signInSuccess = true;

					while(read(fd, buf, bufLength) > 0)
					{
						sscanf(buf, "%s %s %d %d", tempID, tempPW, &tempWIN, &tempLOSE);
						if(strcmp(tempID, id) == 0)
						{
							mvprintw(LINES - 1, 0, "                                          ");
							mvprintw(LINES - 1, 0, "Sign IN Failed : Already exist duplicated id");
							signInSuccess = false;
							close(fd);
							break;
						}
					}

					if(signInSuccess == true){
						mvprintw(LINES - 1, 0, "                                            ");
						mvprintw(LINES - 1, 1, "Welcome to OTHELLO world (Press any key)");
						getch();

						sprintf(buf, "\n%-15s %-15s %-3d %-3d", id, password, 0, 0);
						write(fd, buf, bufLength);

						close(fd);
						delwin(upMenu);
						delwin(downMenu);
						return selectingMenu;
					} else {
						close(fd);
						break;
					}*/
					char s_id[IDPASSLENGTH];
					char s_password[IDPASSLENGTH];
					int mode = 0;
					sprintf(s_id, "%-15s", id);
					sprintf(s_password, "%-15s", password);
					write(sock, &mode, sizeof(mode));
					write(sock, s_id, strlen(s_id));
					write(sock, s_password, strlen(s_password));
					read(sock, &signInSuccess, sizeof(signInSuccess));
					if(signInSuccess == true){
						mvprintw(LINES - 1, 0, "                                               ");
						mvprintw(LINES - 1, 1, "Welcome to OTHELLO world (press any key)");
						getch();
						delwin(upMenu);
						delwin(downMenu);
						return selectingMenu;
					}
					else if(signInSuccess == false){
						mvprintw(LINES - 1, 0, "                                                   ");
						mvprintw(LINES - 1, 0, "Sign In Failed : Already exist duplicated id");
						break;
					}
				}
				if(selectingMenu == MENU_SIGNIO) {
					delwin(upMenu);
					delwin(downMenu);
					return selectingMenu;
				}
			default :
				if((key >= '0' && key <= '9') || (key >= 'a' && key <= 'z'))
				{
					char tempKey[2];
					tempKey[0] = key;
					tempKey[1] = '\0';
					if(selectingMenu == MENU_ID && strlen(id) < IDPASSLENGTH)
						strcat(id, tempKey);
					else if(selectingMenu == MENU_PASS && strlen(password) < IDPASSLENGTH)
						strcat(password, tempKey);
				}
				break;
		}
	}
}

int _signoutMenu(){
	char id[IDPASSLENGTH] = ""; // user input id
	char password[IDPASSLENGTH] = ""; // user input passwd
	int i;
	werase(stdscr); // Clear Window
	char key; // User Input
	int selectingMenu = MENU_ID;

	WINDOW *upMenu, *downMenu;
	init_updownWindow(upMenu, downMenu, BACKFLAG_MEMU);

	mvprintw(2, 23, "SIGN OUT");
	mvprintw(4, 19, " : ");
	mvprintw(6, 19, " : ");

	while(1){
		attron(COLOR_PAIR(1));
		mvprintw(4, 17, "ID");
		mvprintw(6, 11, "PASSWORD");
		mvprintw(4, 22, "               ");
		mvprintw(4, 22, id);
		mvprintw(6, 22, "               ");
		for(i=0; i<strlen(password); i++)
			mvprintw(6, 22+i, "*");

		attron(COLOR_PAIR(2));
		mvprintw(11, 17, "SIGN OUT");
		mvprintw(11, 29, "BACK");

		attron(A_STANDOUT | A_UNDERLINE); // selected effect

		// Overwriting menu font
		switch(selectingMenu){
			case MENU_ID :
				curs_set(1);
				mvprintw(4, 17, "ID");
				move(4, 22 + strlen(id));
				break;
			case MENU_PASS :
				curs_set(1);
				mvprintw(6, 11, "PASSWORD");
				move(6, 22 + strlen(password));
				break;
			case MENU_MAIN1 :
				curs_set(0);
				mvprintw(11, 17, "SIGN OUT");
				break;
			case MENU_SIGNIO :
				curs_set(0);
				mvprintw(11, 29, "BACK");
				break;
		}

		attroff(A_STANDOUT | A_UNDERLINE);

		refresh();

		// Input key
		key = getchar();

		switch(key){
			case KEYBOARD_UP :
				if(selectingMenu == MENU_MAIN1 || selectingMenu == MENU_SIGNIO)
					selectingMenu = MENU_PASS;
				else if(selectingMenu == MENU_PASS)
					selectingMenu = MENU_ID;
				break;
			case KEYBOARD_DOWN :
				if(selectingMenu == MENU_ID)
					selectingMenu = MENU_PASS;
				else if(selectingMenu == MENU_PASS)
					selectingMenu = MENU_MAIN1;
				break;
			case KEYBOARD_RIGHT :
				if(selectingMenu == MENU_MAIN1)
					selectingMenu = MENU_SIGNIO;
				else if(selectingMenu == MENU_SIGNIO)
					selectingMenu = MENU_MAIN1;
				break;
			case KEYBOARD_LEFT :
				if(selectingMenu == MENU_SIGNIO)
					selectingMenu = MENU_MAIN1;
				else if(selectingMenu == MENU_MAIN1)
					selectingMenu = MENU_SIGNIO;
				break;
			case KEYBOARD_BACKSPACE :
				if(selectingMenu == MENU_ID)
						id[strlen(id) - 1] = '\0';
				else if(selectingMenu == MENU_PASS)
						password[strlen(password) - 1] = '\0';
				break;
			case KEYBOARD_ENTER :
				if(selectingMenu == MENU_ID)
					selectingMenu = MENU_PASS;
				else if(selectingMenu == MENU_PASS)
					selectingMenu = MENU_MAIN1;
				else if(selectingMenu == MENU_MAIN1)
				{
					/*int fd = open("info", O_RDWR);

					int bufLength = IDPASSLENGTH + 1 + IDPASSLENGTH + 1 + 3 + 1 + 3 + 1;
					char buf[bufLength]; // ID(15)  PASS(15)  WIN(3)  LOSE(3)
					char tempID[IDPASSLENGTH], tempPW[IDPASSLENGTH];
					int tempWIN, tempLOSE;
					bool signInSuccess = true;
					int lineCount = 0;

					while(read(fd, buf, bufLength) > 0)
					{
						lineCount++;
						sscanf(buf, "%s %s %d %d", tempID, tempPW, &tempWIN, &tempLOSE);
						if(strcmp(tempID, id) == 0)
						{
							if(strcmp(tempPW, password) == 0) // Login Success!
							{
								mvprintw(LINES - 1, 0, "                                          ");
								mvprintw(LINES - 1, 0, "Goodbye %s", id);
								getch();

								while(read(fd, buf, bufLength) > 0){
									lineCount++;
									sscanf(buf, "%s %s %d %d", tempID, tempPW, &tempWIN, &tempLOSE);
									lseek(fd, (off_t)-bufLength*2, SEEK_CUR);
									write(fd, buf, bufLength);
									lseek(fd, (off_t)+bufLength, SEEK_CUR);
								}

								ftruncate(fd, (lineCount-1)*bufLength);

								close(fd);
								delwin(upMenu);
								delwin(downMenu);
								return MENU_MAIN1;
							}
						}
					}

					mvprintw(LINES - 1, 0, "                                            ");
					mvprintw(LINES - 1, 0, "Failed to Sign out");
					close(fd);
					*/
					bool signOutSuccess;
					char s_id[IDPASSLENGTH];
					char s_password[IDPASSLENGTH];
					int mode = 1;
					sprintf(s_id, "%-15s", id);
					sprintf(s_password, "%-15s", password);
					write(sock, &mode, sizeof(mode));
					write(sock, s_id, strlen(s_id));
					write(sock, s_password, strlen(s_password));
					read(sock, &signOutSuccess, sizeof(signOutSuccess));
					if(signOutSuccess == true){
						mvprintw(LINES - 1, 0, "                                              ");
						mvprintw(LINES - 1, 1, "Good bye %s", id);
						getch();
						delwin(upMenu);
						delwin(downMenu);
						return selectingMenu;
					}
					mvprintw(LINES - 1, 0, "                                                      ");
					mvprintw(LINES - 1, 0, "Sign Out fail");
					break;
				}
				if(selectingMenu == MENU_SIGNIO) {
					delwin(upMenu);
					delwin(downMenu);
					return selectingMenu;
				}
			default :
				if((key >= '0' && key <= '9') || (key >= 'a' && key <= 'z'))
				{
					char tempKey[2];
					tempKey[0] = key;
					tempKey[1] = '\0';
					if(selectingMenu == MENU_ID && strlen(id) < IDPASSLENGTH)
						strcat(id, tempKey);
					else if(selectingMenu == MENU_PASS && strlen(password) < IDPASSLENGTH)
						strcat(password, tempKey);
				}
				break;
		}
	}
}

int _playMenu(){
	werase(stdscr); // Clear Window
	curs_set(0); // Not need Cursor pointer
	char key; // User Input
	int selectingMenu = MENU_INGAME;
	int wait;

	WINDOW *upMenu, *downMenu;
	init_updownWindow(upMenu, downMenu, BACKFLAG_MEMU);
	mvprintw(4, 15, "Waiting Another Player");
	mvprintw(11, 1, "Waiting");
	refresh();
	sleep(1);
	read(sock, &loginInfo[0], sizeof(struct loginInformation));
	read(sock, &loginInfo[1], sizeof(struct loginInformation));
	mvprintw(4, 15, "                         ");
	mvprintw(2, 15, "BATTLE PAGE");
	mvprintw(4, 7, "1P(O) : %s", loginInfo[0].id);
	mvprintw(4, 30, "2P(X) : %s", loginInfo[1].id);
	mvprintw(6, 4, "WIN : %d / LOSE : %d", loginInfo[0].win, loginInfo[0].lose);
	int r=0;
	if((loginInfo[0].win + loginInfo[0].lose) != 0) 
		r = loginInfo[0].win * 100/(loginInfo[0].win + loginInfo[0].lose);
	mvprintw(6, 31, "WIN : %d / LOSE : %d", loginInfo[1].win, loginInfo[1].lose);

	
	mvprintw(7, 10, "(%d %)", r);
	r = 0;
	if((loginInfo[1].win + loginInfo[1].lose) != 0)
		r = loginInfo[1].win * 100/(loginInfo[1].win + loginInfo[1].lose);
	mvprintw(7, 37, "(%d %)", r);

	

	while(1){
		attron(COLOR_PAIR(2));
		mvprintw(11, 1, "         ");
		mvprintw(11, 6, "PLAY");
	//	mvprintw(11, 18, "STATISTICS");
		mvprintw(11, 37, "BACK");

		attron(A_STANDOUT | A_UNDERLINE); // selected effect

		// Overwriting menu font
		switch(selectingMenu){
			case MENU_INGAME :
				mvprintw(11, 6, "PLAY");
				break;
			case MENU_STATISTICS :
				mvprintw(11, 18, "STATISTICS");
				break;
			case MENU_MAIN1 :
				mvprintw(11, 37, "BACK");
				break;
		}

		attroff(A_STANDOUT | A_UNDERLINE);

		refresh();

		key = getchar();

		switch(key){
			case KEYBOARD_RIGHT :
				if(selectingMenu == MENU_INGAME)
					selectingMenu = MENU_MAIN1;
				else if(selectingMenu == MENU_STATISTICS)
					selectingMenu = MENU_MAIN1;
				else if(selectingMenu == MENU_MAIN1)
					selectingMenu = MENU_INGAME;
				break;
			case KEYBOARD_LEFT :
				if(selectingMenu == MENU_STATISTICS)
					selectingMenu = MENU_INGAME;
				else if(selectingMenu == MENU_MAIN1)
					selectingMenu = MENU_INGAME;
				else if(selectingMenu == MENU_INGAME)
					selectingMenu = MENU_MAIN1;
				break;
			case KEYBOARD_ENTER :
				if(selectingMenu == MENU_MAIN1){
					mvprintw(11, 1, "                                         ");
					mvprintw(11, 18, "Waiting....");
					refresh();
					int mode = 3;
					write(sock, &mode, sizeof(mode));
					read(sock, &mode ,sizeof(mode));
					loginInfo[PLAYER1].login = false;
					loginInfo[PLAYER2].login = false;
				}
				if(selectingMenu == MENU_INGAME){
					mvprintw(11, 1, "                                         ");
					mvprintw(11, 18, "Play Waiting....");
					refresh();
					int mode = 4;
					write(sock, &mode, sizeof(mode));
					read(sock, &mode, sizeof(mode));
				}
				delwin(upMenu);
				delwin(downMenu);
				return selectingMenu;
		}
	}
}

int _ingameMenu(){
	werase(stdscr); // Clear Window
	curs_set(0); // Not need Cursor pointer
	char key; // User Input
	int a;
	int selectingMenu = ingameSelectHorse(2, 2);
	int lastHorseCursor = 5; // Last horse cursor location before moving menu
	int winner = 0; // 0 = anyone, 1 = 1p, 2 = 2p, 3 = draw
	char myTurn;
	int c =0;
	char thisTurn = 'O';
	char tempBoard[6][6] = {" "};
	int i, j;

	WINDOW *leftMenu, *rightMenu;
	init_updownWindow(leftMenu, rightMenu, BACKFLAG_INGAME);

	//ingameInitBoard();
	recv(sock, &myTurn, sizeof(myTurn), 0);
	while(1){
		attron(COLOR_PAIR(3)); // Board Color
		
		if(c == 0){
			winner = 0;
			mvprintw(21, 1, "recv start");
			refresh();
			recv(sock, *board, sizeof(board), MSG_WAITALL);
			recv(sock, &thisTurn, sizeof(thisTurn), MSG_WAITALL);
			recv(sock, &a, sizeof(a), MSG_WAITALL);
			winner = ingameIsGameEnded();
			
			if(a == 0) a = 414;
			selectingMenu = a;
			if(winner != 0) selectingMenu = INGAME_REGAME;
			mvprintw(22, 2, "recv end");

			if(a == 1001) return MENU_MAIN1;	
		}

		mvprintw(0, 6, "+---+---+---+---+---+---+");
		mvprintw(1, 6, "|   |   |   |   |   |   |");
		mvprintw(2, 6, "+---+---+---+---+---+---+");
		mvprintw(3, 6, "|   |   |   |   |   |   |");
		mvprintw(4, 6, "+---+---+---+---+---+---+");
		mvprintw(5, 6, "|   |   |   |   |   |   |");
		mvprintw(6, 6, "+---+---+---+---+---+---+");
		mvprintw(7, 6, "|   |   |   |   |   |   |");
		mvprintw(8, 6, "+---+---+---+---+---+---+");
		mvprintw(9, 6, "|   |   |   |   |   |   |");
		mvprintw(10, 6, "+---+---+---+---+---+---+");
		mvprintw(11, 6, "|   |   |   |   |   |   |");
		mvprintw(12, 6, "+---+---+---+---+---+---+");

		attron(COLOR_PAIR(1)); // Board Horse Color

		ingameDrawBoard();

		attron(COLOR_PAIR(2)); // Menu Color

		mvprintw(2, 43, "          ");
		mvprintw(3, 43, "          ");
		mvprintw(2, 41, "1P(O) : %d", ingameTheNumberOfHorse('O'));
		mvprintw(3, 41, "2P(X) : %d", ingameTheNumberOfHorse('X'));

		mvprintw(6, 42, "NEXTTURN");
		mvprintw(8, 43, "REGAME");
		mvprintw(10, 44, "EXIT");
			
		if(winner != 0){
			mvprintw(6, 42, "          ");
			if(winner == 1)
				mvprintw(6, 43, "1P Win!");
			else if(winner == 2)
				mvprintw(6, 43, "2P Win!");
			else if(winner == 3)
				mvprintw(6, 43, "Draw!");
		}

		attron(A_UNDERLINE);

		if(winner == 0)
			mvprintw(6, 42, "N");
			
		mvprintw(8, 43, "R");
		mvprintw(10, 45, "X");

		attroff(A_UNDERLINE);

		attron(A_STANDOUT | A_UNDERLINE); // selected effect

		// Overwriting menu font
		if((thisTurn != myTurn) && (winner == 0))
			selectingMenu = INGAME_NEXTTURN;
		switch(selectingMenu){
			case INGAME_NEXTTURN :
				mvprintw(6, 42, "NEXTTURN");
				break;
			case INGAME_REGAME :
				mvprintw(8, 43, "REGAME");
				break;
			case MENU_PLAY :
				mvprintw(10, 44, "EXIT");
				break;
			default :
				attroff(A_STANDOUT | A_UNDERLINE);
				ingameDrawOneHorse(selectingMenu);
				break;
		}

		attron(A_STANDOUT); // Turn effect

		switch(thisTurn){
			case 'O' :
				mvprintw(2, 41, "1P(O)");
				break;
			case 'X' :
				mvprintw(3, 41, "2P(X)");
				break;
		}

		attroff(A_STANDOUT | A_UNDERLINE);

		refresh();

		if(thisTurn == myTurn){
			c = 1;
			key = getchar();
		}
		switch(key){
			case 'n' :
				if(winner != 0)
					break;
				lastHorseCursor = selectingMenu;
				selectingMenu = INGAME_NEXTTURN;
				mvprintw(3, 44, "%c", key);
				break;
			case 'r' :
				if(winner != 0)
					break;
				lastHorseCursor = selectingMenu;
				selectingMenu = INGAME_REGAME;
				mvprintw(3, 44, "%c", key);
				break;
			case 'x' :
				if(winner != 0)
					break;
				lastHorseCursor = selectingMenu;
				selectingMenu = MENU_PLAY;
				mvprintw(3, 44, "%c", key);
				break;
			case KEYBOARD_RIGHT :
				if(winner == 0) // anyone win
				{
					if(selectingMenu >= INGAME_HORSE){ // cursor in the board
						if((selectingMenu - INGAME_HORSE) % 6 == 5){
							lastHorseCursor = selectingMenu;
							selectingMenu = INGAME_NEXTTURN;
						}
						else if((selectingMenu - INGAME_HORSE) % 6 >= 0 && (selectingMenu - INGAME_HORSE) % 6 <= 4)
							selectingMenu++;
					}
				}
				break;
			case KEYBOARD_LEFT :
				if(winner == 0) // anyone win
				{
					if(selectingMenu >= INGAME_HORSE){ // cursor in the board
						if((selectingMenu - INGAME_HORSE) % 6 >= 1 && (selectingMenu - INGAME_HORSE) % 6 <= 5)
							selectingMenu--;
					} else { // cursor in the menu
						selectingMenu = lastHorseCursor;
					}
				}
				break;
			case KEYBOARD_DOWN :
				if(selectingMenu >= INGAME_HORSE){ // cursor in the board
					if((selectingMenu - INGAME_HORSE) / 6 >= 0 && (selectingMenu - INGAME_HORSE) / 6 <= 4)
						selectingMenu += 6;
				} else { // cursor in the menu
					if(selectingMenu == INGAME_NEXTTURN)
						selectingMenu = INGAME_REGAME;
					else if(selectingMenu == INGAME_REGAME)
						selectingMenu = MENU_PLAY;
					else if(selectingMenu == MENU_PLAY){
						if(winner == 0) // is gane ended?
							selectingMenu = INGAME_NEXTTURN;
						else
							selectingMenu = INGAME_REGAME;
					}
				}
				break;
			case KEYBOARD_UP :
				if(selectingMenu >= INGAME_HORSE){ // cursor in the board
					if((selectingMenu - INGAME_HORSE) / 6 >= 1 && (selectingMenu - INGAME_HORSE) / 6 <= 5)
						selectingMenu -= 6;
				} else { // cursor in the menu
					if(selectingMenu == INGAME_NEXTTURN)
						selectingMenu = MENU_PLAY;
					else if(selectingMenu == INGAME_REGAME){
						if(winner == 0) // is gane ended?
							selectingMenu = INGAME_NEXTTURN;
						else
							selectingMenu = MENU_PLAY;
					}
					else if(selectingMenu == MENU_PLAY)
						selectingMenu = INGAME_REGAME;
				}
				break;
			case KEYBOARD_ENTER :
				if(selectingMenu >= INGAME_HORSE){ // cursor in the board
					for(i=0; i<6; i++){
						for(j=0; j<6; j++)
							tempBoard[i][j] = board[i][j]; // board copy
					}

					if(ingameEnterKeyPressInBoard(selectingMenu, thisTurn) == 0){
						for(i=0; i<6; i++){
							for(j=0; j<6; j++)
								board[i][j] = tempBoard[i][j]; // board copy
						}
					} 
					else{
						c = 0;
						write(sock, *board, sizeof(board));
						write(sock, &selectingMenu, sizeof(selectingMenu));
					}
					
					winner = ingameIsGameEnded();
					
					if(winner != 0) // game end?
					{
						if(thisTurn == 'X')
							thisTurn = 'O';
						else if(thisTurn == 'O')
							thisTurn = 'X';
					}/*			
						if(winner == 1){
							loginInfo[PLAYER1].win++;
							loginInfo[PLAYER2].lose++;
						} else if(winner == 2){
							loginInfo[PLAYER2].win++;
							loginInfo[PLAYER1].lose++;
						} else if(winner == 3){
							
						} else {
							printf("ingame winner eroor");
						}
						
						saveInfo();
						selectingMenu = INGAME_REGAME;
					}*/
				} 
				else { // cursor in the menu
					if(selectingMenu == INGAME_NEXTTURN) {
						if(myTurn == thisTurn){
						write(sock, *board, sizeof(board));
						write(sock, &lastHorseCursor, sizeof(lastHorseCursor));
						c = 0;
						}
						selectingMenu = lastHorseCursor;
						thisTurn = ingameOppositeTurn(thisTurn);
					}
					else if(selectingMenu == INGAME_REGAME){
						ingameInitBoard();
						write(sock, *board, sizeof(board));
						int re = 1000;
						write(sock, &re, sizeof(re));
						winner = 0;
						c = 0;
						thisTurn = 'O';
						break;
						//delwin(leftMenu);
						//delwin(rightMenu);
						//return MENU_INGAME;
					}
					else if(selectingMenu == MENU_PLAY){
						write(sock, *board, sizeof(board));
						int re = 1001;
						write(sock, &re, sizeof(re));
						c = 0;
						break;
					}
				}
				break;
			default : break;
				
		}
	}
}


void ingameDrawBoard(){
	int i,j;

	for(i=0; i<6; i++){
		for(j=0; j<6; j++)
			mvprintw(2*i + 1, 8+(j*4), "%c", board[i][j]);
	}
}


int ingameSelectHorse(int x, int y){
	return y*6 + x + INGAME_HORSE;
}


void ingameInitBoard(){
	memset(board, ' ', 36);

	board[2][2] = 'O';
	board[2][3] = 'X';
	board[3][2] = 'X';
	board[3][3] = 'O';
}


void ingameDrawOneHorse(int horse){
	horse = horse - INGAME_HORSE;
	int x = horse / 6;
	int y = horse % 6;

	mvprintw(2*x + 1, 8+(y*4), "%c", board[x][y]);
}

int ingameTheNumberOfHorse(char horse){
	int i, j, count = 0;
	for(i=0; i<6; i++){
		for(j=0; j<6; j++){
			if(board[i][j] == horse)
				count++;
		}
	}

	return count;
}

char ingameIsGameEnded(){
	int p1 = ingameTheNumberOfHorse('O');
	int p2 = ingameTheNumberOfHorse('X');
	int i, j, x, y, eatable1p = 0, eatable2p = 0;

	char tempBoard[6][6];
	
	for(i=0; i<6; i++){
		for(j=0; j<6; j++){
			tempBoard[i][j] = board[i][j]; // board copy
		}
	}

	for(y=0; y<6; y++){
		for(x=0; x<6; x++){
			eatable1p += ingameEnterKeyPressInBoard(ingameSelectHorse(y, x), 'O');
			eatable2p += ingameEnterKeyPressInBoard(ingameSelectHorse(y, x), 'X');
			
			for(i=0; i<6; i++){
				for(j=0; j<6; j++)
			 		board[i][j] = tempBoard[i][j]; // board copy
			}			
		}
	}
	
	if(eatable1p == 0 && eatable2p == 0){
		if(p1 > p2){
			return 1; // 1p win
		}
		else if(p1 < p2){
			return 2; // 2p win
		}	
		else
			return 3; // draw
	}
	else
		return 0; // anyone win
}

int ingameEnterKeyPressInBoard(int selectingHorse, char thisTurn){ // return value is eaten stone
	int x = (selectingHorse - INGAME_HORSE) % 6;
	int y = (selectingHorse - INGAME_HORSE) / 6;
	int i, j, tempX, tempY, count, directionX, directionY, countAll = 0;

	if(board[y][x] != ' ')
		return 0;

	for(directionX = -1; directionX <= 1; directionX++){
		for(directionY = -1; directionY <= 1; directionY++){
			count = 0;
			tempX = x + directionX;
			tempY = y + directionY;

			if(directionX == 0 && directionY == 0)
				continue;

			while(tempX >= 0 && tempX <= 5 && tempY >= 0 && tempY <= 5){
				if(board[tempY][tempX] == ingameOppositeTurn(thisTurn)){
					tempX += directionX;
					tempY += directionY;

					if(tempX < 0 || tempX > 5 || tempY < 0 || tempY > 5){
						count = 0;
						break;
					} else {
						count++;
						continue;
					}
				}
				else if(board[tempY][tempX] == thisTurn)
					break;
				else if(board[tempY][tempX] == ' '){
					count = 0;
					break;
				}
			}

			if(count != 0){
				for(i=0; i<count+1; i++){
					board[y + (i * directionY)][x + (i * directionX)] = thisTurn;
					countAll++;
				}
			}
		}
	}

	return countAll;
}

char ingameOppositeTurn(char horse){
	if(horse == 'O')
		return 'X';
	else if(horse == 'X')
		return 'O';

}


int _statisticsMenu(){
	werase(stdscr); // Clear Window
	curs_set(0); // Not need Cursor pointer
	char key; // User Input
	int selectingMenu = MENU_PLAY;
	WINDOW *upMenu, *downMenu;
	init_updownWindow(upMenu, downMenu, BACKFLAG_MEMU);

	mvprintw(2, 20, "STATISTICS");

	// 1P info
	mvprintw(4, 6, "ID : %s", loginInfo[PLAYER1].id);
	if(loginInfo[PLAYER1].win + loginInfo[PLAYER1].lose != 0){
		mvprintw(6, 4, "WIN : %d / LOSE : %d", loginInfo[PLAYER1].win, loginInfo[PLAYER1].lose);
		mvprintw(7, 10, "(%d %)", (loginInfo[PLAYER1].win * 100) / (loginInfo[PLAYER1].win + loginInfo[PLAYER1].lose));
	}
	else{
		mvprintw(6, 4, "WIN : 0 / LOSE : 0");
		mvprintw(7, 10, "(0 %)");
	}

	// 2P info
	mvprintw(4, 33, "ID : %s", loginInfo[PLAYER2].id);
	if(loginInfo[PLAYER2].win + loginInfo[PLAYER2].lose != 0){
		mvprintw(6, 31, "WIN : %d / LOSE : %d", loginInfo[PLAYER2].win, loginInfo[PLAYER2].lose);
		mvprintw(7, 37, "(%d %)", (loginInfo[PLAYER2].win * 100) / (loginInfo[PLAYER2].win + loginInfo[PLAYER2].lose));
	}
	else{
		mvprintw(6, 31, "WIN : 0 / LOSE : 0");
		mvprintw(7, 37, "(0 %)");
	}

	while(1){
		attron(COLOR_PAIR(2));
		mvprintw(11, 23, "BACK");

		attron(A_STANDOUT | A_UNDERLINE); // selected effect

		// Overwriting menu font
		switch(selectingMenu){
			case MENU_PLAY :
				mvprintw(11, 23, "BACK");
				break;
		}

		attroff(A_STANDOUT | A_UNDERLINE);

		refresh();

		key = getchar();

		switch(key){
			case KEYBOARD_ENTER :
				delwin(upMenu);
				delwin(downMenu);
				return MENU_PLAY;
		}
	}
}

void init_updownWindow(WINDOW *upWindow, WINDOW *downWindow, int flag)
{
	if(flag == BACKFLAG_MEMU){
		upWindow = subwin(stdscr, 10, 53, 0, 0);
		wbkgd(upWindow, COLOR_PAIR(1));
		downWindow = subwin(stdscr, 3, 53, 10, 0);
		wbkgd(downWindow, COLOR_PAIR(2));
		attron(COLOR_PAIR(1));
	} else if(flag == BACKFLAG_INGAME){
		upWindow = subwin(stdscr, 13, 38, 0, 0);
		wbkgd(upWindow, COLOR_PAIR(1));
		downWindow = subwin(stdscr, 13, 16, 0, 38);
		wbkgd(downWindow, COLOR_PAIR(2));
		attron(COLOR_PAIR(1));
	} else {
		perror("Not supported flag : ");
	}
}

void init_scr()
{
	initscr();
	start_color();
	init_pair(1, COLOR_BLUE, COLOR_WHITE);
	init_pair(2, COLOR_WHITE, COLOR_BLUE);
	init_pair(3, COLOR_BLACK, COLOR_WHITE);
}

void saveInfo()
{
	int fd = open("info", O_RDWR);

	int bufLength = IDPASSLENGTH + 1 + IDPASSLENGTH + 1 + 3 + 1 + 3 + 1;
	char buf[bufLength]; // ID(15)  PASS(15)  WIN(3)  LOSE(3)
	char tempID[IDPASSLENGTH], tempPW[IDPASSLENGTH];
	int tempWIN, tempLOSE;

	while(read(fd, buf, bufLength) > 0)
	{
		sscanf(buf, "%s %s %d %d", tempID, tempPW, &tempWIN, &tempLOSE);
		if(strcmp(tempID, loginInfo[PLAYER1].id) == 0)
		{
			sprintf(buf, "\n%-15s %-15s %-3d %-3d", loginInfo[PLAYER1].id, loginInfo[PLAYER1].pass, loginInfo[PLAYER1].win, loginInfo[PLAYER1].lose);
			lseek(fd, (off_t)-bufLength, SEEK_CUR);
			write(fd, buf, bufLength);

			break;
		}
	}
	
	lseek(fd, 0, SEEK_SET);

	while(read(fd, buf, bufLength) > 0)
	{
		sscanf(buf, "%s %s %d %d", tempID, tempPW, &tempWIN, &tempLOSE);
		if(strcmp(tempID, loginInfo[PLAYER2].id) == 0)
		{
			sprintf(buf, "\n%-15s %-15s %-3d %-3d", loginInfo[PLAYER2].id, loginInfo[PLAYER2].pass, loginInfo[PLAYER2].win, loginInfo[PLAYER2].lose);
			lseek(fd, (off_t)-bufLength, SEEK_CUR);
			write(fd, buf, bufLength);

			break;
		}
	}
	
	close(fd);
}
