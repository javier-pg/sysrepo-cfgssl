/*
 * This is automatically generated callbacks file
 * It contains 3 parts: Configuration callbacks, RPC callbacks and state data callbacks.
 * Do NOT alter function signatures or any structures unless you know exactly what you are doing.
 */

#include <stdlib.h>
#include <sys/inotify.h>
#include <libxml/tree.h>
#include <libnetconf_xml.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define CLIENT_RUNNING_DATASTORE_FILEPATH "/usr/local/etc/netopeer/cfgssl/cfgssl-client/datastore.xml"


/* transAPI version which must be compatible with libnetconf */
int transapi_version = 6;

/* Signal to libnetconf that configuration data were modified by any callback.
 * 0 - data not modified
 * 1 - data have been modified
 */
int config_modified = 0;

/*
 * Determines the callbacks order.
 * Set this variable before compilation and DO NOT modify it in runtime.
 * TRANSAPI_CLBCKS_LEAF_TO_ROOT (default)
 * TRANSAPI_CLBCKS_ROOT_TO_LEAF
 */
const TRANSAPI_CLBCKS_ORDER_TYPE callbacks_order = TRANSAPI_CLBCKS_ORDER_DEFAULT;

/* Do not modify or set! This variable is set by libnetconf to announce edit-config's error-option
Feel free to use it to distinguish module behavior for different error-option values.
 * Possible values:
 * NC_EDIT_ERROPT_STOP - Following callback after failure are not executed, all successful callbacks executed till
                         failure point must be applied to the device.
 * NC_EDIT_ERROPT_CONT - Failed callbacks are skipped, but all callbacks needed to apply configuration changes are executed
 * NC_EDIT_ERROPT_ROLLBACK - After failure, following callbacks are not executed, but previous successful callbacks are
                         executed again with previous configuration data to roll it back.
 */
NC_EDIT_ERROPT_TYPE erropt = NC_EDIT_ERROPT_NOTSET;

/**
 * @brief Initialize plugin after loaded and before any other functions are called.

 * This function should not apply any configuration data to the controlled device. If no
 * running is returned (it stays *NULL), complete startup configuration is consequently
 * applied via module callbacks. When a running configuration is returned, libnetconf
 * then applies (via module's callbacks) only the startup configuration data that
 * differ from the returned running configuration data.

 * Please note, that copying startup data to the running is performed only after the
 * libnetconf's system-wide close - see nc_close() function documentation for more
 * information.

 * @param[out] running	Current configuration of managed device.

 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int transapi_init(xmlDocPtr *running) {
	return EXIT_SUCCESS;
}

/**
 * @brief Free all resources allocated on plugin runtime and prepare plugin for removal.
 */
void transapi_close(void) {
	return;
}

/**
 * @brief Retrieve state data from device and return them as XML document
 *
 * @param model	Device data model. libxml2 xmlDocPtr.
 * @param running	Running datastore content. libxml2 xmlDocPtr.
 * @param[out] err  Double pointer to error structure. Fill error when some occurs.
 * @return State data as libxml2 xmlDocPtr or NULL in case of error.
 */
xmlDocPtr get_state_data(xmlDocPtr model, xmlDocPtr running, struct nc_err **err) {
	return(NULL);
}
/*
 * Mapping prefixes with namespaces.
 * Do NOT modify this structure!
 */
struct ns_pair namespace_mapping[] = {{"tlsc", "urn:ietf:params:xml:ns:yang:ietf-tls-client"}, {NULL, NULL}};

/*
 * CONFIGURATION callbacks
 * Here follows set of callback functions run every time some change in associated part of running datastore occurs.
 * You can safely modify the bodies of all function as well as add new functions for better lucidity of code.
 */

 int send_log() {
  char buffer[26];
  int millisec;
  struct tm* tm_info;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  millisec = lrint(tv.tv_usec/1000.0); // Round to nearest millisec
  if (millisec>=1000) { // Allow for rounding up to nearest second
    millisec -=1000;
    tv.tv_sec++;
  }
  tm_info = localtime(&tv.tv_sec);
  strftime(buffer, 26, "%s", tm_info);

  char start_time[50];
  sprintf(start_time, "%s,%03d", buffer, millisec);

  int clientSocket, portNum, nBytes;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

  /*Create UDP socket*/
  clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

  /*Configure settings in address struct*/
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(514);
  serverAddr.sin_addr.s_addr = inet_addr("10.0.1.200");
  memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));

  /*Initialize size variable to be used later on*/
  addr_size = sizeof(serverAddr);
  nBytes = strlen(start_time) + 1;

  /*Send message to server*/
  sendto(clientSocket,start_time,nBytes,0,(struct sockaddr *)&serverAddr,addr_size);

  return 0;
}

