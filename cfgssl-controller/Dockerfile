FROM ubuntu:16.04

RUN apt-get update
RUN apt-get install -y bc supervisor iproute2 tcpdump iputils-ping curl libpam0g-dev python python-pip git
RUN pip install Flask
RUN pip install pyopenssl
RUN pip install bcrypt==3.1.7
RUN pip install paramiko==2.5.0
RUN pip install ncclient==0.6.3

RUN apt-get -y install netcat

COPY conf/id_rsa.pub /root/.ssh/
COPY conf/id_rsa /root/.ssh/

RUN cat /root/.ssh/id_rsa > /etc/ssh/ssh_host_rsa_key
RUN echo "    StrictHostKeyChecking no" >> /etc/ssh/ssh_config

COPY misc/supervisord.conf /etc/supervisord.conf
COPY misc/configure_mesh.sh /etc/configure_mesh.sh

RUN chmod +x /etc/configure_mesh.sh

CMD ["/usr/bin/supervisord", "-c", "/etc/supervisord.conf"]

HEALTHCHECK --interval=5s --timeout=3s --start-period=3s --retries=3 CMD nc -z localhost 5000
