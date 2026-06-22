# ft_traceroute

`ft_traceroute` is a small reimplementation of the Linux `traceroute` command.

It shows the path taken by packets from the local machine to a target host.

Example:

```bash
sudo ./ft_traceroute google.com
```

Output example:

```txt
traceroute to google.com (142.250.75.238), 30 hops max, 60 byte packets
 1  10.0.2.2 (10.0.2.2)  0.421 ms  0.398 ms  0.386 ms
 2  192.168.1.1 (192.168.1.1)  1.102 ms  1.084 ms  1.073 ms
 3  * * *
```

Each line is one network hop.

Each time value is the round-trip time for one probe.

`*` means that no answer was received before the timeout.

## Build

```bash
make
```

Debug build:

```bash
make debug
```

Clean:

```bash
make clean
```

Full clean:

```bash
make fclean
```

Rebuild:

```bash
make re
```

## Usage

```bash
sudo ./ft_traceroute [options] host
```

The program uses a raw ICMP socket to receive replies, so it usually needs root privileges or the appropriate capability.

## Options

```txt
--help                  Show help and exit
-n                      Do not resolve hop IP addresses to names
-f first_ttl            Start from this TTL
-m max_ttl              Maximum TTL / maximum number of hops
-N squeries             Maximum number of simultaneous probes
-p port                 Base destination port
-q nqueries             Number of probes per hop
-z sendwait             Delay between probes
-w waittime             Wait time for replies
```

Default values:

```txt
first TTL:              1
max hops:               30
probes per hop:         3
simultaneous probes:    16
base port:              33434
packet size:            60 bytes
wait time:              5 seconds
send wait:              0 second
```

## Simple explanation

`ft_traceroute` sends UDP packets to the target.

The first packets are sent with a TTL of 1.

The next packets are sent with a TTL of 2.

Then 3, then 4, and so on.

When a router receives a packet whose TTL reaches 0, it drops the packet and sends back an ICMP `Time Exceeded` message.

This lets the program discover each router on the path.

When the target is finally reached, the target usually answers with an ICMP `Port Unreachable` message, because traceroute uses unlikely UDP destination ports.

At that point, the trace is finished.

## How it works internally

The program uses two sockets:

```txt
UDP socket       -> sends probes
raw ICMP socket  -> receives ICMP replies
```

For each probe, the program stores:

```txt
TTL
probe index
destination port
send timestamp
timeout deadline
reply address
RTT
ICMP type
ICMP code
state
```

The destination port starts at `33434` by default and is incremented for each probe.

When an ICMP reply is received, the program parses the packet and reads the embedded original UDP header.
This allows it to recover the original destination port and match the reply with the correct probe.

## Probe states

Each probe has one of these states:

```txt
PROBE_FREE
PROBE_SENT
PROBE_REPLY_PENDING
PROBE_TIMEOUT_PENDING
```

Meaning:

```txt
PROBE_FREE              slot is available
PROBE_SENT              probe was sent and is waiting for a reply
PROBE_REPLY_PENDING     reply was received but not printed yet
PROBE_TIMEOUT_PENDING   timeout expired but not printed yet
```

The program can send several probes in advance, but it still prints them in the correct order.

## Main loop

The main loop follows this order:

```txt
read available ICMP replies
expire old probes
print ready probes
send new probes
wait for the next useful event
```

This allows the program to handle replies arriving in a different order than the probes were sent.

## ICMP replies

The program handles:

```txt
ICMP_TIME_EXCEEDED
ICMP_DEST_UNREACH
```

For `ICMP_DEST_UNREACH`, the program may print suffixes similar to traceroute:

```txt
!N      Network unreachable
!H      Host unreachable
!P      Protocol unreachable
!F      Fragmentation needed
!S      Source route failed
!X      Communication administratively prohibited
!V      Host precedence violation
!C      Precedence cutoff
!<num>  Other unreachable code
```

Any `ICMP_DEST_UNREACH` reply is considered terminal for the current trace.

This means the program prints the current hop and stops after it.

## DNS behavior

By default, `ft_traceroute` tries to resolve hop IP addresses into hostnames.

Example:

```txt
dc3.42.fr (62.210.35.1)
```

With `-n`, DNS resolution is disabled:

```bash
sudo ./ft_traceroute -n google.com
```

Then only IP addresses are printed.

## Project structure

```txt
includes/
  ft_traceroute.h
  traceroute_config.h
  session.h
  packet.h

srcs/
  main.c

  parsing/
    arg_parser.c
    args.c
    args_defaults.c
    args_error.c
    args_options.c
    args_validate.c
    args_values.c

  resolve/
    resolve.c

  socket/
    socket.c

  packet/
    parse_icmp.c

  session/
    init.c
    send.c
    recv.c
    expire.c
    print.c
    select.c
    loop.c

  output/
    error.c
    print_header.c
    print_usage.c

  debug/
    debug_*.c
```

## Parsing

The parser is split in two parts.

The generic parser reads command-line options and arguments.

The traceroute-specific parser applies the meaning of each option.

This keeps the generic option parser reusable and keeps the traceroute rules separate.

## Runtime session

The runtime state is stored in a `t_session`.

It contains:

```txt
scheduler state
output state
probe slots
probe capacity
total probe count
```

The scheduler keeps two positions:

```txt
send_ttl / send_probe_index
print_ttl / print_probe_index
```

This is important because sending and printing are not the same thing.

Replies may arrive out of order, but output must remain ordered.

## Limitations

This project only handles IPv4.

It uses UDP probes and ICMP replies.

It does not implement ICMP mode, TCP mode, IPv6, AS lookup, MTU discovery, or all advanced traceroute options.

## Notes

The project is designed for the 42 `ft_traceroute` subject.

Some bonus options are implemented, but the `-n` flag allows the output to match the simpler expected behavior without DNS resolution.