/**
 * @brief This callback will be run when node in path /tlsc:ietf-client-configurations changes
 *
 * @param[in] data	Double pointer to void. Its passed to every callback. You can share data using it.
 * @param[in] op	Observed change in path. XMLDIFF_OP type.
 * @param[in] old_node	Old configuration node. If op == XMLDIFF_ADD, it is NULL.
 * @param[in] new_node	New configuration node. if op == XMLDIFF_REM, it is NULL.
 * @param[out] error	If callback fails, it can return libnetconf error structure with a failure description.
 *
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
/* !DO NOT ALTER FUNCTION SIGNATURE! */
int callback_tlsc_ietf_client_configurations(void **data, XMLDIFF_OP op, xmlNodePtr old_node, xmlNodePtr new_node, struct nc_err **error) {

	xmlNodePtr client_config = new_node;


	xmlNodePtr ietf_tls_client_container = client_config->xmlChildrenNode;

	while (strcmp(ietf_tls_client_container->name,"ietf-tls-client")) {
			 ietf_tls_client_container = ietf_tls_client_container->next;
	}

    xmlNodePtr element = ietf_tls_client_container->xmlChildrenNode;

	char* node_name;
	char* node_content;
	int measure = 0;
	char init_connection[500];
	strcpy(init_connection, "openvpn");

	do {
		node_name = element->name;

		if (!strcmp(node_name,"connection-info")) {
			 measure = handle_connection_info(element,init_connection);
		}
		else if (!strcmp(node_name,"client-identity")) {
			 handle_client_identity(element,init_connection);
		}
		else if (!strcmp(node_name,"server-auth")) {
			 handle_server_auth(element,init_connection);
		}
		 else if (!strcmp(node_name,"hello-params")) {
			 handle_hello_params(element,init_connection);
		}

	 element = element->next;
	}
	while (element != NULL);

	//// INIT SSL CONNECTION //
	// if (measure) {
	//    send_log();
	// }

	strcat(init_connection, " --config /client.conf");

	pid_t pid = fork();

	if (pid == 0) {
			printf(" [init-ssl-client] Trying to make SSL connection\n");
			int ret = system(init_connection);
			if (ret==-1) {
				printf(" [init-ssl-client] Failed to make the SSL connection\n");
				exit(EXIT_FAILURE);
			}
			printf(" [init-ssl-client] The SSL connection has finished\n");

			exit(EXIT_SUCCESS);
		}
	else {
		return EXIT_SUCCESS;
	}
}


/*
 * Structure transapi_config_callbacks provide mapping between callback and path in configuration datastore.
 * It is used by libnetconf library to decide which callbacks will be run.
 * DO NOT alter this structure
 */
struct transapi_data_callbacks clbks =  {
	.callbacks_count = 1,
	.data = NULL,
	.callbacks = {
		{.path = "/tlsc:ietf-client-configurations", .func = callback_tlsc_ietf_client_configurations}
	}
};

/**
 * @brief Get a node from the RPC input. The first found node is returned, so if traversing lists,
 * call repeatedly with result->next as the node argument.
 *
 * @param name	Name of the node to be retrieved.
 * @param node	List of nodes that will be searched.
 * @return Pointer to the matching node or NULL
 */
xmlNodePtr get_rpc_node(const char *name, const xmlNodePtr node) {
	xmlNodePtr ret = NULL;

	for (ret = node; ret != NULL; ret = ret->next) {
		if (xmlStrEqual(BAD_CAST name, ret->name)) {
			break;
		}
	}

	return ret;
}

/*
 * RPC callbacks
 * Here follows set of callback functions run every time RPC specific for this device arrives.
 * You can safely modify the bodies of all function as well as add new functions for better lucidity of code.
 * Every function takes an libxml2 list of inputs as an argument.
 * If input was not set in RPC message argument is set to NULL. To retrieve each argument, preferably use get_rpc_node().
 */

