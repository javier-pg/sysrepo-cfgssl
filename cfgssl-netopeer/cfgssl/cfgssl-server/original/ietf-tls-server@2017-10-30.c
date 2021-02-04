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
	strcpy(init_server, "openssl s_server -Verify 2");

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
				int ret = system("service apache2 start");
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

/*
 * Structure transapi_rpc_callbacks provides mapping between callbacks and RPC messages.
 * It is used by libnetconf library to decide which callbacks will be run when RPC arrives.
 * DO NOT alter this structure
 */
struct transapi_rpc_callbacks rpc_clbks = {
	.callbacks_count = 0,
	.callbacks = {
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
