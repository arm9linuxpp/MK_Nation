// https://github.com/eclipse-wakaama/wakaama/issues/417

#include "liblwm2m.h"
#include "lwm2mclient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Resource Id's:
// KEY to COT
#define REQ_BINARY_VALUE    0

// COT to us
#define RES_BINARY_VALUE    1

// #define LWM2M_BINARY_OBJECT_ID 19
#define LWM2M_BINARY_OBJECT_ID 34799

typedef struct
{
    uint16_t    shortID;    
    char      buffer[255];
    size_t       length;
    unsigned long timestamp;
} binary_data_t;

static uint8_t prv_set_value(lwm2m_data_t* dataP,
                           binary_data_t* tempData)
{
    uint8_t ret = COAP_205_CONTENT;  
    switch (dataP->id)     // temperature resourceId
    {
    case REQ_BINARY_VALUE:
    case RES_BINARY_VALUE:
  //      lwm2m_data_encode_opaque(tempData->buffer, tempData->length, dataP);
        lwm2m_data_encode_string(tempData->buffer,  dataP);
        break;
    default:
        ret = COAP_404_NOT_FOUND;
        break;
    }
  
    return ret;
}

static uint8_t prv_binary_read(lwm2m_context_t * contextP, uint16_t instanceId, int * numDataP, lwm2m_data_t ** tlvArrayP, lwm2m_object_t * objectP)

{
    int     i;
    uint8_t result = COAP_500_INTERNAL_SERVER_ERROR;
    binary_data_t* tempData = (binary_data_t*)(objectP->userData);

    // defined as single instance object!
    if (instanceId != 0) return COAP_404_NOT_FOUND;

    if (*numDataP == 0)     // full object, readable resources!
    {
        uint16_t readResIds[] = {
                REQ_BINARY_VALUE,
                RES_BINARY_VALUE,
        }; // readable resources!
        
        *numDataP  = sizeof(readResIds)/sizeof(uint16_t);
        *tlvArrayP = lwm2m_data_new(*numDataP);
        if (*tlvArrayP == NULL) return COAP_500_INTERNAL_SERVER_ERROR;
        
        // init readable resource id's
        for (i = 0 ; i < *numDataP ; i++)
        {
            (*tlvArrayP)[i].id = readResIds[i];
        }
    }
    
    for (i = 0 ; i < *numDataP ; i++)
    {
        result = prv_set_value((*tlvArrayP)+i, tempData);
        if (result != COAP_205_CONTENT) break;
    }
    
    return result;
}

//xxx from firmware!!!
static uint8_t prv_binary_write(lwm2m_context_t *contextP,
                                  uint16_t instanceId,
                                  int numData,
                                  lwm2m_data_t * dataArray,
                                  lwm2m_object_t * objectP,
                                  lwm2m_write_type_t writeType)
{
    int i;
    uint8_t result;

    binary_data_t* tempData = (binary_data_t*)(objectP->userData);


           // All write types are treated the same here
    (void)writeType;

           // this is a single instance object
    if (instanceId != 0)
    {
        return COAP_404_NOT_FOUND;
    }

    i = 0;

    do
    {
        /* firmware was not multiple
        // No multiple instance resources
        if (dataArray[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE)
        {
            result = COAP_404_NOT_FOUND;
            continue;
        }
*/

        switch (dataArray[i].id)
        {

        case REQ_BINARY_VALUE:
        case RES_BINARY_VALUE:

            // URL for download the firmware
            // check type?
            tempData->length = dataArray->value.asBuffer.length;
            memcpy(tempData->buffer,dataArray->value.asBuffer.buffer,tempData->length);
            tempData->buffer[tempData->length] = 0;
            result = COAP_204_CHANGED;
            break;

        default:
            result = COAP_405_METHOD_NOT_ALLOWED;
        }

        i++;
    } while (i < numData && result == COAP_204_CHANGED);

    return result;
}


lwm2m_object_t * get_object_binary()
{
    lwm2m_object_t * binaryObj;

    binaryObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
    
    if (NULL != binaryObj)
    {
        memset(binaryObj, 0, sizeof(lwm2m_object_t));

        binaryObj->objID = LWM2M_BINARY_OBJECT_ID;
        
        // and its unique instance
        binaryObj->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
        
        if (NULL != binaryObj->instanceList)
        {
            memset(binaryObj->instanceList, 0, sizeof(lwm2m_list_t));
        }
        else
        {
            lwm2m_free(binaryObj);
            return NULL;
        }

        binaryObj->readFunc    = prv_binary_read;
        binaryObj->writeFunc   = prv_binary_write;
        binaryObj->userData    = lwm2m_malloc(sizeof(binary_data_t));

        // initialize private data structure containing the needed variables
        if (NULL != binaryObj->userData)
        {
            binary_data_t* data = (binary_data_t*)binaryObj->userData;
            data->shortID = 0;
            data->buffer[0] = 'A';
            data->buffer[1] = 'B';
            data->buffer[2] = 0;            // Terminate
            data->length = 2;
            data->timestamp   = time(NULL);
        }
        else
        {
            lwm2m_free(binaryObj);
            binaryObj = NULL;
        }
    }
    
    return binaryObj;
}

void free_object_binary(lwm2m_object_t * object)
{
    lwm2m_list_free(object->instanceList);
    lwm2m_free(object->userData);
    lwm2m_free(object);
}

