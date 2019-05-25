/*
 * Model Data module, provides a attraction the data to menage:
 * as part of the Model - view - controller pattern,
 * this is the data management (model)
 *  -   init reg_client struct
 *  -   get all client stored on linked list model
 *  -   add a client  in reg_client linked list
 *  -   find a client stored in reg_client linked list
 *  -   delete a client stored in linked list
 *  -   free up memory used to store a client in the linked list
 *  -   add a msg in the queuee msg of msg_entity linked list
 *  -   get all msgs in the queuee of a msg_entity
 *  -   free up memory used to store msgs queuee
 */
#include "model_data.h"


/* 
 * ----------------------------------------------------------------- 
 *|                   INIT reg_client struct
 * ----------------------------------------------------------------- 
 * \param   **head     our reg_client struct 
 */
void 
init_reg_client(struct reg_client** head){
    *head = NULL;
}


/* 
 * ----------------------------------------------------------------- 
 *|            Get all client stored on linked list model
 * ----------------------------------------------------------------- 
 * \param   *head      our reg_client struct 
 * \return  info       all infos about stored clients
 */
char*
get_all_clients(struct reg_client* head) {
    char    *info;
    int     dim;
    if( head == NULL){
        dim = strlen("There is no registered client\n")+1; //with '\0'
        info = (char *) malloc(sizeof(char)*dim);   
        strcpy(info, "There is no registered client\n");

    }
    else{
        dim = strlen("Client Status List:\n")+1; //with '\0'
        info = (char *) malloc(sizeof(char)*dim);
        strcpy(info, "Client Status List:\n");
        struct reg_client *temp;
        for (temp = head; temp; temp = temp->next){
            dim = dim + 
                strlen(temp->name) + 
                SPC_DIM +
                strlen(temp->status ? "(online)" : "(offline)") + 
                NL_DIM;
            info = (char*) realloc(info, sizeof(char)*(dim+1));
            strcat(info, temp->name);
            strcat(info, " ");
            strcat(info, temp->status ? "(online)" : "(offline)");
            strcat(info, "\n");

                //printf("%s - %s\n", temp->name, temp->status ? "online" : "offline");
        }
    }
    return info;
}


/* 
 * ----------------------------------------------------------------- 
 *|            Add a client  in reg_client linked list
 * ----------------------------------------------------------------- 
 * \param   *head      our reg_client struct 
 * \param   data       info client to store 
 * \return  head       pointer of the first elem of the list
 */
struct reg_client* 
add_client(struct reg_client* head, struct reg_client* data){

    struct reg_client *p;
    if(head == NULL){
        head = data;     //when linked list is empty
    }
    else{
        p  = head;//assign head to p 
        while(p->next != NULL){
            p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = data;//Point the previous last node to the new node created.
    }

    return head;
}


/* 
 * ----------------------------------------------------------------- 
 *|            Find a client stored in reg_client linked list
 * ----------------------------------------------------------------- 
 * \param   *head      our reg_client struct 
 * \param   name       find reg_client pointer to client with 
                       username: 'name'
 * \return  p          pointer reg_client to that client
 */
struct reg_client*
find_client(struct reg_client* head, char *name){
    log_debug("find client by name: %s", name);
    struct reg_client *p;
    if(head == NULL){
        log_debug("Client List is empty");
        p = head;     //when linked list is empty
    }
    else{
        p=head;
        while(p != NULL){
            if (strcmp(p->name,name)==0)
            {   
                log_debug("User %s has been found", p->name);
                return p;
            }
            p=p->next;
        }            
    }
    return NULL;
}


/* 
 * ----------------------------------------------------------------- 
 *|            Delete a client stored in linked list
 * ----------------------------------------------------------------- 
 * \param   *head      our reg_client struct 
 * \param   name       find reg_client pointer to client with 
 *                     username: 'name'
 * \return  bool       true (client has been found and deleted)
 *                     false (client has not been found)
 */
bool 
delete_client(struct reg_client** head, char *name) 
{ 
    log_debug("delete client by name: %s", name);
    struct reg_client *p=NULL;
    struct reg_client *q;
    for (q = *head; q != NULL && strcmp(q->name,name) != 0; q = q->next) 
        p = q;
    if (q == NULL) 
        return false; 
    if (q == *head)
        *head = q->next; 
    else
        p->next = q->next; 
    free_mem_client(q);
    return true; 
}


/* 
 * ----------------------------------------------------------------- 
 *|     Free up memory used to store a client in the linked list
 * ----------------------------------------------------------------- 
 * \param   *p      reg_client struct to free up 
 */
void
free_mem_client(struct reg_client* p)
{
    free(p->name);
    free(p->addr);
    free_mem_msgs(p->queuee_msg);
    free(p);
    log_debug("free up memory client");
}


/* 
 * ----------------------------------------------------------------- 
 *|     Add a msg in the queuee msg of msg_entity linked list
 * ----------------------------------------------------------------- 
 * \param   *head      our msg_entity entity 
 * \param   data       msg to add
 * \return   head       our msg_entity entity 
 */
struct msg_entity*
add_msg_to_client(struct msg_entity* head, struct msg_entity* data){

    struct msg_entity *p;
    if(head == NULL){
        head = data;     //when linked list is empty
    }
    else{
        p  = head;//assign head to p 
        while(p->next != NULL){
            p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = data;//Point the previous last node to the new node created.
    }

    return head;
}


/* 
 * ----------------------------------------------------------------- 
 * |            Get all msgs in a message queuee
 * ----------------------------------------------------------------- 
 * \param   *head      our msg_entity entity 
 * \return   info      return all msgs in a string 
 */
char*
get_queuee_msgs(struct msg_entity** head) {
    char    *info;
    int     dim;
    if( head == NULL){
        return NULL;
    }
    else{
        info = NULL;
        dim = 0;
        struct msg_entity* temp;
        for (temp = *head; temp; temp = temp->next){
            dim = dim + strlen(temp->msg);
            info = (char*) realloc(info, sizeof(char)*(dim+1));
            strcat(info, temp->msg);
        }
    }
    free_mem_msgs(*head);
    *head = NULL;
    return info;
}

/* 
 * ----------------------------------------------------------------- 
 * |         Free up memory used to store msgs queuee
 * ----------------------------------------------------------------- 
 * \param   *head      our msg_entity entity 
 */

void
free_mem_msgs(struct msg_entity* head){
    if(head == NULL)
        return;
    free_mem_msgs(head->next);
    free(head->msg);
    free(head);
    log_debug("free up memory msgs");

}
