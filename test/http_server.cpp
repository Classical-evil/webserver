#include <iostream>
#include "../Http/HttpServer.h"
#include "../Http/HttpRequest.h"
#include "../Http/HttpResponse.h"
#include "../Tcp/EventLoop.h"
#include "../Log/AsyncLogging.h"
#include <string>
#include <fstream>

std::string ReadFile(const std::string& path){
    std::ifstream is(path.c_str(), std::ifstream::in);

    // 寻找文件末端
    is.seekg(0, is.end);

    // 获取长度
    int flength = is.tellg();

    //重新定位
    is.seekg(0, is.beg);
    char * buffer = new char[flength];

    // 读取文件
    is.read(buffer, flength);
    std::string msg(buffer, flength);
    return msg;
}

void HttpResponseCallback(const HttpRequest &request, HttpResponse *response)
{
    if(request.method() != HttpRequest::Method::kGet){
        response->SetStatusCode(HttpResponse::HttpStatusCode::k400BadRequest);
        response->SetStatusMessage("Bad Request");
        response->SetCloseConnection(true);
    }

    {
        std::string url = request.url();
        if(url == "/"){
            LOG_INFO << "Main" ;
            std::string body = ReadFile("../static/index.html");

            response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
            response->SetBody(body);
            response->SetContentType("text/html");
        }else if(url == "/mhw"){
            LOG_INFO << "mhw";
            std::string body = ReadFile("../static/mhw.html");

            response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
            response->SetBody(body);
            response->SetContentType("text/html");
        }else if(url == "/cat.jpg"){
            LOG_INFO << "cat";
            std::string body = ReadFile("../static/cat.jpg");

            response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
            response->SetBody(body);
            response->SetContentType("image/jpeg");

        }else{
            response->SetStatusCode(HttpResponse::HttpStatusCode::k404NotFound);
            response->SetStatusMessage("Not Found");
            response->SetBody("Sorry Not Found\n");
            response->SetCloseConnection(true);
        }
    }
    return;

    // LOG_INFO << request.GetMethodString() << " " << request.url();
    // std::string url = request.url();
    // if(request.method() == HttpRequest::Method::kGet){
        

    //     if(url == "/"){
    //         LOG_INFO << "index";
            // std::string body = ReadFile("../static/index.html");
    //         response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
    //         response->SetBody(body);
    //         response->SetContentType("text/html");
    //     }else if(url == "/mhw"){
    //         LOG_INFO << "mhw";
    //         std::string body = ReadFile("../static/mhw.html");
    //         response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
    //         response->SetBody(body);
    //         response->SetContentType("text/html");
    //     }else if(url == "/cat.jpg"){
    //         LOG_INFO << "cat";
    //         std::string body = ReadFile("../static/cat.jpg");
    //         response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
    //         response->SetBody(body);
    //         response->SetContentType("image/jpeg");
    //     }else{
    //         LOG_INFO << "ERROR";
    //         response->SetStatusCode(HttpResponse::HttpStatusCode::k404NotFound);
    //         response->SetStatusMessage("Not Found");
    //         response->SetBody("Sorry Not Found\n");
    //         response->SetCloseConnection(true);
    //     }
    // }
    // else if( request.method() == HttpRequest::Method::kPost){
    //     if(url == "/login"){
    //         LOG_INFO << "login";
    //         // 进入登陆界面
    //         std::string rqbody = request.body();

    //         // 解析
    //         int usernamePos = rqbody.find("username=");
    //         int passwordPos = rqbody.find("password=");

    //         usernamePos += 9; // "username="的长度
    //         passwordPos += 9; // 

    //         // 找到中间分割符
    //         size_t usernameEndPos = rqbody.find('&', usernamePos);
    //         size_t passwordEndPos = rqbody.length();

    //         // Extract the username and password substrings
    //         std::string username = rqbody.substr(usernamePos, usernameEndPos - usernamePos);
    //         std::string password = rqbody.substr(passwordPos, passwordEndPos - passwordPos);

    //         if (username == "wlgls"){
    //             response->SetBody("login ok!\n");
    //         }
    //         else{
    //             response->SetBody("error!\n");
    //         }
    //         response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
    //         response->SetStatusMessage("OK");
    //         response->SetContentType("text/plain");
    //     }
    //     }
    // return;
}

std::unique_ptr<AsyncLogging> asynclog;
void AsyncOutputFunc(const char *data, int len)
{

    asynclog->Append(data, len);
  

}

void AsyncFlushFunc() {
    asynclog->Flush();
}

int main(int argc, char *argv[])
{
    int port;
    if (argc <= 1)
    {
        port = 1234;
    }
    else if (argc == 2)
    {
        port = atoi(argv[1]);
    }
    else
    {
        printf("error");
        exit(0);
    }

//    asynclog = std::make_unique<AsyncLogging>();
//    Logger::setOutput(AsyncOutputFunc);
//    Logger::setFlush(AsyncFlushFunc);


    // asynclog->Start();
  


    int size = std::thread::hardware_concurrency();
    EventLoop *loop = new EventLoop();
    HttpServer *server = new HttpServer(loop, "127.0.0.1", port);
    server->SetHttpCallback(HttpResponseCallback);
    server->SetThreadNums(size);
    server->start();
    
    return 0;
}