//This filter is to find Lambda->k- + pi + process. 

#include "StDiLambdaPiPFilter.h"
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
StDiLambdaPiPFilter::StDiLambdaPiPFilter():StarFilterMaker("LambdaPiP")
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
StDiLambdaPiPFilter::~StDiLambdaPiPFilter()
{
	delete nEvents;
}
//_______________________________________________________________
Int_t StDiLambdaPiPFilter::Filter( StarGenEvent *mEvent )
{
	//if(mEvent->GetNumberOfParticles <= 0) {return kError;}
	if(int(mEvent->GetNumberOfParticles()) <= 0)return -1;

	TIter Iterator = mEvent->IterAll();
	StarGenParticle *p = 0;

	// Find Lambda baryon ( id = 3122 and -3122), find their daughters
	int indexLambda(0);
	int LambdaDaughter1(0), LambdaDaughter2(0);
	int LambdaMother1(0), LambdaMother2(0);

	int indexLambda_2(0);
	int Lambda_2Daughter1(0), Lambda_2Daughter2(0);
	int Lambda_2Mother1(0), Lambda_2Mother2(0);

	// Flags for mother Lambda mesons
	int mPassLambdaMotherKineFlag = 0;
	int mPassLambda_2MotherKineFlag = 0;
	TLorentzVector mot1(0,0,0,0);	
	TLorentzVector mot2(0,0,0,0);
	
	int firstLamdaFound = 0; //flag that one Lambda was found

	while( ( p = (StarGenParticle*)Iterator.Next() ) )
	{
	  // first Lambda
		if(p->GetId() == 3122 && firstLamdaFound == 0)
    { 
      firstLamdaFound = 1;
      
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
			
			continue; //continue after first Lambda is found
			
		}
		// second Lambda
		if(p->GetId() == 3122 && firstLamdaFound == 1)
    { 
			indexLambda_2 = p->GetIndex();
			//cout<<"Lambda_2 found!"<<endl;
			//Find hadronization products of c
			Lambda_2Daughter1 = p->GetFirstDaughter();
			Lambda_2Daughter2 = p->GetLastDaughter();
			Lambda_2Mother1 = p->GetFirstMother();
			Lambda_2Mother2 = p->GetLastMother();

			mot2.SetXYZM(p->GetPx(),p->GetPy(),p->GetPz(),LambdaMass);
			//cout<<"Lambda_bar y="<<mot2.Rapidity()<<endl;

			//cout<<"d1\td2\tm1\tm2"<<endl;
			//cout<<Lambda_2Daughter1<<"\t"<<Lambda_2Daughter2<<"\t"<<Lambda_2Mother1<<"\t"<<Lambda_2Mother2<<endl;
		}
	}// particle loop
	
	if(!indexLambda || !indexLambda_2) return  StarGenEvent::kReject;

	//loop for daughters
	TIter Iterator2 = mEvent->IterAll();
	StarGenParticle *p2 = 0;
	int yesLambda(0);
	int LambdadaughterP(0);
	int LambdadaughterPi(0);

	int yesLambda_2(0);
	int Lambda_2daughterP(0);
	int Lambda_2daughterPi(0);

	// Flags for daughter muons
	int mPassLambdadaughterKineFlag = 0;
	int mPassLambda_2daughterKineFlag = 0;
	TLorentzVector dau1pi(0,0,0,0);	
	TLorentzVector dau1p (0,0,0,0);	
	TLorentzVector dau2pi(0,0,0,0);	
	TLorentzVector dau2p (0,0,0,0);	

	while( ( p2 = (StarGenParticle*)Iterator2.Next() ) )
	{
		if((p2->GetIndex() >= LambdaDaughter1) && (p2->GetIndex() <= LambdaDaughter2))
		{
			//cout<<"Lambda daughter:"<<p2->GetId()<<endl;
			if( fabs(p2->GetId()) == 211)
			{
				LambdadaughterPi=1;
				//cout<<"pi- found!"<<endl;
				dau1pi.SetXYZM(p2->GetPx(),p2->GetPy(),p2->GetPz(),piMass);
			}
			if( fabs(p2->GetId()) == 2212)
			{
				LambdadaughterP=1;
				//cout<<"p found!"<<endl;
				dau1p.SetXYZM(p2->GetPx(),p2->GetPy(),p2->GetPz(),kMass);
			}
		}
		if((p2->GetIndex() >= Lambda_2Daughter1) && (p2->GetIndex() <= Lambda_2Daughter2))
		{
			//cout<<"Lambda_bar daughter:"<<p2->GetId()<<endl;
			if( fabs(p2->GetId()) == 211)
			{
				Lambda_2daughterPi=1;
				//cout<<"pi- found!"<<endl;
				dau2pi.SetXYZM(p2->GetPx(),p2->GetPy(),p2->GetPz(),piMass);
			}
			if( fabs(p2->GetId()) == 2212)
			{
				Lambda_2daughterP=1;
				//cout<<"p-bar found!"<<endl;
				dau2p.SetXYZM(p2->GetPx(),p2->GetPy(),p2->GetPz(),kMass);
			}
		}
	}// particle loop 2

	//loop for daughters
	TIter Iterator3 = mEvent->IterAll();
	StarGenParticle *p3 = 0;
	if(LambdadaughterP && LambdadaughterPi)// &&mothercb)
		yesLambda=1;
	if(Lambda_2daughterP && Lambda_2daughterPi)// &&mothercb)
		yesLambda_2=1;

	int mothercbLambda(0);
	int mothercbLambda_2(0);
	while( ( p3 = (StarGenParticle*)Iterator3.Next() ) )
	{
		if(yesLambda && (p3->GetIndex() == LambdaMother1) )//&& (p3->GetIndex() <= LambdaMother2))
			//if(yesLambda_2 && (p3->GetIndex() >= LambdaMother1) && (p3->GetIndex() <= LambdaMother2))
		{
			mothercbLambda = 1;
			//cout<<"Lambda mother found = "<<p3->GetId()<<endl;
		}

		//if(yesLambda_2 && (p3->GetIndex() >= Lambda_2Daughter1) && (p3->GetIndex() <= Lambda_2Daughter2))
		//{
		//	//cout<<"********************************************"<<endl;
		//	//cout<<"daughter found = "<<p3->GetId()<<endl;
		//	//cout<<"********************************************"<<endl;
		//}

		if(yesLambda_2 && (p3->GetIndex() == Lambda_2Mother1))// && (p3->GetIndex() <= Lambda_2Mother2))
			//if(yesLambda_2 && (p3->GetIndex() >= Lambda_2Mother1) && (p3->GetIndex() <= Lambda_2Mother2))
		{
			mothercbLambda_2 = 1;
			//cout<<"Lambda_bar mother found = "<<p3->GetId()<<endl;
		}
	}// particle loop 2

	// Check Lambda kinematics
	if(mMotherKineFlag &&	mot1.Rapidity()>mParent1YMin && mot1.Rapidity()<mParent1YMax) mPassLambdaMotherKineFlag = 1;

	if(mMotherKineFlag && mot2.Rapidity()>mParent2YMin && mot2.Rapidity()<mParent2YMax) mPassLambda_2MotherKineFlag = 1;

	// Check Lambda daughter kinematics
	if(mDaughterKineFlag)
  {
		if(dau1pi.Pt()>mPtMin1&&dau1pi.Pt()<mPtMax1 && dau1pi.Eta()>mEtaMin1&&dau1pi.Eta()<mEtaMax1 &&	dau1pi.Phi()>mPhiMin1&&dau1pi.Phi()<mPhiMax1 &&
			 dau1p.Pt() >mPtMin2&&dau1p.Pt()<mPtMax2 && dau1p.Eta() >mEtaMin2&&dau1p.Eta() <mEtaMax2 && dau1p.Phi() >mPhiMin2&&dau1p.Phi() <mPhiMax2) 
    {
			mPassLambdadaughterKineFlag = 1;
		}
		if(dau2pi.Pt()>mPtMin1&&dau2pi.Pt()<mPtMax1 && dau2pi.Eta()>mEtaMin1&&dau2pi.Eta()<mEtaMax1 &&	dau2pi.Phi()>mPhiMin1&&dau2pi.Phi()<mPhiMax1 &&
			 dau2p.Pt() >mPtMin2&&dau2p.Pt()<mPtMax2 && dau2p.Eta() >mEtaMin2&&dau2p.Eta() <mEtaMax2 && dau2p.Phi() >mPhiMin2&&dau2p.Phi() <mPhiMax2)
		{
			mPassLambda_2daughterKineFlag = 1;
		}
	}

	// Lambda or Lambda-bar not found
	if (
			(
			 (mMotherKineFlag && mPassLambdaMotherKineFlag == 0)  || 
			 (mDaughterKineFlag && mPassLambdadaughterKineFlag == 0)  || 
			 mothercbLambda == 0 
			) 
      || //this should make sure that there are 2 Lambdas in the event
			(
			 (mMotherKineFlag && mPassLambda_2MotherKineFlag == 0)  || 
			 (mDaughterKineFlag && mPassLambda_2daughterKineFlag == 0)  || 
			 mothercbLambda_2 == 0
			)
		 ) 
  {
		return StarGenEvent::kReject;
	}
			 ////cout << mPassLambdaMotherKineFlag <<" "
			 //<<  mPassLambdadaughterKineFlag <<" " 
			 //<< mothercbLambda <<endl;
			 ////cout<<"++++++++++++++++++++++++++++++++++++++++"<<endl;
			 ////cout << mPassLambda_2MotherKineFlag <<" "
			 //<<  mPassLambda_2daughterKineFlag <<" " 
			 //<< mothercbLambda_2 <<endl;
  if((mPassLambdaMotherKineFlag == 1) &&
		 (mPassLambdadaughterKineFlag == 1) &&
		 mothercbLambda == 1 
    &&
     (mPassLambda_2MotherKineFlag == 1) &&
		 (mPassLambda_2daughterKineFlag == 1) &&
		 mothercbLambda_2 == 1 )
  {
    cout<<"Two Lambas found!"<<endl;
  }
    

  return StarGenEvent::kAccept;
}

void  StDiLambdaPiPFilter::SetDauKine(double ptMin1, double ptMax1, double etaMin1, double etaMax1, double phiMin1, double phiMax1, double ptMin2, double ptMax2, double etaMin2, double etaMax2, double phiMin2, double phiMax2)
{
	mPtMin1 = ptMin1; mPtMax1 = ptMax1;
	mEtaMin1 = etaMin1; mEtaMax1 = etaMax1;
	mPhiMin1 = phiMin1; mPhiMax1 = phiMax1;
	mPtMin2 = ptMin2; mPtMax2 = ptMax2;
	mEtaMin2 = etaMin2; mEtaMax2 = etaMax2;
	mPhiMin2 = phiMin2; mPhiMax2 = phiMax2;
	mDaughterKineFlag = true;
}

void StDiLambdaPiPFilter::SetParentRapidities(double yMin1, double yMax1, double yMin2, double yMax2)
{
	mParent1YMin = yMin1;
	mParent1YMax = yMax1;
	mParent2YMin = yMin2;
	mParent2YMax = yMax2;
	mMotherKineFlag = true;
}

void StDiLambdaPiPFilter::SetMode(int val)
{
	mMode = val;
	//cout<<"StDiLambdaPiPFilter::SetMode to mMode = "<<mMode<<endl;
}
