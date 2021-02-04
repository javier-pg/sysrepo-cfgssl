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


ssh_key = "[CONTROL-IP]:830 ssh-rsa AAAAB3NzaC1yc2EAAAADAQABAAACAQDAUb9OmnZqSFZyjvGhIDsaxPfq7KjUQaXvhr/DoqDG7kXWBWeT45ndFOiQzF/PGVGXQULA02SZjjYwbelnmDKHxXRaj4QCG+y/U3x5iFBrtCk3/PklQC8mj8LdWaP2M3aDlTtDYgiA66zQw3d70rlel4gioBrPly+bYo6h/LqQDx1gThmshIWyKkwyhLRQCfscHCZ6g7LEuNk00LWr6GfNcokKzyKiX+HrP0ecg+C2++hiNb9YgJq444Qz5Lh2kIVgFAd7d0ZOmfDOV7Ych5iEZIKkOZG9pm47b8KVVI3/n+AQzCXHgBPOdjaOGxiFxRHsWGGatd2ohNXi6f9HUop0TFIJxZ8Jf/Bu4hQL3tRG/593SX6vQwWg+y0Lc7EzXjVjVWVUeIZ8FJvwv5B/HSDN1O2rL+TP8Z6JnvVzMIExi0wYMVZmZ+XwOUxgQEQIFJ2s11E3EyhxD5P1TAS8rY6YePgOp/rdA9xMbsSTcW1TXZFc8RdzCGxSquwQERrDGBHIYpv1s/RTKbQ+pGwduLVwjWfh4kFn0Pk1VCGa2mGj6zdk1YxD6g9kZaaJXASBvesp3+l+fEVTGioAjhzugb+vyV1zPY63n7wPWSru8VEm4GhUKb17Z95sbMtF8YV5Ga1FQg83RvcqRL0DKxaAQ1JpNMHokm9a/SEssAthZFWxCQ=="

managers = {}
nfs_list = []
cakey = createKeyPair(crypto.TYPE_RSA,1024)
careq = createCertRequest(cakey, CN="Certificate Authority")
cacert = createCertificate(careq, (careq, cakey), 0, (0, 60 * 60 * 24 * 365 * 10))  # ten years

app = Flask(__name__)

def initController():
    #sock = socket.socket(socket.AF_INET,socket.SOCK_DGRAM) # UDP
    #sock.sendto(str(time.time()), ('0.0.0.0', 514))

    @app.route('/register', methods=['POST'])
    def post():
        content = request.get_json()
        nfs = content['control_network_ip'] + "-" + content['data_network_ip'] + "-" + content['cert_req']
        req = "-----BEGIN CERTIFICATE REQUEST-----\n" + content['cert_req'] + "\n-----END CERTIFICATE REQUEST-----"
        nfs_list.append(nfs)
        num_nsfs = len(nfs_list)

        nfs_ssh_key = ssh_key.replace("CONTROL-IP",content['control_network_ip'])
        ssh_key_file = open("/root/.ssh/known_hosts","a")
        ssh_key_file.write(nfs_ssh_key+"\n")
        ssh_key_file.close()

        configureServer(req,nfs.split("-")[0],nfs.split("-")[1])

        # Set up of TLS associations between all stable nodes #
        if num_nsfs > 1:
            servers = []
            for active_nfs in range(0,num_nsfs-1,1):
                servers.append(nfs_list[active_nfs].split("-")[1])
            createSA(nfs.split("-")[0],nfs.split("-")[1],servers)
        # Set up of TLS associations between all stable nodes #

        #if num_nsfs == 50:
            #sock.sendto(str(time.time()), ('0.0.0.0', 514))


        return 'OK'

    app.run(host='0.0.0.0', port=5000)

def configureServer(req, nfs_control, nfs_data):

    # ca cert
    ca_cert = crypto.dump_certificate(crypto.FILETYPE_PEM, cacert).decode('ascii')
    ca_cert = ca_cert[ca_cert.find("-----BEGIN CERTIFICATE-----")+28:ca_cert.find("-----END CERTIFICATE-----")-1]

    # server cert
    cert_req = crypto.load_certificate_request(crypto.FILETYPE_PEM,req)
    cert = createCertificate(cert_req, (cacert, cakey), len(nfs_list), (0, 60 * 60 * 24 * 365 * 10))  # ten years
    server_cert = crypto.dump_certificate(crypto.FILETYPE_PEM, cert).decode('ascii')
    server_cert = server_cert[server_cert.find("-----BEGIN CERTIFICATE-----")+28:server_cert.find("-----END CERTIFICATE-----")-1]
    file_cert = open("/tmp/cert."+nfs_control+".pem","w")
    file_cert.write(server_cert)
    file_cert.close()

    ## netconf config
    m = manager.connect_ssh(host=nfs_control, port=830, timeout=None, username="javier", password=None, key_filename=None, allow_agent=True, hostkey_verify=True, look_for_keys=True, ssh_config=None)
    m.async_mode = True
    managers[nfs_control] = m
    snippet = etree.tostring(etree.parse("/py-sc/server-conf-autostart.xml"), pretty_print=True)
    snippet = snippet.replace("CA-CERTIFICATE",ca_cert)
    snippet = snippet.replace("SERVER-CERTIFICATE",server_cert)
    m.edit_config(target='running', config=snippet, test_option='test-then-set')

def createSA(client_control, client_data, servers):
    configuration = open("/py-sc/client-conf-autostart.xml","r")
    configuration_model = configuration.read()

    # load ca cert #
    #ca_cert = crypto.dump_certificate(crypto.FILETYPE_PEM, cacert)
    #ca_cert = ca_cert[ca_cert.find("-----BEGIN CERTIFICATE-----")+28:ca_cert.find("-----END CERTIFICATE-----")-1]

    # load client cert #
    #client_key = client_keys[client_keys.find("-----BEGIN PRIVATE KEY-----")+28:client_keys.find("-----END PRIVATE KEY-----")-1]
    fd = open("/tmp/cert."+client_control+".pem","r")
    client_cert = fd.read()

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

        #client_conf = client_conf.replace("CA-CERT",ca_cert)
        client_conf = client_conf.replace("CLIENT-CERT",client_cert)
        #client_conf = client_conf.replace("CLIENT-PRIVATE-KEY",client_key)

        client_configuration = client_configuration + client_conf

    client_configuration = client_configuration + "</config>"
    managers[client_control].edit_config(target='running', config=client_configuration, test_option='test-then-set')
    configuration.close()


if __name__ == '__main__':
    print("Starting the controller...")
    #logging.basicConfig(filename='controller.log')
    #logging.getLogger("paramiko").setLevel(logging.DEBUG)
    initController()
