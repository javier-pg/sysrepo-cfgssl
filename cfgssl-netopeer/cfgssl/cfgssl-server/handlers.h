#ifndef __HANDLERS
#define __HANDLERS

#include <libxml/tree.h>
#include <string.h>

int handle_connection_info(xmlNodePtr node, char* init_server);
int handle_server_identity(xmlNodePtr node, char *init_server);
int handle_client_auth(xmlNodePtr node, char *init_server);
int handle_hello_params(xmlNodePtr node, char *init_server);

#endif
