import ROOT
import argparse
import os
import multiprocessing
#enable root batch mode
ROOT.gROOT.SetBatch(True)
def plot_comparison(nominal_hist, up_hist, down_hist, process_name, syst_name, save_path = './'):
    #draw TH1D nominal_hist, up_hist, down_hist in one TCanvas
    #nominal_hist is drawn in black, up_hist is drawn in red, down_hist is drawn in blue
    #nominal_hist is drawn in solid line, up_hist is drawn in dashed line, down_hist is drawn in dotted line
    #legend is drawn in the upper right corner of the canvas
    #canvas is saved in the same directory with the name of "process name"_"syst name".png
    #canvas is cleared after saving the image
    canvas = ROOT.TCanvas(f"{process_name}_{syst_name}", f"{process_name}_{syst_name}", 800, 1000)
    #divide to ratio plot and nomal plot
    canvas.Divide(1,2)
    canvas.cd(1)
    #print current task of code
    print(f"Drawing {process_name}_{syst_name}")
    nominal_hist.SetLineColor(ROOT.kBlack)
    nominal_hist.SetLineStyle(1)
    up_hist.SetLineColor(ROOT.kRed)
    up_hist.SetLineStyle(2)
    down_hist.SetLineColor(ROOT.kBlue)
    down_hist.SetLineStyle(3)
    nominal_hist.Draw()
    up_hist.Draw("SAME")
    down_hist.Draw("SAME")
    legend = ROOT.TLegend(0.7, 0.7, 0.9, 0.9)
    legend.AddEntry(nominal_hist, "Nominal", "l")
    legend.AddEntry(up_hist, "Up", "l")
    legend.AddEntry(down_hist, "Down", "l")
    legend.Draw()
    canvas.cd(2)
    ratio_up = up_hist.Clone()
    ratio_up.Divide(nominal_hist)
    ratio_down = down_hist.Clone()
    ratio_down.Divide(nominal_hist)
    ratio_up.SetLineColor(ROOT.kRed)
    ratio_down.SetLineColor(ROOT.kBlue)
    ratio_up.SetLineStyle(2)
    ratio_down.SetLineStyle(3)
    ratio_up.SetMaximum(1.1)
    ratio_up.SetMinimum(0.9)
    ratio_up.Draw()
    #add black line at y=1
    line = ROOT.TLine(nominal_hist.GetXaxis().GetXmin(), 1, nominal_hist.GetXaxis().GetXmax(), 1)
    line.SetLineColor(ROOT.kBlack)
    line.Draw()
    ratio_down.Draw("SAME")
    
    #if save_path is not exist, make the directory
    if not os.path.exists(save_path):
        os.makedirs(save_path)
    canvas.SaveAs(f"{save_path}/{process_name}_{syst_name}.png")
    #draw same with logscale
    canvas.SetLogy()
    canvas.SaveAs(f"{save_path}/{process_name}_{syst_name}_log.png")
    canvas.Clear()
    
def get_process_and_systs(directory):
    # Get the name of the directory
    process_names = []
    syst_names = []
    #in opened directory, there are multiple histograms.
    #there names are in format of processname_systname_Up/Down
    #we will get the list of histograms and parse the list of processname and systname

    #first, there are historgrams that correspond to nominal value, they only have process name in their name
    #so store there name as process name first. after that, if we find a histogram with same process name, we can extract the systematic name.
    #now iterate over histogram
    for key in directory.GetListOfKeys():
        # Get the name of the histogram
        histogram_name = key.GetName()
        process_name = histogram_name
        #check if histogram name contains the one of process name that already in list
        if any(name in process_name for name in process_names):
            matched_name = next(name for name in process_names if name in process_name)
            #if string matched_name+'_' is in first of process_name, then remove corresponding part.
            if process_name.startswith(matched_name+'_'):
                syst_name = histogram_name[len(matched_name)+1:]
            #remove "_Up" or "_Down" from syst_name
            syst_name = syst_name.replace("_Up", "")
            syst_name = syst_name.replace("_Down", "")
            #append syst_name to list if there is not same syst. in list
            if syst_name not in syst_names:
                syst_names.append(syst_name)
        else:
            process_names.append(process_name)
            
    return process_names, syst_names
    
