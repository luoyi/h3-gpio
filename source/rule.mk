CURDIR := $(SRCTOP)/source
VPATH := $(VPATH) $(CURDIR) 
SRCS := $(SRCS) $(get_src)

$(eval $(call EXE_template,gpio-demo,gpio-demo gpio-h3))
$(eval $(call EXE_template,uinput,uinput))
$(eval $(call EXE_template,json-simple,json-simple jsmn))
$(eval $(call EXE_template,json-demo,json-demo jsmn))
$(eval $(call EXE_template,h3-keypad,h3-keypad gpio-h3 jsmn))
$(eval $(call EXE_template,strtest,strtest))
