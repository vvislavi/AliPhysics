#include "TPDGCode.h"
#include "THnSparse.h"
#include "TH1.h"
#include "AliESDtrackCuts.h"
#include "AlidNdPtTools.h"

class AlidNdPtTools;

using namespace std;

/// \cond CLASSIMP    
ClassImp(AlidNdPtTools)
/// \endcond 

//____________________________________________________________________________

THnSparseD* AlidNdPtTools::fSparseTmp = 0;

//____________________________________________________________________________

/// Function to fill THnSparse or THn with up to 12 dimensions
/// 
/// Ugly, but works and makes filling easier
///
/// \param s   Pointer to histogram to be filled
/// \param x0  x of dimension 0
/// \param x1  x of dimension 1
/// \param x2  x of dimention 2
/// \param x3  x of dimention 3
/// \param x4  x of dimention 4
/// \param x5  x of dimention 5
/// \param x6  x of dimention 6
/// \param x7  x of dimention 7
/// \param x8  x of dimention 8
/// \param x9  x of dimention 9
/// \param x10 x of dimention 10
/// \param x11 x of dimention 11
///
/// \return return value of THnBase->Fill(...) or 0 in case of error

Long64_t AlidNdPtTools::FillHist(THnBase* s,  Double_t x0, Double_t x1, Double_t x2, Double_t x3, Double_t x4, Double_t x5, Double_t x6, 
                     Double_t x7 , Double_t x8 , Double_t x9 , Double_t x10 , Double_t x11 )
{
    if (s->GetNdimensions() > 12) { return 0; }
    Double_t vals[12]={x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11};
    return s->Fill(vals);
}

//____________________________________________________________________________

/// Function to fill THnSparse or THn with up to 12 dimensions with weight
/// 
/// \param w   weight to be used for Histogram filling
/// \param s   Pointer to histogram to be filled
/// \param x0  x of dimension 0
/// \param x1  x of dimension 1
/// \param x2  x of dimention 2
/// \param x3  x of dimention 3
/// \param x4  x of dimention 4
/// \param x5  x of dimention 5
/// \param x6  x of dimention 6
/// \param x7  x of dimention 7
/// \param x8  x of dimention 8
/// \param x9  x of dimention 9
/// \param x10 x of dimention 10
/// \param x11 x of dimention 11
///
/// \return return value of THnBase->Fill(...) or 0 in case of error

Long64_t AlidNdPtTools::FillHist(Double_t w, THnBase* s,  Double_t x0, Double_t x1, Double_t x2, Double_t x3, Double_t x4, Double_t x5, Double_t x6, 
                     Double_t x7 , Double_t x8 , Double_t x9 , Double_t x10 , Double_t x11 )
{
    if (s->GetNdimensions() > 12) { return 0; }
    Double_t vals[12]={x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11};
    return s->Fill(vals,w);
}

//____________________________________________________________________________

/// Add an Axis (Dimension) to the THnSparseD
/// 
/// function to add a user defined axes
/// with normal (linear) binning
/// number of bins, upper and lower range
/// has to be provided 
/// 
/// \param label axis label (short version)
/// \param title axis title (long version with latex code)
/// \param nbins number of bins
/// \param xmin lower edge of first bin
/// \param xmax upper edge of last bin
/// \param option not used and currently ignored
///
/// \return the total number of dimensions after adding this axis, 0 in case of error

