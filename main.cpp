/*

USAGE: ./ProxyChecker [ip1:port1] [ip2:port2] [ip3:port3] [ipN:portN]

*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

using namespace std;

string buffer;
char *ip, *port;
int i=0;

static int writer(char *data, size_t size, size_t nmemb, std::string *buffer){
    int result = 0;
    if (buffer != NULL){
        buffer->append(data, size * nmemb);
        result = size * nmemb;
    }
  return result;
}
string proxyWorks(char ip[], int port){
    CURL *ch;
    CURLcode res;
    ch = curl_easy_init();
    if(ch){
        curl_easy_setopt(ch, CURLOPT_URL, "http://www.ip-address.org/");
        curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(ch, CURLOPT_TIMEOUT, 15);
        curl_easy_setopt(ch, CURLOPT_USERAGENT, "Mozilla/5.0 (compatible; MSIE 8.0; Windows NT 5.1; InfoPath.2; SLCC1; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 2.0.50727)2011-09-08 13:55:49");
        curl_easy_setopt(ch, CURLOPT_PROXY, ip);
        curl_easy_setopt(ch, CURLOPT_PROXYPORT, port);
        curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, writer);
        curl_easy_setopt(ch, CURLOPT_WRITEDATA, &buffer);
        res = curl_easy_perform(ch);
        curl_easy_cleanup(ch);
        if(res == CURLE_OK){
            if(sizeof(buffer)>0)
                return "ok";
            else return "fail";
        } else {
            return curl_easy_strerror(res);
        }
    } else return "chfail";
}
int main(int argc, char **argv)
{
    if(argc>1){
        for(i=1; i<argc; i++){
            char *pch;
            pch = strtok(argv[i], ":");
                ip = new char[strlen(pch)];
                ip = pch;
                pch = strtok(NULL, ":");

                port = new char[strlen(pch)];
                port = pch;

            cout <<ip<<":"<<port<<" - "<<proxyWorks(ip, atoi(port))<<endl;
        }
    }
    return 0;
}
