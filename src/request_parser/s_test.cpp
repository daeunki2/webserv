#include "request_parser.hpp" 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#define PORT 8080
#define BUFFER_SIZE 4096


void receive_and_parse_request() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    // 2. 소켓에 주소 바인딩
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    
    // 3. 연결 대기 (Listen)
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    std::cout << "웹 서버 대기 중... (포트 " << PORT << ")" << std::endl;

    // 4. 연결 수락 (Accept)
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    std::cout << "클라이언트 연결 수락. 요청 수신 중..." << std::endl;

    RequestParser parser;
    ssize_t valread;
    
    // 5. 데이터 수신 및 파싱 루프
    while (parser.get_state() != PARSING_COMPLETED && parser.get_state() != PARSING_ERROR) {
        memset(buffer, 0, BUFFER_SIZE);
        
        // 데이터 수신 (논블로킹이 아닌 블로킹 read를 사용하여 단순화)
        // 실제 웹서버에서는 poll/select를 사용해야 합니다.
        valread = read(new_socket, buffer, BUFFER_SIZE);
        
        if (valread <= 0) {
            // 연결 종료 또는 오류
            break;
        }

        // 수신된 데이터를 파서에 전달
        ParsingState current_state = parser.load_data(buffer, valread);

        std::cout << "  ▶ " << valread << " bytes 수신, 현재 상태: " << current_state << std::endl;
        
        // 파싱 오류 발생 시 중단
        if (current_state == PARSING_ERROR) {
            std::cerr << "❌ 파싱 오류 발생! (상태 6)" << std::endl;
            break;
        }
    }

    // 6. 결과 출력
    if (parser.get_state() == PARSING_COMPLETED) {
        parser.get_request().print_parsed_data("실제 브라우저 요청");

        // 7. 간단한 응답 전송 (필수 아님, 요청 성공 확인용)
        const char *hello = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nHello World!";
        send(new_socket, hello, strlen(hello), 0);
    } else {
        std::cerr << "⚠️ 요청 처리 미완료." << std::endl;
    }

    // 8. 소켓 정리
    close(new_socket);
    close(server_fd);
}

int main() {
    // 기존 s_test.cpp의 테스트 코드 대신 실제 서버 로직 실행
    receive_and_parse_request();
    return 0;
}


// // main.cpp

// #include <iostream>
// #include <string>
// #include <vector>

// // 가정: 아래 헤더 파일들은 RequestParser와 http_request 클래스 및 ENUM을 포함합니다.
// #include "request_parser.hpp"
// #include "http_request.hpp"
// void run_test(const std::string& raw_request, const std::string& description)
// {
//     RequestParser parser;

//     std::cout << "\n=================================================" << std::endl;
//     std::cout << "  Scenario: " << description << std::endl;
//     std::cout << "=================================================" << std::endl;
//     std::cout << "--- 📄 원본 요청 ---" << std::endl;
//     std::cout << raw_request << "\n-----------------------------------" << std::endl;

//     // 데이터를 두 청크로 분할하여 논블로킹 시뮬레이션
//     size_t split_point = raw_request.find("\r\n\r\n");
//     if (split_point != std::string::npos) {
//         split_point += 4; // 헤더 끝 '\r\n\r\n' 직후에서 분할
//     } else {
//         split_point = raw_request.size() / 2;
//     }
    
//     std::string chunk1 = raw_request.substr(0, split_point);
//     std::string chunk2 = raw_request.substr(split_point);

//     // 1차 전송 (헤더 및 헤더 종료까지)
//     std::cout << "▶ 1차 전송 (" << chunk1.size() << " bytes) " << std::flush;
//     ParsingState state = parser.load_data(chunk1.c_str(), chunk1.size());
//     std::cout << "State: " << state << std::endl;

//     // 2차 전송 (나머지 본문)
//     if (state != PARSING_COMPLETED && state != PARSING_ERROR) {
//         std::cout << "▶ 2차 전송 (" << chunk2.size() << " bytes) " << std::flush;
//         state = parser.load_data(chunk2.c_str(), chunk2.size());
//         std::cout << "State: " << state << std::endl;
//     }

//     // 결과 출력
//     if (state == PARSING_COMPLETED) {
//         parser.get_request().print_parsed_data(description);
//     } else if (state == PARSING_ERROR) {
//         std::cerr << "❌ PARSING ERROR 발생! (최종 상태 코드: " << state << ")" << std::endl;
//     } else {
//         std::cerr << "⚠️ 파싱 미완료! (최종 상태 코드: " << state << ")" << std::endl;
//     }
// }

// int main()
// {
//     // 1. Content-Length (고정 길이) 기반 POST 요청 테스트
//     const std::string CL_REQUEST =
//         "POST /submit_form?user=test HTTP/1.1\r\n"
//         "Host: localhost:8080\r\n"
//         "Content-Length: 26\r\n"
//         "Connection: keep-alive\r\n"
//         "\r\n"
//         "data1=hello&data2=world!\r\n"; // 26 bytes body

//     // 2. Chunked (가변 길이) 기반 POST 요청 테스트
//     const std::string CHUNKED_REQUEST =
//         "POST /upload_data HTTP/1.1\r\n"
//         "Host: example.com\r\n"
//         "Transfer-Encoding: chunked\r\n"
//         "\r\n"
//         "4\r\nWiki\r\n"
//         "5\r\npedia\r\n"
//         "0\r\n\r\n"; 
        
//     // 3. Simple GET 요청 테스트 (본문 없음)
//     const std::string GET_REQUEST =
//         "GET /index.html HTTP/1.1\r\n"
//         "Host: example.com\r\n"
//         "User-Agent: test\r\n"
//         "\r\n";


//     run_test(CL_REQUEST, "1. Content-Length (고정 길이) 요청");
//     run_test(CHUNKED_REQUEST, "2. Chunked (가변 길이) 요청");
//     run_test(GET_REQUEST, "3. Simple GET (본문 없음) 요청");

//     return 0;
// }