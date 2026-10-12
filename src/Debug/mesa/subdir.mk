################################################################################
# Automatically-generated file. Do not edit!
################################################################################

S_UPPER_SRCS += \
${addprefix $(ROOT)/mesa/, \
}

CPP_SRCS += \
${addprefix $(ROOT)/mesa/, \
fglmesa.cpp \
}

CC_SRCS += \
${addprefix $(ROOT)/mesa/, \
}

C_SRCS += \
${addprefix $(ROOT)/mesa/, \
}

C_UPPER_SRCS += \
${addprefix $(ROOT)/mesa/, \
}

CXX_SRCS += \
${addprefix $(ROOT)/mesa/, \
}

S_SRCS += \
${addprefix $(ROOT)/mesa/, \
}

# Each subdirectory must supply rules for building sources it contributes
mesa/%.o: $(ROOT)/mesa/%.cpp
	@echo 'Building file: $<'
	@echo g++ -DBPP16 -DINTO_FGL -DX11_DRIVER -DFG_JPEG -I../../include/fastgl -I../linux -I../agg2 -I../ -O0 -g3 -Wall -c -fmessage-length=0 -o$@ $<
	@g++ -DBPP16 -DINTO_FGL -DX11_DRIVER -DFG_JPEG -I../../include/fastgl -I../linux -I../agg2 -I../ -O0 -g3 -Wall -c -fmessage-length=0 -o$@ $< && \
	echo -n $(@:%.o=%.d) 'mesa/' > $(@:%.o=%.d) && \
	g++ -MM -MG -P -w -DBPP16 -DINTO_FGL -DX11_DRIVER -DFG_JPEG -I../../include/fastgl -I../linux -I../agg2 -I../ -O0 -g3 -Wall -c -fmessage-length=0  $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '


