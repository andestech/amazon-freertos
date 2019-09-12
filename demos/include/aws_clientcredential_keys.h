/*
 * Amazon FreeRTOS V201906.00 Major
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

#ifndef AWS_CLIENT_CREDENTIAL_KEYS_H
#define AWS_CLIENT_CREDENTIAL_KEYS_H

/*
 * @brief PEM-encoded client certificate.
 *
 * @todo If you are running one of the Amazon FreeRTOS demo projects, set this
 * to the certificate that will be used for TLS client authentication.
 *
 * @note Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyCLIENT_CERTIFICATE_PEM    "-----BEGIN CERTIFICATE-----\n"\
"MIIDWjCCAkKgAwIBAgIVAOsIG4yy14lxOuX6HUzZGcPaMOlhMA0GCSqGSIb3DQEB\n"\
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n"\
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0xOTA3MTcwMzE5\n"\
"MjdaFw00OTEyMzEyMzU5NTlaMB4xHDAaBgNVBAMME0FXUyBJb1QgQ2VydGlmaWNh\n"\
"dGUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCuZgKwYvZ5NunqrwhT\n"\
"WLcT+twDW3ntyP5YxHUO+rfhiQUftztLZrtv95PVYp3y6tHmT/64VGrSAtkNBrdn\n"\
"is9tDp1uM99uV1xMqWR/iNRLWsvrloYzfO1wIzs51sPSMcfLOqmixT4Dkvu7Qeg6\n"\
"W13lmObvPdJDTAP3/Czi78pwpBhMovpU1LG/lBDpZLSNaaRGx23lTaU81vD7LQWm\n"\
"MXxfrjCWfNItuxiP8PleYb+F5mEt0UyNQhJz0FpmHbywKhKOYsDpcl32PzV+Mmpc\n"\
"f9cW6Ma6oZuSejW7b25ty4rdZqtBk9MXNom51r/ROueKB46U//mCxgR0CJtbjJ09\n"\
"XRAlAgMBAAGjYDBeMB8GA1UdIwQYMBaAFBbk3JAPgSsNa/SgrPomoRQhnujLMB0G\n"\
"A1UdDgQWBBQMpkyn8qJAuNLIPsCdtrF9uG9KTzAMBgNVHRMBAf8EAjAAMA4GA1Ud\n"\
"DwEB/wQEAwIHgDANBgkqhkiG9w0BAQsFAAOCAQEAPAd5b1s62ok1hQlZrrMKmbE2\n"\
"tHzME0qrCXu5qg1SHJwMVJJiNtV6r0qYlfD+KTWlS0qoJBXDrZ0rg7ssTieeD4Em\n"\
"w5F/Tl5lfinfIQpkoj/S8VXSoo/IDAOkxoMJUvjbNNKzBUh583UrANzVFi64kl/U\n"\
"2WX5569A4rvXiaLMdYjPnLcWmi5iWWTOyZjwjwnBpTv683bycja1vSFoxiUMibPs\n"\
"M04nCqAmFmv1wSHYkjJFECfrjLzBGNVSHXW5AP5F6MItL/Dgs+i2cMda7COd8eL+\n"\
"nlT8FvZelfB8wu9XFLRny8rIftMzB117A1JJ4XWrYVLe3FpxounG8DUgIx+7JQ==\n"\
"-----END CERTIFICATE-----"

/*
 * @brief PEM-encoded issuer certificate for AWS IoT Just In Time Registration (JITR).
 *
 * @todo If you are using AWS IoT Just in Time Registration (JITR), set this to
 * the issuer (Certificate Authority) certificate of the client certificate above.
 *
 * @note This setting is required by JITR because the issuer is used by the AWS
 * IoT gateway for routing the device's initial request. (The device client
 * certificate must always be sent as well.) For more information about JITR, see:
 *  https://docs.aws.amazon.com/iot/latest/developerguide/jit-provisioning.html,
 *  https://aws.amazon.com/blogs/iot/just-in-time-registration-of-device-certificates-on-aws-iot/.
 *
 * If you're not using JITR, set below to NULL.
 *
 * Must include the PEM header and footer:
 * "-----BEGIN CERTIFICATE-----\n"\
 * "...base64 data...\n"\
 * "-----END CERTIFICATE-----\n"
 */
#define keyJITR_DEVICE_CERTIFICATE_AUTHORITY_PEM    NULL

/*
 * @brief PEM-encoded client private key.
 *
 * @todo If you are running one of the Amazon FreeRTOS demo projects, set this
 * to the private key that will be used for TLS client authentication.
 *
 * @note Must include the PEM header and footer:
 * "-----BEGIN RSA PRIVATE KEY-----\n"\
 * "...base64 data...\n"\
 * "-----END RSA PRIVATE KEY-----\n"
 */
