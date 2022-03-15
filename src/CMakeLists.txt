add_library(webserver
        server/Server.cpp
        server/SimpleEpollServer.cpp
        epoller/Epoller.cpp
        http/HttpData.cpp
        util/util.cpp
        )