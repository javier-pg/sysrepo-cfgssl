FROM ubuntu:16.04

RUN echo jiji
RUN apt-get update
RUN apt-get -y install tcpdump bc netcat net-tools supervisor curl wget libreadline-dev libxml2-dev python-libxml2 dh-autoreconf libtool-bin libxslt1-dev libcurl4-openssl-dev xsltproc python-setuptools cmake zlib1g-dev libssl-dev git pkg-config python3-dev python-pip
RUN pip install pyang

RUN cd && \
git clone -b v0-7 https://git.libssh.org/projects/libssh.git && \
cd libssh && \
mkdir build && \
cd build && \
cmake .. && \
make && \
make install

RUN cd && \
git clone https://github.com/CESNET/libnetconf.git && \
cd libnetconf && \
autoreconf --force --install && \
./configure && \
make && \
make install

RUN cd && \
git clone https://github.com/CESNET/netopeer.git && \
cd netopeer/server && \
./configure && \
make && \
make install

RUN mkdir -p /root/.ssh/ && \
cd && \
git clone https://github.com/openssl/openssl.git && \
cd openssl && \
./config no-shared && \
make && \
# make test && \
make install

RUN pip install --upgrade "pip < 21.0"
RUN pip install pyopenssl

# NETCONF: TLS functionality in nfs
COPY cfgssl/cfgssl-server /usr/local/etc/netopeer/cfgssl/cfgssl-server

RUN cd /usr/local/etc/netopeer/cfgssl/cfgssl-server && \
lnctool --model ietf-tls-server@2017-10-30.yang transapi --paths paths && \
autoreconf --force --install && \
./configure && \
make clean

RUN cd /usr/local/etc/netopeer/cfgssl/cfgssl-server && \
cp -f original/ietf-tls-server@2017-10-30.c . && \
cp -f original/Makefile . && \
make && \
make install && \
touch /usr/local/etc/netopeer/cfgnetopeer/datastore.xml && \
touch /usr/local/etc/netopeer/cfgssl/cfgssl-server/datastore.xml && \
netopeer-manager add --name cfgssl-server --model /usr/local/etc/netopeer/cfgssl/cfgssl-server/ietf-tls-server@2017-10-30.yin --transapi /usr/local/lib/ietf-tls-server@2017-10-30.so --datastore /usr/local/etc/netopeer/cfgssl/cfgssl-server/datastore.xml && \
netopeer-manager add --name cfgssl-server --import /usr/local/etc/netopeer/cfgssl/cfgssl-server/yin_modules/ietf-tls-common@2017-10-30.yin && \
netopeer-manager add --name cfgssl-server --import /usr/local/etc/netopeer/cfgssl/cfgssl-server/yin_modules/ietf-keystore.yin


# NETCONF : TLS client functionality
COPY cfgssl/cfgssl-client /usr/local/etc/netopeer/cfgssl/cfgssl-client

RUN cd /usr/local/etc/netopeer/cfgssl/cfgssl-client && \
lnctool --model ietf-tls-client@2017-10-30.yang transapi --paths paths && \
autoreconf --force --install && \
./configure && \
make clean

RUN cd /usr/local/etc/netopeer/cfgssl/cfgssl-client  && \
cp -f original/ietf-tls-client@2017-10-30.c . && \
cp -f original/Makefile . && \
make && \
make install && \
touch /usr/local/etc/netopeer/cfgnetopeer/datastore.xml && \
touch /usr/local/etc/netopeer/cfgssl/cfgssl-client/datastore.xml && \
netopeer-manager add --name cfgssl-client --model /usr/local/etc/netopeer/cfgssl/cfgssl-client/ietf-tls-client@2017-10-30.yin --transapi /usr/local/lib/ietf-tls-client@2017-10-30.so --datastore /usr/local/etc/netopeer/cfgssl/cfgssl-client/datastore.xml && \
netopeer-manager add --name cfgssl-client --import /usr/local/etc/netopeer/cfgssl/cfgssl-client/yin_modules/ietf-tls-common@2017-10-30.yin && \
netopeer-manager add --name cfgssl-client --import /usr/local/etc/netopeer/cfgssl/cfgssl-client/yin_modules/ietf-keystore@2017-10-30.yin && \
netopeer-manager list

COPY conf/id_rsa.pub /root/.ssh/
COPY conf/id_rsa /root/.ssh/
RUN cat /root/.ssh/id_rsa > /etc/ssh/ssh_host_rsa_key
COPY conf/startup-config.xml /usr/local/etc/netopeer/cfgnetopeer/datastore.xml
COPY conf/startup-acm-config.xml /var/lib/libnetconf/datastore-acm.xml

COPY misc/register.sh /etc/register.sh
COPY misc/generate-keys.py /etc/generate-keys.py
RUN chmod +x /etc/register.sh
RUN chmod +x /etc/generate-keys.py

COPY misc/supervisord.conf /etc/supervisord.conf

# install openssl 1.1.1 (which supports tls1.3)
RUN wget https://www.openssl.org/source/openssl-1.1.1g.tar.gz && \
tar -zxf openssl-1.1.1g.tar.gz && cd openssl-1.1.1g && \
./config && \
make && \
mv /usr/bin/openssl ~/tmp && \
make install && \
ln -s /usr/local/bin/openssl /usr/bin/openssl && \
ldconfig

# install apache 2.4 (which supports tls1.3)
RUN apt-get install -y libapr1-dev libaprutil1-dev libpcre3-dev
RUN wget https://ftp.cixug.es/apache//httpd/httpd-2.4.46.tar.gz && \
gzip -d httpd-2.4.46.tar.gz && \
tar xvf httpd-2.4.46.tar && \
cd httpd-2.4.46 && \
./configure && \
make && \
make install
COPY conf/httpd.conf /usr/local/apache2/conf/httpd.conf

CMD ["/usr/bin/supervisord", "-c", "/etc/supervisord.conf"]

#HEALTHCHECK --interval=5s --timeout=3s --start-period=3s --retries=3 CMD nc -z localhost 830
