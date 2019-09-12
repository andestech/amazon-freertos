/*
 * Amazon FreeRTOS TLS V1.1.4
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

#ifndef _AWS_TLS_TEST_H_
#define _AWS_TLS_TEST_H_

/*
 * PEM-encoded client certificate.
 *
 * Certificate for P-256 elliptic curve key.
 */
static const char tlstestCLIENT_CERTIFICATE_PEM_EC[] = "-----BEGIN CERTIFICATE-----\n"
"MIIC9jCCAd6gAwIBAgIVANJSqF9CC1tGIThaMdRQobqH8z5JMA0GCSqGSIb3DQEB\n"
"CwUAME0xSzBJBgNVBAsMQkFtYXpvbiBXZWIgU2VydmljZXMgTz1BbWF6b24uY29t\n"
"IEluYy4gTD1TZWF0dGxlIFNUPVdhc2hpbmd0b24gQz1VUzAeFw0xOTA3MTcwMzI2\n"
"MDVaFw00OTEyMzEyMzU5NTlaMIGEMQswCQYDVQQGEwJUVzEPMA0GA1UECAwGVGFp\n"
"d2FuMQ8wDQYDVQQHDAZUYWluYW4xDjAMBgNVBAoMBUFuZGVzMQswCQYDVQQLDAJP\n"
"UzEOMAwGA1UEAwwFa2V2aW4xJjAkBgkqhkiG9w0BCQEWF2tldmluODIwOTMwOThA\n"
"Z21haWwuY29tMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEz+JBiOJ4DALnlCwX\n"
"A1JzOhSVAOugiW89dV2wJn9tyr2rJuppxt2AUIvV0ib5uR+cW+KMVGcnNWKNZDso\n"
"GxuDO6NgMF4wHwYDVR0jBBgwFoAULGP8ZdmC8xenqoBR1Y9XWUBSgoQwHQYDVR0O\n"
"BBYEFEU01d82LkyQVL0HL+CRCxH0OPU7MAwGA1UdEwEB/wQCMAAwDgYDVR0PAQH/\n"
"BAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQCLrcrepG9+Aipn/kvUV8zaxtKjU6Go\n"
"1OBsLdETF0oOkdo9uRMdmGOIhslYq/2+Ll6naK3Dss7DmsfyfMBpenne7BzTmT8H\n"
"IReP+5fYrARAjOlRsvbK+rQV0qTBP4CzxE6LagU8MqcmscsSFjQgh4pzqoGtvL2J\n"
"e8M8uOs/+xOZAxy6AgtVm/g5lnhVqreCoRc8UXeutlEJTztHfs0RtLFEKGWHVzr8\n"
"zeECpw4TNKw073lx1IamJSzcZdMVkseawnEcHqpjD92Pb8CorRwalUEG8nRyvTkp\n"
"0WSqVAKUb3W5mplJy5W5lg9GPEMPATI0tXWzZGBhUvg1FdJIIR+3MSwa\n"
"-----END CERTIFICATE-----";

/*
 * PEM-encoded client private key.
 *
 * This is a P-256 elliptic curve key.
 */
static const char tlstestCLIENT_PRIVATE_KEY_PEM_EC[] = "-----BEGIN EC PRIVATE KEY-----\n"
"MHcCAQEEIE+cnCoTQ5jWEHEAs80eAGhOfXvkwrIR4sFnoazlcg96oAoGCCqGSM49\n"
"AwEHoUQDQgAEz+JBiOJ4DALnlCwXA1JzOhSVAOugiW89dV2wJn9tyr2rJuppxt2A\n"
"UIvV0ib5uR+cW+KMVGcnNWKNZDsoGxuDOw==\n"
"-----END EC PRIVATE KEY-----";

/* One character of this certificate has been changed in the issuer
 * name from Amazon Web Services to Amazon Web Cervices. */
static const char tlstestCLIENT_CERTIFICATE_PEM_MALFORMED[] = "Paste client certificate here.";

