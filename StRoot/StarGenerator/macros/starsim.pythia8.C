// macro to instantiate the Geant3 from within
// STAR  C++  framework and get the starsim prompt
// To use it do
//  root4star starsim.C

const bool GEANT_HANDLES_DECAYS = true;

class St_geant_Maker;
St_geant_Maker *geant_maker = 0;

class StarGenEvent;
StarGenEvent   *event       = 0;

class StarPrimaryMaker;
StarPrimaryMaker *_primary = 0;

class StarPythia8;
StarPythia8* _pythia8;

//class StarFilterMaker;
//StarFilterMaker *filter = 0;

class StLambdaPiPFilter;
StLambdaPiPFilter *sngLambdaPiPFilter = 0;

class StDiLambdaPiPFilter;
StDiLambdaPiPFilter *diLambdaPiPFilter = 0;

class StDiLambdaBarPiPFilter;
StDiLambdaBarPiPFilter *diLambdaBarPiPFilter = 0;

class StL_LbarPiPFilter;
//StL_LbarPiPFilter *LambdaLambdaBarPiPFilter = 0;

class StL_LbarPiPFilter_simple;
StL_LbarPiPFilter_simple *LambdaLambdaBarPiPFilter = 0;

class StProtonPionPairLMassFilter;
StProtonPionPairLMassFilter *ProtonPionPairLMassFilter = 0;

Float_t ptHatMin = 0;
Float_t ptHatMax = 128;

