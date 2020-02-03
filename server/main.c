#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "My_cJSON.c"
#include "ansi_escapes.c"
#define MAX 80
#define PORT 12345
#define SA struct sockaddr

#define max_username 50
#define max_password 50
#define max_channel_name 70
#define max_string_username 100
#define max_string_channel 800
#define max_members 30
#define max_online 10
#define max_token 33
#define max_string_from_client 150
#define max_message 300
#define normal_respond 200
#define huge_respond 800


typedef struct
{
    char username [max_username]  ;
    char token [max_token]  ;
    char channel [max_channel_name]  ;
    int index_message ;
}user ;

int server_socket, client_socket , where_to_write = 0 ;
user online_users [max_online] ;

void ce ();
void socket_maker();
int find_token(char []);
char * tokenmaker ();
void send_respond(cJSON * , int);
int is_word_in_message (char [] , char[]);
int main()
{
    ce();
    mkdir("./resources");
    mkdir("./resources/Users");
    mkdir("./resources/Channels");

    socket_maker();
    closesocket(server_socket);



    char string_from_client[max_string_from_client]="" , command [max_string_from_client] ;
    while(1)
    {
        socket_maker();

        recv(client_socket, string_from_client ,  sizeof(string_from_client), 0);
        char * string_pointer = string_from_client;
        sscanf(string_pointer,"%s", command);
        string_pointer += strlen(command)+1;

        if(strncmp(command,"register", 8) == 0)
        {
            char username [max_username] , address[100] = "./resources/Users/" , password[max_password]="";
            cJSON * respond = cJSON_CreateObject();

            sscanf(string_pointer,"%s", username);
            string_pointer += strlen(username) + 1;
            sscanf(string_pointer,"%s", password);
            username[strlen(username)-1] = '\0'; /// deleting the ',' character
            strcat(address, username);
            strcat(address, ".user.json");



            ///searching whether we have this username or not
            FILE * test= fopen(address, "r");

            if(test != NULL)
            {
                cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                cJSON_AddItemToObject(respond, "content",cJSON_CreateString("this username is not available"));

            }

            else
            {
                FILE * fptr= fopen(address, "w");
                cJSON * json = cJSON_CreateObject();
                cJSON_AddItemToObject(json, "username",cJSON_CreateString(username));
                cJSON_AddItemToObject(json, "password",cJSON_CreateString(password));
                fprintf(fptr,cJSON_Print(json));
                fclose(fptr);
                cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Successful"));
                cJSON_AddItemToObject(respond, "content",cJSON_CreateString(""));

            }
            fclose(test);
            ///send respond
            send_respond(respond, normal_respond);


        }
        if(strncmp(command,"login", 5) == 0)
        {
            char username [max_username] , address[100] = "./resources/Users/" , password[max_password]="";
            cJSON * respond = cJSON_CreateObject();

            sscanf(string_pointer,"%s", username);
            string_pointer += strlen(username) + 1;
            sscanf(string_pointer,"%s", password);
            username[strlen(username)-1] = '\0'; /// deleting the ',' character
            strcat(address, username);
            strcat(address, ".user.json");

            int flag = 0 ;
            for(int i = 0 ; i< where_to_write ;i++)
                if(strcmp(username , online_users[i].username) == 0 )
                    flag= 1;
            if(flag)
            {
                cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                cJSON_AddItemToObject(respond, "content",cJSON_CreateString("This User has already Logged In."));
            }
            else
            {
                FILE * fptr= fopen(address, "r");
                if(fptr == NULL)
                {
                    cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                    cJSON_AddItemToObject(respond, "content",cJSON_CreateString("Username is not valid."));
                }
                else
                {

                    char string [max_string_username];
                    fscanf(fptr,"%[^EOF]",string);
                    cJSON * json = cJSON_CreateObject();
                    json = cJSON_Parse(string);

                    if(strcmp(password, cJSON_GetObjectItem(json ,"password")->valuestring) != 0)
                    {
                        cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                        cJSON_AddItemToObject(respond, "content",cJSON_CreateString("Wrong password."));
                    }
                    else
                    {
                        char token[max_token];
                        do
                        {
                            strcpy(token, tokenmaker());
                        }while (find_token(token) != -1);

                        cJSON_AddItemToObject(respond, "type",cJSON_CreateString("AuthToken"));
                        cJSON_AddItemToObject(respond, "content",cJSON_CreateString(token));


                        strcpy(online_users[where_to_write].username , username);
                        strcpy(online_users[where_to_write].token , token);
                        online_users[where_to_write].index_message = 0 ;
                        strcpy(online_users[where_to_write].channel , "" );
                        where_to_write ++ ;
                    }

                }
            }
            ///send respond
            send_respond(respond , normal_respond);
        }
        if(strncmp(command,"create", 6) == 0)
        {
            sscanf(string_pointer,"%s", command );
            string_pointer += strlen(command)+1;
            if(strncmp(command,"channel", 7) == 0)
            {
                char channel_name [100] , token [32] ,address[100] = "./resources/Channels/";
                cJSON * respond = cJSON_CreateObject();

                sscanf(string_pointer,"%s", channel_name);
                string_pointer += strlen(channel_name) + 1;
                sscanf(string_pointer,"%s", token);
                channel_name[strlen(channel_name)-1] = '\0'; /// deleting the ',' character
                strcat(address, channel_name);
                strcat(address, ".channel.json");

                if(find_token(token) == -1)
                {
                    cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                    cJSON_AddItemToObject(respond, "content",cJSON_CreateString("Invalid Authentication token."));
                }

                else
                {
                    ///searching whether we have this channel or not
                    FILE * test= fopen(address, "r");

                    if(test != NULL)
                    {
                        cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                        cJSON_AddItemToObject(respond, "content",cJSON_CreateString("Channel name is not available."));

                    }
                    else
                    {
                        strcpy(online_users[find_token(token)].channel , channel_name);
                        FILE * fptr= fopen(address, "w");
                        cJSON * json = cJSON_CreateObject();
                        cJSON * avalin_payam = cJSON_CreateObject();
                        cJSON * array = cJSON_CreateArray();
                        char creation_message [100] = "";

                        strcat(creation_message,online_users[find_token(token)].username);
                        strcat(creation_message," created ");
                        strcat(creation_message,channel_name);
                        cJSON_AddItemToObject(avalin_payam, "sender",cJSON_CreateString("server"));
                        cJSON_AddItemToObject(avalin_payam, "content",cJSON_CreateString(creation_message));
                        //printf("%s", cJSON_Print(avalin_payam));

                        cJSON_AddItemToArray(array,avalin_payam );
                        //printf("%s", cJSON_Print(array));
                        cJSON_AddItemToObject(json, "messages",array);
                        cJSON_AddItemToObject(json, "name",cJSON_CreateString(channel_name));
                        fprintf(fptr,cJSON_Print(json));
                        fclose(fptr);
                        cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Successful"));
                        cJSON_AddItemToObject(respond, "content",cJSON_CreateString(""));
                    }
                    fclose(test);
                }
                ///send respond
                send_respond(respond , normal_respond);
            }

        }
        if(strncmp(command,"join", 4) == 0)
        {
            sscanf(string_pointer,"%s", command );
            string_pointer += strlen(command)+1;
            if(strncmp(command,"channel", 6) == 0)
            {
                char channel_name [max_channel_name] , token [max_token] ,address[100] = "./resources/Channels/" , string [max_string_channel];
                cJSON * respond = cJSON_CreateObject();

                sscanf(string_pointer,"%s", channel_name);
                string_pointer += strlen(channel_name) + 1;
                sscanf(string_pointer,"%s", token);
                channel_name[strlen(channel_name)-1] = '\0'; /// deleting the ',' character
                strcat(address, channel_name);
                strcat(address, ".channel.json");

                if(find_token(token) == -1)
                {
                    cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                    cJSON_AddItemToObject(respond, "content",cJSON_CreateString("Invalid Authentication token."));
                }
                else
                {
                    ///searching whether we have this channel or not
                    FILE * test= fopen(address, "r");

                    if(test == NULL)
                    {
                        cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                        cJSON_AddItemToObject(respond, "content",cJSON_CreateString("Channel not found."));
                        fclose(test);

                    }
                    else
                    {


                        strcpy(online_users[find_token(token)].channel , channel_name);
                        online_users[find_token(token)].index_message = 0;

                        fscanf(test,"%[^EOF]",string);
                        fclose(test);

                        cJSON * json = cJSON_CreateObject();
                        json = cJSON_Parse(string);
                        cJSON * array = cJSON_GetObjectItem(json,"messages");


                        cJSON * joining_payam = cJSON_CreateObject();
                        char joining_message [100] = "";
                        strcat(joining_message,online_users[find_token(token)].username);
                        strcat(joining_message," joined the channel.");
                        cJSON_AddItemToObject(joining_payam, "sender",cJSON_CreateString("server"));
                        cJSON_AddItemToObject(joining_payam, "content",cJSON_CreateString(joining_message));

                        cJSON_AddItemToArray(array, joining_payam);

                        FILE * fptr= fopen(address, "w");
                        cJSON * json2 = cJSON_CreateObject();
                        cJSON_AddItemToObject(json2, "messages",array);
                        cJSON_AddItemToObject(json2, "name",cJSON_CreateString(channel_name));
                        fprintf(fptr,cJSON_Print(json2));
                        fclose(fptr);

                        cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Successful"));
                        cJSON_AddItemToObject(respond, "content",cJSON_CreateString(""));
                    }

                }
                ///send respond
                send_respond(respond, normal_respond);
            }
        }
        if(strncmp(command,"logout", 6) == 0)
        {
            char token [max_token];
            cJSON * respond = cJSON_CreateObject();

            sscanf(string_pointer,"%s", token);
            if(find_token(token) == -1)
            {
                cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                cJSON_AddItemToObject(respond, "content",cJSON_CreateString("Invalid Authentication token."));
            }
            else
            {
                strcpy(online_users[find_token(token)].channel , "");
                strcpy(online_users[find_token(token)].username , "");
                strcpy(online_users[find_token(token)].token , "");
                cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Successful"));
                cJSON_AddItemToObject(respond, "content",cJSON_CreateString(""));

            }
            send_respond(respond , normal_respond);
        }
        if(strncmp(command,"send", 4) == 0)
        {
            char message [max_message] , token [max_token] ,address[100] = "./resources/Channels/",string [max_string_channel];
            cJSON * respond = cJSON_CreateObject();

            sscanf(string_pointer,"%[^,]", message);
            string_pointer += strlen(message) + 1;
            sscanf(string_pointer,"%s", token);


            if(find_token(token) == -1)
            {
                cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                cJSON_AddItemToObject(respond, "content",cJSON_CreateString("Invalid Authentication token."));
            }
            else
            {

                if(strcmp(online_users[find_token(token)].channel , "") == 0)
                {
                    cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                    cJSON_AddItemToObject(respond, "content",cJSON_CreateString("you are not in a channel yet"));
                }
                else
                {
                    strcat(address, online_users[find_token(token)].channel);
                    strcat(address, ".channel.json");

                    FILE * fptr_for_reading = fopen(address, "r");
                    fscanf(fptr_for_reading,"%[^EOF]",string);
                    fclose(fptr_for_reading);

                    cJSON * new_message = cJSON_CreateObject();
                    cJSON_AddItemToObject(new_message, "sender",cJSON_CreateString(online_users[find_token(token)].username));
                    cJSON_AddItemToObject(new_message, "content",cJSON_CreateString(message));

                    cJSON * json = cJSON_CreateObject();
                    cJSON * json2 = cJSON_CreateObject();
                    //cJSON * array = cJSON_CreateArray();

                    json = cJSON_Parse(string);

                    //printf("%s", cJSON_Print(json));
                    cJSON * array = cJSON_GetObjectItem(json,"messages");
                    cJSON_AddItemToArray(array ,new_message);


                    cJSON_AddItemToObject(json2, "messages",array);
                    //printf("%s", cJSON_Print(json2));
                    //cJSON_AddItemToObject(json2, "name",cJSON_CreateString(online_users[find_token(token)].channel));

                    FILE * fptr_for_writing =fopen(address , "w");
                    fprintf(fptr_for_writing,cJSON_Print(json2));
                    fclose(fptr_for_writing);

                    cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Successful"));
                    cJSON_AddItemToObject(respond, "content",cJSON_CreateString(""));
                }
                    ///send respond
                    send_respond(respond, normal_respond);

            }


        }
        if(strncmp(command,"refresh", 7) == 0)
        {
            char token [max_token],address[100] = "./resources/Channels/",string [max_string_channel];
            cJSON * respond = cJSON_CreateObject();

            sscanf(string_pointer,"%s", token);
            if(find_token(token) == -1)
            {
                cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                cJSON_AddItemToObject(respond, "content",cJSON_CreateString("Invalid Authentication token."));
            }
            else
            {

                if(strcmp(online_users[find_token(token)].channel , "") == 0)
                {
                    cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                    cJSON_AddItemToObject(respond, "content",cJSON_CreateString("you are not in a channel yet"));
                }
                else
                {
                    strcat(address, online_users[find_token(token)].channel);
                    strcat(address, ".channel.json");

                    FILE * fptr = fopen(address, "r");
                    fscanf(fptr,"%[^EOF]",string);
                    fclose(fptr);

                    cJSON * json = cJSON_CreateObject();
                    json = cJSON_Parse(string);
                    //printf("anche ma darim %s", cJSON_Print(json));
                    cJSON * messages_to_show = cJSON_CreateArray();

                    cJSON * array = cJSON_GetObjectItem(json,"messages");
                    cJSON_AddItemToArray(messages_to_show , cJSON_GetArrayItem(array , online_users[find_token(token)].index_message));
                    //printf("anch dar arraye %s" , cJSON_Print(messages_to_show));
                    online_users[find_token(token)].index_message = cJSON_GetArraySize(array) ;
                    cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Successful"));
                    cJSON_AddItemToObject(respond, "content",messages_to_show);

                }

            }
            ///send respond
            send_respond(respond , huge_respond);
        }
        if(strncmp(command,"channel", 6) == 0)
        {
            sscanf(string_pointer,"%s", command );
            string_pointer += strlen(command)+1;
            if(strncmp(command,"members", 7) == 0)
            {
                char token [max_token];
                cJSON * respond = cJSON_CreateObject();

                sscanf(string_pointer,"%s", token);
                if(find_token(token) == -1)
                {
                    cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                    cJSON_AddItemToObject(respond, "content",cJSON_CreateString("Invalid Authentication token."));
                }
                else
                {

                    if(strcmp(online_users[find_token(token)].channel , "") == 0)
                    {
                        cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                        cJSON_AddItemToObject(respond, "content",cJSON_CreateString("you are not in a channel yet"));
                    }
                    else
                    {
                        cJSON * members = cJSON_CreateArray();
                        for(int i= 0 ; i< where_to_write ; i++)
                        {
                            if(strcmp(online_users[find_token(token)].channel, online_users[i].channel) == 0)
                            {
                                cJSON * name = cJSON_CreateString(online_users[i].username);
                                strcpy(name->string , "");
                                cJSON_AddItemToArray(members , name);
                            }
                        }
                        cJSON_AddItemToObject(respond, "type",cJSON_CreateString("List"));
                        cJSON_AddItemToObject(respond, "content",members);
                    }
                }
                ///send respond
                send_respond(respond, huge_respond);

            }
        }
        if(strncmp(command,"leave", 5) == 0)
        {
            char  token [max_token] ,address[100] = "./resources/Channels/" , string [max_string_channel];
            cJSON * respond = cJSON_CreateObject();

            sscanf(string_pointer,"%s", token);

            if(find_token(token) == -1)
            {
                cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                cJSON_AddItemToObject(respond, "content",cJSON_CreateString("Invalid Authentication token."));
            }
            else if(strcmp(online_users[find_token(token)].channel , "") == 0)
            {
                    cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                    cJSON_AddItemToObject(respond, "content",cJSON_CreateString("you are not in channel."));
            }
            else
            {

                strcat(address , online_users[find_token(token)].channel);
                strcat(address , ".channel.json");

                FILE * fptr_for_reading= fopen(address, "r");
                fscanf(fptr_for_reading,"%[^EOF]",string);
                fclose(fptr_for_reading);

                cJSON * json = cJSON_CreateObject();
                json = cJSON_Parse(string);
                cJSON * array = cJSON_GetObjectItem(json,"messages");


                cJSON * leaving_payam = cJSON_CreateObject();
                char leaving_message [100] = "";
                strcat(leaving_message,online_users[find_token(token)].username);
                strcat(leaving_message," left the channel.");
                cJSON_AddItemToObject(leaving_payam, "sender",cJSON_CreateString("server"));
                cJSON_AddItemToObject(leaving_payam, "content",cJSON_CreateString(leaving_message));

                cJSON_AddItemToArray(array, leaving_payam);
                FILE * fptr= fopen(address, "w");
                cJSON * json2 = cJSON_CreateObject();
                cJSON_AddItemToObject(json2, "messages",array);
                cJSON_AddItemToObject(json2, "name",cJSON_CreateString(online_users[find_token(token)].channel));
                fprintf(fptr,cJSON_Print(json2));
                fclose(fptr);

                strcpy(online_users[find_token(token)].channel , "");
                cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Successful"));
                cJSON_AddItemToObject(respond, "content",cJSON_CreateString(""));
            }
        ///send respond
        send_respond(respond, normal_respond);
    }
        if(strncmp(command,"find", 4) == 0)
        {
                char token [max_token], username [max_username] ;
                cJSON * respond = cJSON_CreateObject();

                sscanf(string_pointer,"%s", username);
                username[strlen(username)-1] = '\0'; /// deleting the ',' character
                string_pointer += strlen(username) + 1;
                sscanf(string_pointer, "%s", token);
                if(find_token(token) == -1)
                {
                    cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                    cJSON_AddItemToObject(respond, "content",cJSON_CreateString("Invalid Authentication token."));
                }
                else
                {

                    if(strcmp(online_users[find_token(token)].channel , "") == 0)
                    {
                        cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                        cJSON_AddItemToObject(respond, "content",cJSON_CreateString("you are not in a channel yet"));
                    }
                    else
                    {
                        int flag = 0 ;
                        for(int i= 0 ; i< where_to_write ; i++)
                        {
                            if(strcmp(online_users[find_token(token)].channel, online_users[i].channel) == 0
                               && strcmp(username, online_users[i].username)== 0 )
                            {
                                flag = 1;
                            }
                        }
                        cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Successful"));
                        if(flag == 1)
                            cJSON_AddItemToObject(respond, "content",cJSON_CreateString("True"));
                        else
                            cJSON_AddItemToObject(respond, "content",cJSON_CreateString("False"));

                    }
                ///send respond
                send_respond(respond, normal_respond);
                }
        }
        if(strncmp(command,"search", 6) == 0)
        {
            char token [max_token], kalame [40] , address[100] = "./resources/Channels/",string [max_string_channel] ;
            cJSON * respond = cJSON_CreateObject();

            sscanf(string_pointer,"%s", kalame);
            kalame[strlen(kalame)-1] = '\0'; /// deleting the ',' character
            string_pointer += strlen(kalame) + 1;
            sscanf(string_pointer, "%s", token);
            if(find_token(token) == -1)
            {
                cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                cJSON_AddItemToObject(respond, "content",cJSON_CreateString("Invalid Authentication token."));
            }
            else
            {

                if(strcmp(online_users[find_token(token)].channel , "") == 0)
                {
                    cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Error"));
                    cJSON_AddItemToObject(respond, "content",cJSON_CreateString("you are not in a channel yet"));
                }
                else
                {
                    strcat(address, online_users[find_token(token)].channel);
                    strcat(address, ".channel.json");

                    FILE * fptr = fopen(address, "r");
                    fscanf(fptr,"%[^EOF]",string);
                    fclose(fptr);

                    cJSON * json = cJSON_CreateObject();
                    json = cJSON_Parse(string);

                    cJSON * array = cJSON_GetObjectItem(json,"messages");
                    cJSON * result = cJSON_CreateArray();
                    char message[max_message];

                    for (int i=0 ; i< cJSON_GetArraySize(array); i++)
                    {
                        cJSON * array_item = cJSON_GetArrayItem( array , i);
                        strcpy(message, cJSON_GetObjectItem(array_item , "content")->valuestring) ;
                        //printf("%s", message);
                        //printf("%s", kalame);

                        if(is_word_in_message(message , kalame))
                        {
                            cJSON * new_message = cJSON_CreateObject();
                            cJSON_AddItemToObject(new_message, "sender", cJSON_GetObjectItem(array_item , "sender"));
                            cJSON_AddItemToObject(new_message,"content",cJSON_CreateString(message));
                            cJSON_AddItemToArray(result,new_message);
                        }

                    }
                    cJSON_AddItemToObject(respond, "type",cJSON_CreateString("Successful"));
                    cJSON_AddItemToObject(respond, "content",result);
                }
            }
            ///send respond
            send_respond(respond , huge_respond);
        }
        closesocket(server_socket);
    }

    return 0;
}
void ce()
{
    setupConsole();
    printf("\x1b[0m\x1b[46m");
    printf("        /\\\\\\\\\\\\\\\\\\  /\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\      \n");
    printf("       /\\\\\\//////// \/\\\\\\///////////       \n");
    printf("      /\\\\\\/          \/\\\\\\                 \n");
    printf("     /\\\\\\             \/\\\\\\\\\\\\\\\\\\\\\\\\\\\\     \n");
    printf("     \\/\\\\\\             \/\\\\\\//////////     \n");
    printf("      \\//\\\\\\            \/\\\\\\              \n");
    printf("        \\///\\\\\\          \/\\\\\\             \n");
    printf("           \\////\\\\\\\\\\\\\\\\\\ \/\\\\\\\\\\\\\\\\\\\\\\\\   \n");
    printf("              \\/////////  \/////////////   \n");
    restoreConsole();
}
void socket_maker()
{

    struct sockaddr_in server, client;

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
    server_socket = socket(AF_INET, SOCK_STREAM, 6);
	if (server_socket == INVALID_SOCKET)
        wprintf(L"socket function failed with error = %d\n", WSAGetLastError() );
    //else
        //printf("Socket successfully created..\n");

    // Assign IP and port
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    // Bind newly created socket to given IP and verify
    if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0)
    {
        printf("Socket binding failed...\n");
        exit(0);
    }
    //else
        //printf("Socket successfully bound..\n");

    // Now server is ready to listen and verify
    if ((listen(server_socket, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    //else
        //printf("Server listening..\n");

    // Accept the data packet from client and verify
    int len = sizeof(client);

    client_socket = accept(server_socket, (SA *)&client, &len);

    if (client_socket < 0)
    {
        printf("Server acceptance failed...\n");
        exit(0);
    }
    //else
        //printf("Server accepted the client..\n");

}
int find_token(char str[])
{
    for(int i = 0 ; i< where_to_write ; i++)
        if(strcmp(str , online_users[i].token ) == 0 )
        {

            return i ;
        }
    return -1 ;

}
char * tokenmaker()
{
    char * tokenptr = (char *) malloc(max_token * sizeof(char ));
    srand(time(NULL));
    for(int i =0 ; i< max_token-1  ;i++ )
        tokenptr [i] = rand()%96 + 35 ;
    tokenptr[max_token -1 ] = '\0';
    return tokenptr;
}
void send_respond(cJSON * respond , int size)
{
    char string_respond [size] ;
    strcpy(string_respond, cJSON_Print(respond)) ;
    printf("server's response : %s \n", cJSON_Print(respond));
    send(client_socket,string_respond, sizeof(string_respond), 0);
}
int is_word_in_message(char message[], char word [])
{
    char kalame [40];
    char * message_pointer = message;
    while(message_pointer < message + strlen(message))
    {
        sscanf(message_pointer , "%s" ,kalame );
        //printf("%s\n", kalame);
        message_pointer += strlen(kalame) + 1;
        if(strcmp(word , kalame) == 0 )
            return 1;
    }
    return 0 ;
}



