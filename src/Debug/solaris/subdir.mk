################################################################################
# Automatically-generated file. Do not edit!
################################################################################

S_UPPER_SRCS += \
${addprefix $(ROOT)/solaris/, \
}

CPP_SRCS += \
${addprefix $(ROOT)/solaris/, \
solaris.cpp \
}

CC_SRCS += \
${addprefix $(ROOT)/solaris/, \
}

C_SRCS += \
${addprefix $(ROOT)/solaris/, \
}

C_UPPER_SRCS += \
${addprefix $(ROOT)/solaris/, \
}

CXX_SRCS += \
${addprefix $(ROOT)/solaris/, \
}

S_SRCS += \
${addprefix $(ROOT)/solaris/, \
}

# Each subdirectory must supply rules for building sources it contributes
solaris/%.o: $(ROOT)/solaris/%.cpp
	@echo 'Building file: $<'
	@echo g++ -DBPP16 -D__REGRESS__ -DINTO_FGL -DX11_DRIVER -DFG_JPEG -I../../include/fastgl -I../linux -I../agg2 -I../ -O0 -g3 -Wall -c -fmessage-length=0 -o$@ $<
	@g++ -DBPP16 -D__REGRESS__ -DINTO_FGL -DX11_DRIVER -DFG_JPEG -I../../include/fastgl -I../linux -I../agg2 -I../ -O0 -g3 -Wall -c -fmessage-length=0 -o$@ $< && \
	echo -n $(@:%.o=%.d) $(dir $@) > $(@:%.o=%.d) && \
	g++ -MM -MG -P -w -DBPP16 -D__REGRESS__ -DINTO_FGL -DX11_DRIVER -DFG_JPEG -I../../include/fastgl -I../linux -I../agg2 -I../ -O0 -g3 -Wall -c -fmessage-length=0  $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '


