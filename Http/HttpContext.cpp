#include "HttpContext.h"
#include "HttpRequest.h"
#include <memory>
#include <string>
#include <algorithm>
#include <iostream>

HttpContext::HttpContext(): state_(HttpRequestParaseState::START) {
    request_ = std::make_unique<HttpRequest>();
}

HttpContext::~HttpContext() {

}


bool HttpContext::GetCompleteRequest(){
    return state_ == HttpRequestParaseState::COMPLETE;
}

void HttpContext::ResetContextStatus() {
    state_ = HttpRequestParaseState::START;
}

bool HttpContext::ParaseRequest(const char *begin, int size) {
    char* start = const_cast<char*>(begin);
    char* end = start;
    char* colon = end;

    while (state_ != HttpRequestParaseState::kINVALID 
            && state_ != HttpRequestParaseState::COMPLETE
            && end - begin <= size) {
        
        char ch = *end;
        
        switch (state_) {
            case HttpRequestParaseState::START: {
                if (ch == CR || ch == LF || isblank(ch)) {
                    
                }
                else if (isupper(ch)) {
                    state_ = HttpRequestParaseState::METHOD;
                } else {
                    state_ = HttpRequestParaseState::kINVALID;
                }
                break;
            }

            case HttpRequestParaseState::METHOD: {
                if (isupper(ch)) {

                } else if (isblank(ch)) {
                    request_->SetMethod(std::string(start, end));
                    state_ = HttpRequestParaseState::BEFORE_URL;
                    start = end + 1;
                } else {
                    state_ = HttpRequestParaseState::kINVALID;
                }
                break;
            }
            case HttpRequestParaseState::BEFORE_URL: {
                if (ch == '/') {
                    state_ = HttpRequestParaseState::IN_URL;
                } else if (isblank(ch)) {

                } else {
                    state_ = HttpRequestParaseState::kINVALID;
                }
                break;
            }

            case HttpRequestParaseState::IN_URL: {
                if (ch == '?') {
                    request_->SetUrl(std::string(start, end));
                    start = end + 1;
                    state_ = HttpRequestParaseState::BEFORE_URL_PARAM_KEY;
                } else if (isblank(ch)) {
                    request_->SetUrl(std::string(start, end));
                    start = end + 1;
                    state_ = HttpRequestParaseState::PROTOCOL;
                }
                break;
            }

            case HttpRequestParaseState::BEFORE_URL_PARAM_KEY: {
                if (ch == CR || ch == LF || isblank(ch)) {
                    state_ = HttpRequestParaseState::kINVALID;
                } else {
                    state_ = HttpRequestParaseState::URL_PARAM_KEY;
                }
                break;
            }

            case HttpRequestParaseState::URL_PARAM_KEY: {
                if (ch == '=') {
                    state_ = HttpRequestParaseState::BEFORE_URL_PARAM_VALUE;
                    colon = end;
                } else if (isblank(ch)) {
                    state_ = HttpRequestParaseState::kINVALID;
                }
                break;
            }

            case HttpRequestParaseState::BEFORE_URL_PARAM_VALUE: {
                if (isblank(ch) || ch == CR || ch == LF) {
                    state_ = HttpRequestParaseState::kINVALID;
                }
                else {
                    state_ = HttpRequestParaseState::URL_PARAM_VALUE;
                }
                break;
            }

            case HttpRequestParaseState::URL_PARAM_VALUE: {
                if (ch == '&') {
                    state_ = HttpRequestParaseState::BEFORE_URL_PARAM_KEY;
                    request_->SetRequestParams(std::string(start, colon), std::string(colon + 1, end));
                    start = end + 1;
                }
                if (isblank(ch)) {
                    state_ = HttpRequestParaseState::BEFORE_PROTOCOL;
                    request_->SetRequestParams(std::string(start, colon), std::string(colon + 1, end));
                    start = end + 1;
                }
                break;
            }
            
            case HttpRequestParaseState::BEFORE_PROTOCOL: {
                if (isblank(ch)) {

                } else {
                    state_ = HttpRequestParaseState::PROTOCOL;
                }
                break;
            }

            case HttpRequestParaseState::PROTOCOL: {
                if (ch == '/') {
                    request_->SetProtocol(std::string(start, end));
                    state_ = HttpRequestParaseState::BEFORE_VERSION;
                    start = end + 1;
                } else {

                }
                break;
            }

            case HttpRequestParaseState::BEFORE_VERSION: {
                if (isdigit(ch)) {
                    state_ = HttpRequestParaseState::VERSION;
                } else {
                    state_ = HttpRequestParaseState::kINVALID;
                }
                break;
            }

            case HttpRequestParaseState::VERSION: {
                if (ch == CR) {
                    state_ = HttpRequestParaseState::WHEN_CR;
                    request_->SetVersion(std::string(start, end));
                    start = end + 1;
                } else if (isdigit(ch) || ch == '.') {

                } else {
                    state_ = HttpRequestParaseState::kINVALID;
                }
                break;
            }

            case HttpRequestParaseState::HEADER_KEY: {
                if (ch == ':') {
                    colon = end;
                    state_ = HttpRequestParaseState::HEADER_VALUE;
                } 
                break;
            }

            case HttpRequestParaseState::HEADER_VALUE: {
                if (isblank(ch)) {

                } else if (ch == CR) {
                    request_->AddHeader(std::string(start, colon), std::string(colon + 2, end));
                    start = end + 1;
                    state_ = HttpRequestParaseState::WHEN_CR;
                }
                break;
            }

            case HttpRequestParaseState::WHEN_CR: {
                if (ch == LF) {
                    start = end + 1;
                    state_ = HttpRequestParaseState::CR_LF;
                } else {
                    state_ = HttpRequestParaseState::kINVALID;
                }
                break;
            }

            case HttpRequestParaseState::CR_LF: {
                if (ch == CR) {
                    state_ = HttpRequestParaseState::CR_LF_CR;
                } else if (isblank(ch)) {
                    state_ = HttpRequestParaseState::kINVALID;
                } else {
                    state_ = HttpRequestParaseState::HEADER_KEY;
                }
                break;
            }

            case HttpRequestParaseState::CR_LF_CR: {
                if (ch == LF) {
                    if (request_->headers().count("Content-Lenght")) {
                        if (atoi(request_->GetHeader("Content-Lenght").c_str())) {
                            state_ = HttpRequestParaseState::BODY;
                        } else {
                            state_ = HttpRequestParaseState::COMPLETE;
                        }
                    }
                    else
                    {
                        if (end - begin < size)
                        {
                            state_ = HttpRequestParaseState::BODY;
                        }
                        else
                        {
                            state_ = HttpRequestParaseState::COMPLETE;
                        }
                    }
                    start = end + 1;
                } else {
                    state_ = HttpRequestParaseState::kINVALID;
                } 
                break;
            }
            
            case HttpRequestParaseState::BODY: {
                int bodylenght = size - (end - begin);
                request_->SetBody(std::string(start, start + bodylenght));
                state_ = HttpRequestParaseState::COMPLETE;
                break;
            }

            default: {
                state_ = HttpRequestParaseState::kINVALID;
                break;
            }
            
        }
        end++;
        
    }
    return HttpRequestParaseState::COMPLETE;

}

HttpRequest* HttpContext::request() {
    return request_.get();
}