Int_t AlidNdPtTools::AddAxis(const char* label, const char* title, Int_t nbins, Double_t xmin, Double_t xmax, const char* option)
{
    Int_t n=1;
    if (fSparseTmp) {
        n += fSparseTmp->GetNdimensions();
    }
    TString s;
    TArrayI bin(n);
    TArrayD min(n);
    TArrayD max(n);
    for (int i=0; i<n-1; i++) {
        bin[i] = fSparseTmp->GetAxis(i)->GetNbins();
        min[i] = fSparseTmp->GetAxis(i)->GetXmin();
        max[i] = fSparseTmp->GetAxis(i)->GetXmax();
        s += fSparseTmp->GetAxis(i)->GetName();
        s += ":";
    }
    bin[n-1] = nbins; 
    min[n-1] = xmin;
    max[n-1] = xmax;
    s += label;
    THnSparseD* h = new THnSparseD("fSparseTmp",s.Data(),n,bin.GetArray(),min.GetArray(),max.GetArray());
    for (int i=0; i<n-1; i++) {
        if (fSparseTmp->GetAxis(i)->GetXbins() && fSparseTmp->GetAxis(i)->GetXbins()->GetSize()) { h->SetBinEdges(i,fSparseTmp->GetAxis(i)->GetXbins()->GetArray()); }
        h->GetAxis(i)->SetTitle(fSparseTmp->GetAxis(i)->GetTitle());
        h->GetAxis(i)->SetName(fSparseTmp->GetAxis(i)->GetName());
    }
    h->GetAxis(n-1)->SetTitle(title);
    h->GetAxis(n-1)->SetName(label);
    if (fSparseTmp) { delete fSparseTmp; }
    fSparseTmp = h;
    return fSparseTmp->GetNdimensions();
}

//____________________________________________________________________________

/// Add an Axis (Dimension) to the THnSparseD
/// 
/// function to add a user defined axes
/// with normal (linear) binning
/// number of bins, upper and lower range
/// has to be provided 
/// 
/// \param label this is used as label AND title for the new axis
/// \param nbins number of bins
/// \param xmin lower edge of first bin
/// \param xmax upper edge of last bin
/// \param option not used and currently ignored
///
/// \return the total number of dimensions after adding this axis, 0 in case of error


Int_t AlidNdPtTools::AddAxis(const char* label, Int_t nbins, Double_t xmin, Double_t xmax, const char* option)
{
    return AddAxis(label, label, nbins, xmin, xmax, option);
}

//____________________________________________________________________________

/// Add an Axis (Dimension) to the THnSparseD
/// 
/// function to add a user defined binning with 
/// the option of variable bin size
/// number of bins and and an array defining the bin edges 
/// has to be provided 
/// 
/// \param label axis label (short version)
/// \param title axis title (long version with latex code)
/// \param nbins number of bins
/// \param xbins array of length nbins+1 containing the bin edges
/// \param option not used and currently ignored
///
/// \return the total number of dimensions after adding this axis, 0 in case of error

Int_t AlidNdPtTools::AddAxis(const char* label, const char* title, Int_t nbins, Double_t* xbins, const char* option)
{
    Int_t n = AddAxis(label, title, nbins, xbins[0], xbins[nbins], option);
    fSparseTmp->SetBinEdges(n-1,xbins);         
    return n;
}

//____________________________________________________________________________

/// Add an Axis (Dimension) to the THnSparseD
/// 
/// function to add a user defined binning with 
/// the option of variable bin size
/// number of bins and and an array defining the bin edges 
/// has to be provided 
/// 
/// \param label this is used as label AND title for the new axis
/// \param nbins number of bins
/// \param xbins array of length nbins+1 containing the bin edges
/// \param option not used and currently ignored
///
/// \return the total number of dimensions after adding this axis, 0 in case of error

Int_t AlidNdPtTools::AddAxis(const char* label, Int_t nbins, Double_t* xbins, const char* option)
{
    return AddAxis(label, label, nbins, xbins, option);
}

//____________________________________________________________________________

/// Add an Axis (Dimension) to the THnSparseD
/// 
/// function to add from a series of pre-defined options
/// option supplied in not case-senstitiv
/// 
/// currently the following ones are available
/// "pt"                standard pT axis 
/// "ptfew"             reduced pt binning
/// "ptveryfew"         much reduced pt binning
/// "ptmario"           marios pt binning
/// "cent"              standard centrality binning
/// "varsig35"
/// "mult6kfine"        multiplicity bining 0-6000 in fine bins
/// "mult6kcoarse"      multiplicity bining 0-6000 in coarse bins
/// "mult100kcoarse"    multiplicity bining 0-100000 in coarse bins
/// 
/// \param label axis label (short version)
/// \param title axis title (long version with latex code)
/// \param option string to steer the binning 
///
/// \return the total number of dimensions after adding this axis, 0 in case of error

Int_t AlidNdPtTools::AddAxis(const char* label, const char* title, const char* option)
{
    TString o(option);
    o.ToLower();
    if (o.Contains("ptfew")) {
        const Int_t nbins = 21;
        Double_t xbins[22] = {0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 2.0,  5.0, 10.0,  20.0, 50.0,  200.0};
        return AddAxis(label, title, nbins, xbins);    
    }
    if (o.Contains("ptveryfew")) {
        const Int_t nbins = 8;
        Double_t xbins[9] = {0.0, 0.15, 0.5, 1.0, 2.0, 5.0, 10, 25.0, 200.0};
        return AddAxis(label, title, nbins, xbins);    
    }
    if (o.Contains("ptmario")) {   
        const Int_t nbins = 52;
        Double_t xbins[53] = {0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5,0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95,1.0,1.1,1.2,1.3,1.4,1.5,1.6,1.7,1.8,1.9,2.0,2.2,2.4,2.6,2.8,3.0,3.2,3.4,3.6,3.8,4.0,4.5,5.0,5.5,6.0,6.5,7.0,8.0,9.0,10.0,20.0,30.0,40.0,50.0,60.0};
        return AddAxis(label, title, nbins, xbins);   
    }
    if (o.Contains("pt")) {
        const Int_t nbins = 81;
        Double_t xbins[82] = {0.0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95, 1.0, 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.0, 2.2, 2.4, 2.6, 2.8, 3.0, 3.2, 3.4, 3.6, 3.8, 4.0, 4.5, 5.0, 5.5, 6.0, 6.5, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0, 13.0, 14.0, 15.0, 16.0, 18.0, 20.0, 22.0, 24.0, 26.0, 28.0, 30.0, 32.0, 34.0, 36.0, 40.0, 45.0, 50.0, 60.0, 70.0, 80.0, 90.0, 100.0, 110.0, 120.0, 130.0, 140.0, 150.0, 160.0, 180.0, 200.0};
        return AddAxis(label, title, nbins, xbins);    
    }      
    if (o.Contains("cent")) {
        const Int_t nbins = 11;
        Double_t xbins[12] = {0.,5.,10.,20.,30.,40.,50.,60.,70.,80.,90.,100.};
        return AddAxis(label, title, nbins, xbins);
    }    
    if (o.Contains("varsig35")) {
        const Int_t nbins = 35;
        Double_t xbins[36] = {-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,30,40,50,60,70,80,90,100,200,300,400,500,1000,2000};
        return AddAxis(label, title, nbins, xbins);
    } 
    if (o.Contains("mult6kfine")) {
        // variable mult binning total 0-6000
        // 1-width 0-100            101
        // 10-width 100-1000         90
        // 100-width 1000-6000       50
        const Int_t nbins = 241;
        Double_t xbins[242];
        xbins[0] = -0.5;
        int i =0;
        for (; i<=100; i++) { xbins[i+1] = xbins[i]+1; }
        for (; i<=100+90; i++) { xbins[i+1] = xbins[i]+10; }
        for (; i<=100+90+50; i++) { xbins[i+1] = xbins[i]+100; }
        return AddAxis(label, title, nbins, xbins);
    }  
    if (o.Contains("mult6kcoarse")) {
        // variable mult binning total 0-6000
        // 1-width 0-10              11
        // 10-width 10-100            9
        // 100-width 100-1000         9
        // 200-width 1000-6000       25
        const Int_t nbins = 54;
        Double_t xbins[55];
        xbins[0] = -0.5;
        int i =0;
        for (; i<=10; i++) { xbins[i+1] = xbins[i]+1; }
        for (; i<=10+9; i++) { xbins[i+1] = xbins[i]+10; }
        for (; i<=10+9+9; i++) { xbins[i+1] = xbins[i]+100; }
        for (; i<=10+9+9+25; i++) { xbins[i+1] = xbins[i]+200; }
        return AddAxis(label, title, nbins, xbins);
    }      
    if (o.Contains("mult100kcoarse")) {
        // variable mult binning total 0-100000
        // 1-width 0-10             11
        // 10-width 10-100           9
        // 100-width 100-1000        9
        // 1000-width 1000-10000     9
        // 10000-width 10000-100000  9
        const Int_t nbins = 47;
        Double_t xbins[48];
        xbins[0] = -0.5;
        int i =0;
        for (; i<=10; i++) { xbins[i+1] = xbins[i]+1; }
        for (; i<=10+9; i++) { xbins[i+1] = xbins[i]+10; }
        for (; i<=10+9+9; i++) { xbins[i+1] = xbins[i]+100; }
        for (; i<=10+9+9+9; i++) { xbins[i+1] = xbins[i]+1000; }
        for (; i<=10+9+9+9+9; i++) { xbins[i+1] = xbins[i]+10000; }            
        return AddAxis(label, title, nbins, xbins);
    }       

    return 0;
}

