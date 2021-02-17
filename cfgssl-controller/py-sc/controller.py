# -*- coding: utf-8 -*-
import sys, os, warnings, lxml.etree as etree, threading, time
warnings.simplefilter("ignore", DeprecationWarning)
from ncclient import manager
from ncclient.transport.session import *
import socket
import os
from flask import Flask, request, jsonify
from OpenSSL import crypto
from ca import createKeyPair, createCertRequest, createCertificate
import time
import logging
from concurrent.futures.thread import ThreadPoolExecutor

PARALLEL = True

ssh_key = "[CONTROL-IP]:830 ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAACAQDAUb9OmnZqSFZyjvGhIDsaxPfq7KjUQaXvhr/DoqDG7kXWBWeT45ndFOiQzF/PGVGXQULA02SZjjYwbelnmDKHxXRaj4QCG+y/U3x5iFBrtCk3/PklQC8mj8LdWaP2M3aDlTtDYgiA66zQw3d70rlel4gioBrPly+bYo6h/LqQDx1gThmshIWyKkwyhLRQCfscHCZ6g7LEuNk00LWr6GfNcokKzyKiX+HrP0ecg+C2++hiNb9YgJq444Qz5Lh2kIVgFAd7d0ZOmfDOV7Ych5iEZIKkOZG9pm47b8KVVI3/n+AQzCXHgBPOdjaOGxiFxRHsWGGatd2ohNXi6f9HUop0TFIJxZ8Jf/Bu4hQL3tRG/593SX6vQwWg+y0Lc7EzXjVjVWVUeIZ8FJvwv5B/HSDN1O2rL+TP8Z6JnvVzMIExi0wYMVZmZ+XwOUxgQEQIFJ2s11E3EyhxD5P1TAS8rY6YePgOp/rdA9xMbsSTcW1TXZFc8RdzCGxSquwQERrDGBHIYpv1s/RTKbQ+pGwduLVwjWfh4kFn0Pk1VCGa2mGj6zdk1YxD6g9kZaaJXASBvesp3+l+fEVTGioAjhzugb+vyV1zPY63n7wPWSru8VEm4GhUKb17Z95sbMtF8YV5Ga1FQg83RvcqRL0DKxaAQ1JpNMHokm9a/SEssAthZFWxCQ=="

netconf_sessions = {}
node_certs = {}

registered_nodes = []
cakey = createKeyPair(crypto.TYPE_RSA,1024)
careq = createCertRequest(cakey, CN="Certificate Authority")
cacert = createCertificate(careq, (careq, cakey), 0, (0, 60 * 60 * 24 * 365 * 10))  # ten years
ca_cert = crypto.dump_certificate(crypto.FILETYPE_PEM, cacert).decode('ascii')
ca_cert = ca_cert[ca_cert.find("-----BEGIN CERTIFICATE-----")+28:ca_cert.find("-----END CERTIFICATE-----")-1]

# template for client config
configuration = open("/py-sc/client-conf-autostart.xml","r")
configuration_model = configuration.read()
configuration.close()

app = Flask(__name__)

def initController():

    @app.route('/register', methods=['POST'])
    def register():

        ####  process node info
        content = request.get_json()
        nfs = content['control_network_ip'] + "-" + content['data_network_ip'] + "-" + content['cert_req']
        control_address = nfs.split("-")[0]
        data_address = nfs.split("-")[1]

        # ssh management for netconf
        nfs_ssh_key = ssh_key.replace("CONTROL-IP",content['control_network_ip'])
        ssh_key_file = open("/root/.ssh/known_hosts","a")
        ssh_key_file.write(nfs_ssh_key+"\n")
        ssh_key_file.close()

        # certificate for node
        req = "-----BEGIN CERTIFICATE REQUEST-----\n" + content['cert_req'] + "\n-----END CERTIFICATE REQUEST-----"
        cert_req = crypto.load_certificate_request(crypto.FILETYPE_PEM,req)
        cert = createCertificate(cert_req, (cacert, cakey), len(registered_nodes), (0, 60 * 60 * 24 * 365 * 10))  # ten years
        node_cert = crypto.dump_certificate(crypto.FILETYPE_PEM, cert).decode('ascii')
        node_cert = node_cert[node_cert.find("-----BEGIN CERTIFICATE-----")+28:node_cert.find("-----END CERTIFICATE-----")-1]
        node_certs[control_address] = node_cert

        registered_nodes.append((control_address,data_address))
        return 'OK'


    @app.route('/mesh', methods=['POST'])
    def mesh():

        num_nodes = len(registered_nodes)

        if PARALLEL:
            # server configuration of nodes
            with ThreadPoolExecutor(max_workers=len(registered_nodes)) as executor:
                for registered_node in range(0,num_nodes,1):
                    executor.submit(configureServer, registered_node)

            # client configuration of nodes
            with ThreadPoolExecutor(max_workers=len(registered_nodes)) as executor:
                for registered_node in range(0,num_nodes,1):
                    executor.submit(createSAs, registered_node)
        else:
            for registered_node in range(0,num_nodes,1):
                configureServer(registered_node)
            for registered_node in range(0,num_nodes,1):
                createSAs(registered_node)

        return 'OK'

    def configureServer(registered_node):
        control_address = registered_nodes[registered_node][0]

        snippet = etree.tostring(etree.parse("/py-sc/server-conf-autostart.xml"), pretty_print=True)
        snippet = snippet.replace("CA-CERTIFICATE",ca_cert)
        snippet = snippet.replace("SERVER-CERTIFICATE",node_certs[control_address])

        netconf_sessions[control_address] = manager.connect_ssh(host=control_address,
                                                                port=830,
                                                                timeout=None,
                                                                username="javier",
                                                                password=None,
                                                                key_filename=None,
                                                                allow_agent=True,
                                                                hostkey_verify=True,
                                                                look_for_keys=True,
                                                                ssh_config=None)
        netconf_sessions[control_address].edit_config(target='running', config=snippet, test_option='test-then-set')


    def createSAs(registered_node):

        control_address = registered_nodes[registered_node][0]

        # set up of TLS associations between all stable nodes #
        if registered_node > 0:

            servers = []
            for server in range(0,registered_node,1):
                servers.append(registered_nodes[server][1])

            # XML
            client_configuration = "<config xmlns=\"urn:ietf:params:xml:ns:netconf:base:1.0\">"
            i=0
            for server in servers:
                i = i+1
                client_conf = configuration_model
                client_conf = client_conf.replace("SA-ID",str(i))
                client_conf = client_conf.replace("SERVER-ADDRESS",server)
                if i==len(servers) or i==1:
                    client_conf = client_conf.replace("LAST-NSF-FLAG","true")
                else:
                    client_conf = client_conf.replace("LAST-NSF-FLAG","false")

                client_conf = client_conf.replace("CLIENT-CERT",node_certs[control_address])
                client_configuration = client_configuration + client_conf

            client_configuration = client_configuration + "</config>"

            # edit config
            netconf_sessions[control_address].edit_config(target='running', config=client_configuration, test_option='test-then-set')

        # new node completely configured
        netconf_sessions[control_address].close_session()

    app.run(host='0.0.0.0', port=5000)


if __name__ == '__main__':
    print("Starting the controller...")
    initController()
