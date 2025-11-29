flowchart TD

    %% ──────────────── PROGRAM START ────────────────
    A[Program starts: ./webserv config.txt] --> B[Check argc == 2?]
    B -->|No| B1[Print usage and exit]
    B -->|Yes| C[Load config file path]

    %% ─────────────── CONFIG PARSING ───────────────
    C --> D[ConfigParser::read_file_lines()]
    D --> E[Tokenize + Validate syntax]
    E --> F[Build vector<Server>]
    F --> G[Config ready]

    %% ─────────────── SERVER INIT ───────────────
    G --> H[Create Server_Manager(servers)]
    H --> I[init_sockets(): create/bind/listen]
    I --> J[Insert listening fds into poll_fds]

    %% ─────────────── EVENT LOOP ───────────────
    J --> L[run(): while(g_running)]
    L --> X[check_idle_clients()]
    X --> M[poll() wait for events]

    %% ─────────────── ACCEPT NEW CLIENTS ───────────────
    M -->|POLLIN on listening fd| N[accept_new_client()]
    N --> N1[Create Client object]
    N1 --> N2[Add client fd into poll_fds]
    N2 --> L

    %% ─────────────── READ FROM CLIENTS ───────────────
    M -->|POLLIN on client fd| O[receive_request()]
    O --> P[Client::handle_recv_data()]
    P --> Q{Request complete?}
    Q -->|No| L
    Q -->|Yes| R[Client::build_response()]

    %% ─────────────── RESPONSE BUILDING ───────────────
    R --> S{Method?}
    S -->|GET| G1[Response_Builder::handleGet()]
    S -->|POST| G2[Response_Builder::handlePost()]
    S -->|DELETE| G3[Response_Builder::handleDelete()]
    S -->|CGI| G4[Execute CGI via fork+execve]

    G1 --> T[Generate HTTP response string]
    G2 --> T
    G3 --> T
    G4 --> T

    T --> U[Store response in Client buffer]
    U --> L

    %% ─────────────── WRITE BACK TO CLIENTS ───────────────
    M -->|POLLOUT on client fd| V[send_response()]
    V --> W{Keep-Alive?}
    W -->|No| Z[close_connection()]
    W -->|Yes| L

    %% ─────────────── SHUTDOWN ───────────────
    L -->|SIGINT/SIGTERM| END[g_running = 0 → cleanup → exit]