//____________________________________________________________________________

/// Add an Axis (Dimension) to the THnSparseD
/// 
/// function to add from a series of pre-defined options
/// option supplied in not case-senstitiv
/// 
/// currently the following ones are available
/// "pt"                standard pT axis 
/// "ptfew"             reduced pt binning
/// "ptveryfew"         much reduced pt binning
/// "ptmario"           marios pt binning
/// "cent"              standard centrality binning
/// "varsig35"
/// "mult6kfine"        multiplicity bining 0-6000 in fine bins
/// "mult6kcoarse"      multiplicity bining 0-6000 in coarse bins
/// "mult100kcoarse"    multiplicity bining 0-100000 in coarse bins
/// 
/// \param label this is used as label AND title for the new axis
/// \param option string to steer the binning 
///
/// \return the total number of dimensions after adding this axis, 0 in case of error

Int_t AlidNdPtTools::AddAxis(const char* label, const char* option)
{
    return AddAxis(label, label, option);
}

//____________________________________________________________________________

/// Add an Axis (Dimension) to the THnSparseD
/// 
/// function to add from a series of pre-defined options
/// option supplied in not case-senstitiv
/// 
/// currently the following ones are available
/// "pt"                standard pT axis 
/// "ptfew"             reduced pt binning
/// "ptveryfew"         much reduced pt binning
/// "ptmario"           marios pt binning
/// "mcpt"              standard pT axis for MC 
/// "mcptfew"           reduced pt binning for MC
/// "mcptveryfew"       much reduced pt binning
/// "mcptmario"         marios pt binning
/// "cent"              standard centrality binning
/// "mult6kfine"        multiplicity bining 0-6000 in fine bins
/// "mult6kcoarse"      multiplicity bining 0-6000 in coarse bins
/// "mult100kcoarse"    multiplicity bining 0-100000 in coarse bins
/// 
/// \param option string to steer the binning 
///
/// \return the total number of dimensions after adding this axis, 0 in case of error

Int_t AlidNdPtTools::AddAxis(const char* option)
{
    TString o(option);
    o.ToLower();
    if (o.EqualTo("pt"))                return AddAxis("pT","p_{T} (GeV/c)","pt");    
    if (o.EqualTo("mcpt"))              return AddAxis("MCpT","p_{T,MC} (GeV/c)","pt");
    if (o.EqualTo("ptfew"))             return AddAxis("pT","p_{T} (GeV/c)","ptfew");    
    if (o.EqualTo("mcptfew"))           return AddAxis("MCpT","p_{T,MC} (GeV/c)","ptfew");
    if (o.EqualTo("ptveryfew"))         return AddAxis("pT","p_{T} (GeV/c)","ptveryfew");    
    if (o.EqualTo("mcptveryfew"))       return AddAxis("MCpT","p_{T,MC} (GeV/c)","ptveryfew");
    if (o.EqualTo("ptmario"))           return AddAxis("pT","p_{T} (GeV/c)","ptmario");    
    if (o.EqualTo("mcptmario"))         return AddAxis("MCpT","p_{T,MC} (GeV/c)","ptmario");    
    if (o.EqualTo("mult6kfine"))        return AddAxis("mult","N","mult6kfine");    
    if (o.EqualTo("mult6kcoarse"))      return AddAxis("mult","N","mult6kcoarse");      
    if (o.EqualTo("mult1000kcoarse"))   return AddAxis("mult","N","mult1000kcoarse");
    if (o.EqualTo("cent"))              return AddAxis("cent","centrality (%)","cent");
    return 0;
}

//____________________________________________________________________________

