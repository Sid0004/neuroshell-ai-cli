#include "ai/http_client.h"
#include <curl/curl.h>
#include <stdexcept>
#include <thread>

namespace NeuroShell {

// Static flag to ensure CURL is initialized once globally
static bool g_curlInitialized = false;

HttpClient::HttpClient()
    : curlHandle_(nullptr)
    , timeout_(30)
    , userAgent_("NeuroShell/2.0")
{
    // Initialize CURL globally (once)
    if (!g_curlInitialized) {
        CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
        if (res != CURLE_OK) {
            throw std::runtime_error("Failed to initialize CURL globally: " + std::string(curl_easy_strerror(res)));
        }
        g_curlInitialized = true;
    }
    
    curlHandle_ = curl_easy_init();
    if (!curlHandle_) {
        throw std::runtime_error("Failed to initialize CURL");
    }
}

HttpClient::~HttpClient() {
    if (curlHandle_) {
        curl_easy_cleanup(curlHandle_);
    }
}

size_t HttpClient::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    std::string* str = static_cast<std::string*>(userp);
    str->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

void HttpClient::SetTimeout(int seconds) {
    timeout_ = seconds;
}

void HttpClient::SetUserAgent(const std::string& userAgent) {
    userAgent_ = userAgent;
}

HttpResponse HttpClient::Post(const std::string& url, 
                              const std::string& jsonBody,
                              const std::map<std::string, std::string>& headers) {
    HttpResponse response;
    
    if (!curlHandle_) {
        response.error = "CURL not initialized";
        return response;
    }
    
    std::string responseBody;
    
    // Set URL
    curl_easy_setopt(curlHandle_, CURLOPT_URL, url.c_str());
    
    // Set POST
    curl_easy_setopt(curlHandle_, CURLOPT_POST, 1L);
    curl_easy_setopt(curlHandle_, CURLOPT_POSTFIELDS, jsonBody.c_str());
    
    // Set headers
    struct curl_slist* headerList = nullptr;
    headerList = curl_slist_append(headerList, "Content-Type: application/json");
    
    for (const auto& [key, value] : headers) {
        std::string header = key + ": " + value;
        headerList = curl_slist_append(headerList, header.c_str());
    }
    
    curl_easy_setopt(curlHandle_, CURLOPT_HTTPHEADER, headerList);
    
    // Set write callback
    curl_easy_setopt(curlHandle_, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curlHandle_, CURLOPT_WRITEDATA, &responseBody);
    
    // Set timeout
    curl_easy_setopt(curlHandle_, CURLOPT_TIMEOUT, timeout_);
    
    // Set user agent
    curl_easy_setopt(curlHandle_, CURLOPT_USERAGENT, userAgent_.c_str());
    
    // Perform request
    CURLcode res = curl_easy_perform(curlHandle_);
    
    // Clean up headers
    curl_slist_free_all(headerList);
    
    if (res != CURLE_OK) {
        response.error = curl_easy_strerror(res);
        response.success = false;
        return response;
    }
    
    // Get response code
    long httpCode = 0;
    curl_easy_getinfo(curlHandle_, CURLINFO_RESPONSE_CODE, &httpCode);
    
    response.statusCode = static_cast<int>(httpCode);
    response.body = responseBody;
    response.success = (httpCode >= 200 && httpCode < 300);
    
    return response;
}

HttpResponse HttpClient::Get(const std::string& url,
                             const std::map<std::string, std::string>& headers) {
    HttpResponse response;
    
    if (!curlHandle_) {
        response.error = "CURL not initialized";
        return response;
    }
    
    std::string responseBody;
    
    // Set URL
    curl_easy_setopt(curlHandle_, CURLOPT_URL, url.c_str());
    
    // Set GET
    curl_easy_setopt(curlHandle_, CURLOPT_HTTPGET, 1L);
    
    // Set headers
    struct curl_slist* headerList = nullptr;
    for (const auto& [key, value] : headers) {
        std::string header = key + ": " + value;
        headerList = curl_slist_append(headerList, header.c_str());
    }
    
    if (headerList) {
        curl_easy_setopt(curlHandle_, CURLOPT_HTTPHEADER, headerList);
    }
    
    // Set write callback
    curl_easy_setopt(curlHandle_, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curlHandle_, CURLOPT_WRITEDATA, &responseBody);
    
    // Set timeout
    curl_easy_setopt(curlHandle_, CURLOPT_TIMEOUT, timeout_);
    
    // Set user agent
    curl_easy_setopt(curlHandle_, CURLOPT_USERAGENT, userAgent_.c_str());
    
    // Perform request
    CURLcode res = curl_easy_perform(curlHandle_);
    
    // Clean up headers
    if (headerList) {
        curl_slist_free_all(headerList);
    }
    
    if (res != CURLE_OK) {
        response.error = curl_easy_strerror(res);
        response.success = false;
        return response;
    }
    
    // Get response code
    long httpCode = 0;
    curl_easy_getinfo(curlHandle_, CURLINFO_RESPONSE_CODE, &httpCode);
    
    response.statusCode = static_cast<int>(httpCode);
    response.body = responseBody;
    response.success = (httpCode >= 200 && httpCode < 300);
    
    return response;
}

void HttpClient::PostAsync(const std::string& url,
                           const std::string& jsonBody,
                           std::function<void(const HttpResponse&)> callback,
                           const std::map<std::string, std::string>& headers) {
    std::thread([this, url, jsonBody, callback, headers]() {
        HttpResponse response = Post(url, jsonBody, headers);
        callback(response);
    }).detach();
}

} // namespace NeuroShell