#define keyCLIENT_PRIVATE_KEY_PEM	"-----BEGIN RSA PRIVATE KEY-----\n"\
"MIIEpAIBAAKCAQEArmYCsGL2eTbp6q8IU1i3E/rcA1t57cj+WMR1Dvq34YkFH7c7\n"\
"S2a7b/eT1WKd8urR5k/+uFRq0gLZDQa3Z4rPbQ6dbjPfbldcTKlkf4jUS1rL65aG\n"\
"M3ztcCM7OdbD0jHHyzqposU+A5L7u0HoOltd5Zjm7z3SQ0wD9/ws4u/KcKQYTKL6\n"\
"VNSxv5QQ6WS0jWmkRsdt5U2lPNbw+y0FpjF8X64wlnzSLbsYj/D5XmG/heZhLdFM\n"\
"jUISc9BaZh28sCoSjmLA6XJd9j81fjJqXH/XFujGuqGbkno1u29ubcuK3WarQZPT\n"\
"FzaJuda/0TrnigeOlP/5gsYEdAibW4ydPV0QJQIDAQABAoIBAGXNRmSQYDABziog\n"\
"YkvEzZnsJeIP0+zqFw7aHzrPP+d3hHYvD33ESkXhz3oQRwM177DxoU8YeAJ/hxQD\n"\
"h6v5a3vNvDhIL3TJqidV+kQ4KzSRm9e51jzYAAIljr0EkOpyBJ6tb7iESt7rPebh\n"\
"Yh4ul3EfAC9HdyWxid+IZOtT7mgxavvbqg2j7mnK5cKsIMUvxMW/A1XbUmdSURyr\n"\
"gDbWRfGpR8cWDhY2JNRcs0ERpirnS6R0MK4V15Pg7iTO/iP1JCpVFgKdOFIhutXB\n"\
"w2S++6HYv3mTHPbtUEIAm/eBB+U92XuI9xXQ0exU6UbmRvO0i1Z27AhjCkHDt+2Z\n"\
"I6FPl0ECgYEA2IYHactyCsCQGh2oesM3sb3RUHo6GYo1m0vas9hN1P8ZssGF+wcr\n"\
"lUrPUwbKd+/0md5cgTD3JbODI3cofUSUj7KFr+pjsplihvr/PNMeCsLL3J6OKhFh\n"\
"u9M5YMJm82q6BT4wcrSHtVwvDfffPmx1ObmKc/hgo4gHg5fgL6AzkTMCgYEAzjHY\n"\
"cmNjv9noV+xFds9UrYvTr206CPMpJdUIP80YrJIM/LSJfT0EQBFoQ4h8/9+EawvK\n"\
"TrfK1VhjeKKHzeiCuEoU9CS5Q0v8LhZxAIZC2llUeVnxrFMYNseoGDcHqRJ9Hmbx\n"\
"+Ul4EODf+ZzeFYI+7oiSvAr9uAsRTWWNsP83CUcCgYB2TBP0y0CNe1O/0QBhwhLF\n"\
"u22GyKDBn29WX4UnyUAM5StHLNBkrDA1qt/G/ujTyngbMQ/dTcFaCc/WKp4mgreg\n"\
"IutDh43c4EyibrPQqY+6VV9INCB7/28kTW0F3nCdAWyMvMN/h/61KZ5TxV9lHaCW\n"\
"Xzaz0boK3qEsVmgX3TEPXQKBgQDAnMd4c2emu8PS/h8VkS+kvk2MPhhMa/kwIgSG\n"\
"tOwrqHMBTRRyLSFfR+OB0moSC5YC+QyO5BlRmjOxaQI7Ustpo6pXNkbpmn2D/+qn\n"\
"dsPUfEXw5hhfxW+FHJTlqeI6Cc9fKwM2v26tNaOD5QmARh7LbEDF7+56kbwj9I2b\n"\
"nYyn2QKBgQDSLUzpKR4kQbhRtvL3UKT7n9pPFWXcv33hjKlJiD3vC8Ic3sbllUtP\n"\
"Jvhoz9jR5pUynyM1bZ0UVh8N1dJna5J5LrVT1xQkszqWDYxWF3B9OT+JPEEdFSO1\n"\
"7ykP9I2zsk4ii/K1RMu1EeLXBfZUVW/Cq661EL7WF9FT1EUOgh055g==\n"\
"-----END RSA PRIVATE KEY-----"

#endif /* AWS_CLIENT_CREDENTIAL_KEYS_H */