/// Create a THnSparseD histogram
/// 
/// Before this function actually creates a histogram
/// axis have to be added using the various AddAxis() functions
/// 
/// \param name  name of the histogram
///
/// \return newly created THnSparseD histogram or 0 in case fo error

THnSparseD* AlidNdPtTools::CreateHist(const char* name)
{
    if (!fSparseTmp) return 0;
    THnSparseD* h = fSparseTmp;
    h->SetName(name);
    fSparseTmp = 0;    
    return h;
}

//____________________________________________________________________________

/// Create a TH1D histogram for Logging purposes
/// 
/// \param name  name of the histogram
/// \param title title of the histogram
///
/// \return newly created TH1D histogram

TH1D* AlidNdPtTools::CreateLogHist(const char* name, const char* title)
{   
   TH1D *h = 0;
   if (title) { 
       h = new TH1D(name,title,200,0,200);        
    } else {
       h = new TH1D(name,name,200,0,200);
    }                   
   return h;
}   

//____________________________________________________________________________

/// Create a TH1D histogram for Logging purposes
/// 
/// \param name  name and title of the histogram
///
/// \return newly created TH1D histogram
  
TH1D* AlidNdPtTools::CreateLogHist(const char* name) 
{ 
    return CreateLogHist(name,name);
}

//____________________________________________________________________________

/// Function to create AliESDtrackCuts with various settings
/// 
/// specfiy the type of track cuts to be created in the option string
/// this is not case senstitiv
/// 
/// currently the folowing options are implemented:
/// ""                          identical to "TPCITSgeo"
/// "default"                   identical to "TPCITSgeo"
/// "TPCITSgeo"                 all standard track cuts for TPC-ITS primaries including the golden chi2 cut and the geometric cut
/// "TPCITSgeoNoDCAr"           same as "TPCITSgeo" but without the DCAr cut
/// "TPCITSforDCArStudy"        "TPCITSgeoNoDCAr" but without the golden chi2 cut (for DCAr fits)
/// "TPCgeo"                    all default TPC cuts, including the geometric length cut
/// "TPCgeoNoDCAr"              same as "TPCgeo" but without the cut on DCAr (for DCAr fits)
/// "TPCgeo+ITShit"             same as "TPCgeo" but in addition require a hit in the ITS (for Matching Efficieny studies)
/// "TPCgeo+ITSrefit"           same as "TPCgeo" but in addition require the ITS refit, i.e. 2 hitsin the ITS (for Matching Efficieny studies)
/// "TPCgeo+SPDhit"             same as "TPCgeo" but in addition require a hit in any layer of the SPD (for Matching Efficieny studies)
/// "TPCgeo+SPDhit+ITSrefit"    identical to "TPCgeo+ITSrefit+SPDhit"
/// "TPCgeo+ITSrefit+SPDhit"    same as "TPCgeo" but in addition require a hit in any layer of the SPD AND a ITS refit (for Matching Efficieny studies) 
/// "TPConlyMinimal"            minimal tpc only cuts
/// 
/// all these cuts do not include the eta-cut, to add add one  of the following options:
/// "Eta05" 
/// "Eta08" 
/// "Eta10"
///
/// \param option string to select the type of track cuts to be created
///
/// \return the newly created AliESDtrackCuts or 0 in case of error

