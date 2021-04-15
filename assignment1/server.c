// Server side C/C++ program to demonstrate Socket programming
#include <errno.h>
#include <netinet/in.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

int main(int argc, char const *argv[])
{
  int server_fd, new_socket, valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[102] = {0};
  char *hello = "Hello from server";
  
  printf("execve=0x%p\n", execve);

  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Forcefully attaching socket to the port 8080
 if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
        &opt, sizeof(opt)))
 /* if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
        &opt, sizeof(opt)))*/
  {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( PORT );

  // Forcefully attaching socket to the port 8080
  if (bind(server_fd, (struct sockaddr *)&address,
        sizeof(address))<0)
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 3) < 0)
  {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
          (socklen_t*)&addrlen))<0)
  {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  // fork master process here
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);  
  }

  if (pid > 0) {
    //printf("Parent process running...\n");
    printf("Parent process - uid : %d, euid : %d\n", getuid(), geteuid());
    int status = 0;
    // Wait for child process to complete.
    while ((wait(&status)) > 0);
    printf("Parent process done.\n");
  } else {
    // printf("Child process running...\n");
    //Drop privilege to the user named 'nobody'.
    struct passwd* nobody_user = getpwnam("nobody");
    if (nobody_user == NULL) {
      perror("user doesn't exist");
      exit(EXIT_FAILURE);  
    }
    printf("Dropping privilege of child\n");
    printf("Setting privilege level to user named 'nobody'\n"); 
    if(setuid(nobody_user->pw_uid) < 0){
      perror("privilege drop");
      exit(EXIT_FAILURE);  
    }
    printf("Child process - uid : %d, euid : %d\n", getuid(), geteuid());
    valread = read( new_socket , buffer, 1024);
    printf("%s\n",buffer );
    send(new_socket , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    printf("Child process done.\n");
  }
  return 0;
}
