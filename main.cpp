/*

USAGE: ./ProxyChecker [ip1:port1] [ip2:port2] [ip3:port3] [ipN:portN]

*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <curl/curl.h>

using namespace std;

string ip; /**< string that contains proxy ip address*/
string buffer; /**< string used by writer function to save cURL*/

//! contains proxy status
/*!
@return "ok" (proxy is working)
@return "fail" (proxy isn't working)
@return curl_strerror() (returns error description)
@return "chfail" (cURL handle failed)
*/
string tmpProxyWorks;

string filename; /**< string containing filename (later used to open/save proxy list file)*/

//! contains proxy type
/*!
@return "http" (CURLPROXY_HTTP)
@return "socks4" (CURLPROXY_SOCKS4)
@return "socks4a" (CURLPROXY_SOCKS4A)
@return "socks5" (CURLPROXY_SOCKS5)
*/
string proxytype;
stringstream ipPort; /**< stringstream concatenating proxytype, ip, port*/
vector<string> proxyTab; /**< vector containing proxy list*/
int i=0; /**< integer used in various loops*/
int proxiesOk = 0; /**< integer used for statistics (tells how many proxies are working)*/
int proxiesFail = 0; /**< integer used for statistics (tells how many proxies aren't working)*/
int port; /**< integer containing proxy port*/

