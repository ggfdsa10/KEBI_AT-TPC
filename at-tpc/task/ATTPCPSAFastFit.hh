#ifndef ATTPCPSAFASTFIT_HH
#define ATTPCPSAFASTFIT_HH

#include "KBChannelHit.hh"
#include "ATTPCPSA.hh"
#include "KBPulseGenerator.hh"

#include <vector>
#include <iostream>
using namespace std;

class ATTPCPSAFastFit : public ATTPCPSA, public KBPulseGenerator
{
  public:
    ATTPCPSAFastFit();
    ATTPCPSAFastFit(TString fileName);
    virtual ~ATTPCPSAFastFit() {}

    void Init();

    /** 
     * Find hits from the pad, pass hits to hitArray
     * Process is done as below:
     *  1. FindPeak()
     *  2. FitPulse(), LSFitPulse()
     *  3. TestPulse()
     */
    virtual void AnalyzeChannel(Double_t *buffer, vector<KBChannelHit> *hitArray, vector<vector<pair<double, double>>> *TBArray);

    /** 1.
     * Find the first peak from adc time-bucket starting from input tbCurrent
     * tbCurrent and tbStart becomes time-bucket of the peak and starting point
     */
    Bool_t FindPeak(Double_t *adc, Int_t &tbCurrent, Int_t &tbStart);

    /** 2.
     * Perform least square fitting with the the pulse around tbStart ~ tbPeak.
     * This process is Iteration based process using method LSFitPuse();
     */
    Bool_t FitPulse(Double_t *adc, Int_t tbStart, Int_t tbPeak,
                    Double_t &tbHit, Double_t &amplitude, 
                    Double_t &squareSum, Int_t &ndf,
                    Bool_t &saturated);

    /** 2.1
     * Perform least square fitting with the fixed parameter tbStart and ndf.
     * This process is analytic. The amplitude is choosen imidiatly.
     */
    void LSFitPulse(Double_t *buffer, Double_t tbStart, Int_t ndf,
                    Double_t &chi2, Double_t &amplitude);

    /** 3.
     * Test pulse with the previous pulse
     * If the pulse is not distinguishable from the previous pulse, return false.
     * If the pulse is distinguishable, subtract pulse distribution from adc,
     * and return true
     */
    Bool_t TestPulse(Double_t *adc,
                     Double_t tbHitPre, Double_t amplitudePre,
                     Double_t tbHit,    Double_t amplitude, vector<pair<double, double>> *TBADCPad);

  private:
    Double_t fThresholdOneTbStep;

    Int_t fTbStartCut;

    /**
     * Maximum number of iteration alowed for fitting parameter tbStart.
     * Where, tbStart is "staring time-bucket of the pulse"
     */
    Int_t fIterMax = 40;

    /** 
     * Number of time-bucket to subtract fitted pulse from the adc distribution.
     *
     * This means:
     * for (tb = tbStart; tb < tbStart + fNumTbsCorrection; tb ++)
     *   [adc distribution]_tb -= [fitted pulse]_tb
     *
     * Note that adc distribution will be used agian to find the next peak.
     */
    Int_t fNumTbsCorrection = 50;

    /**
     * Proportional parameter for the time-bucket step when fitting tbStart.
     *
     * Each time the least square fit is performed for amplitude of the pulse
     * with fixed tbStart, we have to select next candidate of the tbStart. 
     * The step of time-bucket from current tbStart is choosen as
     * step = fAlpha / peak^2 * beta.
     *
     * for beta, see fBetaCut.
     */
    Double_t fAlpha = 50.;

    /**
     * The default cut for beta. If beta becomes lower than fBetaCut
     * for twice in a row, the fit is satisfied.
     *
     * beta is defined by
     *   minus of [difference between least-squares of previous fit and current fit] 
     *   divided by [difference between time-bucket of previous fit and current fit] 
     *   divided by NDF: 
     *   beta = -(lsCur-lsPre)/(tbCur-tbPre)/ndf.
     *
     * This cut is redefined as effective threhold for each pulses 
     * betaCut = fBetaCut * peak^2
     */
    Double_t fBetaCut = 1.e-8;

  ClassDef(ATTPCPSAFastFit, 1)
};

#endif