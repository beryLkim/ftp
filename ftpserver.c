#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h> //sockaddr_in等结构的定义
#include <netdb.h>      //addrinfo等结构的定义
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <pthread.h>

int server_sockfd, client_sockfd;
int server_len, client_len;
struct sockaddr_in server_address; //结构体包括ip、端口等
struct sockaddr_in client_address;

int data_server_sockfd, data_client_sockfd;
int data_server_len, data_client_len;
struct sockaddr_in data_server_address; //结构体包括ip、端口等
struct sockaddr_in data_client_address;

char send_cmd[100], read_buf[100];
char send_file[10000], read_file[10000];
FILE *stream; //存目录内容


int dataconnect()
{
    memset(read_buf, 0, 100);
    read(client_sockfd, read_buf, 100); //read pasv,开始监听数据端口连接
    printf("%s\n", read_buf);
    memset(send_cmd, 0, 100);
    int p1,p2;
    srand((int)time(0));
    while(1)
    {
        p1=(int)(rand()%255);
        p2=(int)(rand()%255);
        if(p1!=0&&p2!=0)
            break;
    }
//printf("%d,%d",p1,p2);
char c_p1[3];
char c_p2[3];
sprintf(c_p1,"%d",p1);
sprintf(c_p2,"%d",p2);
    strcpy(send_cmd, "227 Entering Passive Mode (127,0,0,1,");
    strcat(send_cmd,c_p1);
    strcat(send_cmd,",");
    strcat(send_cmd,c_p2);
    strcat(send_cmd,").\n");
printf("%s\n",send_cmd);
    write(client_sockfd, send_cmd, 100);
    data_server_sockfd = socket(AF_INET, SOCK_STREAM, 0); //套接口文件描述符
    data_server_address.sin_family = AF_INET;
    data_server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); //类型转换（长整数型）
    int p = p1*256+p2;
    data_server_address.sin_port = htons(p); //!!!!!!!!!!!!!!!
    data_server_len = sizeof(data_server_address);
    bind(data_server_sockfd, (struct sockaddr *)&data_server_address, data_server_len);
    listen(data_server_sockfd, 5); //允许5个连接
    printf("waiting data socket\n");
    data_client_len = sizeof(data_client_address);
    data_client_sockfd = accept(data_server_sockfd, (struct sockaddr *)&data_client_address, &data_client_len);
    printf("connect successful\n");
}

int download()
{
    dataconnect();
    FILE *fp;
    while (1)
    {
        memset(read_buf, 0, 100);
        read(client_sockfd, read_buf, 100); //得到文件名
        fp = fopen(read_buf, "r");
        if (fp == NULL)
        {
            memset(send_cmd, 0, 100);
            strcpy(send_cmd, "550 Failed to open file.\n");
            write(client_sockfd, send_cmd, 100);
        }
        else
        {
            memset(send_cmd, 0, 100);
            strcpy(send_cmd, "150 Ok to send data.\n");
            write(client_sockfd, send_cmd, 100);
            break;
        }
    }
    
    for (int i = 0; i < 10000; i++)
    {
        fscanf(fp, "%c", &send_file[i]);
    }
    write(data_client_sockfd, send_file, 10000);
    memset(send_cmd, 0, 100);
    strcpy(send_cmd, "data complete\n");
    write(client_sockfd, send_cmd, 100);
    memset(send_cmd, 0, 100);
    strcpy(send_cmd, "226 Transfer complete.\n");
    write(client_sockfd, send_cmd, 100);
    close(data_client_sockfd);
    close(data_server_sockfd);
}

int rest()
{
    dataconnect();
    FILE *fp;
    while (1)
    {
        memset(read_buf, 0, 100);
        read(client_sockfd, read_buf, 100); //得到文件名
        fp = fopen(read_buf, "r");
        if (fp == NULL)
        {
            memset(send_cmd, 0, 100);
            strcpy(send_cmd, "550 Failed to open file.\n");
            write(client_sockfd, send_cmd, 100);
        }
        else
        {
            memset(send_cmd, 0, 100);
            strcpy(send_cmd, "150 Ok to send data.\n");
            write(client_sockfd, send_cmd, 100);
            break;
        }
    }
    int num;
    memset(read_buf,0,100);
    read(client_sockfd,read_buf,100);
    num=atoi(read_buf);
    printf("lby:%d\n",num);
    fseek(fp,num,SEEK_SET);
    printf("mark\n");
    for (int i = 0; i < 10000; i++)
    {
        fscanf(fp, "%c", &send_file[i]);
    }
    write(data_client_sockfd, send_file, 10000);
    memset(send_cmd, 0, 100);
    strcpy(send_cmd, "data complete\n");
    write(client_sockfd, send_cmd, 100);
    memset(send_cmd, 0, 100);
    strcpy(send_cmd, "226 Transfer complete.\n");
    write(client_sockfd, send_cmd, 100);
    close(data_client_sockfd);
    close(data_server_sockfd);
}


