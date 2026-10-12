################################################################################
# Automatically-generated file. Do not edit!
################################################################################

S_UPPER_SRCS += \
${addprefix $(ROOT)/, \
}

CPP_SRCS += \
${addprefix $(ROOT)/, \
bench.cpp \
detect.cpp \
dll.cpp \
drivers.cpp \
fastgl.cpp \
fgapp.cpp \
fgbase.cpp \
fgbitmap.cpp \
fgcolor.cpp \
fgconcurency.cpp \
fgcontrols.cpp \
fgdrawbuffer.cpp \
fgeditor.cpp \
fgevent.cpp \
fgfontmanager.cpp \
fggradient.cpp \
fglistbox.cpp \
fgmatrix.cpp \
fgpane.cpp \
fgscheme.cpp \
fgshape.cpp \
fgtimer.cpp \
fgttf.cpp \
fgx.cpp \
listbox.cpp \
resource.cpp \
widgets.cpp \
}

CC_SRCS += \
${addprefix $(ROOT)/, \
}

C_SRCS += \
${addprefix $(ROOT)/, \
}

C_UPPER_SRCS += \
${addprefix $(ROOT)/, \
}

CXX_SRCS += \
${addprefix $(ROOT)/, \
}

S_SRCS += \
${addprefix $(ROOT)/, \
}

# Each subdirectory must supply rules for building sources it contributes
%.o: $(ROOT)/%.cpp
	@echo 'Building file: $<'
	@echo g++ -DBPP16 -D__REGRESS__ -DINTO_FGL -DX11_DRIVER -DFG_JPEG -I../../include/fastgl -I../linux -I../agg2 -I../ -O0 -g3 -Wall -c -fmessage-length=0 -o$@ $<
	@g++ -DBPP16 -D__REGRESS__ -DINTO_FGL -DX11_DRIVER -DFG_JPEG -I../../include/fastgl -I../linux -I../agg2 -I../ -O0 -g3 -Wall -c -fmessage-length=0 -o$@ $< && \
	echo -n $(@:%.o=%.d) $(dir $@) > $(@:%.o=%.d) && \
	g++ -MM -MG -P -w -DBPP16 -D__REGRESS__ -DINTO_FGL -DX11_DRIVER -DFG_JPEG -I../../include/fastgl -I../linux -I../agg2 -I../ -O0 -g3 -Wall -c -fmessage-length=0  $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '


