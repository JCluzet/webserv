#include "Request.hpp"
#include <iostream>

int main()
{
    std::string test = "GET / HTTP/1.1\r\n";
    test += "Host: localhost:9943\r\n";
    test += "Connection: keep-alive\r\n";
    test += "sec-ch-ua: \" Not A;Brand\";v=\"99\", \"Chromium\";v=\"100\", \"Google Chrome\";v=\"100\"\r\n";
    test += "sec-ch-ua-mobile: ?0\r\nsec-ch-ua-platform: \"macOS\"\r\n";
    test += "Upgrade-Insecure-Requests: 1\r\n";
    test += "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.127 Safari/537.36\r\n";
    test += "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n";
    test += "Sec-Fetch-Site: none\r\n";
    test += "Sec-Fetch-Mode: navigate\r\nSec-Fetch-User: ?1\r\n";
    test += "Sec-Fetch-Dest: document\r\nAccept-Encoding: gzip, deflate, br\r\n";
    test += "Accept-Language: en-GB,en-US;q=0.9,en;q=0.8,fr;q=0.7\r\n";
    // std::string test = "GET                   /    HTTP/1.1\nHost: localhost:9943\nConnection: keep-alive\nsec-ch-ua: \" Not A;Brand\";v=\"99\", \"Chromium\";v=\"100\", \"Google Chrome\";v=\"100\"\nsec-ch-ua-mobile: ?0\nsec-ch-ua-platform: \"macOS\"\nUpgrade-Insecure-Requests: 1\nUser-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/100.0.4896.127 Safari/537.36\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\nSec-Fetch-Site: none\nSec-Fetch-Mode: navigate\nSec-Fetch-User: ?1\nSec-Fetch-Dest: document\nAccept-Encoding: gzip, deflate, br\nAccept-Language: en-GB,en-US;q=0.9,en;q=0.8,fr;q=0.7\n";
    // std::cout << test << std::endl;
    // std::cout << "----------------------------------------" << std::endl;
    Request data(test);
    // data.print();
    // std::cout << "----------------------------------------" << std::endl;
    data.add("Date: PROUT\r\nRetry-After: LOL\r\n");
    data.add("Accept: PROUT\r\nRetry-After: LOL\r\n");
    // data.print();
    std::cout << "$$" << data.get_request() << "$$" << std::endl;
    return(0);
}