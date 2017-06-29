CURDIR := $(SRCTOP)/source
VPATH := $(VPATH) $(CURDIR) 
SRCS := $(SRCS) $(get_src)

$(eval $(call EXE_template,gpio-demo,gpio-demo gpio-h3))
$(eval $(call EXE_template,uinput,uinput))
