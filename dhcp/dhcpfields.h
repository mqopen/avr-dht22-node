#ifndef __DHCPFIELDS_H__
#define __DHCPFIELDS_H__

struct dhcp_lease_time {
    uint8_t x[4];
};

#define dhcp_lease_time_copy(src, dst)                                  \
                    do {                                                \
                        (dst).x[0] = (src).x[0];                        \
                        (dst).x[1] = (src).x[1];                        \
                        (dst).x[2] = (src).x[2];                        \
                        (dst).x[3] = (src).x[3];                        \
                    } while (0)

#endif
