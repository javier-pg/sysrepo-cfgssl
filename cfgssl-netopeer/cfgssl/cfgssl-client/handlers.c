#include <stdio.h>
#include <errno.h>
#include <sys/inotify.h>
#include <libxml/tree.h>
#include <libnetconf_xml.h>
#include "handlers.h"


int is_equal(char* received_content, char* allowed_content) {
  return !strcmp(received_content,allowed_content);
}

///////////////////////////////////////////////////////////////////////////
////////////               CONNECTION-INFO                     ////////////
///////////////////////////////////////////////////////////////////////////


//// SERVER_IDENTITY NODES HANDLER ////
int handle_connection_info(xmlNodePtr node, char* init_connection) {

  node = node->xmlChildrenNode;

  int autostart = 1;
  char* target_address = NULL;
  char* port = "4433";


  do {

    char* node_name = node->name;
    char* node_content = xmlNodeGetContent(node);

    /////  SERVER-PORT NODE  //////
    if (is_equal(node_name,"server-port")) {
      printf(" [server-info] server-port = %s loaded\n",node_content);
      port = node_content;
    }
    /////   AUTO-START NODE  //////
    else if (is_equal(node_name,"server-address")) {
      printf(" [server-info] server-address = %s\n", node_content);
      target_address = node_content;
    }
    /////   AUTO-START NODE  //////
    else if (is_equal(node_name,"auto-start")) {
      printf(" [server-info] auto-start = %s\n", node_content);
      if (is_equal(node_content,"false")) {
        autostart = 0;
      }
    }
    node = node->next;
  } while (node!=NULL);

  strcat(init_connection," --remote ");
  strcat(init_connection,target_address);
  strcat(init_connection," ");
  strcat(init_connection,port);


  return autostart;
}

///////////////////////////////////////////////////////////////////////////
////////////               CLIENT_IDENTITY                     ////////////
///////////////////////////////////////////////////////////////////////////

//// ALGORITHM KEY HANDLER ////
int handle_algorithm_key(char* node_content) {
  if (is_equal(node_content,"ks:rsa1024")) {
    printf(" [client-identity] key algorithm = %s\n", node_content);
    return EXIT_SUCCESS;
  }
  else {
    fprintf(stderr, " [client-identity] %s key algorithm is NOT implemented\n", node_content);
    return EXIT_FAILURE;
  }
}

//// PRIVATE KEY HANDLER ////
int handle_private_key(char* node_content) {
  FILE *f = fopen("private-key.pem","w+");
  if (f==NULL) {
    fprintf(stderr," [client-identity] Failed to create 'private-key.pem'\n");
    return EXIT_FAILURE;
  }
  else {
    char privatekey[3000];
    strcpy(privatekey,"-----BEGIN PRIVATE KEY-----\n");
    strcat(privatekey,node_content);
    strcat(privatekey,"\n-----END PRIVATE KEY-----");
    if (fputs(privatekey,f)==EOF) {
      fprintf(stderr," [client-identity] Failed to write on 'private-key.pem'\n");
      return EXIT_FAILURE;
    }
  }
  fclose(f);
  printf(" [client-identity] private-key loaded\n");
  return EXIT_SUCCESS;
}

//// PUBLIC KEY HANDLER ////
int handle_public_key(char* node_content) {
  printf(" [client-identity] public-key loaded\n");
  return EXIT_SUCCESS;
}

//// CLIENT-CERTIFICATE HANDLER ////    (only one is necessary)
int handle_client_certificates(xmlNodePtr certificate_list) {
  xmlNodePtr certificate = certificate_list->xmlChildrenNode;

  char* certificate_name;
  char* certificate_value;

	if (certificate == NULL) {
      printf(" [client-identity] is necessary a valid certificate\n");
			return EXIT_FAILURE;
	}

	for (; certificate != NULL; certificate = certificate->next) {
    xmlNodePtr leaf = certificate->xmlChildrenNode;
    int value = 0;

    for(; leaf != NULL; leaf=leaf->next) {
      if (is_equal(leaf->name,"name")) {
        certificate_name =  xmlNodeGetContent(leaf);
      }
      else if (is_equal(leaf->name,"value")) {
        certificate_value =  xmlNodeGetContent(leaf);
	      value = 1;
      }
    }


    if (value) {
       FILE *f = fopen("clientcert.pem","w+");
       if (f==NULL) {
         fprintf(stderr," [client-identity] Failed to create '%s.pem'\n",certificate_name);
         return EXIT_FAILURE;
       }
       else {
         char certificatevalue[3000];
         strcpy(certificatevalue,"-----BEGIN CERTIFICATE-----\n");
         strcat(certificatevalue,certificate_value);
         strcat(certificatevalue,"\n-----END CERTIFICATE-----");
         if (fputs(certificatevalue,f)==EOF) {
           fprintf(stderr," [server_identity] Failed to write on '%s.pem'\n",certificate_name);
           return EXIT_FAILURE;
         }
       }
       fclose(f);
    }
    else {
        printf(" [client-identity] no value associated to %s\n",certificate_name);
	      return EXIT_FAILURE;
    }
  }
  printf(" [client-identity] certificate %s loaded\n",certificate_name);
  return EXIT_SUCCESS;
}



