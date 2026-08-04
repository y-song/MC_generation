# Existing hepmc files
- Pythia 8.303
    - Location on bouchet: `/nfs/roberts/project/pi_im375/ys668/from_rithya/pythia/`
    - Number of events: 200,000 per pTHat bin
    - Settings:
        - `PYTHIA8_*pthat*_200GeV.hepmc` are generated with `main42.cc` and `PYTHIA8_200GeV_STAR.cmnd`
        - `PYTHIA8_*pthat*_200GeV_DEFAULT.hepmc` are generated with `main42.cc` and `PYTHIA8_200GeV_DEFAULT.cmnd`
        - `PYTHIA8_*pthat*_200GeV_STABLE_RESONANCE.hepmc` are generated with `main42_stable_resonance.cc` and `PYTHIA8_200GeV_DEFAULT.cmnd` (I think these are with the default tune? not sure, need to check)
- Herwig 7.2 default setting
    - Location on bouchet: `/nfs/roberts/project/pi_lbh35/shared/sim_youqi/HERWIG7.2/`
    - Number of events: 1,000,000 per pTHat bin (for pTHat > 10 GeV), 1,500,000 per pTHat bin (for 5 < pTHat < 10 GeV)
    - Settings:
        -  `HERWIG_*pthat*_200GeV(-S111111).hepmc` are generated with `RHIC.in`
- Herwig 7.2 alternative settings
    - Location on bouchet: `/nfs/roberts/project/pi_im375/ys668/from_rithya/herwig/`
    - Number of events: 200,000 per pTHat bin
    - Settings:
        - `HERWIG_*pthat*_200GeV_STABLE_RESONANCE-S111111.hepmc` are generated with `RHIC_STABLE_RESONANCE.in`
        - `HERWIG_*pthat*_200GeV_P8I-S111111.hepmc` (I think I lost the config file, but could reproduce it following instructions from email exchange with Rithya)

# What needs to be done
1. Generate more Detroit tune pythia events (1,000,000) in 11-15 GeV pTHat bin
- Run `./compilepythiacode.sh` and then `./main42 PYTHIA8_200GeV_STAR.cmnd`.
2. Run rivet analysis on the newly generated hepmc file
- Run `rivet-build RivetPYTHIA_JETS_RC_TRUTH.so PYTHIA_JETS_RC_TRUTH.cc` and `rivet -a PYTHIA_JETS_RC_TRUTH --pwd <hepmc_file>.hepmc`. May need to rename the output text file to avoid it being overriden by later steps
3. Run rivet analysis over existing hepmc files from other pTHat bins
- I will need to check that the current settings in `generate_events_and_analyze_pythia.py` match the names of the files to be copied over. Then we can run `python generate_events_and_analyze_pythia.py`

# General info on MC generation
## Event generation
### Pythia
- `main42.cc` specifies particle decay settings
- `*.cmnd` specifies number of events, pTHat bins, output hepmc name, and pythia tune
    - `PYTHIA8_200GeV_STAR.cmnd` is for Detroit tune
    - `PYTHIA8_200GeV_DEFAULT.cmnd` is for default tune
1. Compile: `./compilepythiacode.sh`
2. Run: `./main42 <cmnd_file>.cmnd`. This generates pythia events and saves them as hepmc files

### Herwig
- `RHIC.in` specifies all the settings
1. Read in the configuration: `Herwig read RHIC.in`. This creates a new file `<run_file>.run`
2. Run: `Herwig run <run_file>.run -s 111111 -N <number_of_events>`

## Rivet analysis
1. Compile: `rivet-build RivetPYTHIA_JETS_RC_TRUTH.so PYTHIA_JETS_RC_TRUTH.cc`
2. Run: `rivet -a PYTHIA_JETS_RC_TRUTH --pwd <hepmc_file>.hepmc`. This carries out the rivet analysis on hepmc files, saving the analysis results into text files, one row per jet

## Batch process
- `generate_events_and_analyze_pythia.py` loops over different pTHat settings and does pythia event generation and rivet analysis for each 
    - If hepmc files are previously generated, then we set `run_pythia = False` to skip the generation step.
