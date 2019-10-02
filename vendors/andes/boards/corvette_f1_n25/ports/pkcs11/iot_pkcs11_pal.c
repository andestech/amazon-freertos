/* Copyright Statement:
 *
 * Amazon FreeRTOS PKCS glue layer.
 * (c) 2018 MediaTek Inc. or its affiliates. All Rights Reserved.
 *
 * Amazon FreeRTOS PKCS #11 PAL for MT7697Hx-Dev-Kit V1.0.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

/**
 * @file aws_pkcs11_pal.c
 * @brief MT7697H specific helpers for PKCS11 Interface.
 */

/*-----------------------------------------------------------*/

/* Amazon FreeRTOS Includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "iot_crypto.h"
#include "iot_pkcs11.h"
#include "iot_pkcs11_config.h"
#include "semphr.h"

/* C runtime includes. */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* flash driver includes. */
#include "i2c_eeprom.h"


/*-----------------------------------------------------------*/

/* define the storage address of flash*/
#define SIZE_1K				1024
#define CertificateData_SIZE		( 2*SIZE_1K - 2*(sizeof(CK_ULONG)))
#define EEPROM_KEY_ADDRESS		( 0x0 )
#define pkcs11OBJECT_FLASH_CERT_EXIST	( 0xf0f0f0f0uL )

/*define nv seed address*/
#define NV_SEED_ADDRESS			(uint32_t)(8*SIZE_1K)

enum eObjectHandles
{
    eInvalidHandle = 0, /* According to PKCS #11 spec, 0 is never a valid object handle. */
    eAwsDevicePrivateKey = 1,
    eAwsDevicePublicKey,
    eAwsDeviceCertificate,
    eAwsCodeSigningKey
};

/*-----------------------------------------------------------*/
/* key information*/

typedef struct
{
    CK_ULONG ulCertificateExist;
    CK_ULONG ulCertificateSize;
    CK_CHAR  cCertificateData[ CertificateData_SIZE ];/* size of KeyData equal 2K - 2 */
} KeyData;

typedef struct
{
    KeyData xDeviceCertificate;
    KeyData xDeviceKey;
    KeyData xCodeVerificationKey;
} KeySet;

static KeySet P11key_Save;

/*-----------------------------------------------------------*/

static SemaphoreHandle_t Flashing;

/*-----------------------------------------------------------*/

/**
 * @brief Saves an object in non-volatile storage.
 *
 * Port-specific file write for cryptographic information.
 *
 * @param[in] pxLabel       The label of the object to be stored.
 * @param[in] pucData       The object data to be saved
 * @param[in] pulDataSize   Size (in bytes) of object data.
 *
 * @return The object handle if successful.
 * eInvalidHandle = 0 if unsuccessful.
 */