AliESDtrackCuts* AlidNdPtTools::CreateESDtrackCuts(const char* option)
{            
    TString o(option);
    AliESDtrackCuts* cuts = new AliESDtrackCuts(o.Data());
    o.ToLower();
    
    // if eta ranges is provided set the eta range
    // and remove the part of the string containting the eta range
    if ( o.Contains("eta05") ) {
        cuts->SetEtaRange(-0.5,0.5);
        o.ReplaceAll("eta05","");
    } else if ( o.Contains("eta08") ) {
        cuts->SetEtaRange(-0.5,0.5);
        o.ReplaceAll("eta08","");
    } else if ( o.Contains("eta10") ) {
        cuts->SetEtaRange(-1.0,1.0);
        o.ReplaceAll("eta10","");
    }    
        
        
    // as default use the cuts with geometric length cut
    if ( (o.EqualTo("")) || (o.EqualTo("default")) ) { o = "tpcitsgeo"; }
    
    if (o.EqualTo("tpcitsgeo")) {
//         cuts = new AliESDtrackCuts("default TPCITS with geo L cut");
        cuts->SetRequireTPCRefit(kTRUE);    
        cuts->SetMinRatioCrossedRowsOverFindableClustersTPC(0.8);
        cuts->SetMaxChi2PerClusterTPC(4);
        cuts->SetMaxFractionSharedTPCClusters(0.4);        
        cuts->SetRequireITSRefit(kTRUE);
        cuts->SetClusterRequirementITS(AliESDtrackCuts::kSPD,AliESDtrackCuts::kAny);
        cuts->SetMaxChi2PerClusterITS(36.);
        cuts->SetDCAToVertex2D(kFALSE);
        cuts->SetRequireSigmaToVertex(kFALSE);
        cuts->SetMaxDCAToVertexZ(2.0);
        // 7*(0.0026+0.0050/pt^1.01)
        cuts->SetMaxDCAToVertexXYPtDep("0.0182+0.0350/pt^1.01");
        cuts->SetAcceptKinkDaughters(kFALSE);
        // tpcc cut
        cuts->SetMaxChi2TPCConstrainedGlobal(36.);
        // Geometrical-Length Cut
        cuts->SetCutGeoNcrNcl(3,130,1.5,0.85,0.7);
//         cuts->SetEtaRange(-0.8,0.8);
        
    } else if (o.EqualTo("tpcitsgeonodcar")) {
//         cuts = new AliESDtrackCuts("default TPCITS with geo L cut without DCAr");
        cuts->SetRequireTPCRefit(kTRUE);    
        cuts->SetMinRatioCrossedRowsOverFindableClustersTPC(0.8);
        cuts->SetMaxChi2PerClusterTPC(4);
        cuts->SetMaxFractionSharedTPCClusters(0.4);        
        cuts->SetRequireITSRefit(kTRUE);
        cuts->SetClusterRequirementITS(AliESDtrackCuts::kSPD,AliESDtrackCuts::kAny);
        cuts->SetMaxChi2PerClusterITS(36.);
        cuts->SetDCAToVertex2D(kFALSE);
        cuts->SetRequireSigmaToVertex(kFALSE);
        cuts->SetMaxDCAToVertexZ(2.0);
        cuts->SetAcceptKinkDaughters(kFALSE);
        // tpcc cut
        cuts->SetMaxChi2TPCConstrainedGlobal(36.);
        // Geometrical-Length Cut
        cuts->SetCutGeoNcrNcl(3,130,1.5,0.85,0.7);
//         cuts->SetEtaRange(-0.8,0.8);
        
    } else if (o.EqualTo("tpcitsfordcarstudy")) {
//         cuts = new AliESDtrackCuts("default TPCITS with geo L cut without DCAr and Chi2 TPCc vs. Global");
        cuts->SetRequireTPCRefit(kTRUE);    
        cuts->SetMinRatioCrossedRowsOverFindableClustersTPC(0.8);
        cuts->SetMaxChi2PerClusterTPC(4);
        cuts->SetMaxFractionSharedTPCClusters(0.4);        
        cuts->SetRequireITSRefit(kTRUE);
        cuts->SetClusterRequirementITS(AliESDtrackCuts::kSPD,AliESDtrackCuts::kAny);
        cuts->SetMaxChi2PerClusterITS(36.);
        cuts->SetDCAToVertex2D(kFALSE);
        cuts->SetRequireSigmaToVertex(kFALSE);
        cuts->SetMaxDCAToVertexZ(2.0);
        cuts->SetAcceptKinkDaughters(kFALSE);
        // tpcc cut
        // cuts->SetMaxChi2TPCConstrainedGlobal(36.);
        // Geometrical-Length Cut
        cuts->SetCutGeoNcrNcl(3,130,1.5,0.85,0.7);
//         cuts->SetEtaRange(-0.8,0.8);        
        
    } else if (o.EqualTo("tpcgeo")) { 
//         cuts = new AliESDtrackCuts("TPConly with geo L");
        cuts->SetRequireTPCRefit(kTRUE);
        cuts->SetAcceptKinkDaughters(kFALSE);
        cuts->SetMinRatioCrossedRowsOverFindableClustersTPC(0.8);
        cuts->SetMaxChi2PerClusterTPC(4);
        cuts->SetMaxFractionSharedTPCClusters(0.4);
        cuts->SetMaxDCAToVertexZ(3);
        cuts->SetMaxDCAToVertexXY(3);
        // Geometrical-Length Cut
        cuts->SetCutGeoNcrNcl(3,130,1.5,0.85,0.7);
//         cuts->SetEtaRange(-0.8,0.8);
        
    } else if (o.EqualTo("tpcgeonodcar")) { 
        cuts = new AliESDtrackCuts("TPConly with geo L without DCAr");
        cuts->SetRequireTPCRefit(kTRUE);
        cuts->SetAcceptKinkDaughters(kFALSE);
        cuts->SetMinRatioCrossedRowsOverFindableClustersTPC(0.8);
        cuts->SetMaxChi2PerClusterTPC(4);
        cuts->SetMaxFractionSharedTPCClusters(0.4);
        cuts->SetMaxDCAToVertexZ(3);        
        // Geometrical-Length Cut
        cuts->SetCutGeoNcrNcl(3,130,1.5,0.85,0.7);    
//         cuts->SetEtaRange(-0.8,0.8);
        
    } else if (o.EqualTo("tpcgeo+itshit")) { 
//         cuts = new AliESDtrackCuts("TPConly with geo L + hit in ITS");
        cuts->SetRequireTPCRefit(kTRUE);
        cuts->SetAcceptKinkDaughters(kFALSE);
        cuts->SetMinRatioCrossedRowsOverFindableClustersTPC(0.8);
        cuts->SetMaxChi2PerClusterTPC(4);
        cuts->SetMaxFractionSharedTPCClusters(0.4);
        cuts->SetMaxDCAToVertexZ(3);
        cuts->SetMaxDCAToVertexXY(3);
        // Geometrical-Length Cut
        cuts->SetCutGeoNcrNcl(3,130,1.5,0.85,0.7);
        // its hit
        cuts->SetMinNClustersITS(1);
//         cuts->SetEtaRange(-0.8,0.8);
        
    } else if (o.EqualTo("tpcgeo+itsrefit")) { 
//         cuts = new AliESDtrackCuts("TPC with geo L + ITSrefit");
        cuts->SetRequireTPCRefit(kTRUE);
        cuts->SetAcceptKinkDaughters(kFALSE);
        cuts->SetMinRatioCrossedRowsOverFindableClustersTPC(0.8);
        cuts->SetMaxChi2PerClusterTPC(4);
        cuts->SetMaxFractionSharedTPCClusters(0.4);
        cuts->SetMaxDCAToVertexZ(3);
        cuts->SetMaxDCAToVertexXY(3);
        // Geometrical-Length Cut
        cuts->SetCutGeoNcrNcl(3,130,1.5,0.85,0.7);
        // its refit
        cuts->SetRequireITSRefit(kTRUE);
        
    } else if (o.EqualTo("tpcgeo+spdhit")) { 
//         cuts = new AliESDtrackCuts("TPC with geo L + hit in SPD");
        cuts->SetRequireTPCRefit(kTRUE);
        cuts->SetAcceptKinkDaughters(kFALSE);
        cuts->SetMinRatioCrossedRowsOverFindableClustersTPC(0.8);
        cuts->SetMaxChi2PerClusterTPC(4);
        cuts->SetMaxFractionSharedTPCClusters(0.4);
        cuts->SetMaxDCAToVertexZ(3);
        cuts->SetMaxDCAToVertexXY(3);
        // Geometrical-Length Cut
        cuts->SetCutGeoNcrNcl(3,130,1.5,0.85,0.7);
        // spd hit
        cuts->SetClusterRequirementITS(AliESDtrackCuts::kSPD,AliESDtrackCuts::kAny);
//         cuts->SetEtaRange(-0.8,0.8);        
        
    }  else if (o.EqualTo("tpcgeo+itsrefit+spdhit") || o.EqualTo("tpcgeo+spdhit+itsrefit")) { 
//         cuts = new AliESDtrackCuts("TPC with geo L + ITSrefit + hit in SPD");
        cuts->SetRequireTPCRefit(kTRUE);
        cuts->SetAcceptKinkDaughters(kFALSE);
        cuts->SetMinRatioCrossedRowsOverFindableClustersTPC(0.8);
        cuts->SetMaxChi2PerClusterTPC(4);
        cuts->SetMaxFractionSharedTPCClusters(0.4);
        cuts->SetMaxDCAToVertexZ(3);
        cuts->SetMaxDCAToVertexXY(3);
        // Geometrical-Length Cut
        cuts->SetCutGeoNcrNcl(3,130,1.5,0.85,0.7);
        // its refit + spd hit
        cuts->SetRequireITSRefit(kTRUE);
        cuts->SetClusterRequirementITS(AliESDtrackCuts::kSPD,AliESDtrackCuts::kAny);
//         cuts->SetEtaRange(-0.8,0.8);        
    
        
    }  else if (o.EqualTo("tpconlyminimal")) { 
//         cuts = new AliESDtrackCuts("minimal TPC only cuts");
        cuts->SetRequireTPCRefit(kTRUE);                
        cuts->SetMaxChi2PerClusterTPC(4);
        cuts->SetMaxFractionSharedTPCClusters(0.4);        
        cuts->SetMinNClustersTPC(50);
//         cuts->SetEtaRange(-1.0,1.0);        
    } else {
        // in case of no valid argument for cuts supplied
        // return 0 to avoid mistakes
        delete cuts;
        cuts = 0;
    }

    return cuts;
}

