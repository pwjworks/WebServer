add_library(webserver SHARED
        buffer/Buffer.cpp
        server/Server.cpp
        server/ReactorServer.cpp
        server/SimpleEpollServer.cpp
        server/SimpleForkServer.cpp
        epoller/Epoller.cpp
        eventloop/EventLoop.cpp
        eventloop/EventLoopThreadPool.cpp
        http/HttpData.cpp
        util/Util.cpp
        )