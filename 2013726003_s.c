#include<ncurses.h>
#include<string.h>
#include<fcntl.h>
#include<string.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<unistd.h>

#define IDPASSLENGTH 15

struct loginInformation{
	bool login;
	char id[IDPASSLENGTH];
	char pass[IDPASSLENGTH];
	int win;
	int lose;
};


int serv_sock;
int clnt_sock[2];
int clnt_addr_size[2];
char board[6][6];
struct sockaddr_in serv_addr;
struct sockaddr_in clnt_addr[2];
struct loginInformation loginInfo[2];

int main(int argc, char **argv){
	int str_len;
	char id[IDPASSLENGTH];
	char pass[IDPASSLENGTH];
	char tid[IDPASSLENGTH];
	char tpw[IDPASSLENGTH];
	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(serv_sock == -1)
		perror("socket() error");
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi(argv[1]));

	if(bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
		perror("bind() error");

	if(listen(serv_sock, 3) == -1)
		perror("listen() error");

	fd_set readfds;
	int count = 0;
	int fd_num;
	int i;
	int mode = 10;
	int maxfd = 0;
	int loginCount = 0;
	int backcount = 0;
	int playcount = 0;
	int cur = 0;
	memset(board, ' ', 36);
	board[2][2] = 'O';
	board[3][3] = 'O';
	board[2][3] = 'X';
	board[3][2] = 'X';
	/*clnt_addr_size = sizeof(clnt_addr);
	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
	*/
	while(1)
	{
		/*if(clnt_sock == -1)
			perror("accept() error");*/
		FD_ZERO(&readfds);
		FD_SET(serv_sock, &readfds);
		for ( i = 0; i < count; i++)
			FD_SET(clnt_sock[i], &readfds);

		maxfd = 3+count+1;
		if(select(maxfd, &readfds, NULL, NULL, 0) < 0)
			perror("select error");
		if(FD_ISSET(serv_sock, &readfds)){
			clnt_addr_size[count] = sizeof(clnt_addr[count]);
			clnt_sock[count] = accept(serv_sock, (struct sockaddr *)&clnt_addr[count], &clnt_addr_size[count]);
			FD_SET(clnt_sock[count], &readfds);
			fprintf(stderr,"\nclnt_sock = %d", clnt_sock[count]);
			if(clnt_sock[count] > maxfd)
				maxfd = clnt_sock[count];
			count++;
			fprintf(stderr,"\ncount %d",count);
		}

		fprintf(stderr,"\nread start\n");	

		for( i = 0; i <count; i++){
			if(FD_ISSET(clnt_sock[i], &readfds)){
	
				recv(clnt_sock[i], &mode, sizeof(mode), MSG_DONTWAIT);
				if(mode == 0){
					fprintf(stderr,"\nSign In start");
					read(clnt_sock[i], tid, 15);
					read(clnt_sock[i], pass, 15);
					int j = 0;
					for(j = 0; tid[j] != ' ' ; j ++){
						id[j] = tid[j];
						id[j+1] = '\0';
					}		
				
					int fd = open("info", O_RDWR | O_CREAT, 0644);
					int bufLength = IDPASSLENGTH + 1 + IDPASSLENGTH + 1 + 3 +1 +3 +1;
					char buf[bufLength];
					char tempID[IDPASSLENGTH], tempPW[IDPASSLENGTH];
					int tempWIN, tempLOSE;
					bool signInSuccess = true;
				//	fprintf(stderr,"\nid = %s pass = %s\n",id,pass);	
					while(read(fd, buf, bufLength) > 0)
					{
				//printf("\ntempID = %s %d %d %d\n", tempID, strcmp(tempID, id), strlen(id),strlen(tempID));
						sscanf(buf, "%s %s %d %d", tempID, tempPW, &tempWIN, &tempLOSE);		
						if(strcmp(tempID, id) == 0){
							signInSuccess = false;
							close(fd);
							break;
						}	
			
					}
					if (signInSuccess == true){
						sprintf(buf, "\n%-15s %-15s %-3d %-3d", id, pass, 0, 0);
						write(fd, buf, bufLength);
						close(fd);
					}
					printf("\nSign In end result = %d", signInSuccess);
					write(clnt_sock[i], &signInSuccess, sizeof(signInSuccess));
					
				}// mode == 0 (sign in) end

				else if(mode == 1){
					printf("\nSign Out start");
					if(read(clnt_sock[i], tid, 15) < 0) 
						perror("read error");
					if(read(clnt_sock[i], tpw, 15) < 0)
						perror("read error");
					printf("\nsign out id = %s pass = %s", tid, tpw);
					int fd = open("info", O_RDWR);
					int bufLength = IDPASSLENGTH + 1 + IDPASSLENGTH + 1 +3 +1 +3 +1;
					char buf[bufLength];
					char tempID[IDPASSLENGTH], tempPW[IDPASSLENGTH];
					int tempWIN, tempLOSE;
					bool signOutSuccess = false;
					int lineCount = 0;	
					while(read(fd, buf, bufLength) > 0)
					{
						lineCount++;
						sscanf(buf, "%s %s %d %d", tempID, tempPW, &tempWIN, &tempLOSE);
						int j = 0;
						for(j = 0; tid[j] != ' '; j++){
							id[j] = tid[j];
							id[j+1] = '\0';
						}
						int k;
						for(k = 0; tpw[k] != ' '; k++){
							pass[k] = tpw[k];
							pass[k+1] = '\0';
						}
							
						printf("\nid = %s tempid = %s strcmp = %d\npass = %s tempass = %s strcmp = %d\n",id,tempID, strcmp(id,tempID), pass, tempPW, strcmp(pass,tempPW));
						if(strcmp(tempID, id) == 0)
						{
							if(strcmp(tempPW, pass) == 0)
							{
								if((loginCount == 1) && (strcmp(loginInfo[0].id, id) == 0))
									break;
								signOutSuccess = true;
								while(read(fd, buf, bufLength) > 0){
									lineCount++;
									sscanf(buf, "%s %s %d %d", tempID, tempPW, &tempWIN, &tempLOSE);
									lseek(fd, (off_t)-bufLength*2, SEEK_CUR);
									write(fd, buf, bufLength);
									lseek(fd, (off_t)+bufLength, SEEK_CUR);
								}
							ftruncate(fd, (lineCount - 1)*bufLength);
							}
						}
					}
					fprintf(stderr,"\nSign Out end");
					close(fd);
					write(clnt_sock[i], &signOutSuccess, sizeof(signOutSuccess));
					
				}// mode = 1 (sign out) end
				else if(mode == 2){
					fprintf(stderr,"\nlogIn Start");
					read(clnt_sock[i], tid, 15);
					read(clnt_sock[i], tpw, 15);
					int j;
					for(j = 0; tid[j] != ' '; j++){
						id[j] = tid[j];
						id[j+1] = '\0';
					}
					int k;
					for(k = 0; tpw[k] != ' '; k++){
						pass[k] = tpw[k];
						pass[k+1] = '\0';
					}
					int fd = open("info", O_RDWR);
					int bufLength = IDPASSLENGTH + 1 + IDPASSLENGTH + 1 + 3 + 1 + 3 + 1;
					char buf[bufLength];
					char tempID[IDPASSLENGTH], tempPW[IDPASSLENGTH];
					int tempWIN, tempLOSE;
					bool loginSuccess = false;
					while(read(fd, buf, bufLength) > 0)
					{
						sscanf(buf, "%s %s %d %d", tempID, tempPW, &tempWIN, &tempLOSE);
						//fprintf(stderr,"%s %s %s %s %d %d", id, pass, tempID, tempPW, strcmp(tempID,id), strcmp(tempPW, pass));
						if(strcmp(tempID, id) == 0)
						{
							if(strcmp(tempPW, pass) == 0)
							{
								if((loginCount == 1) && (strcmp(loginInfo[0].id, id) == 0))
								{
									break;
								}
								loginSuccess = true;
								loginInfo[loginCount].login = true;
								strcpy(loginInfo[loginCount].id, id);
								strcpy(loginInfo[loginCount].pass, pass);
								loginInfo[loginCount].win = tempWIN;
								loginInfo[loginCount].lose = tempLOSE;
								loginCount++;
								break;
							}
						}
					}
					write(clnt_sock[i], &loginSuccess, sizeof(loginSuccess));
					if(loginCount == 2){
						int l;
						for(l = 0; l < 2; l++){
							send(clnt_sock[l], &loginInfo[0], sizeof(struct loginInformation), 0);
							send(clnt_sock[l], &loginInfo[1], sizeof(struct loginInformation), 0);
						}
						loginCount = 3;
					}
					close(fd);
					fprintf(stderr,"\nlogin end result = %d loginCount = %d", loginSuccess, loginCount);
					
				}// mode = 2 (log in) end
				else if(mode == 3){
					fprintf(stderr, "\nback start");
					backcount++;
					if(backcount == 2){
						int l;
						for(l = 0; l<2; l++){
							send(clnt_sock[l], &l, sizeof(l), 0);
						}
						backcount = 0;
						loginCount = 0;
						fprintf(stderr, "\nback Act");
					}
					fprintf(stderr, "\nback end");
				}// mode = 3 (back) end
				else if(mode == 4){
					fprintf(stderr, "\nplay start");
					playcount++;
					if(playcount == 2){
						int l;
						for(l = 0; l < 2; l ++){
							write(clnt_sock[l], &l, sizeof(l));
						}
						char turn = 'O';
						char turn1 = 'O';
						char turn2 = 'X';
						send(clnt_sock[0], &turn1, sizeof(turn1), 0);
						send(clnt_sock[1], &turn2, sizeof(turn2), 0);
				
						while(1){
						fprintf(stderr,"\nsend start");
						send(clnt_sock[0], *board, sizeof(board), 0);
						send(clnt_sock[1], *board, sizeof(board), 0);
						send(clnt_sock[0], &turn, sizeof(turn), 0);
						send(clnt_sock[1], &turn, sizeof(turn), 0);
						send(clnt_sock[0], &cur, sizeof(cur), 0);
						send(clnt_sock[1], &cur, sizeof(cur), 0);
						if(turn == 'O'){
							fprintf(stderr,"\nturn 'O' recv Start");
							recv(clnt_sock[0], *board, sizeof(board), 0);
							recv(clnt_sock[0], &cur, sizeof(cur), 0);
							turn = 'X';
							fprintf(stderr,"\nturn 'O' recv");
						}
						else if(turn == 'X'){
							fprintf(stderr,"\nturn 'X' recv start");
							recv(clnt_sock[1], *board, sizeof(board), 0);
							recv(clnt_sock[1], &cur, sizeof(cur), 0);
							turn = 'O';
							fprintf(stderr,"\nturn 'X' recv");
						}
						if(cur == 1000){
							turn = 'O';
							cur = 414;
						}
						
						if(cur == 1001){
							int aa;
							for(aa = 0; aa < 2; aa++){
								send(clnt_sock[aa], *board, sizeof(board), 0);
								send(clnt_sock[aa], &turn, sizeof(turn), 0);
								send(clnt_sock[aa], &cur, sizeof(cur), 0);
							}
							
							backcount = 0;
							loginCount = 0;
							playcount = 0;
							cur = 0;
							memset(board, ' ', 36);
							board[2][2] = 'O';
							board[3][3] = 'O';
							board[2][3] = 'X';
							board[3][2] = 'X';
							break;
						}
						if(cur == 1){
							int aa;
							cur = 301;
							turn = 'X';
							for(aa = 0; aa < 2; aa++){
								send(clnt_sock[aa], *board, sizeof(board), 0);
								send(clnt_sock[aa], &turn, sizeof(turn), 0);
								send(clnt_sock[aa], &cur, sizeof(cur), 0);
							}
						}
						}
						playcount = 0;
					}
					fprintf(stderr, "\nplay end");
				}// mode = 4 (play) end
			}
		}
	}
}
