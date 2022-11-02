# Administrowanie Systemami Komputerowymi - setup VMs

## Import VMs

* router:
  * Network 1: NAT
  * Network 2: Internal, 'intnet'

* client 1 & 2:
  * Network 1: Internal, 'intnet'

## Setup network interfaces

```
sudo vim /etc/netplan/01-netcfg.yaml
```

* Router configuration
    ```
    network:
      version: 2
      renderer: networkd
      ethernets:
        enp0s3:
          dhcp4: yes
        enp0s8:
          dhcp4: no
          addresses:
            - 10.0.10.1/24  
    ```

* Client configuration
  IP address should be set according to client index
    ```
    network:
      version: 2
      renderer: networkd
      ethernets:
        enp0s3:
          dhcp4: no
          addresses:
            - 10.0.10.2/24  
    ```


```
sudo netplan apply
```

## Miscellaneous

On ubuntu, root username and password are `ask`
