#ifndef __W_CURL_H
#define __W_CURL_H
/*
Wrapped Curl functions API
This is a wrapper around libcurl to simplify HTTP requests.
*/

#include <curl/curl.h>

struct t_response {
  long   http_code;
  size_t size;
  char * memory;  // allocated memory data
  char * sError;
};

typedef struct t_response t_response;

void free_t_response(t_response *resp);

/*
init_w_curl    -> should be invoked exactly once for each application that uses libcurl and before any call of other libcurl functions.
cleanup_w_curl -> should be invoked exactly once for each application that uses libcurl
*/
void init_w_curl();
void cleanup_w_curl();

/*
  w_curl_http_post Makes a Http Post request and return the raw  data 
  use free_t_response to free the arrays
*/
t_response w_curl_http_post(const char *url, const char *post_fields, const char *headers[], size_t n_headers, int verbose);


#endif