
typedef struct MY_TYPE_ {
    const char *server;
    const char *serverPort;
    const char *name;
    const char *p_pskId;
    char *psk;
    uint16_t pskLen;
    lwm2m_context_t *lwm2mH;
    //struct timeval tv;
    uint32_t myTv;
#ifdef LWM2M_BOOTSTRAP
    lwm2m_client_state_t previousState;
#endif
} MY_TYPE;

int myClientInit(void);


