#include <iostream>
#include "../Http/HttpServer.h"
#include "../Http/HttpRequest.h"
#include "../Http/HttpResponse.h"
#include "../Tcp/EventLoop.h"
#include "../Log/AsyncLogging.h"
#include <string>
#include <fstream>
#include <dirent.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/sendfile.h>



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

void FindAllFiles(const std::string& path, std::vector<std::string>& filelist)
{
    DIR* dir;
    struct dirent* entry;
    if ((dir = opendir(path.c_str())) == nullptr) {
        LOG_INFO << "Opendir error!!";
    }    
    while ((entry = readdir(dir)) != NULL) {
        std::string filename = entry->d_name;
        if (filename != "." && filename != "..") {
            filelist.push_back(filename);
        }
    }
}

// 构建filelist.html
std::string BuildFileHtml(){
    std::vector<std::string> filelist;
    // 以/files文件夹为例
    FindAllFiles("../files", filelist);

    // 为文件生成模板
    std::string file = "";
    for (auto filename : filelist)
    {
        //将fileitem中的所有filename替换成
        file += "<tr><td>" + filename + "</td>" +
                "<td>" +
                "<a href=\"/download/" + filename + "\">下载</a>" +
                "<a href=\"/delete/" + filename + "\">删除</a>" +
                "</td></tr>" + "\n";
    }


    //生成html页面
    // 主要通过将<!--filelist-->直接进行替换实现
    std::string tmp = "<!--filelist-->";
    std::string filehtml = ReadFile("../static/fileserver.html");
    filehtml = filehtml.replace(filehtml.find(tmp), tmp.size(), file);
    return filehtml;
}

void RemoveFile(const std::string & filename){
    int ret = remove(("../files/" + filename).c_str());
    if(ret != 0){
        LOG_ERROR << "删除文件 " << filename << " 失败";
    }
}

void DownloadFile(const std::string &filename, HttpResponse *response){
    int filefd = ::open(("../files/" + filename).c_str(), O_RDONLY);
    if(filefd == -1){
        LOG_ERROR << "OPEN FILE ERROR";
        response->SetStatusCode(HttpResponse::HttpStatusCode::k302K);
        response->SetStatusMessage("Moved Temporarily");
        response->SetContentType("text/html");
        response->AddHeader("Location", "/fileserver");
    }else{
        // 获取文件信息
        struct stat fileStat;
        fstat(filefd, &fileStat);
        // 设置响应头字段
        response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
        response->SetContentLength(fileStat.st_size);
        response->SetContentType("application/octet-stream");
        
        response->SetBodyType(HttpResponse::HttpBodyType::FILE_TYPE);
        // response->AddHeader("Transfer-Encoding", "chunked");

        // 设置文件
        response->SetFileFd(filefd);
    }
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
            response->SetContentLength(body.size());

            response->SetContentType("text/html");
        }else if(url == "/mhw"){
            LOG_INFO << "mhw";
            std::string body = ReadFile("../static/mhw.html");

            response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
            response->SetBody(body);
            response->SetContentLength(body.size());

            response->SetContentType("text/html");
        }else if(url == "/cat.jpg"){
            LOG_INFO << "cat";
            std::string body = ReadFile("../static/cat.jpg");

            response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
            response->SetBody(body);
            response->SetContentLength(body.size());

            response->SetContentType("image/jpeg");

        }else if(url == "/fileserver") {
            std::string body = BuildFileHtml();
            response->SetContentLength(body.size());
            response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
            response->SetBody(body);
            response->SetContentLength(body.size());

            response->SetContentType("text/html");
        }
        else if(url.substr(0, 7) == "/delete") {
            // 删除特定文件，由于使用get请求，并且会将相应删掉文件的名称放在url中
            RemoveFile(url.substr(8));
            // 发送重定向报文，删除后返回自身应在的位置
            response->SetStatusCode(HttpResponse::HttpStatusCode::k302K);
            response->SetStatusMessage("Moved Temporarily");
            response->SetContentType("text/html");
            response->AddHeader("Location", "/fileserver");
        }else if(url.substr(0, 9) == "/download"){
            DownloadFile(url.substr(10), response);
            //response->SetStatusCode(HttpResponse::HttpStatusCode::k200K);
        }
        else{
            response->SetStatusCode(HttpResponse::HttpStatusCode::k404NotFound);
            response->SetStatusMessage("Not Found");
            response->SetBody("Sorry Not Found\n");
            response->SetCloseConnection(true);
        }
    }
    return;

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