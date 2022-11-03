NAME = template
TMPNAME = template

FBT = ./fbt

all: run

run:
	@cd ../.. && \
		$(FBT) launch_app APPSRC=applications/$(NAME)
