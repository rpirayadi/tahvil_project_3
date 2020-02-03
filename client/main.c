#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#define MAX 80
#define PORT 12345
#define SA struct sockaddr
#include "ansi_escapes.c"
#include "My_cJSON.c"

#define max_asami 200
#define max_string 10000
#define max_token 200
#define max_message 5000
int socket_maker();
void account_menu();
void main_menu(char []);
void chat_menu(char []);
void dash ();
void ce();
void set_color(int);
int choice_getter(char [][20], int , char[] );
int main()
{
    int client_socket = socket_maker();
    closesocket(client_socket);
    set_color(0);
    ce();
    account_menu();

}
int socket_maker ()
{
    int client_socket, server_socket;
	struct sockaddr_in servaddr, cli;

	WORD wVersionRequested;
    WSADATA wsaData;
    int err;

	// Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }

	// Create and verify socket
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		printf("Socket creation failed...\n");
		exit(0);
	}

	// Assign IP and port
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	// Connect the client socket to server socket
	if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Connection to the server failed...\n");
		exit(0);
	}
    return client_socket;

}


void account_menu ()
{

    int a_menu=0;
    char choices [][20] = { "Register" , "login" , "Exit" };
    a_menu = choice_getter(choices,3 , "Account menu");
    int client_socket = socket_maker();
    switch(a_menu)
    {
    case 1:
    {
        printf("Welcome!!! \nYou just need to type a username and a password to register \n");
        char username [max_asami] , password [max_asami] ,result [2*max_asami]= "register ";
        printf("Enter your username please:\n");
        scanf("%s", username);
        printf("Enter your password please:\n");
        scanf("%s", password);

        strcat(result , username);
        strcat(result , ", ");
        strcat(result , password);
        strcat(result , "\n");
        send(client_socket,result, sizeof(result), 0);

        char string[max_string];
        recv(client_socket ,string , sizeof(string), 0);

        cJSON * json = cJSON_CreateObject();
        json = cJSON_Parse(string);

        char state [20] , content[50];
        strcpy(state,cJSON_GetObjectItem( json , "type")->valuestring);
        strcpy(content ,cJSON_GetObjectItem( json , "content")->valuestring);

        if(strcasecmp(state , "Error") == 0)
            printf("%s\n", content);
        else
            printf("Your Account Was Succesfully Created\n");

        break;
    }
    case 2:
    {
        char username [max_asami] , password [max_asami] ,result [2*max_asami] = "login ";
        printf("Enter your username please:\n");
        scanf("%s", username);
        printf("Enter your password please:\n");
        scanf("%s", password);

        strcat(result , username);
        strcat(result , ", ");
        strcat(result , password);
        strcat(result , "\n");

        send(client_socket,result, sizeof(result), 0);

        char string[max_string];
        recv(client_socket ,string , sizeof(string), 0);

        cJSON * json = cJSON_CreateObject();
        json = cJSON_Parse(string);

        char state [20] , token[max_token];
        strcpy(state,cJSON_GetObjectItem( json , "type")->valuestring);
        strcpy(token,cJSON_GetObjectItem( json , "content")->valuestring);

        if(strcmp(state , "Error")== 0)
            printf("%s\n", token);
        else
        {
            main_menu(token);
            return;
        }
        break;


    }
    case 3:
    {
        printf("hope to see you again!\n");
        closesocket(client_socket);
        return;
    }

    default:
        printf("Invalid command\n");
    }
    printf("press any key to continue\n");
    getch();
    closesocket(client_socket);
    account_menu();

}

