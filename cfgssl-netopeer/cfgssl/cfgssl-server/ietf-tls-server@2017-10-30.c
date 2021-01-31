/*
 * This is automatically generated callbacks file
 * It contains 3 parts: Configuration callbacks, RPC callbacks and state data callbacks.
 * Do NOT alter function signatures or any structures unless you know exactly what you are doing.
 */

#include <stdlib.h>
#include <sys/inotify.h>
#include <libxml/tree.h>
#include <libnetconf_xml.h>
#include "handlers.h"


#define SERVER_RUNNING_DATASTORE_FILEPATH "/usr/local/etc/netopeer/cfgssl/cfgssl-server/datastore.xml"

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
struct ns_pair namespace_mapping[] = {{"tlss", "urn:ietf:params:xml:ns:yang:ietf-tls-server"}, {NULL, NULL}};

/*
 * CONFIGURATION callbacks
 * Here follows set of callback functions run every time some change in associated part of running datastore occurs.
 * You can safely modify the bodies of all function as well as add new functions for better lucidity of code.
 */

/**
 * @brief This callback will be run when node in path /tlss:ietf-tls-server changes
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
int callback_tlss_ietf_tls_server(void **data, XMLDIFF_OP op, xmlNodePtr old_node, xmlNodePtr new_node, struct nc_err **error) {
	xmlNodePtr element = new_node->xmlChildrenNode;

	if (element == NULL) {
		 return EXIT_SUCCESS;
	}

	char* node_name;
	char* node_content;
	int autostart = 1;
	char init_server[500];
	strcpy(init_server, "openssl s_server");

	do {
		node_name = element->name;

		if (!strcmp(node_name,"connection-info")) {
			 autostart = handle_connection_info(element,init_server);
		}
		 else if (!strcmp(node_name,"server-identity")) {
			 handle_server_identity(element,init_server);
		}
		else if (!strcmp(node_name,"client-auth")) {
			 handle_client_auth(element,init_server);
		}
		 else if (!strcmp(node_name,"hello-params")) {
			 handle_hello_params(element,init_server);
		}

	 element = element->next;
	}
	while (element != NULL);


	if (autostart) {
		pid_t pid = fork();
		if (pid == 0) {
				printf(" [ssl-server] Accepting SSL connections...\n");
				int ret = system(init_server);
				if (ret==-1) {
					printf(" [ssl-server] Failed to run SSL server (system call)\n");
					exit(EXIT_FAILURE);
				}
				printf(" [ssl-server] The SSL connection has finished\n");

				exit(EXIT_SUCCESS);
			}
	}

	return EXIT_SUCCESS;
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
		{.path = "/tlss:ietf-tls-server", .func = callback_tlss_ietf_tls_server}
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

nc_reply *rpc_init_ssl_server(xmlNodePtr input) {
	char init_server[500];
	strcpy(init_server, "openssl s_server -Verify 2");


	/*   HANDLE RPC INPUT

  char* id;
	if (input != NULL) {
		id = xmlNodeGetContent(input);

	}
*/


	/// try to run SSL server with the existing datastore (cfgssl-server) ///

	xmlDocPtr datastore = xmlParseFile(SERVER_RUNNING_DATASTORE_FILEPATH);
	xmlNodePtr root = xmlDocGetRootElement(datastore);
	if (root == NULL) {
		fprintf(stderr," [init-ssl-server] Running datastore (of SSL configuration) doesn't exist\n");
		return nc_reply_error(nc_err_new(NC_ERR_OP_FAILED));
	}

	xmlNodePtr running_datastore = get_rpc_node("running",root->xmlChildrenNode);


	if (running_datastore->xmlChildrenNode == NULL) {
		fprintf(stderr," [init-ssl-server] Running datastore (of server SSL configuration) is empty\n");
		struct nc_err *error = nc_err_new(NC_ERR_DATA_MISSING);
		nc_err_set(error, NC_ERR_PARAM_MSG, "Running datastore (of server SSL configuration) is empty");
		return nc_reply_error(error);
	}


  xmlNodePtr ietf_tls_server = get_rpc_node("ietf-tls-server",running_datastore->xmlChildrenNode);

  if (ietf_tls_server == NULL || ietf_tls_server->xmlChildrenNode == NULL) {
		fprintf(stderr," [init-ssl-server] There is no configuration for TLS server on running datastore\n");
		struct nc_err *error = nc_err_new(NC_ERR_DATA_MISSING);
		nc_err_set(error, NC_ERR_PARAM_MSG, "SSL server configuration is empty (on running datastore)");
		return nc_reply_error(error);
	}



  ///////////////////////////////////////////////////////////////////////////
  ////////////                  CONNECTION INFO                  ////////////
  ///////////////////////////////////////////////////////////////////////////


  char* port = "4433";

  xmlNodePtr connection_info_container = get_rpc_node("connection-info",ietf_tls_server->xmlChildrenNode);
  if (connection_info_container == NULL) {
    printf(" [init-ssl-server] No 'connection-info' found. Default port = 4433");
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
        printf(" [init-ssl-server] server-port = %s loaded\n",port);
        strcat(init_server, " -accept ");
        strcat(init_server,port);
      }

      /// HANDLE AUTO-START ///
      else if (is_equal(node_name,"auto-start")) {

      }
      element = element->next;
		} while (element != NULL);
  }



	///////////////////////////////////////////////////////////////////////////
	////////////                  SERVER-IDENTITY                  ////////////
	///////////////////////////////////////////////////////////////////////////

  xmlNodePtr server_identity_container = get_rpc_node("server-identity",ietf_tls_server->xmlChildrenNode);
	char* certificate_name;

	if (server_identity_container == NULL) {
		printf(" [init-ssl-server] No 'server-identity' params loaded");
		struct nc_err *error = nc_err_new(NC_ERR_DATA_MISSING);
		nc_err_set(error, NC_ERR_PARAM_MSG, "No 'server-identity' params loaded");
		return nc_reply_error(error);
	}
	else {
		xmlNodePtr element = server_identity_container->xmlChildrenNode;
		do {
			char* node_name = element->name;
		  char* node_content = xmlNodeGetContent(element);

			/////   PRIVATE-KEY NODE  //////
		  if (is_equal(node_name,"private-key")) {
				FILE *f = fopen("private-key.pem","w+");
				if (f==NULL) {
					fprintf(stderr," [init-ssl-server] Failed to create 'private-key.pem'\n");
				}
				else {
					char privatekey[1500];
					strcpy(privatekey,"-----BEGIN PRIVATE KEY-----\n");
					strcat(privatekey,node_content);
					strcat(privatekey,"\n-----END PRIVATE KEY-----");
					if (fputs(privatekey,f)==EOF)
						fprintf(stderr," [init-ssl-server] Failed to write on 'private-key.pem'\n");
				}
				fclose(f);
				strcat(init_server, " -key private-key.pem");
		  }

		  /////   PUBLIC-KEY NODE  //////
		  if (is_equal(node_name,"public-key")) {
				// handle public-key
		  }

		  /////   CERTIFICATE (list) NODE  //////
		  if (is_equal(node_name,"certificates")) {
				xmlNodePtr certificate = get_rpc_node("certificate",element->xmlChildrenNode);

				if (certificate == NULL || certificate->xmlChildrenNode == NULL) {
			      printf(" [init-ssl-server] Failed to find valid certificates\n");
						struct nc_err *error = nc_err_new(NC_ERR_DATA_MISSING);
						nc_err_set(error, NC_ERR_PARAM_MSG, "Failed to find valid certificates");
						return nc_reply_error(error);
				}
				else {
					//for (; certificate != NULL; certificate = certificate->next) {
				    xmlNodePtr leaf = certificate->xmlChildrenNode;
				    certificate_name = "server";
						char* certificate_value;
				    for(; leaf != NULL; leaf=leaf->next) {
				      if (is_equal(leaf->name,"name")) {
				        certificate_name =  xmlNodeGetContent(leaf);
				      }
				      else if (is_equal(leaf->name,"value")) {
								certificate_value =  xmlNodeGetContent(leaf);
				      }
				    }
						strcat(certificate_name,".pem");
						FILE *f = fopen(certificate_name,"w+");
						if (f==NULL) {
							fprintf(stderr," [init-ssl-server] Failed to create '%s.pem'\n",certificate_name);
						}
						else {
							char certificatevalue[1500];
							strcpy(certificatevalue,"-----BEGIN CERTIFICATE-----\n");
							strcat(certificatevalue,certificate_value);
							strcat(certificatevalue,"\n-----END CERTIFICATE-----");
							if (fputs(certificatevalue,f)==EOF)
								fprintf(stderr," [init-ssl-server] Failed to write on '%s.pem'\n",certificate_name);
						}
						fclose(f);
						strcat(init_server, " -cert ");
						strcat(init_server, certificate_name);
					//}
			  }
		  }
			element = element->next;
		} while (element != NULL);
	}


	///////////////////////////////////////////////////////////////////////////
	////////////                    CLIENT_AUTH                    ////////////
	///////////////////////////////////////////////////////////////////////////


	xmlNodePtr client_auth_container = get_rpc_node("client-auth",ietf_tls_server->xmlChildrenNode);
	char* ca_certificate_name;

	if (client_auth_container == NULL) {
		printf(" [init-ssl-server] No 'client-auth' params loaded\n");
		//return nc_reply_ok();
	}
	else {

		/// HANDLE PINNED CA CERTS ///
		xmlNodePtr pinned_ca_certs = get_rpc_node("pinned-ca-certs",client_auth_container->xmlChildrenNode);
		if (pinned_ca_certs!=NULL) {
			xmlNodePtr ca_certificate_list = get_rpc_node("certificates",pinned_ca_certs->xmlChildrenNode);
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
						fprintf(stderr," [init-ssl-server] Failed to create '%s'\n", ca_certificate_name);
					}
					else {
						char ca_value[1500];
						strcpy(ca_value,"-----BEGIN CERTIFICATE-----\n");
						strcat(ca_value,ca_certificate_value);
						strcat(ca_value,"\n-----END CERTIFICATE-----");
						if (fputs(ca_value,f)==EOF)
							fprintf(stderr," [init-ssl-server] Failed to write on '%s'\n",ca_certificate_name);
					}
					fclose(f);
					strcat(init_server, " -CAfile ");
					strcat(init_server, ca_certificate_name);
				}
			}
    }
	}


	///////////////////////////////////////////////////////////////////////////
	////////////                    HELLO_PARAMS                   ////////////
	///////////////////////////////////////////////////////////////////////////

	xmlNodePtr hello_params_container = get_rpc_node("hello-params",ietf_tls_server->xmlChildrenNode);

	if (hello_params_container == NULL) {
		printf(" [init-ssl-server] No 'hello-params' loaded");
	}
	else {
			// HANDLE TLS VERSIONS //
	    xmlNodePtr tls_versions = get_rpc_node("tls-versions", hello_params_container->xmlChildrenNode);
			for(xmlNodePtr tls_version = tls_versions->xmlChildrenNode; tls_version != NULL; tls_version = tls_version->next) {
	      char* node_content = xmlNodeGetContent(tls_version);

				// HANDLE EVERY TLS VERSION //
	      if (is_equal(node_content, "tlscmn:tls-1.0"))
					strcat(init_server, " -tls1");
			}

			// HANDLE CIPHER_SUITES //
			xmlNodePtr cipher_suites = get_rpc_node("cipher-suites", hello_params_container->xmlChildrenNode);
			for(xmlNodePtr cipher_suite = cipher_suites->xmlChildrenNode; cipher_suite != NULL; cipher_suite = cipher_suite->next) {
	      char* node_content = xmlNodeGetContent(cipher_suite);

				// HANDLE EVERY CIPHER_SUITE //
				if (is_equal(node_content, "tlscmn:rsa-with-3des-ede-cbc-sha"))
					strcat(init_server, " -cipher DES-CBC3-SHA");
			}
	}



	///////////////////////////////////////////////////////////////////////////
	////////////               SSL SERVER INIT                    ////////////
	///////////////////////////////////////////////////////////////////////////


  pid_t pid = fork();
	if (pid == 0) {
			printf(" [init-ssl-server] Accepting a SSL connection on port %s\n", port);
			int ret = system(init_server);
			if (ret==-1) {
				printf(" [init-ssl-server] Failed to run SSL server (system call)\n");
				exit(EXIT_FAILURE);
			}
			printf(" [init-ssl-server] The SSL connection has finished\n");

			// Remove key and certs //
			ret = remove("private-key.pem");
	   	if(ret == 0) {
	      printf(" [init-ssl-server] Private key deleted successfully\n");
	   	}

			ret = remove(certificate_name);
			if(ret == 0) {
				printf(" [init-ssl-server] Server certificate deleted successfully\n");
			}

			ret = remove(ca_certificate_name);
			if(ret == 0) {
				printf(" [init-ssl-server] CA certificate deleted successfully\n");
			}

			exit(EXIT_SUCCESS);
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
		{.name="init_ssl_server", .func=rpc_init_ssl_server}
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