def doDraw(input_root_file,savePath='./'):
    directories = input_root_file.GetListOfKeys()
    #save the TDirectory name and TDriectory to dictionary
    directory_dict = {}
    for directory in directories:
        directory_name = directory.GetName()
        directory_dict[directory_name] = input_root_file.Get(directory_name)
    
    for dir_name, dir in directory_dict.items():
        process_names, syst_names = get_process_and_systs(dir)
        print(syst_names)
        #remove the 'data_obs' from process_names
        process_names = [name for name in process_names if name != 'data_obs']
        #for each process, make a plot that compare the nominal and up/down histograms
        #process name is in the process_names, and syst name is in the syst_names
        #nominal histogram has the name of "process name"
        #up/down histogram has the name of "process name"_"syst name"_Up/Down
        #they are stored in "dir".
        with multiprocessing.Pool(64) as pool:
            for process_name in process_names:
                nominal_hist = dir.Get(process_name)
                for syst_name in syst_names:
                    up_hist = dir.Get(f"{process_name}_{syst_name}_Up")
                    down_hist = dir.Get(f"{process_name}_{syst_name}_Down")
                    #if there is no up/down histogram(then type of up_hist will TObject), skip the process and save the combination to log
                    if type(up_hist) == type(ROOT.TObject()):
                        print(f"Skipping {process_name}_{syst_name}_Up")
                        continue 
                    #call plot_comparison to save a plot, but use multithreading with maximum 16 simultaneous job
                    
                    #pool.apply_async(plot_comparison, (nominal_hist, up_hist, down_hist, process_name, syst_name, savePath))
            #do same thing for total systematic variation
            for i, process_name in enumerate(process_names):
                if i == 0:
                    nominal_hist = dir.Get(process_name)
                    nominal_hist= nominal_hist.Clone("Whole")
                else:
                    nominal_hist.Add(dir.Get(process_name))
            for syst_name in syst_names:
                for i, process_name in enumerate(process_names):
                    if i == 0:
                        #if up/down histogram is not exist, just get the nominal histogram
                        up_hist = dir.Get(f"{process_name}_{syst_name}_Up")
                        down_hist = dir.Get(f"{process_name}_{syst_name}_Down")
                        if type(up_hist) == type(ROOT.TObject()):
                            up_hist = dir.Get(process_name)
                            up_hist= up_hist.Clone("Whole_Up")
                            down_hist = dir.Get(process_name)
                            down_hist= down_hist.Clone("Whole_Down")
                        up_hist = up_hist.Clone("Whole_Up")
                        down_hist = down_hist.Clone("Whole_Down")
                        
                    else:
                        up_temp = dir.Get(f"{process_name}")
                        down_temp = dir.Get(f"{process_name}")
                        if type(up_hist) == type(ROOT.TObject()):
                            up_temp = dir.Get(f"{process_name}")
                            down_temp = dir.Get(f"{process_name}")
                        up_hist.Add(up_temp)
                        down_hist.Add(down_temp)
                pool.apply_async(plot_comparison, (nominal_hist, up_hist, down_hist, "Whole", syst_name, savePath))
                
            pool.close()
            pool.join()
                    #limit simultaneous job to maximum 16 threads
                    
                    #plot_comparison(nominal_hist, up_hist, down_hist, process_name, syst_name, savePath)

if __name__ == "__main__":
    #make a argparser and get path of file
    parser = argparse.ArgumentParser()
    parser.add_argument('--input', help='input file')
    #get argument input file from parser
    input_root_file = parser.parse_args().input
    if input_root_file is not None:
        input_root_file = ROOT.TFile(input_root_file,'READ')
        doDraw(input_root_file)
    #if there was no input to --input argument, iterate over all the files in '../Vcb*.root'
    else:
        import os
        files = os.listdir('../')
        #add '../' to files
        files = ['../'+file for file in files]
        for file in files:
            if 'Vcb' in file:
                input_root_file = ROOT.TFile(file,'READ')
                #save_path is in format of era/channel/bin
                #where file is in format of Vcb_Template_bin_era_channel.root
                #but beware when bin name is Control_DL because it have _ inside it.
                if 'Control_DL' in file:
                    save_path = file.split('_')[4]+'/'+file.split('_')[5]+'/'+'Control_DL'
                else:
                    save_path = file.split('_')[3]+'/'+file.split('_')[4]+'/'+file.split('_')[2]
                doDraw(input_root_file,savePath=save_path)
                input_root_file.Close()

        

    
