import os

def main():
    kt_ranges = [
            {"min_pt": 11, "max_pt": 15},
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
        r["in_name"] = f"RHIC_NODECAY"
        r["hepmc_name"] = f"HERWIG_{r['min_pt']}pthat{r['max_pt']}_200GeV_P8I"
        
    rivet_name = "HERWIG_JETS_RC_TRUTH_HEPMC3"    
    run_herwig = False 
    run_rivet = True
 
    #########################################################################
    
    if (run_herwig):

        # os.chdir("/work/gen_events/")
    
        for r in kt_ranges:
    
            # create .in input files
            os.system("cp {}.in {}_COPY.in".format(r["in_name"], r["in_name"]))
            with open("{}_COPY.in".format(r["in_name"]), "r") as f:
                code = f.read()
                code = code.replace("PTMIN", str(r["min_pt"]))
                code = code.replace("PTMAX", str(r["max_pt"]))
                code = code.replace("HEPMCNAME", str(r["hepmc_name"]))
                with open("{}_COPY.in".format(r["in_name"]), "w") as ff:
                    ff.write(code)
            
            herwig_read_command = 'Herwig read {}_COPY.in'.format(r["in_name"])
            os.system(herwig_read_command)
        
            herwig_run_command = 'Herwig run {}.run -s {} -N {}'.format(r['hepmc_name'], r['seed'], r['num_events'])
            os.system(herwig_run_command)
        
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
        
            rivet_run_command = "rivet -a {}_COPY -n {} --pwd ../gen_events/{}-S{}.hepmc".format(rivet_name, r["num_events"], r["hepmc_name"], r["seed"])
            os.system(rivet_run_command)

if __name__ == "__main__":
    main()
