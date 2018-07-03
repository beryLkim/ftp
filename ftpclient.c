#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>
int sockfd;
int p1, p2;
struct hostent *hp;
struct sockaddr_in server;
char send_cmd[100];
char read_buf[100];
char send_file[10000];
char read_file[10000];

int Connect()
{
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&server, 0, sizeof(struct sockaddr_in));
    hp = gethostbyname("127.0.0.1");
    memcpy(&server.sin_addr, hp->h_addr_list[0], hp->h_length);
    server.sin_family = AF_INET;
    server.sin_port = htons(9998);
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("unable to connect!\n");
    }
    memset(read_buf, 0, 100);
    read(sockfd, read_buf, 100);
    printf("%s\n", read_buf);
}

int Login()
{
    char user[10];
    char pwd[10];
    while (1)
    {
        printf("Name:");
        scanf("%s", user);
        memset(send_cmd, 0, 100);
        strcpy(send_cmd, "USER ");
        strcat(send_cmd, user);
        // strcat(send_cmd, "\r\n");
        write(sockfd, send_cmd, strlen(send_cmd));
       // printf("%s\n%d\n", send_cmd, strlen(send_cmd));
        memset(read_buf, 0, 100);
        read(sockfd, read_buf, 100);
        if (read_buf[0] == '3' && read_buf[1] == '3' && read_buf[2] == '1')
        {
            memset(send_cmd, 0, 100);
            printf("%s\nPassword:", read_buf);
            scanf("%s", pwd);
            strcpy(send_cmd, "PASS ");
            strcat(send_cmd, pwd);
            //strcat(send_cmd, "\r\n");
            write(sockfd, send_cmd, strlen(send_cmd));
            memset(read_buf, 0, 100);
            read(sockfd, read_buf, 100);
            if (read_buf[0] == '2' && read_buf[1] == '3' && read_buf[2] == '0')
            {
                printf("%s\n", read_buf);
                break;
            }
        }
        printf("%s\n", read_buf);
    }
  //  printf("end\n");
}

int PASV()
{
    memset(send_cmd, 0, 100);
    strcpy(send_cmd, "PASV");
    write(sockfd, send_cmd, strlen(send_cmd));
    read(sockfd, read_buf, 100);
    printf("%s\n", read_buf);
    char c_p1[3];
    char c_p2[3];
    memset(c_p1, 0, 3);
    memset(c_p2, 0, 3);
    int i = 0;
    for (; i < 3; i++)
    {
        // printf("%c\n", read_buf[37 + i]);
        if (read_buf[37 + i] == ',')
        {
            break;
        }
        else
        {
            c_p1[i] = read_buf[37 + i];
        }
    }
    for (int j = 0; j < 3; j++)
    {
        if (read_buf[37 + i + 1 + j] == ')')
            break;
        else
            c_p2[j] = read_buf[37 + i + 1 + j];
    }
   // printf("p1:%s",c_p1);
    p1 = atoi(c_p1);
    p2 = atoi(c_p2);
   // memset(read_buf, 0, 100);
   // printf("p1,p2:%d,%d\n\n", p1, p2);
    //printf("%c%c%c\n\n",read_buf[37],read_buf[38],read_buf[39]);
}

