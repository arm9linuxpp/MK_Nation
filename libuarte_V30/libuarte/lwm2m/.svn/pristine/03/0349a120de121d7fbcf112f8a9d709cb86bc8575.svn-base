// https://github.com/eclipse-wakaama/wakaama/issues/417

#include "liblwm2m.h"
#include "lwm2mclient.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Resource Id's:
#define RES_SENSOR_VALUE    5700

#define LWM2M_TEMPERATURE_OBJECT_ID 3303

typedef struct
{
    uint16_t    shortID;
    double       temp;
    unsigned long timestamp;
} temperature_data_t;

static uint8_t prv_set_value(lwm2m_data_t* dataP,
                           temperature_data_t* tempData)
{
    uint8_t ret = COAP_205_CONTENT;  
    switch (dataP->id)     // temperature resourceId
    {
    case RES_SENSOR_VALUE:
        lwm2m_data_encode_float(tempData->temp, dataP);
        break;
    default:
        ret = COAP_404_NOT_FOUND;
        break;
    }
  
    return ret;
}

static uint8_t prv_temperature_read(lwm2m_context_t * contextP, uint16_t instanceId, int * numDataP, lwm2m_data_t ** tlvArrayP, lwm2m_object_t * objectP)

{
    int     i;
    uint8_t result = COAP_500_INTERNAL_SERVER_ERROR;
    temperature_data_t* tempData = (temperature_data_t*)(objectP->userData);

    // defined as single instance object!
    if (instanceId != 0) return COAP_404_NOT_FOUND;

    if (*numDataP == 0)     // full object, readable resources!
    {
        uint16_t readResIds[] = {
                RES_SENSOR_VALUE,
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

static uint8_t prv_temperature_write(lwm2m_context_t *contextP,
                                  uint16_t instanceId,
                                  int numData,
                                  lwm2m_data_t * dataArray,
                                  lwm2m_object_t * objectP,
                                  lwm2m_write_type_t writeType)
{
    int i;
    uint8_t result;

    /* unused parameter */
    temperature_data_t* tempData = (temperature_data_t*)(objectP->userData);

    (void)contextP;

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
        /* No multiple instance resources */
        if (dataArray[i].type == LWM2M_TYPE_MULTIPLE_RESOURCE)
        {
            result = COAP_404_NOT_FOUND;
            continue;
        }

        switch (dataArray[i].id)
        {
        case RES_SENSOR_VALUE:
            // inline temperature wirte
            if (dataArray->type == LWM2M_TYPE_STRING) {
                double d = atof(dataArray->value.asBuffer.buffer);
                tempData->temp = d;
                result = COAP_204_CHANGED;
            } else {
                result = COAP_405_METHOD_NOT_ALLOWED;
            }


            break;



        default:
            result = COAP_405_METHOD_NOT_ALLOWED;
        }

        i++;
    } while (i < numData && result == COAP_204_CHANGED);

    return result;
}


lwm2m_object_t * get_object_temperaturen()
{
    lwm2m_object_t * temperatureObj;

    temperatureObj = (lwm2m_object_t *)lwm2m_malloc(sizeof(lwm2m_object_t));
    
    if (NULL != temperatureObj)
    {
        memset(temperatureObj, 0, sizeof(lwm2m_object_t));

        temperatureObj->objID = LWM2M_TEMPERATURE_OBJECT_ID;
        
        // and its unique instance
        temperatureObj->instanceList = (lwm2m_list_t *)lwm2m_malloc(sizeof(lwm2m_list_t));
        
        if (NULL != temperatureObj->instanceList)
        {
            memset(temperatureObj->instanceList, 0, sizeof(lwm2m_list_t));
        }
        else
        {
            lwm2m_free(temperatureObj);
            return NULL;
        }

        temperatureObj->readFunc    = prv_temperature_read;
        temperatureObj->writeFunc   = prv_temperature_write;
        temperatureObj->userData    = lwm2m_malloc(sizeof(temperature_data_t));

        // initialize private data structure containing the needed variables
        if (NULL != temperatureObj->userData)
        {
            temperature_data_t* data = (temperature_data_t*)temperatureObj->userData;
            data->shortID = 0;
            data->temp = 0.0;
            data->timestamp   = time(NULL);
        }
        else
        {
            lwm2m_free(temperatureObj);
            temperatureObj = NULL;
        }
    }
    
    return temperatureObj;
}

void free_object_temperature(lwm2m_object_t * object)
{
    lwm2m_list_free(object->instanceList);
    lwm2m_free(object->userData);
    lwm2m_free(object);
}
