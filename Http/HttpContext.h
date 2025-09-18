#pragma once

#include <string>
#include <memory>

class HttpRequest;

#define CR '\r'
#define LF '\n'

enum HttpRequestParaseState
{
    kINVALID, 
    kINVALID_METHOD,
    kINVALID_URL,
    kINVALID_VERSION,
    kINVALID_HEADER,

    START,
    METHOD,

    BEFORE_URL,
    IN_URL,

    BEFORE_URL_PARAM_KEY,
    URL_PARAM_KEY,
    BEFORE_URL_PARAM_VALUE,
    URL_PARAM_VALUE,
    
    BEFORE_PROTOCOL,
    PROTOCOL,

    BEFORE_VERSION,
    VERSION_SPLIT,
    VERSION,

    HEADER,
    HEADER_KEY,

    HEADER_BEFORE_COLON,
    HEADER_AFTER_COLON,
    HEADER_VALUE,

    WHEN_CR,

    CR_LF,

    CR_LF_CR,

    BODY,

    COMPLETE
};

class HttpContext 
{
public:
    HttpContext();
    ~HttpContext();
    
    bool ParaseRequest(const char *begin, int size);
    bool ParaseRequest(const std::string& msg);
    bool ParaseRequest(const char *begin);

    bool GetCompleteRequest();
    HttpRequest* request();
    void ResetContextStatus();

private:
    std::unique_ptr<HttpRequest> request_;
    HttpRequestParaseState state_;
};