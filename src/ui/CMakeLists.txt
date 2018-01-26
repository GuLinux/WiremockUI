set(ui_SRCS
    mainwindow.cpp
    mainwindow.ui
    trafficpage.cpp
    trafficpage.ui
    mappingspage.cpp
    mappingspage.ui
    bodyviewer.cpp
    addeditmapping.ui
    addeditmapping.cpp
    mappingview.cpp
    )

add_library(ui STATIC ${ui_SRCS})