//// CLIENT_IDENTITY NODES HANDLER ////
int handle_client_identity(xmlNodePtr node, char* init_connection) {

  xmlNodePtr iterator = node->xmlChildrenNode;

  char* node_name = iterator->name;
  char* node_content = xmlNodeGetContent(iterator);

  if (is_equal(node_name,"certificate")) {     // SE AUTENTICA CON CERTIFICADO

    iterator = iterator->xmlChildrenNode;

    do {

    node_name = iterator->name;
    node_content = xmlNodeGetContent(iterator);

    /////  ALGORITHM-KEY NODE  //////
    if (is_equal(node_name,"algorithm")) {
        handle_algorithm_key(node_content);
    }

    /////   PRIVATE-KEY NODE  //////
    else if (is_equal(node_name,"private-key")) {
        handle_private_key(node_content);
    }

    /////   PUBLIC-KEY NODE  //////
    else if (is_equal(node_name,"public-key")) {
        handle_public_key(node_content);
    }

    /////   CERTIFICATE (list) NODE  //////
    else if (is_equal(node_name,"certificates")) {
        handle_client_certificates(iterator);
        strcat(init_connection, " --cert /clientcert.pem");    // CARGAREMOS SOLO UNO
    }
      iterator = iterator->next;

    } while (iterator != NULL);

    strcat(init_connection, " --key /private-key.pem");

    return EXIT_SUCCESS;

  }
  else {
    return EXIT_FAILURE;
  }
}



///////////////////////////////////////////////////////////////////////////
////////////                   SERVER_AUTH                     ////////////
///////////////////////////////////////////////////////////////////////////


//// CA CERTIFICATES HANDLER ////
int handle_ca_certificates(xmlNodePtr certificate_list) {

  xmlNodePtr certificate = certificate_list->xmlChildrenNode;

  char* ca_certificate_name;
  char* ca_certificate_value;


  if (certificate == NULL) {
      return EXIT_SUCCESS;
  }

  for (; certificate != NULL; certificate = certificate->next) {
    xmlNodePtr leaf = certificate->xmlChildrenNode;
    int value = 0;
    for(; leaf != NULL; leaf=leaf->next) {
      if (is_equal(leaf->name,"name")) {
        ca_certificate_name =  xmlNodeGetContent(leaf);
      }
      else if (is_equal(leaf->name,"value")) {
        ca_certificate_value = xmlNodeGetContent(leaf);
         value = 1;
      }
    }
    if (value) {
      FILE *f = fopen("cacert.pem","w+");   // SE PUEDE PERSONALIZAR AL NOMBRE DEL CERTIFICADO
      if (f==NULL) {
        fprintf(stderr," [server-auth] Failed to create '%s'\n", ca_certificate_name);
        return EXIT_FAILURE;
      }
      else {
        char ca_value[3000];
        strcpy(ca_value,"-----BEGIN CERTIFICATE-----\n");
        strcat(ca_value,ca_certificate_value);
        strcat(ca_value,"\n-----END CERTIFICATE-----");
        if (fputs(ca_value,f)==EOF) {
          fprintf(stderr," [server-auth] Failed to write on '%s'\n",ca_certificate_name);
          return EXIT_FAILURE;
        }
        printf(" [server-auth] CA certificate %s loaded\n",ca_certificate_name);
      }
      fclose(f);
    }
    else {
        printf(" [server-auth] no value associated to %s\n",ca_certificate_name);
        return EXIT_FAILURE;
    }
  }

  return EXIT_SUCCESS;
}


//// SERVER_AUTH NODES HANDLER ////
int handle_server_auth(xmlNodePtr node, char* init_connection) {

  xmlNodePtr iterator = node->xmlChildrenNode;

 do {
  char* node_name = iterator->name;
  char* node_content = xmlNodeGetContent(iterator);

  /// HANDLE PINNED CA CERTS ///
  if (is_equal(node_name,"pinned-ca-certs")) {
      node = iterator->xmlChildrenNode;
      node_name = node->name;
      node_content = xmlNodeGetContent(node);

      /////   CERTIFICATE LIST //////
      if (is_equal(node_name,"certificates")) {
          handle_ca_certificates(node);
      }
  }
  iterator = iterator->next;

  } while (iterator!=NULL);

  strcat(init_connection, " --ca /cacert.pem");

  return EXIT_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////
////////////                    HELLO_PARAMS                   ////////////
///////////////////////////////////////////////////////////////////////////


int handle_hello_params(xmlNodePtr node, char* init_connection) {


  xmlNodePtr iterator = node->xmlChildrenNode;

do {
  char* node_name = iterator->name;
  char* node_content = xmlNodeGetContent(iterator);

  /// HANDLE TLS VERSIONS ///
  if (is_equal(node_name,"tls-versions")) {
    for(node = iterator->xmlChildrenNode; node != NULL; node = node->next) {
      node_content = xmlNodeGetContent(node);
      if (is_equal(node_content, "tlscmn:tls-1.3")){
        printf(" [hello-params] %s version will be used\n", node_content);
        // strcat(init_connection, " -tls1");     TLSv1.3 is used by default
      }
      else {
        printf(" [hello-params] %s version is not implemented\n", node_content);
      }
    }
  }

  /// HANDLE CIPHER SUITES ///
  if (is_equal(node_name,"cipher-suites")) {
    for(node = iterator->xmlChildrenNode; node != NULL; node = node->next) {
      node_content = xmlNodeGetContent(node);
      if (is_equal(node_content, "tlscmn:ecdhe-rsa-with-aes-256-gcm-sha384")){
        printf(" [hello-params] %s cipher suite will be used\n", node_content);
        //strcat(init_connection, " -cipher ECDHE -ciphersuites \"TLS_AES_256_GCM_SHA384\"");
      }
      else {
        printf(" [hello-params] %s cipher suite is not implemented\n", node_content);
      }
    }
  }
  iterator = iterator->next;

} while (iterator != NULL);

  return EXIT_SUCCESS;
}