void main_menu (char token[])
{
    int m_menu;
    char choices [][20] = { "Create Channel" , "Join Channel","Logout"};
    m_menu = choice_getter(choices, 3, "Main Menu");
    int client_socket = socket_maker();
    switch(m_menu)
    {
    case 1:
    {
        char channel_name [max_asami] ,result [max_asami]= "create channel ";
        printf("Enter the channel's name please:\n");
        scanf("%s", channel_name);

        strcat(result , channel_name);
        strcat(result , ", ");
        strcat(result , token);
        strcat(result , "\n");
        send(client_socket,result, sizeof(result), 0);

        char string[max_string];
        recv(client_socket ,string , sizeof(string), 0);
        cJSON * json = cJSON_CreateObject();
        json = cJSON_Parse(string);
        char state [20] , content[50];
        strcpy(state,cJSON_GetObjectItem( json , "type")->valuestring);
        strcpy(content ,cJSON_GetObjectItem( json , "content")->valuestring);
        if(strcasecmp(state , "Error") == 0)
            printf("%s\n", content);
        else
        {
            printf("Channel Was Succesfully Created\n");
            chat_menu(token);
            return;
        }

        break;

    }
    case 2:
    {
        char channel_name [max_asami] ,result [max_asami]= "join channel ";
        printf("Enter the channel's name please:\n");
        scanf("%s", channel_name);

        strcat(result , channel_name);
        strcat(result , ", ");
        strcat(result , token);
        strcat(result , "\n");
        send(client_socket,result, sizeof(result), 0);

        char string[max_string];
        recv(client_socket ,string , sizeof(string), 0);
        cJSON * json = cJSON_CreateObject();
        json = cJSON_Parse(string);
        char state [20] , content[50];
        strcpy(state,cJSON_GetObjectItem( json , "type")->valuestring);
        strcpy(content ,cJSON_GetObjectItem( json , "content")->valuestring);
        if(strcasecmp(state , "Error") == 0)
            printf("%s\n", content);
        else
        {
            printf("You have Succesfully joined to %s\n",channel_name);
            chat_menu(token);
            return;
        }

        break;
    }
    case 3:
    {
        char result [max_asami]= "logout ";
        strcat(result , token);
        strcat(result , "\n");
        send(client_socket,result, sizeof(result), 0);
        account_menu();
        return;
        break;
    }
    }
    printf("press any key to continue\n");
    getch();
    closesocket(client_socket);
    main_menu(token);
}
void chat_menu(char token[])
{
    int c_menu;
    char choices [][20] = {"Send Message", "Refresh", "Channel Members","Search Members","Search Messages", "Leave Channel"};
    c_menu = choice_getter(choices, 6 , "Chat Menu");
    int client_socket = socket_maker();
    switch(c_menu)
    {
    case 1:
    {
        char message[max_message] ,result [max_message+40]= "send ";
        printf("Enter your message:\n");
        scanf(" %[^\n]", message);
        strcat(result , message);
        strcat(result , ", ");
        strcat(result , token);
        strcat(result , "\n");
        send(client_socket,result, sizeof(result), 0);
        break;

    }
    case 2:
    {
        char result [max_token]= "refresh ";
        strcat(result ,token);
        strcat(result , "\n");
        send(client_socket,result, sizeof(result), 0);

        char string [max_string];
        recv(client_socket ,string , sizeof(string), 0);

        //printf("gerefte : %s" , string);
        cJSON * json = cJSON_CreateObject();
        json = cJSON_Parse(string);
        char state [20] ,sender [max_asami] , message[max_message];
        strcpy(state,cJSON_GetObjectItem( json , "type")->valuestring);
        if(strcmp(state, "Error") == 0 )
            printf("%s" , cJSON_GetObjectItem(json , "content"));
        else
        {
            cJSON * array = cJSON_GetObjectItem( json , "content");

            for (int i =0 ;i <cJSON_GetArraySize(array) ; i++)
            {
                cJSON * arrayitem = cJSON_GetArrayItem(array , i);
                strcpy(sender , cJSON_GetObjectItem(arrayitem , "sender")->valuestring);
                strcpy(message, cJSON_GetObjectItem(arrayitem , "content")->valuestring);
                printf("%s : %s\n",sender, message);
            }
        }
        break;
    }
    case 3:
    {
        char result [max_token]= "channel members ";
        strcat(result ,token);
        strcat(result , "\n");
        send(client_socket,result, sizeof(result), 0);

        char string [max_string];
        recv(client_socket ,string , sizeof(string), 0);

        cJSON * json = cJSON_CreateObject();
        json = cJSON_Parse(string);
        char state [20] ;
        strcpy(state,cJSON_GetObjectItem( json , "type")->valuestring);
        if(strcmp(state, "Error") == 0)
        {
            printf("%s", cJSON_GetObjectItem(json , "content")->valuestring);
        }
        cJSON * members =cJSON_GetObjectItem(json , "content");
        printf("Channel Members: ");
        for (int i=0 ; i< cJSON_GetArraySize(members)-1 ; i++)
        {
            printf("%s ," , cJSON_GetArrayItem( members , i)->child->valuestring);
        }
        printf("%s " , cJSON_GetArrayItem( members , cJSON_GetArraySize(members)-1)->child->valuestring);
        printf("\n");
        break;
    }
    case 4:
    {
        char result [max_token]= "find " , username[max_asami] ;
        printf("Enter the username you want to search:\n");
        scanf("%s", username);
        strcat(result ,username);
        strcat(result ,", ");
        strcat(result ,token);
        strcat(result , "\n");
        send(client_socket,result, sizeof(result), 0);

        char string[max_string];
        recv(client_socket ,string , sizeof(string), 0);
        cJSON * json = cJSON_CreateObject();
        json = cJSON_Parse(string);
        char state [20] , content[30];
        strcpy(state,cJSON_GetObjectItem( json , "type")->valuestring);
        strcpy(content ,cJSON_GetObjectItem( json , "content")->valuestring);
        if(strcasecmp(state , "Error") == 0)
            printf("%s\n", content);
        else
        {
            if(strcmp(content , "True" ) == 0 )
                printf("This user is in the channel\n");
            else
                printf("User not found in the channel\n");

        }
        break ;
    }
    case 5:
    {
        char result [max_token]= "search " , kalame[40] ;
        printf("Enter the word you want to search:\n");
        scanf("%s" , kalame);
        strcat(result , kalame);
        strcat(result ,", ");
        strcat(result ,token);
        strcat(result , "\n");
        send(client_socket,result, sizeof(result), 0);

               char string [max_string];
        recv(client_socket ,string , sizeof(string), 0);

        cJSON * json = cJSON_CreateObject();
        json = cJSON_Parse(string);
        char state [20] ,sender [max_asami] , message[max_message];
        strcpy(state,cJSON_GetObjectItem( json , "type")->valuestring);
        if(strcmp(state, "Error") == 0 )
            printf("%s" , cJSON_GetObjectItem(json , "content"));
        else
        {
            cJSON * array = cJSON_GetObjectItem( json , "content");

            for (int i =0 ;i <cJSON_GetArraySize(array) ; i++)
            {
                cJSON * arrayitem = cJSON_GetArrayItem(array , i);
                strcpy(sender , cJSON_GetObjectItem(arrayitem , "sender")->valuestring);
                strcpy(message, cJSON_GetObjectItem(arrayitem , "content")->valuestring);
                printf("%s : %s\n",sender, message);
            }
        }
        break;

    }
    case 6:
    {
        char result [max_token]= "leave ";
        strcat(result ,token);
        strcat(result , "\n");
        send(client_socket,result, sizeof(result), 0);
        main_menu(token);
        return;
    }
    }
    printf("press any key to continue\n");
    getch();
    closesocket(client_socket);
    chat_menu(token);
}
void dash()
{
    printf("-----------------------------------------\n");
}
void ce()
{

    printf("        /\\\\\\\\\\\\\\\\\\  /\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      \n");
    printf("       /\\\\\\//////// \/\\\\\\///////////       \n");
    printf("      /\\\\\\/          \/\\\\\\                 \n");
    printf("     /\\\\\\             \/\\\\\\\\\\\\\\\\\\\\\\\\\\\\     \n");
    printf("     \\/\\\\\\             \/\\\\\\//////////     \n");
    printf("      \\//\\\\\\            \/\\\\\\              \n");
    printf("        \\///\\\\\\          \/\\\\\\             \n");
    printf("           \\////\\\\\\\\\\\\\\\\\\ \/\\\\\\\\\\\\\\\\\\\\\\\\   \n");
    printf("              \\/////////  \/////////////   \n");

}
void set_color(int type)
{
    if(type == 0 )
    {
        setupConsole();
        printf("\x1b[0m\x1b[45m");
    }
    if(type == 1)
    {
        setupConsole();
        printf("\x1b[0m\x1b[46m");
    }
}
int choice_getter(char choices[][20], int tedad , char title[])
{

    char key , key2;
    int currentlyselecteditem = 1;

    ///changing the place of  \0
    for(int i = 0 ; i< tedad ;i++)
    {
        for(int j= strlen(choices[i]) ; j< 19 ; j++)
            choices[i][j] = ' ';
        choices[i][19] = '\0';
    }
    choices[currentlyselecteditem-1][18] = '-';
    choices[currentlyselecteditem-1][17] = '-';
    choices[currentlyselecteditem-1][16] = '<';

    while(key != ' ')
    {
        system("cls");

        set_color(1);
        ce();
        set_color(0);
        dash();
        printf("%s \n", title);
        for(int i = 0 ;i< tedad ;i++)
            printf("%.20s\n", choices[i]);
        printf("press space key to confirm...\n");
        dash();
        key = getch();
        key2 = 0;
        if(key == -32)
        {
        key2 = getch();
        }
        else
        {
            continue;
        }
        if(key2 == 80)
        {
            currentlyselecteditem++;
        }
        else if(key2 == 72)
        {
            currentlyselecteditem--;
        }

        //make sure the selected item stays in range
        if(currentlyselecteditem < 1)
            currentlyselecteditem = 1;
        if(currentlyselecteditem > tedad)
            currentlyselecteditem = tedad;

        for(int i =0 ; i< tedad ; i++)
            for(int j= 18 ; j>=16 ; j--)
            {
                choices[i][j] = ' ';
            }
        choices[currentlyselecteditem-1][18] = '-';
        choices[currentlyselecteditem-1][17] = '-';
        choices[currentlyselecteditem-1][16] = '<';

    }
    system("cls");
    set_color(1);
    ce();
    set_color(0);
    return currentlyselecteditem;
}
