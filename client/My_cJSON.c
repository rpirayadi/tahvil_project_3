#include "My_cJSON.h"

#define max_string 100
#define max_print 1000
cJSON* cJSON_CreateItem()
{
    cJSON * json =  (cJSON * )calloc(1 , sizeof(cJSON));
    json->child = NULL;
    json->next =NULL;
    json->prev = NULL;
    json->type = -1 ;
    json->valuestring = (char *)calloc( max_string , sizeof(char));
    json->string = (char *)calloc(max_string, sizeof(char));
}
cJSON* cJSON_CreateObject()
{
    cJSON * json =  (cJSON * )calloc(1 , sizeof(cJSON));
    json->child = NULL;
    json->next =NULL;
    json->prev = NULL;
    json->type = 0 ;
    json->valuestring = (char *)calloc( max_string , sizeof(char));
    json->string = (char *)calloc(max_string, sizeof(char));
}
cJSON * cJSON_CreateArray()
{
    cJSON * json =  (cJSON * )calloc(1, sizeof(cJSON));
    json->child = NULL;
    json->next =NULL;
    json->prev = NULL;
    json->type = 1 ;
    json->valuestring = (char *)calloc(max_string, sizeof(char));
    json->string = (char *)calloc(max_string, sizeof(char));
}
cJSON* cJSON_CreateString(char * string)
{
    cJSON * new_item = cJSON_CreateItem();
    strcpy(new_item->valuestring, string);
    return new_item;
}
void cJSON_AddItemToObject (cJSON * first , char * name , cJSON * need_to_be_added )
{
    strcpy(need_to_be_added->string , name);
    if(first->child == NULL)
    {
       first->child = need_to_be_added;
       return ;
    }
    cJSON * finder = first->child ;
    while(finder->next != NULL)
    {
        finder = finder->next;
    }

    finder->next = need_to_be_added;
    need_to_be_added ->prev = finder ;
}

char *  cJSON_Print (cJSON * json)
{
    char * result = calloc(sizeof(char) , max_print);

    if(json->type == -1 )
    {
        strcat(result , "{ ");
        strcat(result, "\"");
        strcat(result, json->string);
        strcat(result, "\" : \"");
        strcat(result, json->valuestring);
        strcat(result, "\"");
                strcat(result , " }");
        return result;
    }
    if(json->type == 0 )
    {
        strcat(result , "{ ");
        cJSON * finder = json->child ;
        finder->next;
        while(finder->next != NULL)
        {
            if(finder->type == -1 )
            {
                strcat(result, "\"");
                strcat(result, finder->string);
                strcat(result, "\" : \"");
                strcat(result, finder->valuestring);
                strcat(result, "\"");
                strcat(result, " , ");
            }
            else if(finder->type == 0)
            {
                strcat(result, "\"");
                strcat(result, finder->string);
                strcat(result, "\" : ");
                strcat(result , cJSON_Print(finder));
                strcat(result, " , ");
            }
            else if(finder->type == 1)
            {
                strcat(result, "\"");
                strcat(result, finder->string);
                strcat(result, "\" : ");
                strcat(result , cJSON_PrintArray(finder));
                strcat(result, " , ");

            }

            finder = finder->next;

        }
        if(finder->type == -1 )
        {
                strcat(result, "\"");
                strcat(result, finder->string);
                strcat(result, "\" : \"");
                strcat(result, finder->valuestring);
                strcat(result, "\"");
        }
        else if(finder->type == 0)
        {
            strcat(result, "\"");
            strcat(result, finder->string);
            strcat(result, "\" : ");
            strcat(result , cJSON_Print(finder));
        }
        else
        {
                strcat(result, "\"");
                strcat(result, finder->string);
                strcat(result, "\" : ");
                strcat(result , cJSON_PrintArray(finder));
        }

        strcat(result , " }");
        return result;

    }
    else
    {
        return cJSON_PrintArray(json);
    }
}
char * cJSON_PrintArray(cJSON * json)
{
    char * result = calloc(sizeof(char ) , max_print);
    strcat(result , "[ ");
    if(json->child == NULL)
    {
        strcat(result , " ]");
        return result;
    }

    cJSON * finder = json->child ;
    while(finder->next != NULL)
    {
        strcat(result , cJSON_Print(finder));
        strcat(result, " , ");
        finder = finder->next;

    }

    strcat(result , cJSON_Print(finder));
    strcat(result , " ]");
    return result;
}
char * string_pointer_g ;
cJSON * cJSON_Parse(char * string )
{

    cJSON * first = cJSON_CreateObject();
    cJSON * farzand = cJSON_CreateItem();
    first->child = farzand;
    char * string_pointer = string ;

    string_pointer = put_in_json(string_pointer , farzand);
    cJSON * old_json = cJSON_CreateItem();
    old_json = farzand;

    while(*string_pointer != '}' && *string_pointer != ']')
    {
            cJSON * new_json = cJSON_CreateItem();
                old_json->next = new_json;
            new_json->prev = old_json;
            string_pointer = put_in_json(string_pointer , new_json);

            old_json = new_json;
    }
    string_pointer_g = string_pointer;
    return first;

}
char * put_in_json (char * buffer , cJSON * json)
{
    char content[100] ;
    char * buffer_pointer = buffer+ 3;
    sscanf(buffer_pointer ,"%[^\"]", content);
    strcpy(json ->string ,content);

    buffer_pointer += strlen(content )+4;
    strcpy(content ,"");




    if(*buffer_pointer != '[')
    {
        buffer_pointer ++;
        sscanf(buffer_pointer , "%[^\"]" , content);
        strcpy(json->valuestring ,content);
        buffer_pointer += strlen(content )+2;
    }
    else
    {
        json->type = 1;
        buffer_pointer =cJSON_ParseArray(buffer_pointer ,json) + 2;
    }

    return buffer_pointer;
}
char * cJSON_ParseArray(char *string , cJSON * json)
{
    if(*(string+3) == ']')
        return string + 3;
    cJSON * farzand = cJSON_CreateObject();
    json->child = farzand;
    json->type = 1; /// indicating that json is an array not an object
    char * string_pointer = string ;

    string_pointer += 2;
    farzand = cJSON_Parse(string_pointer);
    json->child = farzand;
    string_pointer = string_pointer_g;

    cJSON * old_json = farzand;

    string_pointer = string_pointer_g ;

    while(*(string_pointer+2) != ']')
    {
        string_pointer += 4;
        cJSON * new_json = cJSON_Parse(string_pointer);
        string_pointer = string_pointer_g ;
        old_json->next = new_json;
        new_json->prev = old_json;
        old_json = new_json;

    }


    /*string_pointer = put_in_json_array(string_pointer , farzand);
    cJSON * old_json = cJSON_CreateObject();
    old_json = farzand;

    while(*string_pointer != ']')
    {
        cJSON * new_json = cJSON_CreateObject();
        string_pointer = put_in_json_array(string_pointer , new_json);
        old_json->next = new_json;
        new_json->prev = old_json;
        old_json = new_json;

    }*/
    return string_pointer+2 ;
}

cJSON * cJSON_GetObjectItem(cJSON * first  , char *string )
{
    cJSON* finder = first->child;
    while(finder != NULL)
    {
        if(strcmp(finder->string , string) == 0 )
            return finder;
        finder = finder->next;
    }
    return NULL;
}
cJSON* cJSON_GetArrayItem (cJSON * array , int n)
{
    cJSON * finder = array->child;
    for(int i =0 ; i< n; i++)
        finder=finder->next;
    return finder;
}
int cJSON_GetArraySize (cJSON * array)
{
    cJSON * finder = array->child;
    int size = 0 ;
    while(finder != NULL)
    {
        finder = finder->next ;
        size ++;
    }
    return size ;

}
void cJSON_AddItemToArray (cJSON * array , cJSON * need_to_be_added )
{
    if(need_to_be_added == NULL )
        return;
    if(array->child == NULL)
    {
       array->child = need_to_be_added;
       return ;
    }
    cJSON * finder = array->child ;
    while(finder->next != NULL)
    {
        finder = finder->next;
    }

    finder->next = need_to_be_added;
    need_to_be_added ->prev = finder ;
}