int upload()
{
    dataconnect();
    FILE *fp;
    while (1)
    {
        memset(read_buf, 0, 100);
        read(client_sockfd, read_buf, 100); //得到文件名
        fp = fopen(read_buf, "w+");
        if (fp == NULL)
        {
            memset(send_cmd, 0, 100);
            strcpy(send_cmd, "550 Failed to open file.\n");
            write(client_sockfd, send_cmd, 100);
        }
        else
        {
            memset(send_cmd, 0, 100);
            strcpy(send_cmd, "150 Ok to send data.\n");
            write(client_sockfd, send_cmd, 100);
            break;
        }
	}
	memset(read_buf,0,100);
	read(client_sockfd,read_buf,100);
	printf("%s\n",read_buf);
	memset(read_file,0,10000);
        read(data_client_sockfd, read_file, 10000);
	//printf("mark\n");
      //  printf("%s\n\n",read_file);//////////////////
        fprintf(fp, read_file);
        fclose(fp);
        memset(send_cmd, 0, 100);
        strcpy(send_cmd, "226 Transfer complete.\n");
        write(client_sockfd, send_cmd, 100);
        close(data_client_sockfd);
    
}

int fun1(char *dir, char *filename)
{

    struct stat buf;
    char out[100];
    if (stat(dir, &buf) < 0)
    {
        perror("stat");
        return (-1);
    }
    switch (buf.st_mode & S_IFMT) //判断文件类型
    {
    case S_IFREG:
        fseek(stream,0,SEEK_END);
        fprintf(stream, "-");
        break;
    case S_IFDIR:
        fseek(stream,0,SEEK_END);
        fprintf(stream, "d");
        break;
    case S_IFCHR:
        fseek(stream,0,SEEK_END);
        fprintf(stream, "c");
        break;
    case S_IFBLK:
        fseek(stream,0,SEEK_END);
        fprintf(stream, "b");
        break;
    case S_IFIFO:
        fseek(stream,0,SEEK_END);
        fprintf(stream, "p");
        break;
    case S_IFLNK:
        fseek(stream,0,SEEK_END);
        fprintf(stream, "l");
        break;
    case S_IFSOCK:
        fseek(stream,0,SEEK_END);
        fprintf(stream, "s");
        break;
    }

    int n;
    for (n = 8; n >= 0; n--)
    {
        if (buf.st_mode & (1 << n)) //移位，得出权限
        {
            switch (n % 3)
            {
            case 2:
                fseek(stream,0,SEEK_END);
                fprintf(stream, "r");
                break;
            case 1:
                fseek(stream,0,SEEK_END);
                fprintf(stream, "w");
                break;
            case 0:
                fseek(stream,0,SEEK_END);
                fprintf(stream, "x");
                break;
            default:
                break;
            }
        }
        else
        {
            fseek(stream,0,SEEK_END);
            fprintf(stream, "-");
        }
    }

    fseek(stream,0,SEEK_END);
    fprintf(stream, " %d", buf.st_nlink);
    //printf(" %d",buf.st_nlink);  //硬连接数
    fseek(stream,0,SEEK_END);
    fprintf(stream, " %ld", buf.st_size);
    //printf(" %ld",buf.st_size);  //文件大小

    struct tm *t;
    t = localtime(&buf.st_atime);
    fseek(stream,0,SEEK_END);
    fprintf(stream, " %d-%d-%d %d:%d", t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min);
    fseek(stream,0,SEEK_END);
    fprintf(stream," %s ", filename); //文件名

    if (S_ISLNK(buf.st_mode)) //如果是连接 则用file1->file2表示
    {
        fseek(stream,0,SEEK_END);
        fprintf(stream," -> ");
        if (readlink(filename, out, 100) == -1)
        {
        }
        fseek(stream,0,SEEK_END);
        printf(stream,"%s", out);
    }
    fseek(stream,0,SEEK_END);
    fprintf(stream,"\n");
}

