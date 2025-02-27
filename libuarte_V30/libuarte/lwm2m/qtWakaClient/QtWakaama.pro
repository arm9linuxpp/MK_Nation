
# client c call
#  git clone --recurse-submodules https://github.com/eclipse/wakaama.git
#  cmake -S examples/client -B build-client -DWAKAAMA_MODE_CLIENT=ON -DWAKAAMA_CLIENT_INITIATED_BOOTSTRAP=ON -DWAKAAMA_ENABLE_EXAMPLES=ON -DWAKAAMA_DATA_SENML_JSON=OFF
# cmake --build build-client
# ./build-client/lwm2mclient
#  ./lwm2mclient -4 -h lwm2m.mudip.com -p 5683 -n urn:imei:86448606188409

QT       += core gui
QT += network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += console


# tcpdump -i lo -n udp port 5683 -X

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#DEFINES += LWM2M_BOOTSTRAP

DEFINES += LWM2M_CLIENT_MODE
DEFINES += LWM2M_COAP_DEFAULT_BLOCK_SIZE=1024
# Win11 LE und DSPIC LE!
DEFINES += LWM2M_LITTLE_ENDIAN
#DEFINES += LWM2M_SUPPORT_SENML_JSON
DEFINES += SHA2_USE_INTTYPES_H
DEFINES += WITH_SHA256

# needed to read data from leshan
#DEFINES += LWM2M_SUPPORT_JSON

# standard for leshan
DEFINES += LWM2M_SUPPORT_TLV

#DEFINES += LWM2M_FIRMWARE_UPGRADES
#DEFINES += LWM2M_DEVICE_INFO_WITH_TIME

# Qt Windows needed
DEFINES += CRT_SECURE_NO_WARNINGS

# no debugging next 2 off
DEFINES += LWM2M_WITH_LOGS

DEFINES += LWM2M_LOG_LEVEL=DEBUG
DEFINES += HAVE_VPRINTF

# we do not use POSIX !
DEFINES += NO_SYSTEM_SOCKETS


#DEFINES += NDEBUG
#DEFINES += DTLS_ECC
#DEFINES += DTLS_PSK

#DEFINES += WITH_LWIP
#DEFINES += IS_WINDOWS
#DEFINES += DTLS_PEERS_NOHASH

# uncomment for DTLS
#DEFINES += MU_DTLS
#DEFINES += WITH_TINYDTLS


#// Client mode, posix, with json and dtls
#DEFINES += LWM2M_CLIENT_MODE LWM2M_SUPPORT_JSON LWM2M_WITH_DTLS LWM2M_WITH_LOGS LWM2M_FIRMWARE_UPGRADES LWM2M_DEVICE_INFO_WITH_TIME
#// Enable logs
#// Enable additional features

#    ..\data\senml_common.c \

SOURCES += \
    qmain.cpp \
    mainwindow.cpp \
    ..\examples\client\qtlwm2mclient.c \
    ..\examples\shared\platform.c \
    ..\examples\shared\commandline.c \
    ..\core\bootstrap.c \
    ..\core\discover.c \
    ..\core\liblwm2m.c \
    ..\core\list.c \
    ..\core\management.c \
    ..\core\objects.c \
    ..\core\observe.c \
    ..\core\packet.c \
    ..\core\registration.c \
    ..\core\uri.c \
    ..\core\utils.c \
    ..\core\logging.c \
    ..\data\data.c \
    ..\data\tlv.c \
    ..\data\senml_json.c \
    ..\data\json.c \
    ..\data\json_common.c \
    ..\examples\client\object_device.c \
    ..\examples\client\object_firmware.c \
    ..\examples\client\object_location.c \
    ..\examples\client\object_test.c \
    ..\examples\client\object_temperature.c \
    ..\examples\client\object_binary.c \
    ..\examples\client\object_server.c \
    ..\examples\client\object_connectivity_moni.c \
    ..\examples\client\object_connectivity_stat.c \
    ..\examples\client\object_access_control.c \
    ..\examples\client\system_api.c \
    ..\examples\client\object_security.c \
    ..\coap\transaction.c \
    ..\coap\block.c \
    ..\coap\er-coap-13\er-coap-13.c \
    ..\transport\udp\viconnection.c \
    qtipv4ud.cpp \
    qtsimpleudp.cpp \    
    modem_sim_udp.c \
    fake_time.c

# unix like
#..\examples\shared\connection.c \
#    ..\examples\shared\dtlsconnection.c \

HEADERS += \
    mainwindow.h \
    win_fake_include\unistd.h \
    win_fake_include\sys\socket.h \
    win_fake_include\sys\select.h \
    win_fake_include\sys\stat.h \
    dtls_config.h \
    qtipv4ud.h \
    qtsimpleudp.h
#    include\liblwm2m.h

# C:\work\git\wakaamaWithModemUds\include
# C:\Users\mza\Downloads\wakaama-master\include
INCLUDEPATH  += ..\include
INCLUDEPATH  += ..\examples\shared
INCLUDEPATH  += ..\core
INCLUDEPATH  += win_fake_include
INCLUDEPATH  += ..\coap
INCLUDEPATH  += ..\transport\udp\include


FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
