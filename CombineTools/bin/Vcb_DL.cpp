#include <string>
#include <map>
#include <set>
#include <iostream>
#include <utility>
#include <vector>
#include <cstdlib>
#include "CombineHarvester/CombineTools/interface/CombineHarvester.h"
#include "CombineHarvester/CombineTools/interface/Observation.h"
#include "CombineHarvester/CombineTools/interface/Process.h"
#include "CombineHarvester/CombineTools/interface/Utilities.h"
#include "CombineHarvester/CombineTools/interface/Systematics.h"
#include "CombineHarvester/CombineTools/interface/BinByBin.h"

using namespace std;

int WriteFinalLine(string fname){
    ofstream datacardFile(fname, std::ios_base::app);
    if (datacardFile.is_open())
    {
        //datacardFile << "theory group = CP5_ FSR_ ISR_ MuF_ MuR_ PDF_Alternative_ PDF_As_ Pileup_ Top_Pt_Reweight_ UE_ hDamp_ ttxsec\n";
        // datacardFile << "experimental group = C_Tag_Extrap_ C_Tag_Interp_ C_Tag_Stat_ C_Tag_XSec_Br_Unc_DYJets_B_ C_Tag_XSec_Br_Unc_DYJets_C_ C_Tag_XSec_Br_Unc_WJets_C_ Jet_En_ Jet_Res_ Mu_Id_ Mu_Iso_ PU_Jet_Veto_ Prefire_ QCDNorm STNorm Trig_ VJetsNorm VVNorm lumi_uncorr_2018 ttVNorm\n";
        //datacardFile << "experimental group =  B_Tag_LFStats1_ B_Tag_LFStats2_ B_Tag_CFErr2_ B_Tag_HFStats1_ B_Tag_HFStats2_ Jet_En_ Jet_Res_ Mu_Id_ Mu_Iso_ PU_Jet_Veto_ Prefire_ QCDNorm STNorm Trig_ VJetsNorm VVNorm lumi_uncorr_2018 ttVNorm\n";
        datacardFile << "ttbbXsec rateParam * BB_* 1.0\n";
        datacardFile << "ttbbXsec param 1.36 0.15\n";
        datacardFile << "ttccXsec rateParam * CC_* 1.0\n";
        datacardFile << "ttccXsec param 1.11 0.15\n";
        datacardFile << "ttjjXsec rateParam * JJ_* (453.63-@0*10.42-@1*34.82)/(408.39) ttbbXsec,ttccXsec\n";

        datacardFile << "nuisance edit rename ST * MuF_ MuFST_ \n";
        datacardFile << "nuisance edit rename ST * MuR_ MuRST_ \n";
        datacardFile << "nuisance edit rename ST * ISR_ ISRST_ \n";

        datacardFile.close();
    }
    return 0;
}