//____________________________________________________________________________

/// Retrieve the scaling factor for MC primaries and secondaries
/// 
/// WARNING! only dummy function - use only for testing
///
/// This method looks up the proper scaling factors and returns them
/// for offline and -- especially -- online use
///
/// For now this is only a dummy until proper implementation of the 
/// functionality is available
/// 
/// 
/// \param prod MC production type (primary, secondary from material or secondary from decay)
/// \param part MC particle species (pion, proton, kaon, sigma+, sigma-, xi-, omega-, electron, muon, other)
/// \param pt   MC pT of the particle
///
/// \return scaling factor accoring to the supplied arguments

Double_t AlidNdPtTools::MCScalingFactor(ProductionType prod, ParticleType part, Double_t pt) 
{
    //if prod or part type not set return scaling of 1
    if (prod == kUnknown || part  == kUndefined) return 1.0;
    //dummy function for testing, scaling sigmas up by a factor two
    //secondaries from decay by factor 1.5
    //and leaves the rest unchanged
    // TODO this should call the ALiMCSpectra weights once they are ready
    // TODO and the corresponding solution for secondariy scaling
    if (prod == kSecDecay) return 1.5;
    if (prod == kPrim) {
        if ( (part == kSigmaP) || (part == kSigmaM) ) return 2.0;        
    }
    // in other case return scaling factor 1
    return 1.0;
}

