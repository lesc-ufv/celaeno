>  vim: set expandtab fdm=marker ts=2 sw=2 tw=80 et :

# Prerequisites

To compile the sample projects, you must have pip3 installed in your linux
system.

For _Ubuntu_ or any other distribution with the synaptic package manager,
[python3-pip](https://packages.ubuntu.com/focal/python3-pip) must be installed:

```sh
sudo apt install python3-pip
```

Now the [conan](https://conan.io/) package manager must be installed:

```sh
pip3 install --user conan
```

The source code requires [gcc10](https://gcc.gnu.org/gcc-10/changes.html),
for ubuntu you can install with:

```
sudo apt install gcc-10 g++-10
```

The last step is to set the correct version of _gcc_ with _update-alternatives_.

```
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-10 10
```
