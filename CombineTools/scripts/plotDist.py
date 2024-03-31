import sys
from ROOT import TFile
from Plotter.DataAndMC import DataAndMC
from argparse import ArgumentParser
def draw(fkey,histkey,x_min,x_max):
    DATASTREAM = "SingleMuon"
    MCs = ["JJ_TTLJ_4","JJ_TTLJ_2","BB_TTLJ_4","BB_TTLJ_2","CC_TTLJ_4","CC_TTLJ_2","JJ_TTLL","CC_TTLL","BB_TTLL","ttV","VV","VJets","ST","QCD_bEn","WtoCB"]

    # set plotting parameters
    cvs_params = {"logy": False,
                "grid": False
                }
    hist_params = {"x_title": "Template_MVA_Score",
                "x_range": [x_min,x_max],
                "y_title": "Events",
                "ratio_title": "Data/MC",
                "ratio_range": [0.1, 1.5],
                #"rebin" : 5,
                }
    info_params = {"info": "L^{int} = 41.5 fb^{-1}",
                "cms_text": "CMS",
                "extra_text": "Work in progress"
                }

    def TGraphToTH1D(grph):
        import ctypes
        from ROOT import TH1D, TFile
        Npoint = grph.GetN()
        x = []
        y = []
        ey = []

        
        for i in range(Npoint):
            x_temp = ctypes.c_double(0)
            y_temp = ctypes.c_double(0)
            grph.GetPoint(i,x_temp,y_temp)
            x.append(x_temp.value)
            y.append(y_temp.value)
            ey.append((grph.GetErrorYhigh(i) + grph.GetErrorYlow(i))/2)
        
        #binwidth = x[1] - x[0]
        #x_min = x[0] - binwidth/2.
        #x_max = x[Npoint-1] + binwidth/2
        hist = TH1D("data","data",Npoint,x_min,x_max)
        
        for i in range(Npoint):
            print(y[i],ey[i])
            hist.SetBinContent(i+1,y[i])
            hist.SetBinError(i+1,ey[i])
        return hist
        
        
            
    # get histograms
    def get_hist(sample, histkey):
        
        if sample == "data":
            f = TFile.Open(fkey)
            h = TGraphToTH1D(f.Get(histkey+"/data"))
        else:
            f = TFile.Open(fkey)
            h = f.Get(histkey+"/"+sample)
        
        print(sample,histkey)
        
        h.SetDirectory(0)
        return h

        

    h_data = get_hist("data", histkey)
    h_mc = dict()
    for mc in MCs:
        try:
            h_mc[mc] = get_hist(mc, histkey)
        except:
            print("no %s" % mc)

    plotter = DataAndMC(cvs_params, hist_params, info_params)
    plotter.get_hists(h_data, h_mc)
    plotter.combine()
    plotter.save("DataAndMC_%s.png" % histkey.replace('/', '_'))

if __name__ == '__main__':
    parser = ArgumentParser(usage="usage: %prog [options] in.root  \nrun with --help to get list of options")
    parser.add_argument('--fitFile',  default = '')
    args = parser.parse_args()
    fitFile = args.fitFile
    
    draw(fitFile,"shapes_prefit/Control0_Mu",0,25)
    draw(fitFile,"shapes_fit_s/Control0_Mu",0,25)
    draw(fitFile,"shapes_fit_b/Control0_Mu",0,25)
    draw(fitFile,"shapes_prefit/Control0_El",0,25)
    draw(fitFile,"shapes_fit_s/Control0_El",0,25)
    draw(fitFile,"shapes_fit_b/Control0_El",0,25)
    draw(fitFile,"shapes_prefit/MM",0,25)
    draw(fitFile,"shapes_fit_s/MM",0,25)
    draw(fitFile,"shapes_prefit/ME",0,25)
    draw(fitFile,"shapes_fit_s/ME",0,25)
    draw(fitFile,"shapes_prefit/EE",0,25)
    draw(fitFile,"shapes_fit_s/EE",0,25)

