#include <unistd.h>
#include <curl/curl.h>
#include <string>
#include <sstream>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <rapidxml/rapidxml.hpp>

#define BASE_URL "http://192.168.0.100/"
#define AUTH "admin:admin"

using namespace rapidxml;
using namespace std;

typedef struct s_stats {
    double cur0;
    string stat0;
    double curBan;
    double tempBan;
    double humBan;
    string statBan;
    string outlet0;
    string outlet1;
    string outlet2;
    string outlet3;
    string outlet4;
    string outlet5;
    string outlet6;
    string outlet7;
    int userVerifyRes;
} t_stats;

typedef struct s_opts {
    uint8_t outlets;
    int status : 1;
    int on : 1;
    int off : 1;
} t_opts;

t_opts opts{0};
t_stats stats{0};

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
    curl_easy_setopt(curl, CURLOPT_URL, (string(BASE_URL) + strData).c_str());
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

string intToBinaryString(unsigned long n) {
    char bArray[ (sizeof ( unsigned long) * 8) + 1 ];
    unsigned index = sizeof ( unsigned long) * 8;
    char temp = 0;
    bArray[ index ] = '\0';
    do {
        temp = (n & 1); // Finding 0 or 1 in LSB
        temp = temp + '0';
        bArray[ --index ] = temp;
    } while (n >>= 1);
    return string(bArray + index);
}

char *getNodeValue(xml_node<char> *node) {
    if (node) {
        return node->value();
    }
    throw exception();
}

void showStatus() {
    stringstream ss;
    ss << "status.xml";
    string response = getHttpResponse(ss.str());
    xml_document<char> doc;
    char *m;
    m = (char*) malloc(1024);
    strncpy(m, response.c_str(), 1024);
    doc.parse<0>(m);
    xml_node<char> *node = doc.first_node("response");
    if (node) {
        try {
            stats.cur0 = stod(node->first_node("cur0")->value());
            stats.stat0 = node->first_node("stat0")->value();
            stats.curBan = stod(node->first_node("curBan")->value());
            stats.tempBan = stod(node->first_node("tempBan")->value());
            stats.humBan = stod(node->first_node("humBan")->value());
            stats.statBan = node->first_node("statBan")->value();
            stats.outlet0 = node->first_node("outletStat0")->value();
            stats.outlet1 = node->first_node("outletStat1")->value();
            stats.outlet2 = node->first_node("outletStat2")->value();
            stats.outlet3 = node->first_node("outletStat3")->value();
            stats.outlet4 = node->first_node("outletStat4")->value();
            stats.outlet5 = node->first_node("outletStat5")->value();
            stats.outlet6 = node->first_node("outletStat6")->value();
            stats.outlet7 = node->first_node("outletStat7")->value();
            stats.userVerifyRes = stoi(node->first_node("userVerifyRes")->value());

            cout << "Power: " << stats.cur0 << "/" << stats.curBan << " " << stats.stat0 << " " << stats.tempBan << "°C " << stats.humBan << "% " << stats.statBan << endl;
            cout << "Outlets: 0:" << stats.outlet0 << " 1:" << stats.outlet1 << " 2:" << stats.outlet2 << " 3:" << stats.outlet3 << " 4:" << stats.outlet4 << " 5:" << stats.outlet5 << " 6:" << stats.outlet6 << " 7:" << stats.outlet7 << endl;
        } catch (exception &e) {
        }
    }
}

int main(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        try {
            int outlet = stoi(argv[i]);
            if (outlet >= 0 && outlet <= 7) {
                opts.outlets |= 1 << outlet;
            }
        } catch (exception &e) {
            if (strcasecmp(argv[i], "on") == 0) {
                opts.on = 1;
            } else if (strcasecmp(argv[i], "off") == 0) {
                opts.off = 1;
            } else if (strcasecmp(argv[i], "status") == 0) {
                opts.status = 1;
            }
        }
    }

    try {
        int test = opts.outlets;
        int outlet = 0;
        bool first = true;
        stringstream ss;

        if (opts.status) {
            showStatus();
            exit(EXIT_SUCCESS);
        }
        
        if (opts.outlets == 0) {
            cerr << "no outlet given" << endl;
            usage(argv[0]);
            exit(-2);
        }
        if (!opts.on && !opts.off) {
            cerr << "no state given" << endl;
            usage(argv[0]);
            exit(-1);
        }
        
        ss << "control_outlet.htm?";
        do {
            if (test & 1) {
                if (!first) {
                    ss << "&";
                } else {
                    first = false;
                }
                ss << "outlet" << outlet << "=1";
            }
            outlet++;
        } while (test >>= 1);

        if (opts.outlets > 0) {
            ss << "&op=" << (opts.on != 0 ? 0 : 1) << "&submit=Apply";
            getHttpResponse(ss.str());
        }

    } catch (string& errormsg) {
        cerr << "[E] " << errormsg << endl;
    }
}
