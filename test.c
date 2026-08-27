#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

int main() {
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // редиректы
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L); // таймаут 10с
    curl_easy_setopt(curl, CURLOPT_URL, "https://duckduckgo.com/?t=ffab&q=rt");
    printf("%lg\n", 0/0);
    return 0;
}
