/*

USAGE: ./ProxyChecker [ip1:port1] [ip2:port2] [ip3:port3] [ipN:portN]

*/

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

using namespace std;

string buffer;
char *ip, *port;
string *proxyTab;
unsigned int i=0;

static int writer(char *data, size_t size, size_t nmemb, std::string *buffer){
    unsigned int result = 0;
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
        curl_easy_setopt(ch, CURLOPT_URL, "http://google.com/");
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
int main(unsigned int argc, char **argv)
{
    if(argc>1){
        unsigned int proxiesOk = 0, proxiesFail = 0;
        string tmpProxyWorks;

        proxyTab = new string[argc-1];
        int pTabIndex=0;

        for(i=1; i<argc; i++){
            char *pch=strchr(argv[i], ':');
            if(pch!=NULL){
                pch = NULL;
                pch = strtok(argv[i], ":");
                if(pch){
                    ip = new char[strlen(pch)];
                    ip = pch;
                    pch = strtok(NULL, ":");
                    if(pch){
                        port = new char[strlen(pch)];
                        port = pch;
                        tmpProxyWorks = proxyWorks(ip, atoi(port));
                        cout <<ip<<":"<<port<<" - "<<tmpProxyWorks<<endl;
                    }
                }
            }
            if(!pch){
                tmpProxyWorks = "badformat";
                cout <<argv[i]<<" - "<<tmpProxyWorks<<endl;
            }
            if(tmpProxyWorks=="ok"){
                proxyTab[pTabIndex] = (string)ip+":"+(string)port;
                pTabIndex++;
                proxiesOk++;
            }
            else proxiesFail++;
        }
        cout <<"------"<<endl;
        cout <<"Proxies OK:\t"<<proxiesOk<<endl;
        cout <<"Proxies FAILED:\t"<<proxiesFail<<endl;

        char choice;
        cout <<endl<<"Save proxies to file? [(Y)es or (N)o] ";
        if(cin >> choice){
            if(choice=='Y' || choice == 'N' || choice=='y' || choice=='n'){
                ofstream fp;
                string filename;
                cout <<"Path (filename): ";
                cin >> filename;
                cout <<endl;
                fp.open(filename.c_str(), ios::in | ios::trunc);
                for(i=0; i<proxiesOk; i++)
                    fp << (string)proxyTab[i]+"\r\n";

                fp.close();
            }
        }
    } else
        cout <<"No proxies specified."<<endl;
    return 0;
}
