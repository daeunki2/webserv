// main.cpp

#include <iostream>
#include <string>
#include <vector>

// 가정: 아래 헤더 파일들은 RequestParser와 http_request 클래스 및 ENUM을 포함합니다.
#include "request_parser.hpp"
#include "http_request.hpp"
void run_test(const std::string& raw_request, const std::string& description)
{
    RequestParser parser;

    std::cout << "\n=================================================" << std::endl;
    std::cout << "  Scenario: " << description << std::endl;
    std::cout << "=================================================" << std::endl;
    std::cout << "--- 📄 원본 요청 ---" << std::endl;
    std::cout << raw_request << "\n-----------------------------------" << std::endl;

    // 데이터를 두 청크로 분할하여 논블로킹 시뮬레이션
    size_t split_point = raw_request.find("\r\n\r\n");
    if (split_point != std::string::npos) {
        split_point += 4; // 헤더 끝 '\r\n\r\n' 직후에서 분할
    } else {
        split_point = raw_request.size() / 2;
    }
    
    std::string chunk1 = raw_request.substr(0, split_point);
    std::string chunk2 = raw_request.substr(split_point);

    // 1차 전송 (헤더 및 헤더 종료까지)
    std::cout << "▶ 1차 전송 (" << chunk1.size() << " bytes) " << std::flush;
    ParsingState state = parser.load_data(chunk1.c_str(), chunk1.size());
    std::cout << "State: " << state << std::endl;

    // 2차 전송 (나머지 본문)
    if (state != PARSING_COMPLETED && state != PARSING_ERROR) {
        std::cout << "▶ 2차 전송 (" << chunk2.size() << " bytes) " << std::flush;
        state = parser.load_data(chunk2.c_str(), chunk2.size());
        std::cout << "State: " << state << std::endl;
    }

    // 결과 출력
    if (state == PARSING_COMPLETED) {
        parser.get_request().print_parsed_data(description);
    } else if (state == PARSING_ERROR) {
        std::cerr << "❌ PARSING ERROR 발생! (최종 상태 코드: " << state << ")" << std::endl;
    } else {
        std::cerr << "⚠️ 파싱 미완료! (최종 상태 코드: " << state << ")" << std::endl;
    }
}

int main()
{
    // 1. Content-Length (고정 길이) 기반 POST 요청 테스트
    const std::string CL_REQUEST =
        "POST /submit_form?user=test HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "Content-Length: 26\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "data1=hello&data2=world!\r\n"; // 26 bytes body

    // 2. Chunked (가변 길이) 기반 POST 요청 테스트
    const std::string CHUNKED_REQUEST =
        "POST /upload_data HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "Transfer-Encoding: chunked\r\n"
        "\r\n"
        "4\r\nWiki\r\n"
        "5\r\npedia\r\n"
        "0\r\n\r\n"; 
        
    // 3. Simple GET 요청 테스트 (본문 없음)
    const std::string GET_REQUEST =
        "GET /index.html HTTP/1.1\r\n"
        "Host: example.com\r\n"
        "User-Agent: test\r\n"
        "\r\n";


    run_test(CL_REQUEST, "1. Content-Length (고정 길이) 요청");
    run_test(CHUNKED_REQUEST, "2. Chunked (가변 길이) 요청");
    run_test(GET_REQUEST, "3. Simple GET (본문 없음) 요청");

    return 0;
}