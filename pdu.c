#include <unistd.h>
#include <curl/curl.h>
#include <string>
#include <sstream>
#include <cstring>
#include <iostream>
#include <stdio.h>

#define BASE_URL "http://192.168.0.100/"
#define AUTH "admin:admin"

using namespace std;
void usage(char *name) {
    cout << "USAGE: " << name << " <on|off> <port0..7> ..." << endl;
}
size_t response_callback(void *ptr, size_t size, size_t nmemb, void *userp) {
    (*((std::stringstream*)userp)) << (char*) ptr;
    return size * nmemb;
}

string getHttpResponse(string strData) {
    CURL* curl = curl_easy_init();
    if (curl == NULL) {
        throw string("cannot to initialise curl");
    }
    stringstream response;
    curl_easy_setopt(curl, CURLOPT_URL, (string(BASE_URL "control_outlet.htm?") + strData).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
    curl_easy_setopt(curl, CURLOPT_USERPWD, AUTH);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_callback);

    CURLcode res = curl_easy_perform(curl);

    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        throw string("curl: (") + to_string(res) + ") " + curl_easy_strerror(res);
    }
    return response.str();
}

int main(int argc, char** argv) {
    int *op = NULL;
    stringstream ss;
    for (int i = 1; i < argc; i++) {
        try {
            int outlet = stoi(argv[i]);
            if (outlet >= 0 && outlet <= 7) {
                if (ss.rdbuf()->in_avail()) {
                    ss << "&";
                }
                ss << "outlet" << outlet << "=1";
            }
        } catch (exception &e) {
            if (strcasecmp(argv[i], "on") == 0) {
                op = (int*) malloc(sizeof (int));
                *op = 0;
            } else if (strcasecmp(argv[i], "off") == 0) {
                op = (int*) malloc(sizeof (int));
                *op = 1;
            }
        }
    }
    if (op == NULL) {
        cerr << "no state given" << endl;
        usage(argv[0]);
        exit(-1);
    }
    if (ss.rdbuf()->in_avail() == 0) {
        cerr << "no outlet given" << endl;
        usage(argv[0]);
        exit(-2);
    }
    
    ss << "&op=" << *op << "&submit=Apply";
    cout << "Enable all" << endl;
    try {
        getHttpResponse(ss.str());
    } catch (string& errormsg) {
        cerr << "[E] " << errormsg << endl;
    }
}
