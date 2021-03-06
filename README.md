CARIBOU
=====

Caribou is a multiphysics transport code for environmental assessment of nuclear
accidents under development at the
[Ontario Tech University](https://ontariotechu.ca/).

Caribou is a MOOSE-based application. For more information on the MOOSE
framework see:
[https://www.mooseframework.org/](https://www.mooseframework.org/).

## Use

Caribou currently consists of an atmospheric eulerian transport model.
Additional capabilities are planned for the future. Documentation of the
airborne transport kernels, boundary conditions and materials can be found
[here](https://github.com/ksawatzky777/caribou/wiki).
## Installation

Caribou requires the installation of the MOOSE framework. Detailed instructions
for installation of MOOSE can be found
[here](https://www.mooseframework.org/getting_started/index.html). After the
MOOSE framework has been installed, Caribou can be installed by running the
following series of commands in the same directory that holds your MOOSE
installation (if the tutorial above was followed, `~projects`):

```bash
git clone https://github.com/ksawatzky777/caribou.git
cd caribou
make -j4
```

To ensure Caribou has been built properly, execute the tests using the
following commands:

```bash
./run_tests -j4
```

The python scripts provided for the purpose of weather data formatting require
the installation of pygrib. Pygrib, and its series of dependancies can be
found [here](https://jswhit.github.io/pygrib/docs/).
## Acknowledgements

We acknowledge the support of the Natural Sciences and Engineering Research
Council of Canada (NSERC).

Cette recherche a été financée par le Conseil de recherches en sciences naturelles et en génie du Canada (CRSNG).

![NSERC Logo](https://www.nserc-crsng.gc.ca/img/logos/img-logo2-en.png)
