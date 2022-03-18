add_library(webserver SHARED
        channel/Channel.cpp
        server/Server.cpp
        server/SimpleEpollServer.cpp
        server/SimpleForkServer.cpp
        epoller/Epoller.cpp
        http/HttpData.cpp
        util/util.cpp
        )