```mermaid

flowchart TD

%% ===============================
%% PROGRAM STARTUP
%% ===============================
A["Program starts (./webserv config.txt)"]
  --> B["Parse configuration file"]
  --> C["Create Server_Manager"]

%% ===============================
%% SERVER INITIALIZATION
%% ===============================
C --> D["Initialize server sockets"]
D --> D1["socket()"]
D --> D2["bind()"]
D --> D3["listen()"]
D3 --> E["Server ready"]

%% ===============================
%% MAIN EVENT LOOP
%% ===============================
E --> F["Enter main loop"]
F --> G{"Server running?"}

G --> H["poll(): wait for events"]

H -->|No events| G

%% ===============================
%% EVENT DISPATCH
%% ===============================
H --> I["Event detected"]
I --> J{Listening socket_ready?}

%% ===============================
%% NEW CLIENT CONNECTION
%% ===============================
J -->|Yes| K["accept() new connection"]
K --> K1["Create Client object"]
K1 --> K2["Register client socket to poll()"]
K2 --> G

%% ===============================
%% CLIENT SOCKET EVENT
%% ===============================
J -->|No| L{Client socket_event?}

L -->|Error / Hangup| X["Close connection"]

%% ===============================
%% RECEIVE REQUEST
%% ===============================
L -->|Readable (POLLIN)| M["recv(): read incoming data"]
M --> N["Append data to Client buffer"]
N --> O{"Is HTTP request complete?"}

O -->|No| G

%% ===============================
%% REQUEST PARSING COMPLETE
%% ===============================
O -->|Yes| P["Client state → REQUEST_COMPLETE"]
P --> Q["Build HTTP response"]
Q --> Q1["Analyze request (method, path, headers)"]
Q1 --> Q2["Generate response via Response_Builder"]
Q2 --> R["Prepare socket for sending (POLLOUT)"]

%% ===============================
%% SEND RESPONSE
%% ===============================
L -->|Writable (POLLOUT)| S["send(): transmit response"]
S --> T{"Response fully sent?"}

T -->|No| G

%% ===============================
%% CONNECTION DECISION
%% ===============================
T -->|Yes| U{"Keep-Alive?"}

U -->|Yes| V["Reset Client state"]
V --> V1["Wait for next request on same connection"]
V1 --> G

U -->|No| X




```

