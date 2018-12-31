TEMPLATE = subdirs

# List all dependencies
# TODO: add thirdparty files there
SUBDIRS = \
     screencapture
#    thirdparty/

CONFIG += ordered

# build espia core eventually
SUBDIRS += espia_core
