
TEMPLATE = subdirs


SUBDIRS = \
    src \
    

# build order: 3rdparty -> libs -> app-static -> app & test
#libs.depends = 3rdparty
#app.depends = libs app-static
#test.depends = libs app-static