int ls()
{

    dataconnect();
    memset(send_cmd,0,100);
    strcpy(send_cmd,"150 Here comes the directory listing.\n");
    write(client_sockfd,send_cmd,strlen(send_cmd));
    char w[100]; //目录
    stream = fopen("dir_stream", "w+"); //存目录内容
    memset(w, 0, 100);
    strcpy(w, "./");
    struct stat buf;
    char name[100];
    if (stat(w, &buf) < 0) //stat执行失败，返回-1
    {
        fprintf(stderr, "stat error:%s\n", strerror(errno));
        return -1;
    }
    DIR *dir;
    dir = opendir(w);
    struct dirent *pdr;
    while ((pdr = readdir(dir)) != NULL)
    {
        if (pdr->d_name[0] == '.')
        {
        }
        else
        {
            //printf("dir:%s\n",pdr->d_name);
            memset(name, 0, 100);
            strcpy(name, w);
            //  strcat(name,"/");
            strcat(name, pdr->d_name);
            fun1(name, pdr->d_name);
        }
    }
    FILE *stream=fopen("dir_stream","r");
    fseek(stream,0,SEEK_SET);
    memset(send_file,0,10000);
        for (int i = 0; i < 10000; i++)
    {
        fscanf(stream, "%c", &send_file[i]);
    }
        write(data_client_sockfd, send_file, strlen(send_file));
	memset(send_cmd,0,100);
      //  strcpy(send_cmd,"send\n");
        write(client_sockfd,send_cmd,100);
    //printf("%s\n", send_file);
    fclose(stream);
    close(data_client_sockfd);
    memset(send_cmd,0,100);
    strcpy(send_cmd,"226 Directory send OK.\n");
    write(client_sockfd,send_cmd,100);
    close(stream);
}

int quit()
{
    memset(send_cmd,0,100);
    strcpy(send_cmd,"221 Goodbye.\n");
    write(client_sockfd,send_cmd,100);
    sleep(1);
    close(client_sockfd);
    close(server_sockfd);
}

void *thread_function(int *arg)
{
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0); //套接口文件描述符
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); //类型转换（长整数型）
    server_address.sin_port = htons(9998);
    server_len = sizeof(server_address);
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
    listen(server_sockfd, 5); //允许5个连接
    printf("server waiting\n");
    client_len = sizeof(client_address);
    client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, &client_len);
    write(client_sockfd, "220 vsFTPd\n", strlen("220 vsFTPd\n"));
    while (1)
    {
        memset(read_buf, 0, 100);
        read(client_sockfd, read_buf, 100);
        printf("%s\n", read_buf);
        if (strcmp(read_buf, "USER beryl") != 0)
        {
            strcpy(send_cmd, "530 Please login with USER and PASS.\n");
            write(client_sockfd, send_cmd, strlen(send_cmd));
            memset(send_cmd, 0, 100);
        }
        else
        {
            strcpy(send_cmd, "331 Please specify the password.\n");
            write(client_sockfd, send_cmd, strlen(send_cmd));            memset(send_cmd, 0, 100);
            memset(read_buf, 0, 100);
            read(client_sockfd, read_buf, 100);
            if (strcmp(read_buf, "PASS 4650") == 0)
            {
                strcpy(send_cmd, "230 Login successful.\n");
                write(client_sockfd, send_cmd, strlen(send_cmd));
                memset(send_cmd, 0, 100);
                break;
            }
            else
            {
                strcpy(send_cmd, "530 Login incorrect\n");
                write(client_sockfd, send_cmd, strlen(send_cmd));
                memset(send_cmd, 0, 100);
                continue;
            }
        }
    }
    while(1)
    {
    memset(read_buf,0,100);
    read(client_sockfd,read_buf,100);
    printf("%s\n",read_buf);
    memset(send_cmd,0,100);
    strcpy(send_cmd,"already read\n");//否则易出现retr、pasv一起接受的情况
    write(client_sockfd,send_cmd,100);
        if (strcmp(read_buf, "RETR") == 0)
        {
            download();
        }
        if (strcmp(read_buf, "STOR") == 0)
        {
            upload();
        }
        if (strcmp(read_buf, "LIST") == 0)
        {
            ls();
        }
        if(strcmp(read_buf,"REST")==0)
	{
	    rest(); 
	}
        if (strcmp(read_buf, "QUIT") == 0)
        {
            quit();
            break;
        }
    }
}

int main()
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
