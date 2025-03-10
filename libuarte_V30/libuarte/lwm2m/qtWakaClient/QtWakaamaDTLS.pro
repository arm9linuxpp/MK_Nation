QT       += core gui
QT += network
QT +=  serialport


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += console

# WE use BG97 or BG777   http://192.168.45.1:17685
DEFINES += BG95

# QT directive we send via AT command the UDS! ( not for NRF52 =
#DEFINES += USE_BG95

# tcpdump -i lo -n udp port 5683 -X

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#DEFINES += LWM2M_BOOTSTRAP

DEFINES += LWM2M_CLIENT_MODE LWM2M_COAP_DEFAULT_BLOCK_SIZE=1024 LWM2M_LITTLE_ENDIAN SHA2_USE_INTTYPES_H LWM2M_SUPPORT_TLV

#DEFINES += LWM2M_SUPPORT_SENML_JSON

DEFINES += WITH_SHA256

# needed to read data from leshan
#DEFINES += LWM2M_SUPPORT_JSON



#DEFINES += LWM2M_FIRMWARE_UPGRADES
#DEFINES += LWM2M_DEVICE_INFO_WITH_TIME

# Qt Windows needed
DEFINES += CRT_SECURE_NO_WARNINGS

# no debugging next 2 off
DEFINES += LWM2M_WITH_LOGS

DEFINES += LWM2M_LOG_LEVEL=DEBUG
# dtls_debug.log
# see log level static int maxlog =  DTLS_LOG_WARN;      /* default maximum log level */ in



# we do not use POSIX !
DEFINES += NO_SYSTEM_SOCKETS

# must be both DT_COT + MU_DTLS
# leshan example in code! searc for DT_COT
#DEFINES += DT_COT


#DEFINES += NDEBUG
#DEFINES += DTLS_ECC
#DEFINES += DTLS_PSK

#DEFINES += WITH_LWIP
#DEFINES += IS_WINDOWS
#DEFINES += DTLS_PEERS_NOHASH

# uncomment for DTLS ( both)
DEFINES += MU_DTLS
DEFINES += WITH_TINYDTLS

# kill DTLS debug imp!
#DEFINES += NDEBUG

# Vfprintf for dtls debug log.
DEFINES += HAVE_VPRINTF


# cmake --build build-client-tinydtls -v
# cmake --build build-client-tinydtls --target clean
#cd /home/mza/work/git/wakaama_2024_11/wakaama/build-client-tinydtls/wakaama_lib && /usr/bin/cc
#-DLWM2M_LITTLE_ENDIAN -DSHA2_USE_INTTYPES_H -DWITH_SHA256 -DWITH_TINYDTLS -D_POSIX_C_SOURCE=200809

# from DTTRACKER
#DEFINES += BG95
DEFINES += WITH_VI_DEBUG_LEVEL


#// Client mode, posix, with json and dtls
#DEFINES += LWM2M_CLIENT_MODE LWM2M_SUPPORT_JSON LWM2M_WITH_DTLS LWM2M_WITH_LOGS LWM2M_FIRMWARE_UPGRADES LWM2M_DEVICE_INFO_WITH_TIME
#// Enable logs
#// Enable additional features



WAKAPATH = ..\wakaama


SOURCES += \
    qmain.cpp \
    mainwindow.cpp \
    qtlwm2mclient.c \
    viconnection_dtls.c \
    $${WAKAPATH}\examples\shared\platform.c \ 
    $${WAKAPATH}\core\bootstrap.c \
    $${WAKAPATH}\core\discover.c \
    $${WAKAPATH}\core\liblwm2m.c \
    $${WAKAPATH}\core\list.c \
    $${WAKAPATH}\core\management.c \
    $${WAKAPATH}\core\objects.c \
    $${WAKAPATH}\core\observe.c \
    $${WAKAPATH}\core\packet.c \
    $${WAKAPATH}\core\registration.c \
    $${WAKAPATH}\core\uri.c \
    $${WAKAPATH}\core\utils.c \
    $${WAKAPATH}\core\logging.c \
    $${WAKAPATH}\data\data.c \
    $${WAKAPATH}\data\tlv.c \
    $${WAKAPATH}\data\senml_json.c \
    $${WAKAPATH}\data\json.c \
    $${WAKAPATH}\data\json_common.c \
    $${WAKAPATH}\examples\client\object_device.c \
    $${WAKAPATH}\examples\client\object_firmware.c \
    $${WAKAPATH}\examples\client\object_location.c \
    $${WAKAPATH}\examples\client\object_test.c \
    $${WAKAPATH}\examples\client\object_temperature.c \
    $${WAKAPATH}\examples\client\object_binary.c \
    $${WAKAPATH}\examples\client\object_server.c \
    $${WAKAPATH}\examples\client\object_connectivity_moni.c \
    $${WAKAPATH}\examples\client\object_connectivity_stat.c \
    $${WAKAPATH}\examples\client\object_access_control.c \
    $${WAKAPATH}\examples\client\system_api.c \
    $${WAKAPATH}\examples\client\object_security.c \
    $${WAKAPATH}\coap\transaction.c \
    $${WAKAPATH}\coap\block.c \
    $${WAKAPATH}\coap\er-coap-13\er-coap-13.c \
    qtipv4ud.cpp \
    qtsimpleudp.cpp \
    tinydtls\ccm.c \
    tinydtls\crypto.c \
    tinydtls\dtls.c \
    tinydtls\dtls_debug.c \
    tinydtls\dtls_prng.c \
    tinydtls\dtls_time.c \
    tinydtls\hmac.c \
    tinydtls\netq.c \
    tinydtls\peer.c \
    tinydtls\session.c \
    tinydtls\aes\rijndael_wrap.c \
    tinydtls\sha2\sha2.c \
    tinydtls\ecc\ecc.c \
    tinydtls\aes\rijndael.c \    
    fake_time.c \
    ..\ViApi\vimoapi.c \
    ..\ViApi\vidummy.cpp

# ?! modem_sim_udp.c \

HEADERS += \
    mainwindow.h \
    win_fake_include\unistd.h \
    win_fake_include\sys\socket.h \
    win_fake_include\sys\select.h \
    win_fake_include\sys\stat.h \
    dtls_config.h \
    qtipv4ud.h \
    qtsimpleudp.h

INCLUDEPATH  += $${WAKAPATH}\include
INCLUDEPATH  += $${WAKAPATH}\examples\shared
INCLUDEPATH  += $${WAKAPATH}\core
INCLUDEPATH  += win_fake_include
INCLUDEPATH  += $${WAKAPATH}\coap
INCLUDEPATH  += $${WAKAPATH}\transport\tinydtls\include
INCLUDEPATH  += $${WAKAPATH}\examples\client



#$${WAKAPATH}\qtWakaClient\tinydtls
INCLUDEPATH  += tinydtls

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
