// simplified version of PYTHIA_JETS_0810.cc
// -*- C++ -*-
#include "Rivet/Analysis.hh"
#include "Rivet/AnalysisHandler.hh"
#include "Rivet/Projections/FinalState.hh"
#include "Rivet/Projections/FastJets.hh"
#include "Rivet/Projections/DressedLeptons.hh"
#include "Rivet/Projections/MissingMomentum.hh"
#include "Rivet/Projections/PromptFinalState.hh"
#include "fastjet/contrib/SoftDrop.hh"
#include "HepMC/GenEvent.h"
#include "HepMC/GenParticle.h"

#include "cmath"
#include "iostream"
#include "fstream"
#include "random"
#include "vector"
#include "stdio.h"
#include "stdlib.h"

using namespace std;

namespace fastjet
{
    class MyUserInfo : public PseudoJet::UserInfoBase
    {
    public:
        MyUserInfo(vector<double> q, const int &parton) : _q(q), _parton(parton) {}
        vector<double> q() const { return _q; }
        int parton() const { return _parton; }

    protected:
        vector<double> _q;
        int _parton;
    };

    class ParticleInfo : public PseudoJet::UserInfoBase
    {
    public:
        ParticleInfo(const double charge, const int &pid) : _charge(charge), _pid(pid) {}
        double charge() const { return _charge; }
        int pid() const { return _pid; }

    protected:
        double _charge;
        int _pid;
    };
}

namespace Rivet
{
    class PYTHIA_JETS_MULTIFOLD_TRUTH : public Analysis
    {
    public:
        RIVET_DEFAULT_ANALYSIS_CTOR(PYTHIA_JETS_MULTIFOLD_TRUTH);

        void init()
        {
            const FinalState fs(Cuts::pt > 0.2 && Cuts::abseta < 1.0);
            declare(fs, "fs");
            // open output file
            mytxtfile.open("herwig_PTMINpthatPTMAX_multifold_truth.txt");
        }

        /// Perform the per-event analysis
        void analyze(const Event &event)
        {

            vector<double> k{0.5, 2};
            double xsecweight = handler().nominalCrossSection();
            HepMC::GenEvent *theEvent = (HepMC::GenEvent *)event.genEvent();
            int evid = theEvent->event_number();

            //===========================================
            // process particles
            Particles fsParticles = applyProjection<FinalState>(event, "fs").particles();
            PseudoJets parts;
            for (const Particle &p : fsParticles)
            {
                if (abs(p.pid()) == 12 || abs(p.pid()) == 14 || abs(p.pid()) == 16)
                    continue;
                PseudoJet pseudojet(p.px(), p.py(), p.pz(), p.E());
                double charge = p.charge();
                int pid = p.pid();
                pseudojet.set_user_info(new fastjet::ParticleInfo(charge, pid));
                parts.push_back(pseudojet);
            }

            //===========================================
            // process jets
            fastjet::Selector selector = fastjet::SelectorPtMin(15.0) * fastjet::SelectorEtaRange(-0.6, 0.6);
            fastjet::JetDefinition jet_def(fastjet::antikt_algorithm, 0.4);
            fastjet::contrib::SoftDrop sd(0, 0.1);
            fastjet::ClusterSequence cs(parts, jet_def);
            vector<fastjet::PseudoJet> jets = sorted_by_pt(selector(cs.inclusive_jets()));
            vector<fastjet::PseudoJet> sdjets = sd(jets);

            //-------------------------------------------
            // calculate jet charge
            for (unsigned int i = 0; i < jets.size(); i++)
            {
                vector<double> q{};
                for (const double &kappa : k)
                {
                    double numerator = 0;
                    for (unsigned int j = 0; j < jets[i].constituents().size(); j++)
                    {
                        PseudoJet part = jets[i].constituents().at(j);
                        double charge = part.user_info<fastjet::ParticleInfo>().charge();
                        numerator += pow(part.perp(), kappa) * charge;
                    }
                    q.push_back(numerator / pow(jets[i].perp(), kappa));
                }
                jets[i].set_user_info(new fastjet::MyUserInfo(q, -999));
            }

            // match jets to partons, comment out for herwig
            // for (HepMC::GenEvent::particle_iterator p = theEvent->particles_begin(); p != theEvent->particles_end(); ++p)
            // {
            //     if ((*p)->status() != 23)
            //     {
            //         continue;
            //     }
            //     PseudoJet parton_pseudojet((*p)->momentum().px(), (*p)->momentum().py(), (*p)->momentum().pz(), (*p)->momentum().e());
            //     for (unsigned int i = 0; i < jets.size(); i++)
            //     {
            //         if (jets[i].delta_R(parton_pseudojet) > 0.4 || jets[i].user_info<fastjet::MyUserInfo>().parton() != -999)
            //             continue;
            //         vector<double> jet_q = jets[i].user_info<fastjet::MyUserInfo>().q();
            //         jets[i].set_user_info(new fastjet::MyUserInfo(jet_q, (*p)->pdg_id()));
            //         break;
            //     }
            // }

            for (unsigned int i = 0; i < jets.size(); i++)
            {
                vector<double> jet_q = jets[i].user_info<fastjet::MyUserInfo>().q();
                int parton = jets[i].user_info<fastjet::MyUserInfo>().parton();

                mytxtfile << evid << ", " << xsecweight << ", " << jets[i].perp() << ", " << jets[i].constituents().size() << ", " << parton << ", "; // 'evid','xsec','pt','n','parton'
                for (vector<double>::iterator it = jet_q.begin(); it != jet_q.end(); ++it)
                {
                    mytxtfile << *it << ", "; // 'q05', 'q2'
                }
                mytxtfile << jets[i].m() << ", " << sdjets[i].m() << ", " << sdjets[i].structure_of<fastjet::contrib::SoftDrop>().delta_R() << ", " << sdjets[i].structure_of<fastjet::contrib::SoftDrop>().symmetry() << ", " << sdjets[i].constituents().size() << "\n"; // 'm','mg','rg','zg','ng'
            }
        }

        /// Normalise histograms etc., after the run
        void finalize()
        {
            double norm = crossSection() / sumOfWeights();
            std::cout << norm << std::endl;
            mytxtfile.close();
        }

    private:
        std::ofstream mytxtfile;
    };

    RIVET_DECLARE_PLUGIN(PYTHIA_JETS_MULTIFOLD_TRUTH);
}