nc_reply *rpc_init_ssl_client(xmlNodePtr input) {
	char* ca_certificate_name;

	if (input==NULL) {
		printf(" [ssl-connection] Failed to init the SSL connection (target address/port missing)\n");
		struct nc_err *error = nc_err_new(NC_ERR_MISSING_ATTR);
		nc_err_set(error, NC_ERR_PARAM_MSG, "Missing target address/port to init the connection");
		return nc_reply_error(error);
	}

	char init_connection[500];
	strcpy(init_connection, "openssl s_client");


/*    HANDLE ARGUMENTS
	for(;input !=NULL; input = input->next) {
		if (is_equal(input->name,"id")){
			port = xmlNodeGetContent(input);
		}
	}    */

	/// try the SSL connection with the existing datastore (cfgssl-client) ///

	xmlDocPtr datastore = xmlParseFile(CLIENT_RUNNING_DATASTORE_FILEPATH);
	xmlNodePtr root = xmlDocGetRootElement(datastore);
	char* certificate_name;



	if (root == NULL) {
		printf(" [ssl-connection] Running datastore (of client SSL configuration) doesn't exist\n");
		return nc_reply_ok();
	}

	xmlNodePtr running_datastore = get_rpc_node("running",root->xmlChildrenNode);
	char* port = "4433";
	char* target_address;

	if (running_datastore->xmlChildrenNode == NULL) {
		printf(" [ssl-connection] Running datastore (of client SSL configuration) is empty. Using default parameters...\n");
	}
	else {

		xmlNodePtr ietf_tls_client = get_rpc_node("ietf-tls-client",running_datastore->xmlChildrenNode);

		if (ietf_tls_client == NULL || ietf_tls_client->xmlChildrenNode == NULL) {
			fprintf(stderr," [init-ssl-client] There is no configuration for TLS client on running datastore\n");
			struct nc_err *error = nc_err_new(NC_ERR_DATA_MISSING);
			nc_err_set(error, NC_ERR_PARAM_MSG, "SSL client configuration is empty (on running datastore)");
			return nc_reply_error(error);
		}


		for(xmlNodePtr container = ietf_tls_client->xmlChildrenNode; container != NULL; container = container -> next) {

			char* container_name = container->name;

			///////////////////////////////////////////////////////////////////////////
			////////////                  CONNECTION INFO                  ////////////
			///////////////////////////////////////////////////////////////////////////

			if (is_equal(container_name, "connection-info")) {

			xmlNodePtr connection_info_container = container;
			if (connection_info_container == NULL) {
				printf(" [init-ssl-client] No 'connection-info' found. Default port = 4433");
				port = "4433";
			}
			else {
				xmlNodePtr element = connection_info_container->xmlChildrenNode;
				do {
					char* node_name = element->name;
					char* node_content = xmlNodeGetContent(element);

					/// HANDLE SERVER PORT ///
					if (is_equal(node_name,"server-port")) {
						port = node_content;
						printf(" [init-ssl-client] server-port = %s loaded\n",port);
					}

					/// HANDLE SERVER ADDRESS ///
					else if (is_equal(node_name,"server-address")) {
						target_address = node_content;
						printf(" [init-ssl-client] server-address = %s loaded\n",target_address);
					}
					/// HANDLE AUTO-START ///
					else if (is_equal(node_name,"auto-start")) {

					}
					element = element->next;
				} while (element != NULL);
			}
			strcat(init_connection," -connect ");
		  strcat(init_connection,target_address);
		  strcat(init_connection,":");
		  strcat(init_connection,port);
		}

			///////////////////////////////////////////////////////////////////////////
			////////////                  CLIENT-IDENTITY                  ////////////
			///////////////////////////////////////////////////////////////////////////

		 if (is_equal(container_name, "client-identity")) {
			xmlNodePtr client_identity_container = container;
			xmlNodePtr auth_type = get_rpc_node("certificate",client_identity_container->xmlChildrenNode);
			if (!is_equal(auth_type->name,"certificate")) {
				struct nc_err *error = nc_err_new(NC_ERR_OP_NOT_SUPPORTED);
				nc_err_set(error, NC_ERR_PARAM_MSG, "Only CERTIFICATE choice is allowed for client authentication");
				return nc_reply_error(error);
			}
			else {
				xmlNodePtr element = auth_type->xmlChildrenNode;
			do {
				char* node_name = element->name;
				char* node_content = xmlNodeGetContent(element);

				/////   PRIVATE-KEY NODE  //////
				if (is_equal(node_name,"private-key")) {
					FILE *f = fopen("private-key.pem","w+");
					if (f==NULL) {
						fprintf(stderr," [ssl-connection] Failed to create 'private-key.pem'\n");
					}
					else {
						char privatekey[1500];
						strcpy(privatekey,"-----BEGIN PRIVATE KEY-----\n");
						strcat(privatekey,node_content);
						strcat(privatekey,"\n-----END PRIVATE KEY-----");
						if (fputs(privatekey,f)==EOF)
							fprintf(stderr," [ssl-connection] Failed to write on 'private-key.pem'\n");
					}
					fclose(f);
					strcat(init_connection, " -key private-key.pem");
				}

				/////   PUBLIC-KEY NODE  //////
				if (is_equal(node_name,"public-key")) {
					// handle public-key
				}

				/////   CERTIFICATE (list) NODE  //////
				if (is_equal(node_name,"certificates")) {
					xmlNodePtr certificate = get_rpc_node("certificate",element->xmlChildrenNode);

					if (certificate == NULL || certificate->xmlChildrenNode == NULL) {
							printf(" [ssl-connection] No certificates loaded\n");
							return nc_reply_ok();
					}
					else {
						//for (; certificate != NULL; certificate = certificate->next) {
							xmlNodePtr leaf = certificate->xmlChildrenNode;
							certificate_name = "client";
							char* certificate_value;
							for(; leaf != NULL; leaf=leaf->next) {
								if (is_equal(leaf->name,"name")) {
									certificate_name =  xmlNodeGetContent(leaf);
								}
								else if (is_equal(leaf->name,"value")) {
									certificate_value =  xmlNodeGetContent(leaf);
								}
							}
							FILE *f = fopen(certificate_name,"w+");
							if (f==NULL) {
								fprintf(stderr," [ssl-connection] Failed to create '%s'\n",certificate_name);
							}
							else {
								char certificatevalue[1500];
								strcpy(certificatevalue,"-----BEGIN CERTIFICATE-----\n");
								strcat(certificatevalue,certificate_value);
								strcat(certificatevalue,"\n-----END CERTIFICATE-----");
								if (fputs(certificatevalue,f)==EOF)
									fprintf(stderr," [ssl-connection] Failed to write on '%s'\n",certificate_name);
							}
							fclose(f);
							strcat(init_connection, " -cert ");
							strcat(init_connection, certificate_name);
						//}
					}
				}
				element = element->next;
			} while (element != NULL);
		}
	}

		///////////////////////////////////////////////////////////////////////////
		////////////                    SERVER_AUTH                    ////////////
		///////////////////////////////////////////////////////////////////////////

		if (is_equal(container_name, "server-auth")) {
			xmlNodePtr server_auth_container = container;
			if (server_auth_container == NULL) {
				printf(" [ssl-connection] No 'server-auth' params loaded\n");
				//return nc_reply_ok();
			}
			else {

				/// HANDLE PINNED CA CERTS ///
				xmlNodePtr pinned_ca_certs = server_auth_container->xmlChildrenNode;
				while (pinned_ca_certs!=NULL && !is_equal(pinned_ca_certs->name,"pinned_ca_certs"))
					pinned_ca_certs = pinned_ca_certs->next;

				if (pinned_ca_certs!=NULL) {
					xmlNodePtr ca_certificate_list = pinned_ca_certs->xmlChildrenNode;
					while (ca_certificate_list!=NULL && !is_equal(ca_certificate_list->name,"certificates"))
						ca_certificate_list = ca_certificate_list->next;

					if (ca_certificate_list!=NULL){
						for(xmlNodePtr ca_certificate = ca_certificate_list->xmlChildrenNode; ca_certificate!=NULL; ca_certificate = ca_certificate->next){
							ca_certificate_name = "cacert";
							char* ca_certificate_value;
							for(xmlNodePtr ca_leaf = ca_certificate->xmlChildrenNode; ca_leaf != NULL; ca_leaf=ca_leaf->next) {
								if (is_equal(ca_leaf->name,"name")) {
									ca_certificate_name =  xmlNodeGetContent(ca_leaf);
								}
								else if (is_equal(ca_leaf->name,"value")) {
									ca_certificate_value =  xmlNodeGetContent(ca_leaf);
								}
							}

							FILE *f = fopen(ca_certificate_name,"w+");
							if (f==NULL) {
								fprintf(stderr," [ssl-connection] Failed to create '%s.pem'\n", ca_certificate_name);
							}
							else {
								char ca_value[3000];
								strcpy(ca_value,"-----BEGIN CERTIFICATE-----\n");
								strcat(ca_value,ca_certificate_value);
								strcat(ca_value,"\n-----END CERTIFICATE-----");
								if (fputs(ca_value,f)==EOF)
									fprintf(stderr," [ssl-connection] Failed to write on '%s'\n",ca_certificate_name);
							}
							fclose(f);
							strcat(init_connection, " -CAfile ");
							strcat(init_connection, ca_certificate_name);
						}
					}
				}
		 }
 	}

	 ///////////////////////////////////////////////////////////////////////////
	 ////////////                    HELLO_PARAMS                   ////////////
	 ///////////////////////////////////////////////////////////////////////////
	 if (is_equal(container_name, "hello-params")) {

	 xmlNodePtr hello_params_container = container;

	 if (hello_params_container == NULL) {
		 printf(" [ssl-connection] No 'hello-params' loaded\n");
	 }
	 else {
			 for(xmlNodePtr hello_params = hello_params_container->xmlChildrenNode; hello_params!=NULL; hello_params = hello_params->next ) {

				 // HANDLE TLS VERSIONS //
				 if (is_equal(hello_params->name,"tls-versions")) {
				 	for(xmlNodePtr tls_version = hello_params->xmlChildrenNode; tls_version != NULL; tls_version = tls_version->next) {
					 char* node_content = xmlNodeGetContent(tls_version);
					 // HANDLE EVERY TLS VERSION //
					 if (is_equal(node_content, "tlscmn:tls-1.0"))
						 strcat(init_connection, " -tls1");
				 }
  	 		}

				 // HANDLE CIPHER_SUITES //
				 if (is_equal(hello_params->name,"cipher-suites")) {
					 for(xmlNodePtr cipher_suite = hello_params->xmlChildrenNode; cipher_suite != NULL; cipher_suite = cipher_suite->next) {
						 char* node_content = xmlNodeGetContent(cipher_suite);
						 // HANDLE EVERY CIPHER_SUITE //
						 if (is_equal(node_content, "tlscmn:rsa-with-3des-ede-cbc-sha"))
							 strcat(init_connection, " -cipher DES-CBC3-SHA");
					 }
		 			}
			}
		}
	}
}

  //// INIT SSL CONNECTION ///

	pid_t pid = fork();
	if (pid == 0) {
			printf(" [ssl-connection] Trying to make a SSL connection to %s:%s...\n",target_address,port);
			int ret = system(init_connection);
			if (ret==-1) {
				printf(" [ssl-connection] Failed to make a SSL connection to %s:%s...\n",target_address,port);
				exit(EXIT_FAILURE);
			}
			printf(" [ssl-connection] The SSL connection to %s:%s has finished\n",target_address,port);


			// Remove key and certs //
			ret = remove("private-key.pem");
	   	if(ret == 0) {
	      printf(" [ssl-connection] Private key deleted successfully\n");
	   	}

			ret = remove(certificate_name);
			if(ret == 0) {
				printf(" [ssl-connection] Certificate deleted successfully\n");
			}

			ret = remove(ca_certificate_name);
			if(ret == 0) {
				printf(" [ssl-connection] CA certificate deleted successfully\n");
			}

			exit(EXIT_SUCCESS);
		}
	}
	return nc_reply_ok();
}
/*
 * Structure transapi_rpc_callbacks provides mapping between callbacks and RPC messages.
 * It is used by libnetconf library to decide which callbacks will be run when RPC arrives.
 * DO NOT alter this structure
 */
struct transapi_rpc_callbacks rpc_clbks = {
	.callbacks_count = 1,
	.callbacks = {
		{.name="init_ssl_client", .func=rpc_init_ssl_client}
	}
};

/*
 * Structure transapi_file_callbacks provides mapping between specific files
 * (e.g. configuration file in /etc/) and the callback function executed when
 * the file is modified.
 * The structure is empty by default. Add items, as in example, as you need.
 *
 * Example:
 * int example_callback(const char *filepath, xmlDocPtr *edit_config, int *exec) {
 *     // do the job with changed file content
 *     // if needed, set edit_config parameter to the edit-config data to be applied
 *     // if needed, set exec to 1 to perform consequent transapi callbacks
 *     return 0;
 * }
 *
 * struct transapi_file_callbacks file_clbks = {
 *     .callbacks_count = 1,
 *     .callbacks = {
 *         {.path = "/etc/my_cfg_file", .func = example_callback}
 *     }
 * }
 */
struct transapi_file_callbacks file_clbks = {
	.callbacks_count = 0,
	.callbacks = {{NULL}}
};