int main()
{

    
    ch::CombineHarvester cb;
    ch::Categories cats = {{1, "Control_DL"}};
    vector<string> channels = {"MM", "EE", "ME"};
    vector<string> eras = {"2018", "2017", "2016preVFP", "2016postVFP"};

    cb.AddObservations({"*"}, {"WtoCB"}, eras, channels, cats);


    //vector<string> bkg_procs_non_ttbar = {"ttV", "VV", "VJets", "ST", "QCD_bEn"};
    vector<string> bkg_procs_non_ttbar = {"Others", "ST", "QCD_bEn"};

    vector<string> bkg_proc_ttbar = {"JJ_TTLJ_2", "BB_TTLJ_2", "CC_TTLJ_2", "JJ_TTLL", "BB_TTLL", "CC_TTLL", "JJ_TTLJ_4", "BB_TTLJ_4", "CC_TTLJ_4"};
    vector<string> ttbar_2 = {"JJ_TTLJ_2", "BB_TTLJ_2", "CC_TTLJ_2"};
    vector<string> ttbar_4 = {"JJ_TTLJ_4", "BB_TTLJ_4", "CC_TTLJ_4"};

    vector<string> bkg_procs = ch::JoinStr({bkg_proc_ttbar, bkg_procs_non_ttbar});
    cb.AddProcesses({"*"}, {"WtoCB"}, eras, channels, bkg_procs, cats, false);
    vector<string> sig_procs = {"WtoCB"};
    cb.AddProcesses({"*"}, {"WtoCB"}, eras, channels, sig_procs, cats, true);
    //! [part4]

    // Some of the code for this is in a nested namespace, so
    //  we'll make some using declarations first to simplify things a bit.
    using ch::syst::bin_id;
    using ch::syst::era;
    using ch::syst::channel;
    using ch::syst::process;
    using ch::syst::SystMap;
    using ch::syst::SystMapFunc;
    //tt xsec: https://twiki.cern.ch/twiki/bin/view/LHCPhysics/TtbarNNLO
    cb.cp().process(ch::JoinStr({sig_procs, bkg_proc_ttbar})).AddSyst(cb, "ttxsec", "lnN", SystMap<>::init(1.0439));

    //Luminosity:https://twiki.cern.ch/twiki/bin/view/CMS/LumiRecommendationsRun2#
    cb.cp().AddSyst(cb, "lumi_uncorr_$ERA_", "lnN", SystMap<era>::init
                                                                                                            ({"2018"}, 1.015)
                                                                                                            ({"2017"}, 1.020)
                                                                                                            ({"2016preVFP"}, 1.01)
                                                                                                            ({"2016postVFP"}, 1.01));
                                                                                                            
    cb.cp().AddSyst(cb, "lumi_corr_17_18", "lnN", SystMap<era>::init
                                                                                                            ({"2018"}, 1.002)
                                                                                                            ({"2017"}, 1.006));

    cb.cp().AddSyst(cb, "lumi_corr_16_17_18", "lnN", SystMap<era>::init
                                                                                                            ({"2018"}, 1.020)
                                                                                                            ({"2017"}, 1.009)
                                                                                                            ({"2016preVFP"}, 1.006)
                                                                                                            ({"2016postVFP"}, 1.006));



    //L1 Prefire
    cb.cp().AddSyst(cb, "Prefire_", "shape", SystMap<era>::init(eras,1.00)); 
    //Pileup: https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopSystematics#Pile_up
    cb.cp().AddSyst(cb, "Pileup_", "shape", SystMap<era>::init(eras,1.00));
    //Trigger Eff.: https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopSystematics#Trigger_efficiency
    cb.cp().AddSyst(cb, "Mu_Trig_", "shape", SystMap<channel>::init({"MM", "ME"},1.00));
    cb.cp().AddSyst(cb, "El_Trig_", "shape", SystMap<channel>::init({"EE", "ME"},1.00));
    
    //Muon ID and Res: https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopSystematics#Muon_identification_isolation_mo
    cb.cp().AddSyst(cb, "Mu_Id_", "shape", SystMap<channel>::init({"MM", "ME"},1.00));
    cb.cp().AddSyst(cb, "Mu_Iso_", "shape", SystMap<channel>::init({"MM", "ME"},1.00));

    //El ID and Reco: https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopSystematics#Electron_and_photon_identificati
    cb.cp().AddSyst(cb, "El_Id_", "shape", SystMap<channel>::init({"EE", "ME"},1.00));
    cb.cp().AddSyst(cb, "El_Reco_", "shape", SystMap<channel>::init({"EE", "ME"},1.00));

    //PU ID SF
    cb.cp().AddSyst(cb, "PU_Jet_Veto_", "shape", SystMap<>::init(1.00));

    //JET: https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopSystematics#Jets_and_MET
    cb.cp().AddSyst(cb, "Jet_En_", "shape", SystMap<>::init(1.00));
    cb.cp().AddSyst(cb, "Jet_Res_$ERA_", "shape", SystMap<era>::init(eras,1.00));

    //B-Tagging: https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagShapeCalibration#Correlation_across_years_2016_20
    cb.cp().AddSyst(cb, "B_Tag_LFStats1_$ERA_", "shape", SystMap<era>::init(eras,1.00));
    cb.cp().AddSyst(cb, "B_Tag_LFStats2_$ERA_", "shape", SystMap<era>::init(eras,1.00));
    cb.cp().AddSyst(cb, "B_Tag_HFStats1_$ERA_", "shape", SystMap<era>::init(eras,1.00));
    cb.cp().AddSyst(cb, "B_Tag_HFStats2_$ERA_", "shape", SystMap<era>::init(eras,1.00));
    cb.cp().AddSyst(cb, "B_Tag_CFErr1_", "shape", SystMap<>::init(1.00));
    cb.cp().AddSyst(cb, "B_Tag_CFErr2_", "shape", SystMap<>::init(1.00));
    cb.cp().AddSyst(cb, "B_Tag_HF_", "shape", SystMap<>::init(1.00));
    //cb.cp().AddSyst(cb, "B_Tag_LF_", "shape", SystMap<>::init(1.00));


    //PDF: https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopSystematics#PDF
    cb.cp().AddSyst(cb, "PDF_As_", "shape", SystMap<>::init(1.00));
    cb.cp().AddSyst(cb, "PDF_Alternative_", "shape", SystMap<>::init(1.00));


    //Factorization and Renormalization Scale: https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopSystematics#Factorization_and_renormalizatio
    //ST(EWK-induced) process and tt(QCD-induced) process should be deccorelated.
    //this decorr. was treated at last part of this file
    cb.cp().AddSyst(cb, "MuR_", "shape", SystMap<process>::init(bkg_proc_ttbar,1.00));
    cb.cp().AddSyst(cb, "MuF_", "shape", SystMap<process>::init(bkg_proc_ttbar,1.00));
    
    cb.cp().AddSyst(cb, "MuR_", "shape", SystMap<process>::init({"ST"},1.00));
    cb.cp().AddSyst(cb, "MuF_", "shape", SystMap<process>::init({"ST"},1.00));

    
    //PS uncertainites: https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopSystematics#Parton_shower_uncertainties
    //ISR is decorrelated between process as MuF
    cb.cp().AddSyst(cb, "FSR_", "shape", SystMap<process>::init(bkg_proc_ttbar,1.00));
    cb.cp().AddSyst(cb, "ISR_", "shape", SystMap<process>::init(bkg_proc_ttbar,1.00));

    cb.cp().AddSyst(cb, "FSR_", "shape", SystMap<process>::init({"ST"},1.00));
    cb.cp().AddSyst(cb, "ISR_", "shape", SystMap<process>::init({"ST"},1.00));


    //UE Tunes:https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopSystematics#Underlying_event
    cb.cp().AddSyst(cb, "UE_", "shape", SystMap<>::init(1.00));
    cb.cp().process(bkg_proc_ttbar).AddSyst(cb, "CP5_", "shape", SystMap<>::init(1.00));


    //hdamp: https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopSystematics#Parton_shower_ME_PS_matching_sca
    cb.cp().process(bkg_proc_ttbar).AddSyst(cb, "hDamp_", "shape", SystMap<>::init(1.00));


    
    // Top Pt reweighting: https://twiki.cern.ch/twiki/bin/viewauth/CMS/TopSystematics#Top_pt_reweighting
    cb.cp().process(bkg_proc_ttbar).AddSyst(cb, "Top_Pt_Reweight_", "shape", SystMap<>::init(1.00));

    // mtop 
    cb.cp().process(bkg_proc_ttbar).AddSyst(cb, "mTop_", "shape", SystMap<>::init(1.00));
    

    //Minor Bkg normaliztaion
    /*cb.cp().process({{"ttV"}}).AddSyst(cb, "ttVNorm", "lnN", SystMap<>::init(1.2));
    cb.cp().process({{"VV"}}).AddSyst(cb, "VVNorm", "lnN", SystMap<>::init(1.2));
    cb.cp().process({{"VJets"}}).AddSyst(cb, "VJetsNorm", "lnN", SystMap<>::init(1.2));*/
    cb.cp().process({{"Others"}}).AddSyst(cb, "ttVNorm_", "shape", SystMap<>::init(1.00));
    cb.cp().process({{"Others"}}).AddSyst(cb, "VVNorm_", "shape", SystMap<>::init(1.00));
    cb.cp().process({{"Others"}}).AddSyst(cb, "VJetsNorm_", "shape", SystMap<>::init(1.00));
    cb.cp().process({{"ST"}}).AddSyst(cb, "STNorm_", "lnN", SystMap<>::init(1.2));
    cb.cp().process({{"QCD_bEn"}}).AddSyst(cb, "QCDNorm_", "lnN", SystMap<>::init(2.0));

    cb.cp().SetAutoMCStats(cb, 5.);



    string aux_shapes = "/data6/Users/yeonjoon/Vcb_Post_Analysis/Workplace/CR_DL";
    for(auto e : eras){
        for(auto ch : channels){
            string var = "BvsC_3rd_4th_Jets_Unrolled";
            cout << e << ch << endl;
            cb.cp().era({e}).channel({ch}).backgrounds().ExtractShapes(
                aux_shapes + "/Vcb_DL_Histos_"+e+"_"+ch+"_processed.root",
                "$BIN/Nominal/$PROCESS/" + var,
                "$BIN/$SYSTEMATIC/$PROCESS/" + var);
            cb.cp().era({e}).channel({ch}).signals().ExtractShapes(
                aux_shapes + "/Vcb_DL_Histos_"+e+"_"+ch+"_processed.root",
                "$BIN/Nominal/$PROCESS/" + var,
                "$BIN/$SYSTEMATIC/$PROCESS/" + var);
        }
    }



    set<string> bins = cb.bin_set();
    



    for(auto e : eras){
        for(auto channel : channels){
            for(auto b : bins){
                std::cout << "write datacard for " << b << ", " << channel << ", " << e << std::endl;
                TString fname = "Vcb_Template_"+b+"_"+e+"_"+channel+".root";
                TFile output(fname, "RECREATE");
                cb.cp().bin({b}).era({e}).channel({channel}).WriteDatacard(b+"_"+e+"_"+channel+".txt", output);
                
                WriteFinalLine(b+"_"+e+"_"+channel+".txt");
            }
        }
    }
    
    //! [part9]
}


