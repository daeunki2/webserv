# 🧠 Architecture (Mermaid Diagrams) ## 1. Main Request Flow
mermaid
flowchart TD

    %% ──────────────── PROGRAM START ────────────────
    A[Program starts: ./webserv config.txt] --> B[Check argc == 2?]
    B -->|No| B1[Print usage and exit]
    B -->|Yes| C[Load config file path]

    %% ─────────────── CONFIG PARSING ───────────────
    C --> D[ConfigParser(parser).read_file_lines()]
    D --> E[tokenize + syntax validation]
    E --> F[Build Server objects]
    F --> G[vector<Server> ready]

    %% ─────────────── SERVER INIT ───────────────
    G --> H[Create Server_Manager(servers)]
    H --> I[init_sockets(): For each server block → create socket()]
    I --> J[bind(), listen()]
    J --> K[push listening sockets into poll_fds]

    %% ─────────────── EVENT LOOP ───────────────
    K --> L[run(): enter while(g_running)]
    L --> M[poll() wait for events]

    %% ─────────────── ACCEPT NEW CLIENTS ───────────────
    M -->|POLLIN on listening fd| N[accept_new_client()]
    N --> N1[create Client object]
    N1 --> N2[add client fd to poll_fds]
    N2 --> L

    %% ─────────────── READ FROM CLIENTS ───────────────
    M -->|POLLIN on client fd| O[receive_request()]
    O --> P[Client::handle_recv_data()]
    P --> Q{Request complete?}

    Q -->|No| L
    Q -->|Yes| R[Client.build_response()]

    %% ─────────────── RESPONSE BUILDING ───────────────
    R --> R1{Method?}

    R1 -->|GET| G1[Response_Builder.handleGet()]
    R1 -->|POST| G2[Response_Builder.handlePost()]
    R1 -->|DELETE| G3[Response_Builder.handleDelete()]
    R1 -->|CGI| G4[spawn CGI process via execve()]

    G1 --> S[Generate HTTP response string]
    G2 --> S
    G3 --> S
    G4 --> S

    S --> T[Store response in Client buffer]
    T --> L

    %% ─────────────── WRITE BACK TO CLIENTS ───────────────
    M -->|POLLOUT on client fd| U[send_response()]
    U --> V{Keep-Alive?}
    V -->|No| W[close_connection()]
    V -->|Yes| L

    %% ─────────────── IDLE TIMEOUT ───────────────
    L --> X[check_idle_clients()]
    X --> L

    %% ─────────────── SHUTDOWN ───────────────
    L -->|SIGINT/SIGTERM| Y[g_running = 0]
    Y --> Z[close all sockets and exit]
## 2. Class Structure
mermaid
classDiagram
    class Server_Manager {
        - vector~Server~ _servers
        - map<int, Client> _clients
        + run()
        + accept_new_client()
        + receive_request()
        + send_response()
    }

    class Client {
        - http_request _req
        - string _response_buffer
        + handle_recv_data()
        + build_response()
    }

    class Response_Builder {
        - Server* _server
        - http_request& _req
        + build()
    }

    class Server {
        - root
        - vector<Location> locations
    }

    class Location {
        - path
        - allowed_methods
    }

    Server_Manager --> Client
    Client --> Response_Builder
    Response_Builder --> Server
    Server --> Location
