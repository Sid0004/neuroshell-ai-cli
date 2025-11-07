#pragma once

#include <string>
#include <map>
#include <functional>

namespace NeuroShell {

struct HttpResponse {
    int statusCode;
    std::string body;
    std::map<std::string, std::string> headers;
    std::string error;
    bool success;
    
    HttpResponse() : statusCode(0), success(false) {}
};

class HttpClient {
public:
    HttpClient();
    ~HttpClient();
    
    // Synchronous POST request
    HttpResponse Post(const std::string& url, 
                     const std::string& jsonBody,
                     const std::map<std::string, std::string>& headers = {});
    
    // Synchronous GET request
    HttpResponse Get(const std::string& url,
                    const std::map<std::string, std::string>& headers = {});
    
    // Asynchronous POST request with callback
    void PostAsync(const std::string& url,
                   const std::string& jsonBody,
                   std::function<void(const HttpResponse&)> callback,
                   const std::map<std::string, std::string>& headers = {});
    
    // Set timeout in seconds
    void SetTimeout(int seconds);
    
    // Set user agent
    void SetUserAgent(const std::string& userAgent);
    
private:
    void* curlHandle_;
    int timeout_;
    std::string userAgent_;
    
    // CURL callback for writing response
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
};

} // namespace NeuroShell