/**
* This function gets called by libcurl as soon as there is data received that needs to be saved.
* The size of the data pointed to by char *data is size multiplied with size_t nmemb, it will not be zero terminated.
* Return the number of bytes actually taken care of.
* If that amount differs from the amount passed to your function, it'll signal an error to the library.
* This will abort the transfer and return CURLE_WRITE_ERROR.
*/
static int writer(char *data, size_t size, size_t nmemb, std::string *buffer){
    unsigned int result = 0;
    if (buffer != NULL){
        buffer->append(data, size * nmemb);
        result = size * nmemb;
    }
  return result;
}
/**
* function used to determine if proxy passed by arguments works
* @param ip contains proxy ip (eg. 192.168.1.1)
* @param port contains proxy port (eg. 8080)
* @param proxytype it can contain one of 4 proxy types (http, socks4, socks4a, socks5), by default it contains http
* @return proxy status ("ok", "fail", "chfail" [cURL handle initialization failed], "badformat", descriptive error returned by curl_strerror(res))
*/
string proxyWorks(string ip, int port, string proxytype="http"){
    CURL *ch;
    CURLcode res;
    ch = curl_easy_init();
    if(ch){
        curl_easy_setopt(ch, CURLOPT_URL, "http://google.com/");
        curl_easy_setopt(ch, CURLOPT_FOLLOWLOCATION, true);
        curl_easy_setopt(ch, CURLOPT_TIMEOUT, 15);
        curl_easy_setopt(ch, CURLOPT_USERAGENT, "Mozilla/5.0 (compatible; MSIE 8.0; Windows NT 5.1; InfoPath.2; SLCC1; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 2.0.50727)2011-09-08 13:55:49");
        if(proxytype=="http" || !(proxytype=="socks4" || proxytype=="sock4a" || proxytype=="socks5" || proxytype=="http"))
            curl_easy_setopt(ch, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
        else
            if(proxytype=="socks4")
                curl_easy_setopt(ch, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4);
        else
            if(proxytype=="socks4a")
                curl_easy_setopt(ch, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS4A);
        else
            if(proxytype=="socks5")
                curl_easy_setopt(ch, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
        curl_easy_setopt(ch, CURLOPT_PROXY, ip.c_str());
        curl_easy_setopt(ch, CURLOPT_PROXYPORT, port);
        curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, writer);
        curl_easy_setopt(ch, CURLOPT_WRITEDATA, &buffer);
        res = curl_easy_perform(ch);
        curl_easy_cleanup(ch);
        if(res == CURLE_OK){
            if(sizeof(buffer)>0)
                return "ok";
            else return "fail";
        } else
            return curl_easy_strerror(res);
    } else return "chfail";
}
int main(int argc, char **argv)
{
    if(argc>1){
        int pTabIndex=0; /**< proxyTab Index value*/
        if((strcmp(argv[1], "-f")==0 || strcmp(argv[1], "--filename")==0) && argv[2]){
            filename = argv[2];

            ifstream fp; /**< file pointer used to read proxy list*/
            fp.open(filename.c_str());
            if(fp.is_open()){
                string line; /**< string containing one line from proxy list*/

                while(getline(fp, line)){
                    char *dup=new char[line.length()+1]; /**< char pointer containing line in char* format*/
                    strcpy(dup, line.c_str());
                    char *pch=strchr(dup, ':'); /**< char pointer containing ':' position and later tokens divided by ":"*/
                    int n=0; /**< integer used in while loop, it contains number of ':' in @see line variable*/
                    while(pch!=NULL){
                        pch = strchr(pch+1, ':');
                        n++;
                    }
                    pch=NULL;
                    if(n==1 || n==2){
                        pch = strtok(dup, ":");
                        if(pch!=NULL){
                            if(n==1) ip = pch;
                            else if(n==2) proxytype = pch;
                            pch = strtok(NULL, ":");
                            if(pch!=NULL){
                                if(n==1) port = atoi(pch);
                                else if(n==2){
                                    ip = pch;
                                    pch = strtok(NULL, ":");
                                    if(pch!=NULL)
                                        port = atoi(pch);
                                }
                                if(proxytype.length()>0 && (proxytype!="http" || proxytype=="socks4" || proxytype=="socks4a" || proxytype=="socks5")) ipPort << proxytype << ":" << ip << ":" << port;
                                else ipPort << ip << ":" << port;
                                tmpProxyWorks = proxyWorks(ip, port, proxytype);
                                cout <<ipPort.str()<<" - "<<tmpProxyWorks<<endl;
                            }
                        }
                    }
                    if(n!=1 && n!=2){
                        tmpProxyWorks = "badformat";
                        cout <<line<<" - "<<tmpProxyWorks<<endl;
                    }
                    if(tmpProxyWorks=="ok"){
                        proxyTab.push_back(ipPort.str());
                        pTabIndex++;
                        proxiesOk++;
                    }
                    else proxiesFail++;
                    ipPort.str("");
                    free(dup);
                }
                fp.close();
            } else{
                cout <<"File does not exist.";
                return 0;
            }
        } else {
            for(i=1; i<argc; i++){
                char *pch=strchr(argv[i], ':'); /**< char pointer containing ':' position and later tokens divided by ":"*/
                int n=0; /**< integer used in while loop, it contains number of ':' in @see line variable*/
                while(pch!=NULL){
                    pch = strchr(pch+1, ':');
                    n++;
                }
                if(n==1 || n==2){
                    pch = NULL;
                    pch = strtok(argv[i], ":");
                    if(pch!=NULL){
                        if(n==1) ip = pch;
                        else if(n==2) proxytype = pch;
                        pch = strtok(NULL, ":");
                        if(pch!=NULL){
                            if(n==1) port = atoi(pch);
                            else if(n==2){
                                ip = pch;
                                pch = strtok(NULL, ":");
                                if(pch!=NULL)
                                    port = atoi(pch);
                            }
                            if(proxytype.length()>0 && (proxytype!="http" || proxytype=="socks4" || proxytype=="socks4a" || proxytype=="socks5")) ipPort << proxytype << ":" << ip << ":" << port;
                            else ipPort << ip << ":" << port;
                            tmpProxyWorks = proxyWorks(ip, port, proxytype);
                            cout <<ipPort.str()<<" - "<<tmpProxyWorks<<endl;
                        }
                    }
                }
                if(n!=1 && n!=2){
                    tmpProxyWorks = "badformat";
                    cout <<argv[i]<<" - "<<tmpProxyWorks<<endl;
                }
                if(tmpProxyWorks=="ok"){
                    proxyTab.push_back(ipPort.str());
                    pTabIndex++;
                    proxiesOk++;
                }
                else proxiesFail++;
                ipPort.str("");
            }
        }

        cout <<"------"<<endl;
        cout <<"Proxies OK:\t"<<proxiesOk<<endl;
        cout <<"Proxies FAILED:\t"<<proxiesFail<<endl;

        if(proxiesOk>0){
            char choice; /**< char container used for user decision ('Y' == yes, 'N' == no)*/
            cout <<endl<<"Save proxies to file? [(Y)es or (N)o] ";
            if(cin >> choice){
                if(choice=='Y' || choice == 'y'){
                    ofstream fp; /**< file pointer used to save proxyTab to file*/
                    cout <<"Path (filename): ";
                    cin >> filename;
                    cout <<endl;
                    fp.open(filename.c_str(), ios::in | ios::trunc);
                    for(i=0; i<proxiesOk; i++)
                        fp << proxyTab[i]+"\r\n";
                    fp.close();
                }
            }
        }
    } else
        cout <<"No proxies specified."<<endl;
    return 0;
}
