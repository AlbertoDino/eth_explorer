#include "w_curl.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <curl/curl.h>
#include <curl/curl.h>


void free_t_response(t_response *resp)
{
    if(resp->memory)
        free(resp->memory);
    resp->memory = 0;

    if(resp->sError)
        free(resp->sError);

    resp->sError = 0;
}

void init_w_curl() {
    curl_global_init(CURL_GLOBAL_ALL);
}

void cleanup_w_curl() {
    curl_global_cleanup();
}

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    struct t_response *mem = (struct t_response *)userp;
 
    size_t new_size = size * nmemb;

    char* data      = realloc(mem->memory, mem->size + new_size + 1);
    if(data == NULL) {
        /* out of memory */
        printf("WCurl> WriteMemoryCallback not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory            = data;
    memcpy(&(mem->memory[mem->size]),contents,new_size);

    mem->size              += new_size;
    mem->memory[mem->size] = 0;

    return new_size;
}

struct t_response w_curl_http_post(const char *url, const char *post_fields, const char *headers[], size_t n_headers,int verbose) {
    
    CURLcode res;

    t_response c_result;
    c_result.memory    = malloc(1); 
    c_result.size      = 0;
    c_result.http_code = 0;
    c_result.sError    = NULL;

    CURL *c_handle = curl_easy_init();
    if(c_handle) {
        /*set URL */
        curl_easy_setopt(c_handle, CURLOPT_URL,url);

        /*set Headers*/
        struct curl_slist *c_headers = NULL;    
        for(size_t i=0; i<n_headers; i++) {
            c_headers = curl_slist_append(c_headers, headers[i]);
        }
        curl_easy_setopt(c_handle,CURLOPT_HTTPHEADER, c_headers);

        /*set POST*/
        curl_easy_setopt(c_handle, CURLOPT_POST, 1L);

        /*Provide POST body*/
        curl_easy_setopt(c_handle, CURLOPT_POSTFIELDS, post_fields);

        /* set up memory callback for response and memory state*/
        curl_easy_setopt(c_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(c_handle, CURLOPT_WRITEDATA, (void *)&c_result);

        curl_easy_setopt(c_handle,CURLOPT_FOLLOWLOCATION, 1L);

        /*make the actual request*/
        res = curl_easy_perform(c_handle);

        long http_code     = 0;
        curl_easy_getinfo(c_handle, CURLINFO_RESPONSE_CODE, &http_code);
        c_result.http_code = http_code;

        if(verbose)
        {
            printf("WCurl> HTTP/s Url: %s\n"          , url);
            printf("WCurl> HTTP/s Post Data: %s\n"    , post_fields);
            printf("WCurl> HTTP Response Code: %ld\n" , http_code);
        }

        if(res != CURLE_OK) {
            c_result.sError = strdup((char*)curl_easy_strerror(res));
            if(verbose && c_result.sError)
            {
                printf("WCurl> HTTP Error : %s\n"     , c_result.sError);
            }
        }
        else {            
            c_result.sError = NULL;
            if(verbose)
            {
                printf("WCurl> Received data size: %zu bytes\n", c_result.size);
                printf("WCurl> Received data : %s \n"          , c_result.memory);
            }
        }

        // clean up
        if(c_headers)
            curl_slist_free_all(c_headers);
        curl_easy_cleanup(c_handle);
    }

    return c_result;
}