

.DEFAULT_GOAL := all

# **************************************************************************** #
#                                  PROGRAM                                     #
# **************************************************************************** #

NAME := ft_traceroute

# **************************************************************************** #
#                                  COMPILER                                    #
# **************************************************************************** #

CC := cc
CFLAGS := -Wall -Wextra -Werror
CPPFLAGS := -Iincludes
DEPFLAGS := -MMD -MP
RM := rm -rf

# **************************************************************************** #
#                                NETWORK FLAGS                                 #
# **************************************************************************** #

BRIDGE_IFACE := enp0s8
NAT_IFACE := enp0s3
NAT_GW := 10.0.2.2

# **************************************************************************** #
#                                DIRECTORIES                                   #
# **************************************************************************** #

OBJS_DIR := objs
DEBUG_OBJS_DIR := objs_debug

# **************************************************************************** #
#                                  SOURCES                                     #
# **************************************************************************** #

SRCS :=	srcs/main.c \
		srcs/output/error.c \
		srcs/output/print_header.c \
		srcs/output/print_usage.c \
		srcs/parsing/arg_parser.c \
		srcs/parsing/args.c \
		srcs/parsing/args_defaults.c \
		srcs/parsing/args_error.c \
		srcs/parsing/args_options.c \
		srcs/parsing/args_validate.c \
		srcs/parsing/args_values.c \
		srcs/resolve/resolve.c \
		srcs/socket/socket.c \
		srcs/session/init.c \
		srcs/session/send.c \
		srcs/packet/parse_icmp.c \
		srcs/session/loop.c \
		srcs/session/recv.c \
		srcs/session/expire.c \
		srcs/session/print.c \
		srcs/session/select.c

DEBUG_SRCS :=	$(SRCS) \
				srcs/debug/debug_args.c \
				srcs/debug/debug_resolve.c \
				srcs/debug/debug_socket.c \
				srcs/debug/debug_send.c \
				srcs/debug/debug_packet.c \
				srcs/debug/debug_probe.c \
				srcs/debug/debug_recv.c

# **************************************************************************** #
#                                  OBJECTS                                     #
# **************************************************************************** #

OBJS := $(patsubst %.c,$(OBJS_DIR)/%.o,$(SRCS))
DEBUG_OBJS := $(patsubst %.c,$(DEBUG_OBJS_DIR)/%.o,$(DEBUG_SRCS))

DEPS := $(OBJS:.o=.d)
DEBUG_DEPS := $(DEBUG_OBJS:.o=.d)

# **************************************************************************** #
#                                   RULES                                      #
# **************************************************************************** #

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

debug: $(DEBUG_OBJS)
	$(CC) $(CFLAGS) -DDEBUG_ARGS $(DEBUG_OBJS) -o $(NAME)

$(OBJS_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(DEPFLAGS) -c $< -o $@

$(DEBUG_OBJS_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DDEBUG_ARGS $(CPPFLAGS) $(DEPFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS_DIR)
	$(RM) $(DEBUG_OBJS_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

# **************************************************************************** #
#                              NETWORK RULES                                   #
# **************************************************************************** #

net-check:
	@ip -br a
	@ip route
	@ip route get 1.1.1.1 || true

net-bridge:
	@printf "Activating bridge interface: $(BRIDGE_IFACE)\n"
	@sudo ip link set $(BRIDGE_IFACE) up
	@sudo dhclient -r $(BRIDGE_IFACE) >/dev/null 2>&1 || true
	@sudo dhclient -v $(BRIDGE_IFACE)
	@GW=$$(grep -h "option routers" \
		/var/lib/dhcp/dhclient.$(BRIDGE_IFACE).leases \
		/var/lib/dhcp/dhclient.leases \
		2>/dev/null | tail -1 | awk '{gsub(/;/, "", $$3); print $$3}'); \
	if [ -z "$$GW" ]; then \
		printf "Error: no gateway found for $(BRIDGE_IFACE)\n" >&2; \
		exit 1; \
	fi; \
	printf "Setting default route via %s dev $(BRIDGE_IFACE)\n" "$$GW"; \
	sudo ip route replace default via "$$GW" dev $(BRIDGE_IFACE)
	@ip route get 1.1.1.1

net-nat:
	@printf "Restoring NAT default route\n"
	@sudo ip route replace default via $(NAT_GW) dev $(NAT_IFACE)
	@ip route get 1.1.1.1

# **************************************************************************** #
#                                DEPENDENCIES                                  #
# **************************************************************************** #

-include $(DEPS)
-include $(DEBUG_DEPS)

.PHONY: all debug clean fclean re net-check net-bridge net-nat

