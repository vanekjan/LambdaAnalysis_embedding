//////////////////////////////////////////////////////////////////////////
//
// $Id: bfcMixer_Jet.C,v 1.1 2018/08/31 15:14:36 zchang Exp $
//
// $Log: bfcMixer_Jet.C,v $
// Revision 1.1  2018/08/31 15:14:36  zchang
// embedding macros
//
//
// JET EMBEDDING MACRO
//
// Pibero Djawotho <pibero@tamu.edu>
// Texas A&M University
// 27 July 2011
//
//////////////////////////////////////////////////////////////////////////

//#include "StSpinPool/StBfcTriggerFilterMaker/StPythiaEventMaker.h"

class StChain;
StChain* Chain = 0;

class StBFChain;
StBFChain* chain1 = 0;
StBFChain* chain2 = 0;
StBFChain* chain3 = 0;

//_____________________________________________________________________
void bfcMixer(const Int_t Nevents = 1,
		  //const Char_t* daqfile = "./bfc_input/st_zerobias_adc_13078014_raw_1600001.daq",
		  const Char_t* daqfile = "./bfc_input/Run12_200/st_zerobias_adc_13057005_raw_1600001.daq",
		  const Char_t* fzdfile = "./bfc_input/pythia8.minbiasLambdaLambdaBar.starsim.fzd",
//		  const Char_t* fzdfile = "output/13078014/pt3_4_st_zerobias_adc_13078014_raw_2570001_2.fzd",
		  const Char_t* prodName = "P12idpp200",
		  const Char_t* DbVoption = "")
{
   
   //check chain options at https://www.star.bnl.gov/devcgi/dbProdOptionRetrv.pl
   
   //Run12 pp500 RFF chain
   // Will not run without Sti !!!
  //  TString prodP13ibpp500RFF("DbV20130502 pp2012b Sti AgML mtdDat btof fmsDat VFPPVnoCTB beamline BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D -evout -hitfilt");
  //TString prodP13ibpp500RFF("DbV20130502 DbV20160318_EMC_Calibrations Dbv20160318_TRG_Calibrations pp2012b Sti AgML mtdDat btof fmsDat VFPPVnoCTB beamline BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D -evout -hitfilt");
  TString prodP13ibpp500RFF("DbV20130502 DbV20160318_EMC_Calibrations Dbv20160318_TRG_Calibrations pp2012b Sti AgML mtdDat btof fmsDat VFPPVnoCTB beamline BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D -hitfilt");
  
  
  
  //Run 12 pp200
  //TString prodP13ibpp200("DbV20130212 pp2012b Sti AgML mtdDat btof fmsDat VFPPVnoCTB useBTOF4Vtx beamline BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D -hitfilt"); //options directy from page above
  TString prodP12idpp200("DbV20130212 DbV20160506_EMC_Calibrations pp2012b Sti mtdDat btof fmsDat VFPPVnoCTB useBTOF4Vtx beamline BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D -hitfilt");//options from Dima
                            
  //Run17 pp 510 GeV - cross check                      
  //TString prodP18ibpp510("DbV20171001 pp2017a StiCA btof mtd mtdCalib PicoVtxDefault fmsDat fmsPoint fpsDat BEmcChkStat OSpaceZ2 OGridLeak3D -hitfilt");  //options directy from page above
  TString prodP18ibpp510("DbV20171001 pp2017a StiCA btof PicoVtxDefault fmsDat fmsPoint fpsDat BEmcChkStat OSpaceZ2 OGridLeak3D -hitfilt");  //changed for L analysis
  

  // Additional tags needed for embedding
  prodP13ibpp500RFF += " TpxClu -VFMinuit";
  prodP12idpp200 += " TpxClu -VFMinuit";

  //geometry 
  TString geomP12id("ry2012a"); 
  TString geomP13ib("ry2012a");
  TString geomP18ib("ry2017a");

  TString chain1Opt("in,magF,tpcDb,NoDefault,TpxRaw,-ittf,NoOutput");
  TString chain2Opt("fzin,gen_T,geomT,sim_T,TpcRS,-ittf,-tpc_daq,nodefault");
  chain2Opt += " ";

  TString chain3Opt;   
  
  if(prodName == "P13ibpp500RFF") { chain3Opt = prodP13ibpp500RFF; chain2Opt += geomP13ib; }
  else if(prodName == "P12idpp200") { chain3Opt = prodP12idpp200; chain2Opt += geomP12id; }
  else if(prodName == "P18ibpp510") { chain3Opt = prodP18ibpp510; chain2Opt += geomP18ib; }
  else
  {
   cout << "Choice prodName " << prodName << " does not correspond to known chain. Processing impossible." << endl;
   return;
  }

  chain3Opt.Prepend(' ');
  chain3Opt.Prepend(DbVoption);
  chain3Opt += ",Embedding,TpcMixer,GeantOut,evout,MiniMcMk,McAna,-in,NoInput,useInTracker,nodefault"; 
  chain3Opt += ",";

  if(prodName == "P13ibpp500RFF"){ chain3Opt += geomP13ib; }
  else if(prodName == "P12idpp200"){ chain3Opt += geomP12id; }
  else if(prodName == "P18ibpp510"){ chain3Opt += geomP18ib; }
  else
  {
    cout << "Choice prodName " << prodName << " does not correspond to known chain. Processing impossible. " << endl;
    return;
  }
  
  // Add BEMC simulators to chain
  chain3Opt += ",emcSim";

  // Add EEMC fast simulator to chain
  chain3Opt += ",EEfs";
     
  // Dynamically link some shared libs
  gROOT->LoadMacro("bfc.C");
  if (gClassTable->GetID("StBFChain") < 0) Load();
  //______________Create the main chain object______________________________________
  Chain = new StChain("Embedding");
  //________________________________________________________________________________
  bfc(-1,chain1Opt,daqfile);
  chain1 = chain;
  chain1->SetName("One");
  chain1->SetAttr(".call","SetActive(0)","St_db_Maker::"); // Use DB cache to reduce overhead
  Chain->cd();
  //________________________________________________________________________________  
  bfc(-1,chain2Opt,fzdfile);
  chain2 = chain;
  chain2->SetName("Two");
  Chain->cd();
  if (chain2->GetOption("TRS")){
    StTrsMaker *trsMk = (StTrsMaker *) chain2->GetMaker("Trs");
    if (!trsMk) {
      cout << "Cannot find Trs in chain2" << endl;
      return;
    }
    trsMk->setNormalFactor(1.32);
    trsMk->SetMode(0);
  }
  //________________________________________________________________________________
  //  gSystem->Load("StFtpcMixerMaker");
  //  StFtpcMixerMaker  *ftpcmixer = new StFtpcMixerMaker("FtpcMixer","daq","trs");
  //________________________________________________________________________________
  TString OutputFileName(gSystem->BaseName(fzdfile));
  //TString OutputFileName(fzdfile);  
  OutputFileName.ReplaceAll("*","");
  OutputFileName.ReplaceAll(".fzd","");
  //  OutputFileName.Append("_emb.root");
  OutputFileName.Append(".root");
  //OutputFileName.ReplaceAll("output_starsim","output_bfc");
  cout<<OutputFileName.Data()<<endl;
  bfc(-1,chain3Opt,0,OutputFileName);
  chain3 = chain;
  chain3->SetName("Three"); 
  Chain->cd();
  //________________________________________________________________________________
  StTpcMixerMaker  *mixer = (StTpcMixerMaker *) chain3->Maker("TpcMixer");
  
  if( prodName == "P08icAuAu200")
	{
	  mixer->SetInput("Input1","MixerEvent");
	}
  else
	{
	  mixer->SetInput("Input1","TpxRaw/.data/Event");
  }

  if (chain2Opt.Contains("TpcRS",TString::kIgnoreCase))
  {
    mixer->SetInput("Input2","TpcRS/Event");
  }
  else
  {
    mixer->SetInput("Input2","Trs/.const/Event");
  }
  
  Chain->cd();

  //------------------------------------ EMC MIXERS ------------------------------------
  // Add BEMC mixer to chain3
  StEmcRawMaker* emcRaw = (StEmcRawMaker*)chain3->GetMaker("emcRaw");
  emcRaw->getBemcRaw()->saveAllStEvent(true); // use all 4800 BEMC towers
  gSystem->Load("StEmcMixerMaker");
  StEmcMixerMaker* bemcMixer = new StEmcMixerMaker;
  chain3->AddAfter("EmcSimulator",bemcMixer);
  // Set EEMC fast and slow simulator in embedding mode
  StEEmcFastMaker* eefs = (StEEmcFastMaker*)chain3->GetMaker("eefs");
  eefs->SetEmbeddingMode(); // Use local StEmcCollection
  eefs->UseFullTower(true); // Use full ETOW detector
  StEEmcSlowMaker* eess = new StEEmcSlowMaker;
  eess->setEmbeddingMode(true);
  // Add EEMC mixer to chain3
  StEEmcMixerMaker* eemcMixer = new StEEmcMixerMaker;
  //------------------------------------------------------------------------------------

  //----------------------------- TRIGGER FILTER -----------------------------
  // We want to achieve the following ordering for makers:
  // 1. BBC simulator
  // 2. BEMC simulator
  // 3. BEMC mixer
  // 4. EEMC fast simulator
  // 5. EEMC slow simulator
  // 6. EEMC mixer
  // 7. Pythia event maker
  // 8. Trigger simulator
  // 9. Trigger filter
  // 10. TPC maker

  // Place TPC chain after EMC makers
  chain3->AddAfter("eefs",chain3->GetMaker("tpcChain"));
  chain3->AddAfter("eefs",eemcMixer);
  chain3->AddAfter("eefs",eess);
   

  // Place Pythia maker after GEANT maker
  // and trigger filter after EMC makers
  // 
  gSystem->Load("StTriggerUtilities");
  gSystem->Load("StJetSkimEvent");
  gSystem->Load("StBfcTriggerFilterMaker");
 
  StPythiaEventMaker* pythia = new StPythiaEventMaker;
  TString pyfile = gSystem->BaseName(fzdfile);
  pyfile.ReplaceAll(".fzd",".pythia.root");
  pyfile.ReplaceAll("output_starsim","output_bfc");
  pythia->SetPythiaFile(pyfile);
  chain3->AddAfter("geant", pythia);
  // 
  // Place trigger simulator after EMC makers
  StTriggerSimuMaker* trgsim = new StTriggerSimuMaker;
  trgsim->useOfflineDB();
  trgsim->setMC(1);
  trgsim->useBemc();
  trgsim->bemc->setConfig(StBemcTriggerSimu::kOnline);
  trgsim->useEemc();
  trgsim->eemc->setSource("StEvent");
  // set up trigger thresholds for 2012 pp510
 
  trgsim->setBarrelJetPatchTh(0,28);
  trgsim->setBarrelJetPatchTh(1,36);
  trgsim->setBarrelJetPatchTh(2,66);

  trgsim->setOverlapJetPatchTh(0,28);
  trgsim->setOverlapJetPatchTh(1,36);
  trgsim->setOverlapJetPatchTh(2,66);

  trgsim->setEndcapJetPatchTh(0,28);
  trgsim->setEndcapJetPatchTh(1,36);
  trgsim->setEndcapJetPatchTh(2,66);

  trgsim->setBarrelHighTowerTh(0, 11);
  trgsim->setBarrelHighTowerTh(1, 18);
  trgsim->setBarrelHighTowerTh(2, 25);
  trgsim->setBarrelHighTowerTh(3, 31);

  trgsim->setEndcapHighTowerTh(0, 25);
  trgsim->setEndcapHighTowerTh(1, 31);

  //no trigger filter
  StBfcTriggerFilterMaker* trgfilt = new StBfcTriggerFilterMaker;
  //The BFC trigger filter will select only JP0 events
  //trgfilt->SetJP0();
  trgfilt->SetOkAllEvents(1);
  chain3->AddBefore("tpcChain",trgsim);
  chain3->AddBefore("tpcChain",trgfilt);

  // Move these makers after trigger decision
  // *** VERY IMPORTANT ***
  // The order of TpxRaw and TpcRS *must* be preserved
  // or the embedding will *not* work. [RT# 2299]
  // http://www.star.bnl.gov/rt2/Ticket/Display.html?id=2299
  StTpcRSMaker* TpcRS = (StTpcRSMaker*)chain2->GetMaker("TpcRS");
  StTpcHitMaker* TpxRaw = (StTpcHitMaker*)chain1->GetMaker("TpxRaw");
  chain3->AddBefore("TpcMixer",TpxRaw);
  chain3->AddBefore("TpcMixer",TpcRS);
  //--------------------------------------------------------------------------

  TString trgfile = gSystem->BaseName(fzdfile);
  trgfile.ReplaceAll(".fzd",".trig.root");
  trgfile.ReplaceAll("output_starsim","output_bfc");
  TFile* ofile = TFile::Open(trgfile,"recreate");
  assert(ofile);
  
  TH2F* hBarrelHighTowerSimu = new TH2F("hBarrelHighTowerSimu","BEMC high tower simu;trigger patch;high tower",300,0,300,64,0,64);
  TH2F* hBarrelPatchSumSimu = new TH2F("hBarrelPatchSumSimu","BEMC patch sum simu;trigger patch;patch sum",300,0,300,64,0,64);
  TH2F* hEndcapHighTowerSimu = new TH2F("hEndcapHighTowerSimu","EEMC high tower simu;trigger patch;high tower",90,0,90,64,0,64);
  TH2F* hEndcapPatchSumSimu = new TH2F("hEndcapPatchSumSimu","EEMC patch sum simu;trigger patch;patch sum",90,0,90,64,0,64);
  TH2F* hBarrelJetPatchSimu = new TH2F("hBarrelJetPatchSimu","BEMC jet patch;jet patch;adc",18,0,18,160,0,160);
  TH2F* hEndcapJetPatchSimu = new TH2F("hEndcapJetPatchSimu","EEMC jet patch;jet patch;adc",6,0,6,160,0,160);
  TH2F* hOverlapJetPatchSimu = new TH2F("hOverlapJetPatchSimu","BEMC-EEMC-overlap;jet patch;adc",6,0,6,160,0,160);
  //--------------------------------------------------------------------------

  // Initialize chain
  Chain->Init();
  PrintTimer(Chain);
  puts("Order of makers in BFCMIXER:");
  StMaker::lsMakers(Chain);
  // Event loop
  int mNTotal = 0;
  int mNFailed = 0;
  TBenchmark evnt;
  StIOMaker* inputStream = (StIOMaker*)chain1->GetMaker("inputStream");
  for (int iEvent = 1; iEvent <= Nevents; ++iEvent) {
    evnt.Reset();
    evnt.Start("QAInfo:");
    Chain->Clear();
    int iMake = Chain->Make(iEvent);
    if (iMake == kStErr) ++mNFailed;
    if (inputStream->GetMakeReturn() % 10 == kStEOF) {
      inputStream->Rewind();
      --iEvent;
      continue;
    }
    if (iMake % 10 == kStEOF || iMake % 10 == kStFatal) break;
    ++mNTotal;
    PrintTimer(Chain);
    //--------------------------------------------------------------------------
/*
    // BEMC high towers and trigger patches
    for (int triggerpatch = 0; triggerpatch < 300; ++triggerpatch) {
      hBarrelHighTowerSimu->Fill(triggerpatch,trgsim->bemc->getBEMC_FEE_HT_ADC()[triggerpatch]);
      hBarrelPatchSumSimu->Fill(triggerpatch,trgsim->bemc->getBEMC_FEE_TP_ADC()[triggerpatch]);
    } // for triggerpatch
    // BEMC jet patches
    for (int jetpatch = 0; jetpatch < 18; ++jetpatch) {
      hBarrelJetPatchSimu->Fill(jetpatch,trgsim->bemc->barrelJetPatchAdc(jetpatch));
    } // for jetpatch
    // EEMC high towers and trigger patches
    for (int triggerpatch = 0; triggerpatch < 90; ++triggerpatch) {
      hEndcapHighTowerSimu->Fill(triggerpatch,trgsim->eemc->getOutHT(triggerpatch));
      hEndcapPatchSumSimu->Fill(triggerpatch,trgsim->eemc->getOutTPsum(triggerpatch));
    } // for triggerpatch
    // EEMC jet patches
    for (int jetpatch = 0; jetpatch < 6; ++jetpatch) {
      hEndcapJetPatchSimu->Fill(jetpatch,trgsim->eemc->endcapJetPatchAdc(jetpatch));
    } // for jetpatch
    // BEMC-EEMC-overlap jet patches
    for (int i = 0; i < 2; ++i) {
      int jetpatch, adc;
      trgsim->emc->getOverlapJetPatchAdc(i,jetpatch,adc);
      hOverlapJetPatchSimu->Fill(jetpatch,adc);
    } // for i
    //--------------------------------------------------------------------------
*/
    evnt.Stop("QAInfo:");
    printf("QAInfo: Done with Event [no. %d/run %d/evt. %d/Date.Time %d.%d/sta %d] Real Time = %10.2f seconds Cpu Time = %10.2f seconds\n",iEvent,Chain->GetRunNumber(),Chain->GetEventNumber(),Chain->GetDate(),Chain->GetTime(),chain3->GetMakeReturn(),evnt.GetRealTime("QAInfo:"),evnt.GetCpuTime("QAInfo:"));
  } // End event loop
  printf("QAInfo:EventLoop completed code %d\n",iMake);
  gSystem->Exec("date");
  TDatime t;
  printf("QAInfo:Run is finished at Date/Time %i/%i; Total events processed: %i and not completed: %i\n",t.GetDate(),t.GetTime(),mNTotal,mNFailed);
  //--------------------------------------------------------------------------
  //ofile->Write();
  //ofile->Close();
  //--------------------------------------------------------------------------
  
  return;
}

// Print timers for all makers in chain
class StMaker;
void PrintTimer(StMaker* chain)
{
  TIter next(chain->GetMakeList());
  StMaker* maker;
  while (maker = (StMaker*)next()) {
    maker->PrintTimer();
    PrintTimer(maker);
    // Hack to reset timer
    maker->StartTimer(true);
    maker->StopTimer();
  }
}