// ----------------------------------------------------------------------------
void geometry( TString tag, Bool_t agml=true )
{
  TString cmd = "DETP GEOM "; cmd += tag;
  if ( !geant_maker ) geant_maker = (St_geant_Maker *)chain->GetMaker("geant");
  geant_maker -> LoadGeometry(cmd);
  //  if ( agml ) command("gexec $STAR_LIB/libxgeometry.so");
}
// ----------------------------------------------------------------------------
void command( TString cmd )
{
  if ( !geant_maker ) geant_maker = (St_geant_Maker *)chain->GetMaker("geant");
  geant_maker -> Do( cmd );
}
// ----------------------------------------------------------------------------
// trig()  -- generates one event
// trig(n) -- generates n+1 events.
//
// NOTE:  last event generated will be corrupt in the FZD file
//
void trig( Int_t n=1 )
{
  chain->EventLoop(n);
  _primary->event()->Print();
  
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Pythia8( TString config="pp:W", Int_t collEnergy = 200)
{
  gSystem->Load( "/star/u/tdrk/software/PDF/LHAPDF-6.1.4/lib/libLHAPDF.so"  ); // LHAPDF needs to be called before PYTHIA8
  gSystem->Load( "libPythia8_1_62.so"  );
  
  //gSystem->Load( "Pythia8_3_03.so"  );
  

  //
  // Create the pythia 8 event generator and add it to 
  // the primary generator
  //
  StarPythia8 *pythia8 = new StarPythia8();
  
  _pythia8=pythia8;
  
  pythia8->SetFrame("CMS", collEnergy);
  pythia8->SetBlue("proton");
  pythia8->SetYell("proton");
    
  if ( config=="pp:W" )
  {
    
    pythia8->Set("WeakSingleBoson:all=off");
    pythia8->Set("WeakSingleBoson:ffbar2W=on");
    pythia8->Set("24:onMode=0");              // switch off all W+/- decaus
    pythia8->Set("24:onIfAny 11 -11");        // switch on for decays to e+/-
    
  }
  
  if ( config=="pp:minbias" )
  {

    //pythia8->Set("HardQCD:all = on");
    pythia8->Set("SoftQCD:minBias = on");

  }
  
  if ( config=="pp:heavyflavor:D0jets" ) 
  {
    pythia8->Set("HardQCD:gg2ccbar = on");
    pythia8->Set("HardQCD:qqbar2ccbar = on");
    pythia8->Set("Charmonium:all = on");

    pythia8->Set("421:mayDecay = 0");

  }
    
  if ( config=="pp:minbiasLambda" ||  config=="pp:minbiasDiLambda" ||  config=="pp:minbiasDiLambdaBar" ||  config=="pp:minbiasLambdaLambdaBar" || config=="pp:minbiasProtonPionPair")
  {
    pythia8->Set("SoftQCD:minBias = on");

    //pythia8->Set("3122:mayDecay= on");
    //pythia8->Set("3122:onMode=1");
    //pythia8->Set("3122:onMode=0");
    //pythia8->Set("3122:OnIfMatch=2212 -211");
    //pythia8->Set("-3122:onMode=0");
    //pythia8->Set("-3122:OnIfMatch=-2212 211");
  }

  // Setup phase space cuts
  pythia8 -> Set(Form("PhaseSpace:ptHatMin=%f", ptHatMin ));
  pythia8 -> Set(Form("PhaseSpace:ptHatMax=%f", ptHatMax ));

  _primary -> AddGenerator( pythia8 );
  
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void starsim( Int_t nevents=10, Int_t collEnergy = 510, UInt_t rngSeed = 12345, TString config = "pp:minbiasLambda" )
{ 

  gROOT->ProcessLine(".L bfc.C");
  {
    //TString simple = "y2014x geant gstar usexgeom agml sdt20140530 DbV20150316 misalign ";
    TString simple;
    
    if(collEnergy == 510) simple = "y2017 geant gstar usexgeom agml ";
    else if(collEnergy == 200) simple = "y2012 geant gstar usexgeom agml ";
    else
    {
      cout<<"Invalid collisions energy!"<<endl;
      
      return;    
    }
    
    //TString simple = "tables nodefault";
    bfc(0, simple );
  }
  
  //gSystem->Load("libgstar.so");

  gSystem->Load( "libVMC.so");
  gSystem->Load( "St_g2t.so" );
  gSystem->Load( "St_geant_Maker.so" );

  gSystem->Load( "StarGeneratorUtil.so");
  gSystem->Load( "StarGeneratorEvent.so");
  gSystem->Load( "StarGeneratorBase.so" );
  gSystem->Load( "StarGeneratorFilt.so" ); 
  

  gSystem->Load( "libMathMore.so"   );  

  // Force loading of xgeometry
  gSystem->Load( "xgeometry.so"     );

  //gSystem->Load("$OPTSTAR/lib/libfastjet.so");
  
  //gSystem->Load( "FastJetFilter.so" );

//   // And unloading of geometry
//   TString geo = gSystem->DynamicPathName("geometry.so");
//   if ( !geo.Contains("Error" ) ) {
//     std::cout << "Unloading geometry.so" << endl;
//     gSystem->Unload( gSystem->DynamicPathName("geometry.so") );
//   }


  // Setup RNG seed and map all ROOT TRandom here
  StarRandom::seed( rngSeed );
  StarRandom::capture();
  
  //
  // Create the primary event generator and insert it
  // before the geant maker
  //
  //  StarPrimaryMaker *
  _primary = new StarPrimaryMaker();
  {    
     
    if(config.Contains("minbiasDiLambdaBar"))
    {
      _primary->SetFileName("./output_starsim/pythia8.minbiasDiLambdaBar.starsim.root");
    }
    
    else if(config.Contains("minbiasDiLambda"))
    {
      _primary->SetFileName("./output_starsim/pythia8.minbiasDiLambda.starsim.root");
    }
    
    else if(config.Contains("minbiasLambdaLambdaBar"))
    {
      _primary->SetFileName("./output_starsim/pythia8.minbiasLambdaLambdaBar.starsim.root");
    }
    
    else if(config.Contains("minbiasLambda"))
    {
      _primary->SetFileName("./output_starsim/pythia8.minbiasLambda.starsim.root"); 
    }
    else if(config.Contains("minbiasProtonPionPair"))
    {
      _primary->SetFileName("./output_starsim/pythia8.minbiasProtonPionPair.starsim.root"); 
    }
    else if(config.Contains("minbias"))
    {
      _primary->SetFileName("./output_starsim/pythia8.minbias.starsim.root"); 
    }
    //_primary -> SetVertex( 0.1, -0.1, 0.0 );
    //_primary -> SetSigma ( 0.1,  0.1, 30.0 );
    chain -> AddBefore( "geant", _primary ); //is rhis needed?
  }

  //
  // Setup an event generator
  //

  Pythia8( config, collEnergy );
  command("call gstar_part"); 
  //command("call gstar"); 


//#if 1
  //
  // Setup the generator filter
  //
  //  filter = new StDijetFilter();

  
  
  if(config.Contains("minbiasDiLambdaBar")) 
  {
    diLambdaBarPiPFilter = new StDiLambdaBarPiPFilter();
    //StDiLambdaBarPiPFilter::SetDauKine(double ptMin1, double ptMax1, double etaMin1, double etaMax1, double phiMin1, double phiMax1, double ptMin2, double ptMax2, double etaMin2, double etaMax2, double phiMin2, double phiMax2)
    //diLambdaBarPiPFilter->SetDauKine(0.15, 20., -1, 1, -TMath::Pi(), TMath::Pi(), 0.15, 20., -1, 1, -TMath::Pi(), TMath::Pi()); //analysis cuts
    diLambdaBarPiPFilter->SetDauKine(0., 1e6, -100, 100, -TMath::Pi(), TMath::Pi(), 0., 1e6, -100, 100, -TMath::Pi(), TMath::Pi()); //open cuts for check of acceptance effect
    diLambdaBarPiPFilter->SetParentRapidities(-1, 1, -1, 1); //setter for mother rapidities (Lambda and Lambda-bar)

    _primary->AddFilter( diLambdaBarPiPFilter );
    _primary->SetAttr("FilterKeepAll",    int(0));
    //_primary->SetAttr("FilterKeepHeader", int(0));
    //_primary->SetAttr("Debug", int(1));
    //_primary->SetAttr("FilterSkipRejects",    int(1)); //accept events which do not pass filter - for testing
  }
  
  else if(config.Contains("minbiasDiLambda")) 
  {
    diLambdaPiPFilter = new StDiLambdaPiPFilter();
    //StDiLambdaPiPFilter::SetDauKine(double ptMin1, double ptMax1, double etaMin1, double etaMax1, double phiMin1, double phiMax1, double ptMin2, double ptMax2, double etaMin2, double etaMax2, double phiMin2, double phiMax2)
    //diLambdaPiPFilter->SetDauKine(0.15, 20., -1, 1, -TMath::Pi(), TMath::Pi(), 0.15, 20., -1, 1, -TMath::Pi(), TMath::Pi()); //analysis cuts
    diLambdaPiPFilter->SetDauKine(0., 1e6, -100, 100, -TMath::Pi(), TMath::Pi(), 0., 1e6, -100, 100, -TMath::Pi(), TMath::Pi()); //open cuts for check of acceptance effect
    diLambdaPiPFilter->SetParentRapidities(-1, 1, -1, 1); //setter for mother rapidities (Lambda and Lambda-bar)

    _primary->AddFilter( diLambdaPiPFilter );
    _primary->SetAttr("FilterKeepAll",    int(0));
    //_primary->SetAttr("FilterKeepHeader", int(0));
    //_primary->SetAttr("Debug", int(1));
    //_primary->SetAttr("FilterSkipRejects",    int(1)); //accept events which do not pass filter - for testing
  }
  
  else if(config.Contains("minbiasLambdaLambdaBar")) 
  {
    //LambdaLambdaBarPiPFilter = new StL_LbarPiPFilter();
    LambdaLambdaBarPiPFilter = new StL_LbarPiPFilter_simple();
    //StL_LbarPiPFilter::SetDauKine(double ptMin1, double ptMax1, double etaMin1, double etaMax1, double phiMin1, double phiMax1, double ptMin2, double ptMax2, double etaMin2, double etaMax2, double phiMin2, double phiMax2)
    //LambdaLambdaBarPiPFilter->SetDauKine(0.15, 20., -1, 1, -TMath::Pi(), TMath::Pi(), 0.15, 20., -1, 1, -TMath::Pi(), TMath::Pi()); //analysis cuts
    //LambdaLambdaBarPiPFilter->SetDauKine(0., 1e6, -100, 100, -TMath::Pi(), TMath::Pi(), 0., 1e6, -100, 100, -TMath::Pi(), TMath::Pi()); //open cuts for check of acceptance effect
    LambdaLambdaBarPiPFilter->SetParentRapidities(-1, 1, -1, 1); //setter for mother rapidities (Lambda and Lambda-bar)

    _primary->AddFilter( LambdaLambdaBarPiPFilter );
    _primary->SetAttr("FilterKeepAll",    int(0));
    //_primary->SetAttr("FilterKeepHeader", int(0));
    //_primary->SetAttr("Debug", int(1));
    //_primary->SetAttr("FilterSkipRejects",    int(1)); //accept events which do not pass filter - for testing
  }
  
  else if(config.Contains("minbiasLambda")) 
  {
    sngLambdaPiPFilter = new StLambdaPiPFilter();
    //StLambdaPiPFilter::SetDauKine(double ptMin1, double ptMax1, double etaMin1, double etaMax1, double phiMin1, double phiMax1, double ptMin2, double ptMax2, double etaMin2, double etaMax2, double phiMin2, double phiMax2)
    //sngLambdaPiPFilter->SetDauKine(0.15, 20., -1, 1, -TMath::Pi(), TMath::Pi(), 0.15, 20., -1, 1, -TMath::Pi(), TMath::Pi()); //analysis cuts
    sngLambdaPiPFilter->SetDauKine(0., 1e6, -100, 100, -TMath::Pi(), TMath::Pi(), 0., 1e6, -100, 100, -TMath::Pi(), TMath::Pi()); //open cuts for check of acceptance effect
    sngLambdaPiPFilter->SetParentRapidities(-1, 1, -1, 1); //setter for mother rapidities (Lambda and Lambda-bar)

    _primary->AddFilter( sngLambdaPiPFilter );
    _primary->SetAttr("FilterKeepAll",    int(0));
    //_primary->SetAttr("FilterKeepHeader", int(0));
    //_primary->SetAttr("Debug", int(1));
    //_primary->SetAttr("FilterSkipRejects",    int(1)); //accept events which do not pass filter - for testing
  }
  
  else if(config.Contains("minbiasProtonPionPair")) 
  {
    ProtonPionPairLMassFilter = new StProtonPionPairLMassFilter();

    //ProtonPionPairLMassFilter->SetDauKine(0., 1e6, -100, 100, -TMath::Pi(), TMath::Pi(), 0., 1e6, -100, 100, -TMath::Pi(), TMath::Pi()); //open cuts for check of acceptance effect
    ProtonPionPairLMassFilter->SetParentRapidities(-1, 1); //setter for p-pi pair rapidity

    _primary->AddFilter( ProtonPionPairLMassFilter );
    _primary->SetAttr("FilterKeepAll",    int(0));
    //_primary->SetAttr("FilterKeepHeader", int(0));
    //_primary->SetAttr("Debug", int(1));
    //_primary->SetAttr("FilterSkipRejects",    int(1)); //accept events which do not pass filter - for testing
  }

  // If set to 1, tracks will be saved in the tree on events which were
  // rejected.  If the tree size is too big (because the filter is too
  // powerful) you may want to set this equal to zero.  In which case
  // only header information is saved for the event.
  //_primary->SetAttr("FilterKeepHeader", int(0));

  // By default, the primary maker enters an infinite loop and executes
  // the event generator until it yields an event which passes the filter.
  // The big full chain treats this as a single event.
  //
  // If you want the BFC to see an empty event, set the FilterSkipRejects
  // attribute on the primary maker and give it the priveledge it needs
  // to kill the event. 
  //---  primary->SetAttr("FilterSkipRejects", int(1) ); // enables event skipping 
  //---  chain->SetAttr(".Privilege",1,"StarPrimaryMaker::*" );
//#endif

  //
  // Setup cuts on which particles get passed to geant for
  //   simulation.  (To run generator in standalone mode,
  //   set ptmin=1.0E9.)
  //                    ptmin  ptmax
  _primary->SetPtRange  (0.0,  10.);         // GeV
  //                    etamin etamax
  _primary->SetEtaRange ( -2.0, +2.0 );
  //                    phimin phimax
  _primary->SetPhiRange ( 0., TMath::TwoPi() );
  
  
  // 
  // Setup a realistic z-vertex distribution:
  //   x = 0 gauss width = 1mm
  //   y = 0 gauss width = 1mm
  //   z = 0 gauss width = 30cm
  // 
  _primary->SetVertex( 0., 0., 0. );
  _primary->SetSigma( 0.1, 0.1, 30.0 );

  
  //
  // Initialize primary event generator and all sub makers
  //
  _primary -> Init();

  if(collEnergy == 510) geometry("y2017a");
  if(collEnergy == 200) geometry("y2012a");
  
  command("gkine -4 0");
  
     
  if(config.Contains("minbiasDiLambdaBar"))
  {
    command("gfile o ./output_starsim/pythia8.minbiasDiLambdaBar.starsim.fzd");
  }
  
  else if(config.Contains("minbiasDiLambda"))
  {
   command("gfile o ./output_starsim/pythia8.minbiasDiLambda.starsim.fzd");
  }
  
  else if(config.Contains("minbiasLambdaLambdaBar"))
  {
    command("gfile o ./output_starsim/pythia8.minbiasLambdaLambdaBar.starsim.fzd");
  }
  
  else if(config.Contains("minbiasLambda"))
  {
    command("gfile o ./output_starsim/pythia8.minbiasLambda.starsim.fzd");
  }
  else if(config.Contains("minbiasProtonPionPair"))
  {
    command("gfile o ./output_starsim/pythia8.minbiasProtonPionPair.starsim.fzd");
  }
  else if(config.Contains("minbias"))
  {
    command("gfile o ./output_starsim/pythia8.minbias.starsim.fzd");
  }
  
  
  
  
  
  if ( GEANT_HANDLES_DECAYS ) 
  {
    _pythia8->Set("111:onMode=1"); // pi0 
    _pythia8->Set("211:onMode=1"); // pi+/-                         
    _pythia8->Set("221:onMode=1"); // eta                              
    _pythia8->Set("321:onMode=1"); // K+/-                             
    _pythia8->Set("310:onMode=1"); // K short                                               
    _pythia8->Set("130:onMode=1"); // K long                                               
    //_pythia8->Set("3122:onMode=1"); // Lambda 0
    _pythia8->Set("3122:onMode=0");
    _pythia8->Set("3122:OnIfMatch=2212 -211");
    _pythia8->Set("-3122:onMode=0");
    _pythia8->Set("-3122:OnIfMatch=-2212 211");                                          
    _pythia8->Set("3112:onMode=1"); // Sigma -                                              
    _pythia8->Set("3222:onMode=1"); // Sigma +                                              
    _pythia8->Set("3212:onMode=1"); // Sigma 0                                              
    _pythia8->Set("3312:onMode=1"); // Xi -                                                 
    _pythia8->Set("3322:onMode=1"); // Xi 0                                                 
    _pythia8->Set("3334:onMode=1"); // Omega -              
  }
  

  //
  // Trigger on nevents
  //
  trig( nevents );

  //
  // Finish the chain
  //
  chain->Finish();

  //
  // EXIT starsim
  //
  command("call agexit");  // Make sure that STARSIM exits properly

}
// ----------------------------------------------------------------------------

