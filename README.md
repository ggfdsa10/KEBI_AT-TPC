# KEBI

## Required packages
 - ROOT6
 - Geant4 (optional)
 - Git
 - Doxygen (optional)

----

## Install

### 1. Clones source
```bash
git clone https://github.com/LAMPS-Collaboration/kebi.git
```

### 2. Set environment variable $KEBIPATH
```bash
cd kebi
echo "export KEBIPATH='$PWD'" >> ~/.bash_profile
source ~/.bash_profile
```

### 3. Add rootlogon.C to .rootrc
```bash
echo 'Rint.Logon: $(KEBIPATH)/macros/rootlogon.C' >> ~/.rootrc
```

### 4. Compile
```bash
mkdir build
cd build
cmake ..
...
make
...
[100%] Built target KEBI
```

### 5. Compiled information
```bash
root [0] KBRun::PrintKEBI()
===========================================================================================
[KEBI] Compiled Information
-------------------------------------------------------------------------------------------
  KEBI Version       : master.252.894134d
  GETDecoder Version : master.252
  KEBI Host Name     : Mary.local
  KEBI User Name     : ejungwoo
  KEBI Path          : /Users/ejungwoo/kebi
===========================================================================================
root [1]
```

----

## LAMPS-HighEnergy TPC simulation to reconstruction

Default directory is [kebi/LAMPS-HighEnergy/macros_tpc](https://github.com/LAMPS-Collaboration/kebi/tree/master/LAMPS-HighEnergy/macros_tpc).
Default parameter file is [kbpar_iqmd_test.conf](https://github.com/LAMPS-Collaboration/kebi/blob/master/LAMPS-HighEnergy/macros_tpc/kbpar_iqmd_test.conf)

### 1. Geant4 simulation
kbpar_iqmd_test.conf
```bash
./run_mc.g4sim kbpar_iqmd_test.conf
```

### 2. Digitization (electronics simulation)
```bash
root run_digi.C
```

### 3. Hit, Track finding
```bash
root run_reco.C
```

### 4. Momentum fitting
To be added.

### Event display 
```bash
root run_eve.C
```
The eve parameter file [kbpar_eve.conf](https://github.com/LAMPS-Collaboration/kebi/blob/master/LAMPS-HighEnergy/macros_tpc/kbpar_eve.conf) is added by default

### Example macro reading tracks
```bash
root example_read_track.C
```

----
## LAMPS AT-TPC Simulation

Default execution directory is [kebi/at-tpc/macros](https://github.com/ggfdsa10/KEBI_AT-TPC/tree/main/kebi/at-tpc/macros)
Default parameter file is [attpc.par](https://github.com/ggfdsa10/KEBI_AT-TPC/tree/main/kebi/at-tpc/macros)

### 0. Setup Simulation Parameters

Before execution AT-TPC Simulation, you have to set parameters.
Parameter file is [kebi/at-tpc/macros/input/par_at-tpc.conf](https://github.com/ggfdsa10/KEBI_AT-TPC/tree/main/kebi/at-tpc/macros/input/par_at-tpc.conf)


### 1. Geant4 simulation

```bash
./attpc_mc.g4sim attpc.par
```

### 2. Digitization (electronics simulation)
```bash
root digi.C
```

### 3. Event display 
```bash
root eve.C

nx()
```
The eve parameter file [input/par_at-tpc_eve.conf](https://github.com/ggfdsa10/KEBI_AT-TPC/tree/main/kebi/at-tpc/macros/input)

### ADC data (ADC data simulation)
```bash
root ADC_Analysis.C
```
Output File [kebi/at-tpc/output_data/](https://github.com/ggfdsa10/KEBI_AT-TPC/tree/main/kebi/at-tpc/output_data)



## Links
 - [KEBI Github repository](https://github.com/LAMPS-Collaboration/kebi)
 - [KEBI class reference (doxygen)](https://nuclear.korea.ac.kr/~lamps/kebi_doc/index.html)
 - [KEBI Issues](https://github.com/LAMPS-Collaboration/kebi/issues)