int download()
{
    PASV();
    int data_sock;
    char name[15];
    FILE *fp;
    data_sock = socket(AF_INET, SOCK_STREAM, 0);
    //printf("%d,%d\n\n",p1,p2);////////////////////////////
    int p=p1*256+p2;
    server.sin_port = htons(p);
    connect(data_sock, (struct sockaddr *)&server, sizeof(server));
    while (1)
    {
        printf("(remote-file) ");
        memset(name, 0, 15);
        scanf("%s", name);
        write(sockfd, name, strlen(name));
        memset(read_buf, 0, 100);
        read(sockfd, read_buf, 100);
        printf("%s", read_buf);
        if (read_buf[0] == '5' && read_buf[1] == '5' && read_buf[2] == '0')
        {
        }
        else
        {

            break;
        }
    }
    while (1)
    {
        memset(name, 0, 15);
        printf("(local-file) ");
        scanf("%s", name);
        fp = fopen(name, "w");
        if (fp == NULL)
        {
            printf("fail to open %s!\n", name);
        }
        else
            break;
    }
    memset(read_buf, 0, 100);
    read(sockfd, read_buf, 100);//确保服务器数据完全写入


    memset(read_file, 0, 10000);
    read(data_sock, read_file, 10000);
    //printf("%s\n",read_buf);
    fprintf(fp, read_file);
    fclose(fp);
    memset(read_buf, 0, 100);
    read(sockfd, read_buf, 100);
    printf("%s\n", read_buf);
    close(data_sock);
}


int rest()
{
    PASV();
    int data_sock;
    char name[15];
    FILE *fp;
    data_sock = socket(AF_INET, SOCK_STREAM, 0);
    //printf("%d,%d\n\n",p1,p2);////////////////////////////
    int p=p1*256+p2;
    server.sin_port = htons(p);
    connect(data_sock, (struct sockaddr *)&server, sizeof(server));
    while (1)
    {
        printf("(remote-file) ");
        memset(name, 0, 15);
        scanf("%s", name);
        write(sockfd, name, strlen(name));
        memset(read_buf, 0, 100);
        read(sockfd, read_buf, 100);
        printf("%s", read_buf);
        if (read_buf[0] == '5' && read_buf[1] == '5' && read_buf[2] == '0')
        {
        }
        else
        {

            break;
        }
    }
    while (1)
    {
        memset(name, 0, 15);
        printf("(local-file) ");
        scanf("%s", name);
        fp = fopen(name, "w");
        if (fp == NULL)
        {
            printf("fail to open %s!\n", name);
        }
        else
            break;
    }
    char c_num[10];
    printf("请输入偏移量:");
    scanf("%s",c_num);
    //printf("%s\n",c_num);
    write(sockfd,c_num,strlen(c_num));
    memset(read_buf, 0, 100);
    read(sockfd, read_buf, 100);//确保服务器数据完全写入
    memset(read_file, 0, 10000);
    read(data_sock, read_file, 10000);
    //printf("%s\n",read_buf);
    fprintf(fp, read_file);
    fclose(fp);
    memset(read_buf, 0, 100);
    read(sockfd, read_buf, 100);
    printf("%s\n", read_buf);
    close(data_sock);
}

int upload()
{

    PASV();
    int data_sock;
    FILE *fp;
    char name[15];
    data_sock = socket(AF_INET, SOCK_STREAM, 0);
  //  printf("%d,%d\n\n", p1, p2);///////////////////////////////////
    int p=p1*256+p2;
    server.sin_port = htons(p);
    connect(data_sock, (struct sockaddr *)&server, sizeof(server));
    while (1)
    {
        memset(name, 0, 15);
        printf("(local-file) ");
        scanf("%s", name);
        fp = fopen(name, "r");
        if (fp == NULL)
        {
            printf("fail to open %s!\n", name);
        }
        else
            break;
    }
    while (1)
    {
        printf("(remote-file) ");
        memset(name, 0, 15);
        scanf("%s", name);
        write(sockfd, name, strlen(name));
        memset(read_buf, 0, 100);
        read(sockfd, read_buf, 100);
//printf("150\n\n");
        printf("%s", read_buf);
        if (read_buf[0] == '5' && read_buf[1] == '5' && read_buf[2] == '0')
        {
            //printf("%s", read_buf);
        }
        else
        {
            //printf("%s", read_buf);
            break;
        }
    }
    for (int i = 0; i < 10000; i++)
    {
        fscanf(fp, "%c", &send_file[i]);
    }
  //  printf("%s\n",send_file);///////////////////
    write(data_sock, send_file, strlen(send_file));
    memset(send_cmd,0,100);
    strcpy(send_cmd,"data complete");//确保数据传输完成再读
    write(sockfd,send_cmd,100);
    //printf("%s\n", send_file);
    fclose(fp);
    memset(read_buf, 0, 100);
    read(sockfd, read_buf, 100);
    printf("%s\n", read_buf);
    close(data_sock);
}


