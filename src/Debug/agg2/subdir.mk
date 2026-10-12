################################################################################
# Automatically-generated file. Do not edit!
################################################################################

S_UPPER_SRCS += \
${addprefix $(ROOT)/agg2/, \
}

CPP_SRCS += \
${addprefix $(ROOT)/agg2/, \
agg_rasterizer_scanline_aa.cpp \
agg_sqrt_tables.cpp \
agg_trans_affine.cpp \
}

CC_SRCS += \
${addprefix $(ROOT)/agg2/, \
}

C_SRCS += \
${addprefix $(ROOT)/agg2/, \
}

C_UPPER_SRCS += \
${addprefix $(ROOT)/agg2/, \
}

CXX_SRCS += \
${addprefix $(ROOT)/agg2/, \
}

S_SRCS += \
${addprefix $(ROOT)/agg2/, \
}

# Each subdirectory must supply rules for building sources it contributes
agg2/%.o: $(ROOT)/agg2/%.cpp
	@echo 'Building file: $<'
	@echo g++ -DBPP16 -D__REGRESS__ -DINTO_FGL -DX11_DRIVER -DFG_JPEG -I../../include/fastgl -I../linux -I../agg2 -I../ -O0 -g3 -Wall -c -fmessage-length=0 -o$@ $<
	@g++ -DBPP16 -D__REGRESS__ -DINTO_FGL -DX11_DRIVER -DFG_JPEG -I../../include/fastgl -I../linux -I../agg2 -I../ -O0 -g3 -Wall -c -fmessage-length=0 -o$@ $< && \
	echo -n $(@:%.o=%.d) $(dir $@) > $(@:%.o=%.d) && \
	g++ -MM -MG -P -w -DBPP16 -D__REGRESS__ -DINTO_FGL -DX11_DRIVER -DFG_JPEG -I../../include/fastgl -I../linux -I../agg2 -I../ -O0 -g3 -Wall -c -fmessage-length=0  $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '


