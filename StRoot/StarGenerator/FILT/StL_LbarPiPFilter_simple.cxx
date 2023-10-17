//This filter is to find Lambda->k- + pi + process. 

#include "StL_LbarPiPFilter_simple.h"
#include "StarGenerator/EVENT/StarGenParticle.h"
#include "StarGenerator/EVENT/StarGenEvent.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>

#define muMass 0.106
#define piMass 0.139570
#define kMass 0.493677
#define LambdaMass 1.115683 //mass in GeV/c^2 from latest PDG

using namespace std;
//_______________________________________________________________
StL_LbarPiPFilter_simple::StL_LbarPiPFilter_simple():StarFilterMaker("LambdaPiP")
{

	nEvents = new int;
	(*nEvents) = 0;

	mMotherKineFlag = false;
	mDaughterKineFlag = false;
	mParent1YMin = -1.;
	mParent1YMax = 1.;
	mParent2YMin = -1.;
	mParent2YMax = 1.;

	mMode = 1;

	mPtMin1  = 0.2;
	mPtMax1  = 1e5;
	mEtaMin1 = -1.;
	mEtaMax1 = 1.;
	mPhiMin1 = -TMath::Pi();
	mPhiMax1 = TMath::Pi();
	mPtMin2  = 0.2;
	mPtMax2  = 1e5;
	mEtaMin2 = -1.;
	mEtaMax2 = 1.;
	mPhiMin2 = -TMath::Pi();
	mPhiMax2 = TMath::Pi();
}
//_______________________________________________________________
StL_LbarPiPFilter_simple::~StL_LbarPiPFilter_simple()
{
	delete nEvents;
}
//_______________________________________________________________
Int_t StL_LbarPiPFilter_simple::Filter( StarGenEvent *mEvent )
{
	//if(mEvent->GetNumberOfParticles <= 0) {return kError;}
	if(int(mEvent->GetNumberOfParticles()) <= 0)return -1;

	TIter Iterator = mEvent->IterAll();
	StarGenParticle *p = 0;

	// Find Lambda baryon ( id = 3122 and -3122), find their daughters
	int indexLambda(0);
	int LambdaDaughter1(0), LambdaDaughter2(0);
	int LambdaMother1(0), LambdaMother2(0);

	int indexLambdabar(0);
	int LambdabarDaughter1(0), LambdabarDaughter2(0);
	int LambdabarMother1(0), LambdabarMother2(0);

	// Flags for mother Lambda mesons
	int mPassLambdaMotherKineFlag = 0;
	int mPassLambdabarMotherKineFlag = 0;
	TLorentzVector mot1(0,0,0,0);	
	TLorentzVector mot2(0,0,0,0);	

	while( ( p = (StarGenParticle*)Iterator.Next() ) )
	{
		if(p->GetId() == 3122)
    { // Lambda
			indexLambda = p->GetIndex();
			//cout<<"Lambda found!"<<endl;
			//Find hadronization products of c
			LambdaDaughter1 = p->GetFirstDaughter();
			LambdaDaughter2 = p->GetLastDaughter();
			LambdaMother1 = p->GetFirstMother();
			LambdaMother2 = p->GetLastMother();

			mot1.SetXYZM(p->GetPx(),p->GetPy(),p->GetPz(),LambdaMass);
			//cout<<"Lambda y="<<mot1.Rapidity()<<endl;

			//cout<<"d1\td2\tm1\tm2"<<endl;
			//cout<<LambdaDaughter1<<"\t"<<LambdaDaughter2<<"\t"<<LambdaMother1<<"\t"<<LambdaMother2<<endl;
		}
		if(p->GetId() == -3122)
    { // Lambda bar
			indexLambdabar = p->GetIndex();
			//cout<<"Lambdabar found!"<<endl;
			//Find hadronization products of c
			LambdabarDaughter1 = p->GetFirstDaughter();
			LambdabarDaughter2 = p->GetLastDaughter();
			LambdabarMother1 = p->GetFirstMother();
			LambdabarMother2 = p->GetLastMother();

			mot2.SetXYZM(p->GetPx(),p->GetPy(),p->GetPz(),LambdaMass);
			//cout<<"Lambda_bar y="<<mot2.Rapidity()<<endl;

			//cout<<"d1\td2\tm1\tm2"<<endl;
			//cout<<LambdabarDaughter1<<"\t"<<LambdabarDaughter2<<"\t"<<LambdabarMother1<<"\t"<<LambdabarMother2<<endl;
		}
	}// particle loop
	if(!indexLambda || !indexLambdabar) return  StarGenEvent::kReject;

	
	// Check Lambda kinematics
	if(mMotherKineFlag &&	mot1.Rapidity()>mParent1YMin && mot1.Rapidity()<mParent1YMax) mPassLambdaMotherKineFlag = 1;

	if(mMotherKineFlag && mot2.Rapidity()>mParent2YMin && mot2.Rapidity()<mParent2YMax) mPassLambdabarMotherKineFlag = 1;

	// Lambda or Lambda-bar not found
	if ( (mMotherKineFlag && mPassLambdaMotherKineFlag == 0) || //this should make sure there is L-Lbar pair in the event, for single L(Lbar), && was used
			 (mMotherKineFlag && mPassLambdabarMotherKineFlag == 0)	) 
  {
		return StarGenEvent::kReject;
	}

  if( mPassLambdaMotherKineFlag == 1 && 
      mPassLambdabarMotherKineFlag == 1 )
  {
    cout<<"Good L-L-bar pair found!"<<endl;
  }
    

  return StarGenEvent::kAccept;
}

void  StL_LbarPiPFilter_simple::SetDauKine(double ptMin1, double ptMax1, double etaMin1, double etaMax1, double phiMin1, double phiMax1, double ptMin2, double ptMax2, double etaMin2, double etaMax2, double phiMin2, double phiMax2)
{
	mPtMin1 = ptMin1; mPtMax1 = ptMax1;
	mEtaMin1 = etaMin1; mEtaMax1 = etaMax1;
	mPhiMin1 = phiMin1; mPhiMax1 = phiMax1;
	mPtMin2 = ptMin2; mPtMax2 = ptMax2;
	mEtaMin2 = etaMin2; mEtaMax2 = etaMax2;
	mPhiMin2 = phiMin2; mPhiMax2 = phiMax2;
	mDaughterKineFlag = true;
}

void StL_LbarPiPFilter_simple::SetParentRapidities(double yMin1, double yMax1, double yMin2, double yMax2)
{
	mParent1YMin = yMin1;
	mParent1YMax = yMax1;
	mParent2YMin = yMin2;
	mParent2YMax = yMax2;
	mMotherKineFlag = true;
}

void StL_LbarPiPFilter_simple::SetMode(int val)
{
	mMode = val;
	//cout<<"StL_LbarPiPFilter_simple::SetMode to mMode = "<<mMode<<endl;
}