int quit()
{
    memset(read_buf, 0, 100);
    //write(sockfd, "QUIT\r\n", strlen("QUIT\r\n"));
    read(sockfd, read_buf, 100);
    printf("%s\n", read_buf);
    close(sockfd);
}

int dir()
{
    PASV();
    int data_sock;
    int read_len;
    FILE *fp;
    data_sock = socket(AF_INET, SOCK_STREAM, 0);
    //printf("%d,%d\n\n", p1, p2);
    int p=p1*256+p2;
    server.sin_port = htons(p);
    connect(data_sock, (struct sockaddr *)&server, sizeof(server));
    memset(read_buf, 0, 100);
    read(sockfd, read_buf, 100);
    printf("%s\n", read_buf);

    memset(read_buf, 0, 100);//确保服务器数据完全写入
    read(sockfd, read_buf, 100);

    memset(read_file, 0, 10000);
    read(data_sock, read_file, 10000);
   
 printf("%s\n", read_file);
    close(data_sock);
    memset(read_buf, 0, 100);
    read(sockfd, read_buf, 100);
    printf("%s\n", read_buf);
}


void *thread_function()
{
        char cmd[10];
    Connect();
    Login();
    while (1)
    {
        printf("ftp->");
        scanf("%s", cmd);
        if (strcmp(cmd, "get") == 0)
        {
            memset(send_cmd, 0, 100);
	    strcpy(send_cmd,"RETR");
            write(sockfd, send_cmd, strlen(send_cmd));
	    memset(read_buf,0,100);
	    read(sockfd,read_buf,100);///////
            download();
	    continue;
        }
        if (strcmp(cmd, "put") == 0)
        {
            memset(send_cmd, 0, 100);
            strcpy(send_cmd,"STOR");
            write(sockfd, send_cmd, strlen(send_cmd));
	    memset(read_buf,0,100);
	    read(sockfd,read_buf,100);///////
//printf("lby\n");
//printf("%s\n",read_buf);
            upload();
	    continue;
        }
        if (strcmp(cmd, "ls") == 0)
        {
            memset(send_cmd, 0, 100);
	    strcpy(send_cmd,"LIST");
            write(sockfd, send_cmd, strlen(send_cmd));
	    memset(read_buf,0,100);
	    read(sockfd,read_buf,100);///////
            dir();
	    continue;
        }
	if(strcmp(cmd,"rest")==0)
	{
	    memset(send_cmd, 0, 100);
	    strcpy(send_cmd,"REST");
            write(sockfd, send_cmd, strlen(send_cmd));
	    memset(read_buf,0,100);
	    read(sockfd,read_buf,100);///////
            rest();
	    continue;
	}
        if (strcmp(cmd, "quit") == 0)
        {
            memset(send_cmd, 0, 100);
            strcpy(send_cmd,"QUIT");
            write(sockfd, send_cmd, strlen(send_cmd));
	    memset(read_buf,0,100);
	    read(sockfd,read_buf,100);///////
            quit();
            break;
        }
        
	printf("?invalid command\n");
    }
}

int main(int argc, char *argv[])
{
    
	int res;
	pthread_t a_thread;
	void *thread_result;
	res = pthread_create(&a_thread, NULL, thread_function,NULL);
		if(res != 0) {
			perror("Thread creation failed");
			exit(EXIT_FAILURE);
		}
		res = pthread_join(a_thread, &thread_result);
		if(res != 0) {
			perror("Thread join failed");
			exit(EXIT_FAILURE);
		}
		exit(EXIT_SUCCESS);
}