/* Certificate which is not trusted by the broker. */
static const char tlstestCLIENT_UNTRUSTED_CERTIFICATE_PEM[] = "-----BEGIN CERTIFICATE-----\n"
"MIIB6DCCAY4CCQDvwzz+z3kVMDAKBggqhkjOPQQDAjBzMQswCQYDVQQGEwJUVzEP\n"
"MA0GA1UECAwGVGFpd2FuMQ4wDAYDVQQKDAVBbmRlczELMAkGA1UECwwCT1MxDjAM\n"
"BgNVBAMMBWtldmluMSYwJAYJKoZIhvcNAQkBFhdrZXZpbjgyMDkzMDk4QGdtYWls\n"
"LmNvbTAeFw0xOTA3MDgwODAyMDhaFw0yMDExMTkwODAyMDhaMIGEMQswCQYDVQQG\n"
"EwJUVzEPMA0GA1UECAwGVGFpd2FuMQ8wDQYDVQQHDAZUYWluYW4xDjAMBgNVBAoM\n"
"BUFuZGVzMQswCQYDVQQLDAJPUzEOMAwGA1UEAwwFa2V2aW4xJjAkBgkqhkiG9w0B\n"
"CQEWF2tldmluODIwOTMwOThAZ21haWwuY29tMFkwEwYHKoZIzj0CAQYIKoZIzj0D\n"
"AQcDQgAEWuGN83RLmK43tStTjaBbro1nVQv7mYb/o40lBN6N+CG/U1jXXqz+p+J7\n"
"R6q8371wXefwyNVHusUkfQ5+videLzAKBggqhkjOPQQDAgNIADBFAiEAnAAmMXt+\n"
"z3/N6rAqlhrUxZAJ9bl7NDry/AHkf75OjG4CID0koHgmcl6pPtXCSEr2NWYElz8u\n"
"xZar68DqmqGjNatm\n"
"-----END CERTIFICATE-----";

/* Private key corresponding to the untrusted certificate. */
static const char tlstestCLIENT_UNTRUSTED_PRIVATE_KEY_PEM[] = "-----BEGIN EC PRIVATE KEY-----\n"
"MHcCAQEEIFxreHNfGjpRJNAQQ5aY0e8BWgQJC9g/TS90EuzusO+6oAoGCCqGSM49\n"
"AwEHoUQDQgAEWuGN83RLmK43tStTjaBbro1nVQv7mYb/o40lBN6N+CG/U1jXXqz+\n"
"p+J7R6q8371wXefwyNVHusUkfQ5+videLw==\n"
"-----END EC PRIVATE KEY-----";

/* Device certificate created using BYOC instructions. */
static const char tlstestCLIENT_BYOC_CERTIFICATE_PEM[] = "-----BEGIN CERTIFICATE-----\n"
"MIIB6DCCAY4CCQCO7bPoc3MI6DAKBggqhkjOPQQDAjBzMQswCQYDVQQGEwJUVzEP\n"
"MA0GA1UECAwGVGFpd2FuMQ4wDAYDVQQKDAVBbmRlczELMAkGA1UECwwCT1MxDjAM\n"
"BgNVBAMMBWtldmluMSYwJAYJKoZIhvcNAQkBFhdrZXZpbjgyMDkzMDk4QGdtYWls\n"
"LmNvbTAeFw0xOTA3MTcwNTI3MTFaFw0yMDExMjgwNTI3MTFaMIGEMQswCQYDVQQG\n"
"EwJUVzEPMA0GA1UECAwGVGFpd2FuMQ8wDQYDVQQHDAZUYWluYW4xDjAMBgNVBAoM\n"
"BUFuZGVzMQswCQYDVQQLDAJPUzEOMAwGA1UEAwwFa2V2aW4xJjAkBgkqhkiG9w0B\n"
"CQEWF2tldmluODIwOTMwOThAZ21haWwuY29tMFkwEwYHKoZIzj0CAQYIKoZIzj0D\n"
"AQcDQgAE9zjtYS6C/F168GQzhAXd0WJJZDbhieiM/jpshGdNQ/OSlI2ggTue9/bx\n"
"TCcnFZBc4T9XK/LIi5UGqcHHMADWJDAKBggqhkjOPQQDAgNIADBFAiEAz4XPezJK\n"
"one/U6CZfRCK/m/BwPM9wvCGJXwLXB8ZpKcCIH7bcD7mqp5GjvhLfOQBWZ3/kVj9\n"
"D1MpMHF+LhQFh+nr\n"
"-----END CERTIFICATE-----";

/* Device private key created using BYOC instructions. */
static const char tlstestCLIENT_BYOC_PRIVATE_KEY_PEM[] = "-----BEGIN EC PRIVATE KEY-----\n"
"MHcCAQEEINR8PHu1MQjCso9sK7XSXNh+n9/2tRttc8LunT7CCAXGoAoGCCqGSM49\n"
"AwEHoUQDQgAE9zjtYS6C/F168GQzhAXd0WJJZDbhieiM/jpshGdNQ/OSlI2ggTue\n"
"9/bxTCcnFZBc4T9XK/LIi5UGqcHHMADWJA==\n"
"-----END EC PRIVATE KEY-----";

#endif /* ifndef _AWS_TLS_TEST_H_ */
