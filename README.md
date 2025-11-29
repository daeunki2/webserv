
flowchart LR

    subgraph SM[Server_Manager]
        SM_Servers[servers: vector<Server>]
        SM_Clients[clients: map<fd, Client>]
        SM_poll[poll_fds]
    end

    subgraph S[Server]
        S_root[root]
        S_locations[locations]
    end

    subgraph C[Client]
        C_req[http_request]
        C_buffer[response_buffer]
        C_state[state]
    end

    subgraph RB[Response_Builder]
        RB_build[build()]
    end

    ClientInput[Client sends HTTP request] --> SM
    SM -->|accept()| C

    C -->|raw bytes| C_req
    C_req -->|parsed request| C_state

    C_state -->|REQUEST_COMPLETE| RB
    RB -->|build response string| C_buffer
    C_buffer -->|POLLOUT| SM
    SM -->|send()| ClientOutput[HTTP response to client]
