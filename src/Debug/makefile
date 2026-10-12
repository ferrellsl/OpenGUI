################################################################################
# Automatically-generated file. Do not edit!
################################################################################

ROOT := ..

-include $(ROOT)/makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include $(SUBDIRS:%=%/subdir.mk)
-include objects.mk
-include $(DEPS)
-include $(ROOT)/makefile.defs

all: fgl-eclipse

fgl-eclipse: $(OBJS)
	@echo 'Building target: $@'
	g++ -L/usr/X11R6/lib -o $@ $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building: $@'

clean:
	-$(RM) $(OBJS) $(DEPS) fgl-eclipse

.PHONY: all clean dependents

-include $(ROOT)/makefile.targets
