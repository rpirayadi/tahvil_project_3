typedef struct cJSON
{
    /* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
    struct cJSON *next;
    struct cJSON *prev;
    /* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
    struct cJSON *child;

    /* The type of the item*/
    /// -1 for item , 0 for object , 1 for array
    int type;

    /* The item's string, if type==cJSON_String  and type == cJSON_Raw */
    char * valuestring;
    /* writing to valueint is DEPRECATED, use cJSON_SetNumberValue instead */

    /* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
    char * string;
} cJSON;
extern cJSON * cJSON_CreateObject ();
extern cJSON * cJSON_CreateArray ();
extern void cJSON_addStringtoObject (cJSON * , char * , char * );
extern char * cJSON_PrintArray (cJSON * );
extern char *  cJSON_Print (cJSON *);
extern  cJSON * cJSON_Parse (char * );
extern  char * cJSON_ParseArray(char * , cJSON *);
extern char * put_in_json (char * , cJSON * );
extern char * put_in_json_array(char * , cJSON * );
extern void cJSON_AddItemToObject (cJSON *  , char *  , cJSON * );
extern cJSON * cJSON_GetObjectItem(cJSON *  , char * );
extern cJSON* cJSON_GetArrayItem (cJSON * , int );
extern int cJSON_GetArraySize (cJSON *);
extern void cJSON_AddItemToArray (cJSON *, cJSON *);

