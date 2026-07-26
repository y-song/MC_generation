import os

def main():
    kt_ranges = [
            # {"min_pt": 11, "max_pt": 15},
            {"min_pt": 15, "max_pt": 20},
            {"min_pt": 20, "max_pt": 25},
            {"min_pt": 25, "max_pt": 30},
            {"min_pt": 30, "max_pt": 35},
            {"min_pt": 35, "max_pt": 40},
            {"min_pt": 40, "max_pt": 45},
            {"min_pt": 45, "max_pt": 50},
            {"min_pt": 50, "max_pt": 1000}
    ]

    for r in kt_ranges:
        r["num_events"] = 200000
        r["seed"] = 111111
        r["cmnd_name"] = f"PYTHIA8_200GeV_STAR"
        r["hepmc_name"] = f"PYTHIA8_{r['min_pt']}pthat{r['max_pt']}_200GeV"

    rivet_name = "PYTHIA_JETS_RC_TRUTH"   
    run_pythia = False
    run_rivet = True
    
    #########################################################################
    
    if (run_pythia):
        
        os.chdir("/work/gen_events/")
    
        for r in kt_ranges:
    
            # create .cmnd files
            os.system("cp {}.cmnd {}_COPY.cmnd".format(r["cmnd_name"], r["cmnd_name"]))
            with open("{}_COPY.cmnd".format(r["cmnd_name"]), "r") as f:
                code = f.read()
                code = code.replace("NEVT", str(r["num_events"]))
                code = code.replace("PTMIN", str(r["min_pt"]))
                code = code.replace("PTMAX", str(r["max_pt"]))
                with open("{}_COPY.cmnd".format(r["cmnd_name"]), "w") as ff:
                    ff.write(code)
                #print(code)
           
            pythia_run_command = "./main42 {}_COPY.cmnd {}.hepmc".format(r["cmnd_name"], r["hepmc_name"])
            os.system(pythia_run_command)
        
    #########################################################################

    if (run_rivet):
        
        # os.chdir("/work/analysis/")
    
        for r in kt_ranges:

        # create .cc files
            os.system("cp {}.cc {}_COPY.cc".format(rivet_name, rivet_name))
            with open("{}_COPY.cc".format(rivet_name), "r") as f:
                code = f.read()
                code = code.replace("{}".format(rivet_name), "{}_COPY".format(rivet_name))
                code = code.replace("PTMIN", str(r["min_pt"]))
                code = code.replace("PTMAX", str(r["max_pt"]))
                with open("{}_COPY.cc".format(rivet_name), "w") as ff:
                    ff.write(code)
        
            rivet_build_command = "rivet-build Rivet{}_COPY.so {}_COPY.cc".format(rivet_name, rivet_name)
            os.system(rivet_build_command)

            rivet_run_command = "rivet -a {}_COPY --pwd ../gen_events/{}.hepmc".format(rivet_name, r["hepmc_name"])
            os.system(rivet_run_command)

if __name__ == "__main__":
    main()
