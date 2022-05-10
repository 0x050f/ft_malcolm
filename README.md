# ft_malcolm

Introduction to network security with Man In The Middle attack by doing ARP spoofing.
`ft_malcolm` will wait for a ARP broadcast to be sent over the network and will then send an ARP reply to `<target>` from `<source>`.

## Compilation

```
make
```

## Usage

```
sudo ./ft_malcolm <source_ip> <source_mac> <address_target_ip> <target_mac_address>
```
(need sudo to bind socket)