//____________________________________________________________________________

/// Convert the PDG code to a ParticleType
/// 
/// \param pdgCode PDG code
///
/// \return AlidNdPtTools::ParticleType (pion, proton, kaon, sigma+, sigma-, xi-, omega-, electron, muon, other)

AlidNdPtTools::ParticleType AlidNdPtTools::ParticleTypeFromPDG(Int_t pdgCode)
{
    if ( pdgCode == kElectron   || pdgCode == kPositron )      { return kEl; }
    if ( pdgCode == kMuonMinus  || pdgCode == kMuonPlus )      { return kMu; }
    if ( pdgCode == kPiPlus     || pdgCode == kPiMinus )       { return kPi; }
    if ( pdgCode == kProton     || pdgCode == kProtonBar )     { return kPr; }
    if ( pdgCode == kKPlus      || pdgCode == kKMinus )        { return kKa; }
    if ( pdgCode == kSigmaPlus  || pdgCode == kSigmaBarMinus ) { return kSigmaP; }
    if ( pdgCode == kSigmaMinus || pdgCode == kSigmaBarPlus )  { return kSigmaM; }
    if ( pdgCode == kXiMinus    || pdgCode == kXiPlusBar )     { return kXi; }
    if ( pdgCode == kOmegaMinus || pdgCode == kOmegaPlusBar )  { return kOmega; }
    return kOther;    
}

//____________________________________________________________________________