CK_OBJECT_HANDLE PKCS11_PAL_SaveObject( CK_ATTRIBUTE_PTR    pxLabel,
					uint8_t             *pucData,
					uint32_t            ulDataSize )
{
    CK_OBJECT_HANDLE xHandle = eInvalidHandle;
    KeyData * pEEPROM_CertData;//for data verify
    KeyData * pSave_CertData = 0;  //point to which key need to be save
    KeySet * const KeySet_EEPROMPtr = ( KeySet *) ( EEPROM_KEY_ADDRESS );
    KeySet * KeySet_SavePtr = ( KeySet * ) (( uint32_t )&P11key_Save);

    uint8_t * Data_Receive = ( uint8_t * ) pvPortMalloc( 2 * SIZE_1K );

    if (ulDataSize <= CertificateData_SIZE )
    {
        /* Translate from the PKCS#11 label to local storage file name. */
        if( 0 == memcmp( pxLabel->pValue,
                         &pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS,
                         sizeof( pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS ) ) )
        {
            pSave_CertData = &KeySet_SavePtr->xDeviceCertificate;
            pEEPROM_CertData = &KeySet_EEPROMPtr->xDeviceCertificate;
            xHandle = eAwsDeviceCertificate;
        }
        else if( 0 == memcmp( pxLabel->pValue,
                              &pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS ) ) )
        {
            pSave_CertData = &KeySet_SavePtr->xDeviceKey;
            pEEPROM_CertData = &KeySet_EEPROMPtr->xDeviceKey;
            xHandle = eAwsDevicePrivateKey;
        }
        else if( 0 == memcmp( pxLabel->pValue,
                              &pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS,
                              sizeof( pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS ) ) )
        {
            pSave_CertData = &KeySet_SavePtr->xDeviceKey;
            pEEPROM_CertData = &KeySet_EEPROMPtr->xDeviceKey;
            xHandle = eAwsDevicePublicKey;
        }
        else if( 0 == memcmp( pxLabel->pValue,
                              &pkcs11configLABEL_CODE_VERIFICATION_KEY,
                              sizeof( pkcs11configLABEL_CODE_VERIFICATION_KEY ) ) )
        {
            pSave_CertData = &KeySet_SavePtr->xCodeVerificationKey;
            pEEPROM_CertData = &KeySet_EEPROMPtr->xCodeVerificationKey;
            xHandle = eAwsCodeSigningKey;
        }

        if( pSave_CertData != 0 )
        {
            if(xSemaphoreTake( Flashing, portMAX_DELAY) == pdFALSE){
                vPortFree(Data_Receive);
                return eInvalidHandle;
            }

            memcpy( pSave_CertData->cCertificateData, pucData, sizeof(KeyData) - 2*(sizeof(CK_ULONG)));
            pSave_CertData->ulCertificateExist = pkcs11OBJECT_FLASH_CERT_EXIST;
            pSave_CertData->ulCertificateSize = ulDataSize;

            if(EEPROM_Write( (uint32_t)pEEPROM_CertData, (uint8_t *)pSave_CertData, sizeof(KeyData)) != 0)
            {
               xHandle = eInvalidHandle;
            }

            if(EEPROM_Read((uint32_t)pEEPROM_CertData, Data_Receive, sizeof(KeyData))!=0)
            {
                xHandle = eInvalidHandle;
            }

            if(memcmp(Data_Receive, pSave_CertData, sizeof(KeyData)) != 0)
            {
                xHandle = eInvalidHandle;
            }

            xSemaphoreGive(Flashing);
        }
    }

    vPortFree(Data_Receive);
    return xHandle;
}

/**
 * @brief Translates a PKCS #11 label into an object handle.
 *
 * Port-specific object handle retrieval.
 *
 *
 * @param[in] pLabel         Pointer to the label of the object
 *                           who's handle should be found.
 * @param[in] usLength       The length of the label, in bytes.
 *
 * @return The object handle if operation was successful.
 * Returns eInvalidHandle if unsuccessful.
 */

CK_OBJECT_HANDLE PKCS11_PAL_FindObject( uint8_t * pLabel,
					uint8_t usLength ){

    CK_OBJECT_HANDLE xHandle = eInvalidHandle;

    if( 0 == memcmp( pLabel, pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS, usLength )  )
    {
        xHandle = eAwsDeviceCertificate;

    }
    else if( 0 == memcmp( pLabel, pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS, usLength ) )
    {
        xHandle = eAwsDevicePrivateKey;
    }
    else if( 0 == memcmp( pLabel, pkcs11configLABEL_DEVICE_PUBLIC_KEY_FOR_TLS, usLength ) )
    {
        xHandle = eAwsDevicePublicKey;
    }
    else if( 0 == memcmp( pLabel, pkcs11configLABEL_CODE_VERIFICATION_KEY, usLength ) )
    {
        xHandle = eAwsCodeSigningKey;
    }

    if(xSemaphoreTake( Flashing, portMAX_DELAY) == pdFALSE){
        return eInvalidHandle;
    }

    xSemaphoreGive(Flashing);

    return xHandle;
}


/**
 * @brief Gets the value of an object in storage, by handle.
 *
 * Port-specific file access for cryptographic information.
 *
 * This call dynamically allocates the buffer which object value
 * data is copied into.  PKCS11_PAL_GetObjectValueCleanup()
 * hould be called after each use to free the dynamically allocated
 * buffer.
 *
 * @sa PKCS11_PAL_GetObjectValueCleanup
 *
 * @param[in] pcFileName    The name of the file to be read.
 * @param[out] ppucData     Pointer to buffer for file data.
 * @param[out] pulDataSize  Size (in bytes) of data located in file.
 * @param[out] pIsPrivate   Boolean indicating if value is private (CK_TRUE)
 *                          or exportable (CK_FALSE)
 *
 * @return CKR_OK if operation was successful.  CKR_KEY_HANDLE_INVALID if
 * no such object handle was found, CKR_DEVICE_MEMORY if memory for
 * buffer could not be allocated, CKR_FUNCTION_FAILED for device driver
 * error.
 */

