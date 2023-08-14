# Linux stable kernel builds
Those are kernel builds made and supported by Zabbly.
They track the latest stable mainline kernel and are build for both `x86_64` and `aarch64`.

The general goal behind those kernel builds is to provide a recent
stable mainline kernel with wide hardware support and a configuration
that's optimal for running [Incus](https://github.com/lxc/incus) containers and VMs.

Those are usually updated weekly, shortly after a new bugfix release.
They do not immediately roll to a new kernel release, instead waiting for its first bugfix release to be out.

## Availability
Those kernels are built for:

 * Ubuntu 20.04 LTS (`focal`)
 * Ubuntu 22.04 LTS (`jammy`)
 * Ubuntu 24.04 LTS (`noble`)
 * Debian 11 (`bullseye`) (`x86_64` only)
 * Debian 12 (`bookworm`)

## Installation

All commands should be run as root.

### Repository key

Packages provided by the repository are signed. In order to verify the integrity of the packages, you need to import the public key. First, verify that the fingerprint your observe on the package repository matches the one listed below:

```sh
curl -fsSL https://pkgs.zabbly.com/key.asc | gpg --show-keys --fingerprint
```

Alternatively using wget (curl is not installed by default on Debian):

```sh
wget -qO- https://pkgs.zabbly.com/key.asc | gpg --show-keys --fingerprint
```

Compare that using your eyes with this authoritative fingerprint:
```sh
pub   rsa3072 2023-08-23 [SC] [expires: 2025-08-22]
      4EFC 5906 96CB 15B8 7C73  A3AD 82CC 8797 C838 DCFD
uid                      Zabbly Kernel Builds <info@zabbly.com>
sub   rsa3072 2023-08-23 [E] [expires: 2025-08-22]
```

If they match, save the key locally:

```sh
mkdir -p /etc/apt/keyrings/
curl -fsSL https://pkgs.zabbly.com/key.asc -o /etc/apt/keyrings/zabbly.asc
```
Alternatively using wget:

```sh
mkdir -p /etc/apt/keyrings/
wget -q https://pkgs.zabbly.com/key.asc -O /etc/apt/keyrings/zabbly.asc
```


### Stable repository

On any of the distributions above, you can add the package repository at `/etc/apt/sources.list.d/zabbly-kernel-stable.sources`.

Run the following command to add the stable repository:

```sh
sh -c 'cat <<EOF > /etc/apt/sources.list.d/zabbly-kernel-stable.sources
Enabled: yes
Types: deb
URIs: https://pkgs.zabbly.com/kernel/stable
Suites: $(. /etc/os-release && echo ${VERSION_CODENAME})
Components: main
Architectures: $(dpkg --print-architecture)
Signed-By: /etc/apt/keyrings/zabbly.asc

EOF'
```

### Installing the kernel

First update the package list using: `apt-get update`, you should see it hit the pkgs.zabbly.com repository.

Finally, install the kernel, with: `apt-get install linux-zabbly`.

## Secure boot
As those kernels aren't signed by a trusted distribution key, you may
need to turn off Secure Boot on your system in order to boot this kernel.

## Configuration
The kernel configuration is a derivative of the Ubuntu configuration for the matching architecture.
That is, almost everything is enabled and as many components as possible are built as modules.

## Additional changes
On top of the mainline kernel, the following changes have been made:

 * Support for VFS idmap mounts for cephfs (both architectures)
 * Revert of a PCIe change breaking Qualcomm servers (aarch64 only)
 * Revert of the change making `kernel_neon_begin` and `kernel_neon_end` GPL-only (breaks ZFS) (aarch64 only)

## Ceph VFS idmap
The Ceph VFS idmap support requires protocol changes which haven't been included in upstream Ceph yet.
To function with stable Ceph, the module must be loaded with the `enable_unsafe_idmap=Y` option.

This can be easily done by creating a file at `/etc/modprobe.d/ceph.conf` containing:
```
options ceph enable_unsafe_idmap=Y
```

## ZFS availability
For users who need ZFS support, an up to date ZFS package repository can be found: [here](https://github.com/zabbly/zfs)
That ZFS package repository is tested prior to new kernels being rolled out and so will avoid breakages due to upstream kernel changes.

## Support
Commercial support for those kernel packages is provided by [Zabbly](https://zabbly.com).

You can also help support the work on those packages through:

 - [Github Sponsors](https://github.com/sponsors/stgraber)
 - [Patreon](https://patreon.com/stgraber)
 - [Ko-Fi](https://ko-fi.com/stgraber)

## Repository
This repository gets actively rebased as new releases come out, DO NOT expect a linear git history.
