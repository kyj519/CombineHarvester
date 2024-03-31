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

int main()
{
    //! [part1]
    // First define the location of the "auxiliaries" directory where we can
    // source the input files containing the datacard shapes
    string aux_shapes = "/data6/Users/yeonjoon/Vcb_Post_Analysis/Workplace/Histo_Syst";

    // Create an empty CombineHarvester instance that will hold all of the
    // datacard configuration and histograms etc.
    ch::CombineHarvester cb;
    // Uncomment this next line to see a *lot* of debug information
    // cb.SetVerbosity(3);

    // Here we will just define two categories for an 8TeV analysis. Each entry in
    // the vector below specifies a bin name and corresponding bin_id.

    ch::Categories cats = {
        {1, "Signal_2016postVFP_Mu"},
        {2, "Control0_2016postVFP_Mu"},

        //{3, "El_2b"},
        //{4, "El_3b"} no DATA yet
    };
    // ch::Categories is just a typedef of vector<pair<int, string>>
    //! [part1]

    //! [part2]
    // vector<string> masses = ch::MassesFromRange("120-135:5");
    //  Or equivalently, specify the mass points explicitly:
    //     vector<string> masses = {"120", "125", "130", "135"};
    //! [part2]

    //! [part3]
    cb.AddObservations({"*"}, {"WtoCB"}, {"2016postVFP"}, {"Template_MVA_Score"}, cats);
    //! [part3]

    //! [part4]
    vector<string> bkg_procs_non_ttbar = {"ttV", "VV", "VJets", "ST", "QCD_bEn"};
    vector<string> bkg_proc_ttbar = {"JJ_TTLJ_2", "BB_TTLJ_2", "CC_TTLJ_2", "JJ_TTLL", "BB_TTLL", "CC_TTLL", "JJ_TTLJ_4", "BB_TTLJ_4", "CC_TTLJ_4"};
    // vector<string> bkg_proc_ttbar = {"JJ_TTLJ", "BB_TTLJ","CC_TTLJ","JJ_TTLL"};

    vector<string> ttbar_2 = {"JJ_TTLJ_2", "BB_TTLJ_2", "CC_TTLJ_2"};
    vector<string> ttbar_4 = {"JJ_TTLJ_4", "BB_TTLJ_4", "CC_TTLJ_4"};

    vector<string> bkg_procs = ch::JoinStr({bkg_proc_ttbar, bkg_procs_non_ttbar});
    cb.AddProcesses({"*"}, {"WtoCB"}, {"2016postVFP"}, {"Template_MVA_Score"}, bkg_procs, cats, false);

    cb.AddProcesses({"*"}, {"WtoCB"}, {"2016postVFP"}, {"Template_MVA_Score"}, bkg_procs_non_ttbar, cats, false);

    vector<string> sig_procs = {"WtoCB"};
    cb.AddProcesses({"*"}, {"WtoCB"}, {"2016postVFP"}, {"Template_MVA_Score"}, sig_procs, cats, true);
    //! [part4]

    // Some of the code for this is in a nested namespace, so
    //  we'll make some using declarations first to simplify things a bit.
    using ch::syst::bin_id;
    using ch::syst::era;
    using ch::syst::process;
    using ch::syst::SystMap;
    using ch::syst::SystMapFunc;

    //! [part5]
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "lumi_uncorr_$ERA", "lnN", SystMap<era>::init({"2016postVFP"}, 1.020));
  cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "lumi_corr_16_17_18", "lnN", SystMap<era>::init({"2016postVFP"}, 1.006));

    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "Prefire_", "shape", SystMap<>::init(1.00));

    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "Pileup_", "shape", SystMap<>::init(1.00));

    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "Trig_", "shape", SystMap<>::init(1.00));

    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "Mu_Id_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "Mu_Iso_", "shape", SystMap<>::init(1.00));

    // cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "El_Id_", "shape", SystMap<>::init(1.00));
    // cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "el_reco", "shape", SystMap<>::init(1.00));

    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "PU_Jet_Veto_", "shape", SystMap<>::init(1.00));
    // JES, JER, Jet ID, MET syst should be added later
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "Jet_En_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "Jet_Res_", "shape", SystMap<>::init(1.00));

    /* C tagging Uncert.
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "C_Tag_Extrap_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "C_Tag_Interp_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "C_Tag_Stat_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "C_Tag_XSec_Br_Unc_DYJets_B_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "C_Tag_XSec_Br_Unc_DYJets_C_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "C_Tag_XSec_Br_Unc_WJets_C_", "shape", SystMap<>::init(1.00));
    */

    /* B tagging Uncert.*/
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "B_Tag_LFStats1_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "B_Tag_LFStats2_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "B_Tag_CFErr1_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "B_Tag_CFErr2_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "B_Tag_HFStats1_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "B_Tag_HFStats2_", "shape", SystMap<>::init(1.00));
    // forward proton? https://twiki.cern.ch/twiki/bin/viewauth/CMS/TaggedProtonsGettingStarted#Efficiency_corrections

    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "PDF_As_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "PDF_Alternative_", "shape", SystMap<>::init(1.00));

    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "UE_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "MuR_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "MuF_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "FSR_", "shape", SystMap<>::init(1.00));
    cb.cp().process(ch::JoinStr({sig_procs, bkg_procs})).AddSyst(cb, "ISR_", "shape", SystMap<>::init(1.00));

    // generator syst should be added later

    // //! [part6]
    cb.cp().process(ch::JoinStr({sig_procs, bkg_proc_ttbar})).AddSyst(cb, "ttxsec", "lnN", SystMap<>::init(1.0439));
    cb.cp().process(bkg_proc_ttbar).AddSyst(cb, "hDamp_", "shape", SystMap<>::init(1.00));
    // cb.cp().process(bkg_proc_ttbar).AddSyst(cb, "mTop_", "shape", SystMap<>::init(1.00));
    cb.cp().process(bkg_proc_ttbar).AddSyst(cb, "Top_Pt_Reweight_", "shape", SystMap<>::init(1.00));
    cb.cp().process(bkg_proc_ttbar).AddSyst(cb, "CP5_", "shape", SystMap<>::init(1.00));

    cb.cp().process({{"ttV"}}).AddSyst(cb, "ttVNorm", "lnN", SystMap<era>::init({"2016postVFP"}, 1.2));
    cb.cp().process({{"VV"}}).AddSyst(cb, "VVNorm", "lnN", SystMap<era>::init({"2016postVFP"}, 1.2));
    cb.cp().process({{"ST"}}).AddSyst(cb, "STNorm", "lnN", SystMap<era>::init({"2016postVFP"}, 1.2));
    cb.cp().process({{"VJets"}}).AddSyst(cb, "VJetsNorm", "lnN", SystMap<era>::init({"2016postVFP"}, 1.2));
    cb.cp().process({{"QCD_bEn"}}).AddSyst(cb, "QCDNorm", "lnN", SystMap<era>::init({"2016postVFP"}, 2.0));

    // cb.cp().process(ttbar_2).AddSyst(cb, "udNorm", "lnN", SystMap<era>::init({"2016postVFP"}, 1.2));
    // cb.cp().process(ttbar_4).AddSyst(cb, "csNorm", "lnN", SystMap<era>::init({"2016postVFP"}, 1.2));

    // cb.cp().process(bkg_proc_ttbar).AddSyst(cb, "Top_pTreweight", "shape", SystMap<>::init(1.00));
    // cb.cp().process(bkg_proc_ttbar).AddSyst(cb, "Top_Syst_mtop", "shape", SystMap<>::init(1.00));

    /*cb.cp().process({"Others"}).AddSyst(cb, "QCDNorm", "shape", SystMap<>::init(1.00));

    cb.cp().process({"Others"}).AddSyst(cb, "VVNorm", "shape", SystMap<>::init(1.00));
    //cb.cp().process({"Others"}).AddSyst(cb, "STNorm", "shape", SystMap<>::init(1.00));
    cb.cp().process({"Others"}).AddSyst(cb, "DYJetsNorm", "shape", SystMap<>::init(1.00));
    //cb.cp().process({"Others"}).AddSyst(cb, "WJetsNorm", "shape", SystMap<>::init(1.00));
    cb.cp().process({"Others"}).AddSyst(cb, "ttHNorm", "shape", SystMap<>::init(1.00));
    cb.cp().process({"Others"}).AddSyst(cb, "ttVNorm", "shape", SystMap<>::init(1.00));*/

    cb.cp().SetAutoMCStats(cb, 5.);
    //! [part7]
    // ch::SetStandardBinNames(cb);
    string var = "Template_MVA_Score";
    cb.cp().backgrounds().ExtractShapes(
        aux_shapes + "/Vcb_Histos_2016postVFP_Mu_All_processed.root",
        "$BIN/Nominal/$PROCESS/" + var,
        "$BIN/$SYSTEMATIC/$PROCESS/" + var);
    cb.cp().signals().ExtractShapes(
        aux_shapes + "/Vcb_Histos_2016postVFP_Mu_All_processed.root",
        "$BIN/Nominal/$PROCESS/" + var,
        "$BIN/$SYSTEMATIC/$PROCESS/" + var);
    //! [part7]

    //! [part8]

    //! [part8]
    //! [part9]
    // First we generate a set of bin names:
    set<string> bins = cb.bin_set();
    // This method will produce a set of unique bin names by considering all
    // Observation, Process and Systematic entries in the CombineHarvester
    // instance.

    // We create the output root file that will contain all the shapes.
    TFile output("Vcb_Template_Mu_2016postVFP.root", "RECREATE");

    // Finally we iterate through each bin,mass combination and write a
    // datacard.
    for (auto b : bins)
    {
        cout << ">> Writing datacard for bin: " << b
             << "\n";
        std::replace(b.begin(), b.end(), '/', '_');
        // We need to filter on both the mass and the mass hypothesis,
        // where we must remember to include the "*" mass entry to get
        // all the data and backgrounds.
        cb.cp().bin({b}).WriteDatacard(
            b+".txt", output);
        ofstream datacardFile(b+".txt", std::ios_base::app); // Open the file in append mode
        if (datacardFile.is_open())
        {
            datacardFile << "theory group = CP5_ FSR_ ISR_ MuF_ MuR_ PDF_Alternative_ PDF_As_ Pileup_ Top_Pt_Reweight_ UE_ hDamp_ ttxsec\n";
            // datacardFile << "experimental group = C_Tag_Extrap_ C_Tag_Interp_ C_Tag_Stat_ C_Tag_XSec_Br_Unc_DYJets_B_ C_Tag_XSec_Br_Unc_DYJets_C_ C_Tag_XSec_Br_Unc_WJets_C_ Jet_En_ Jet_Res_ Mu_Id_ Mu_Iso_ PU_Jet_Veto_ Prefire_ QCDNorm STNorm Trig_ VJetsNorm VVNorm lumi_uncorr_2016postVFP ttVNorm\n";
            datacardFile << "experimental group =  B_Tag_LFStats1_ B_Tag_LFStats2_ B_Tag_CFErr2_ B_Tag_HFStats1_ B_Tag_HFStats2_ Jet_En_ Jet_Res_ Mu_Id_ Mu_Iso_ PU_Jet_Veto_ Prefire_ QCDNorm STNorm Trig_ VJetsNorm VVNorm lumi_uncorr_2016postVFP ttVNorm\n";
            datacardFile << "ttbbXsec rateParam * BB_* 1.0\n";
            datacardFile << "ttbbXsec param 1.36 0.15\n";
            datacardFile << "ttccXsec rateParam * CC_* 1.0\n";
            datacardFile << "ttccXsec param 1.11 0.15\n";
            datacardFile << "ttjjXsec rateParam * JJ_* (453.63-@0*10.42-@1*34.82)/(408.39) ttbbXsec,ttccXsec\n";

            datacardFile << "cs_norm rateParam * *_4 1.0\n";
            datacardFile << "cs_norm param 1.0 0.15\n";
            datacardFile << "ud_norm rateParam * *_2 2.0-@0 cs_norm\n";

            datacardFile.close();
        }
    }
    //! [part9]
}