CK_RV PKCS11_PAL_GetObjectValue( CK_OBJECT_HANDLE xHandle,
				 uint8_t ** ppucData,
				 uint32_t * pulDataSize,
				 CK_BBOOL * pIsPrivate ){
    CK_RV xResult = CKR_OBJECT_HANDLE_INVALID;
    KeyData * pCertData = 0;

    KeyData * pEEPROM_CertData;
    KeySet * const KeySet_EEPROMPtr = ( KeySet *) ( EEPROM_KEY_ADDRESS );

    if( xHandle == eAwsDeviceCertificate )
    {
        pEEPROM_CertData = &KeySet_EEPROMPtr->xDeviceCertificate;
        pCertData = &(P11key_Save.xDeviceCertificate);
        *pIsPrivate = CK_FALSE;
    }
    else if( xHandle == eAwsDevicePrivateKey )
    {
        pEEPROM_CertData = &KeySet_EEPROMPtr->xDeviceKey;
        pCertData = &(P11key_Save.xDeviceKey);
        *pIsPrivate = CK_TRUE;
    }
    else if( xHandle == eAwsDevicePublicKey )
    {
        pEEPROM_CertData = &KeySet_EEPROMPtr->xDeviceKey;
        pCertData = &(P11key_Save.xDeviceKey);
        *pIsPrivate = CK_FALSE;
    }
    else if( xHandle == eAwsCodeSigningKey )
    {
        pEEPROM_CertData = &KeySet_EEPROMPtr->xCodeVerificationKey;
        pCertData = &(P11key_Save.xCodeVerificationKey);
        *pIsPrivate = CK_FALSE;
    }
    else
    {
        pEEPROM_CertData = NULL;
        xResult = CKR_OBJECT_HANDLE_INVALID;
    }

    if(xSemaphoreTake( Flashing, portMAX_DELAY) == pdFALSE){
        return eInvalidHandle;
    }

    if(EEPROM_Read((uint32_t)pEEPROM_CertData, (uint8_t *)pCertData, sizeof(KeyData)) != 0){
        pCertData = NULL;
    }

    if( ( pCertData != NULL ) && ( pCertData->ulCertificateExist == pkcs11OBJECT_FLASH_CERT_EXIST ) )
    {
        *pulDataSize = pCertData->ulCertificateSize;
        *ppucData = (uint8_t *)(pCertData->cCertificateData);
        xResult = CKR_OK;
    }

    xSemaphoreGive(Flashing);

    return xResult;
}

/**
 * @brief Cleanup after PKCS11_GetObjectValue().
 *
 * @param[in] pucData       The buffer to free.
 *                          (*ppucData from PKCS11_PAL_GetObjectValue())
 * @param[in] ulDataSize    The length of the buffer to free.
 *                          (*pulDataSize from PKCS11_PAL_GetObjectValue())
 */

void PKCS11_PAL_GetObjectValueCleanup( uint8_t * pucData,
				       uint32_t ulDataSize )
{
    /* Unused parameters. */
    ( void ) pucData;
    ( void ) ulDataSize;
}

void Init_PKCS11(){

    Flashing = xSemaphoreCreateBinary();
    xSemaphoreGive(Flashing);
    EEPROM_Initial();
}


int nv_seed_read_func( unsigned char *buf, size_t buf_len ){

    if(xSemaphoreTake( Flashing, portMAX_DELAY) == pdFALSE)
    {
        return eInvalidHandle;
    }
    portENABLE_INTERRUPTS();

    if(EEPROM_Read( NV_SEED_ADDRESS, buf, buf_len) != 0)
    {
        xSemaphoreGive(Flashing);
        return -1;
    }
    xSemaphoreGive(Flashing);
    return 0;
}

int nv_seed_write_func( unsigned char *buf, size_t buf_len ){

    if(xSemaphoreTake( Flashing, portMAX_DELAY) == pdFALSE)
    {
        return eInvalidHandle;
    }
    portENABLE_INTERRUPTS();

    if(EEPROM_Write( NV_SEED_ADDRESS, buf, buf_len)!=0)
    {
        xSemaphoreGive(Flashing);
        return -1;
    }
    xSemaphoreGive(Flashing);
    return 0;
}
