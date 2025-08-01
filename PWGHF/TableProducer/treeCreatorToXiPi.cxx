// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

/// \file treeCreatorToXiPi.cxx
/// \brief Writer of the omegac0 or xic0 to Xi Pi candidates in the form of flat tables to be stored in TTrees.
///        In this file are defined and filled the output tables
///
/// \author Federica Zanone <federica.zanone@cern.ch>, Heidelberg University

#include "PWGHF/Core/CentralityEstimation.h"
#include "PWGHF/DataModel/CandidateReconstructionTables.h"
#include "PWGHF/DataModel/CandidateSelectionTables.h"

#include "Common/Core/RecoDecay.h"
#include "Common/DataModel/Centrality.h"
#include "Common/DataModel/EventSelection.h"
#include "Common/DataModel/TrackSelectionTables.h"

#include <Framework/ASoA.h>
#include <Framework/AnalysisDataModel.h>
#include <Framework/AnalysisHelpers.h>
#include <Framework/AnalysisTask.h>
#include <Framework/Configurable.h>
#include <Framework/InitContext.h>
#include <Framework/Logger.h>
#include <Framework/runDataProcessing.h>

#include <cstdint>

using namespace o2;
using namespace o2::framework;

namespace o2::aod
{
namespace full
{
// collision info
DECLARE_SOA_COLUMN(IsEventSel8, isEventSel8, bool);
DECLARE_SOA_COLUMN(IsEventSelZ, isEventSelZ, bool);
DECLARE_SOA_COLUMN(Centrality, centrality, float);
// from creator
DECLARE_SOA_COLUMN(XPv, xPv, float);
DECLARE_SOA_COLUMN(YPv, yPv, float);
DECLARE_SOA_COLUMN(ZPv, zPv, float);
DECLARE_SOA_COLUMN(XDecayVtxCharmBaryon, xDecayVtxCharmBaryon, float);
DECLARE_SOA_COLUMN(YDecayVtxCharmBaryon, yDecayVtxCharmBaryon, float);
DECLARE_SOA_COLUMN(ZDecayVtxCharmBaryon, zDecayVtxCharmBaryon, float);
DECLARE_SOA_COLUMN(XDecayVtxCascade, xDecayVtxCascade, float);
DECLARE_SOA_COLUMN(YDecayVtxCascade, yDecayVtxCascade, float);
DECLARE_SOA_COLUMN(ZDecayVtxCascade, zDecayVtxCascade, float);
DECLARE_SOA_COLUMN(XDecayVtxV0, xDecayVtxV0, float);
DECLARE_SOA_COLUMN(YDecayVtxV0, yDecayVtxV0, float);
DECLARE_SOA_COLUMN(ZDecayVtxV0, zDecayVtxV0, float);
DECLARE_SOA_COLUMN(SignDecay, signDecay, int8_t); // sign of pi <- xi
DECLARE_SOA_COLUMN(CovVtxCharmBaryonXX, covVtxCharmBaryonXX, float);
DECLARE_SOA_COLUMN(CovVtxCharmBaryonYY, covVtxCharmBaryonYY, float);
DECLARE_SOA_COLUMN(CovVtxCharmBaryonZZ, covVtxCharmBaryonZZ, float);
DECLARE_SOA_COLUMN(PxCharmBaryon, pxCharmBaryon, float);
DECLARE_SOA_COLUMN(PyCharmBaryon, pyCharmBaryon, float);
DECLARE_SOA_COLUMN(PzCharmBaryon, pzCharmBaryon, float);
DECLARE_SOA_COLUMN(PxCasc, pxCasc, float);
DECLARE_SOA_COLUMN(PyCasc, pyCasc, float);
DECLARE_SOA_COLUMN(PzCasc, pzCasc, float);
DECLARE_SOA_COLUMN(PxPiFromCharmBaryon, pxPiFromCharmBaryon, float);
DECLARE_SOA_COLUMN(PyPiFromCharmBaryon, pyPiFromCharmBaryon, float);
DECLARE_SOA_COLUMN(PzPiFromCharmBaryon, pzPiFromCharmBaryon, float);
DECLARE_SOA_COLUMN(PxLambda, pxLambda, float);
DECLARE_SOA_COLUMN(PyLambda, pyLambda, float);
DECLARE_SOA_COLUMN(PzLambda, pzLambda, float);
DECLARE_SOA_COLUMN(PxPiFromCasc, pxPiFromCasc, float);
DECLARE_SOA_COLUMN(PyPiFromCasc, pyPiFromCasc, float);
DECLARE_SOA_COLUMN(PzPiFromCasc, pzPiFromCasc, float);
DECLARE_SOA_COLUMN(PxPosV0Dau, pxPosV0Dau, float);
DECLARE_SOA_COLUMN(PyPosV0Dau, pyPosV0Dau, float);
DECLARE_SOA_COLUMN(PzPosV0Dau, pzPosV0Dau, float);
DECLARE_SOA_COLUMN(PxNegV0Dau, pxNegV0Dau, float);
DECLARE_SOA_COLUMN(PyNegV0Dau, pyNegV0Dau, float);
DECLARE_SOA_COLUMN(PzNegV0Dau, pzNegV0Dau, float);
DECLARE_SOA_COLUMN(ImpactParCascXY, impactParCascXY, float);
DECLARE_SOA_COLUMN(ImpactParPiFromCharmBaryonXY, impactParPiFromCharmBaryonXY, float);
DECLARE_SOA_COLUMN(ImpactParCascZ, impactParCascZ, float);
DECLARE_SOA_COLUMN(ImpactParPiFromCharmBaryonZ, impactParPiFromCharmBaryonZ, float);
DECLARE_SOA_COLUMN(ErrImpactParCascXY, errImpactParCascXY, float);
DECLARE_SOA_COLUMN(ErrImpactParPiFromCharmBaryonXY, errImpactParPiFromCharmBaryonXY, float);
DECLARE_SOA_COLUMN(InvMassLambda, invMassLambda, float);
DECLARE_SOA_COLUMN(InvMassCascade, invMassCascade, float);
DECLARE_SOA_COLUMN(InvMassCharmBaryon, invMassCharmBaryon, float);
DECLARE_SOA_COLUMN(CosPAV0, cosPAV0, float);
DECLARE_SOA_COLUMN(CosPACharmBaryon, cosPACharmBaryon, float);
DECLARE_SOA_COLUMN(CosPACasc, cosPACasc, float);
DECLARE_SOA_COLUMN(CosPAXYV0, cosPAXYV0, float);
DECLARE_SOA_COLUMN(CosPAXYCharmBaryon, cosPAXYCharmBaryon, float);
DECLARE_SOA_COLUMN(CosPAXYCasc, cosPAXYCasc, float);
DECLARE_SOA_COLUMN(CTauOmegac, cTauOmegac, float);
DECLARE_SOA_COLUMN(CTauCascade, cTauCascade, float);
DECLARE_SOA_COLUMN(CTauV0, cTauV0, float);
DECLARE_SOA_COLUMN(CTauXic, cTauXic, float);
DECLARE_SOA_COLUMN(EtaV0PosDau, etaV0PosDau, float);
DECLARE_SOA_COLUMN(EtaV0NegDau, etaV0NegDau, float);
DECLARE_SOA_COLUMN(EtaPiFromCasc, etaPiFromCasc, float);
DECLARE_SOA_COLUMN(EtaPiFromCharmBaryon, etaPiFromCharmBaryon, float);
DECLARE_SOA_COLUMN(EtaCharmBaryon, etaCharmBaryon, float);
DECLARE_SOA_COLUMN(EtaCascade, etaCascade, float);
DECLARE_SOA_COLUMN(EtaV0, etaV0, float);
DECLARE_SOA_COLUMN(DcaXYToPvV0Dau0, dcaXYToPvV0Dau0, float);
DECLARE_SOA_COLUMN(DcaXYToPvV0Dau1, dcaXYToPvV0Dau1, float);
DECLARE_SOA_COLUMN(DcaXYToPvCascDau, dcaXYToPvCascDau, float);
DECLARE_SOA_COLUMN(DcaZToPvV0Dau0, dcaZToPvV0Dau0, float);
DECLARE_SOA_COLUMN(DcaZToPvV0Dau1, dcaZToPvV0Dau1, float);
DECLARE_SOA_COLUMN(DcaZToPvCascDau, dcaZToPvCascDau, float);
DECLARE_SOA_COLUMN(DcaCascDau, dcaCascDau, float);
DECLARE_SOA_COLUMN(DcaV0Dau, dcaV0Dau, float);
DECLARE_SOA_COLUMN(DcaCharmBaryonDau, dcaCharmBaryonDau, float);
DECLARE_SOA_COLUMN(DecLenCharmBaryon, decLenCharmBaryon, float);
DECLARE_SOA_COLUMN(DecLenCascade, decLenCascade, float);
DECLARE_SOA_COLUMN(DecLenV0, decLenV0, float);
DECLARE_SOA_COLUMN(ErrorDecayLengthCharmBaryon, errorDecayLengthCharmBaryon, float);
DECLARE_SOA_COLUMN(ErrorDecayLengthXYCharmBaryon, errorDecayLengthXYCharmBaryon, float);
DECLARE_SOA_COLUMN(NormImpParCascade, normImpParCascade, double);
DECLARE_SOA_COLUMN(NormImpParPiFromCharmBar, normImpParPiFromCharmBar, double);
DECLARE_SOA_COLUMN(NormDecayLenCharmBar, normDecayLenCharmBar, double);
DECLARE_SOA_COLUMN(IsPionGlbTrkWoDca, isPionGlbTrkWoDca, bool);
DECLARE_SOA_COLUMN(PionItsNCls, pionItsNCls, uint8_t);
DECLARE_SOA_COLUMN(NTpcRowsPion, nTpcRowsPion, int16_t);
DECLARE_SOA_COLUMN(NTpcRowsPiFromCasc, nTpcRowsPiFromCasc, int16_t);
DECLARE_SOA_COLUMN(NTpcRowsPosV0Dau, nTpcRowsPosV0Dau, int16_t);
DECLARE_SOA_COLUMN(NTpcRowsNegV0Dau, nTpcRowsNegV0Dau, int16_t);
// from creator - MC
DECLARE_SOA_COLUMN(FlagMcMatchRec, flagMcMatchRec, int8_t); // reconstruction level
DECLARE_SOA_COLUMN(DebugMcRec, debugMcRec, int8_t);         // debug flag for mis-association reconstruction level
DECLARE_SOA_COLUMN(OriginMcRec, originMcRec, int8_t);
DECLARE_SOA_COLUMN(CollisionMatched, collisionMatched, bool);
// from selector
DECLARE_SOA_COLUMN(StatusPidLambda, statusPidLambda, bool);
DECLARE_SOA_COLUMN(StatusPidCascade, statusPidCascade, bool);
DECLARE_SOA_COLUMN(StatusPidCharmBaryon, statusPidCharmBaryon, bool);
DECLARE_SOA_COLUMN(StatusInvMassLambda, statusInvMassLambda, bool);
DECLARE_SOA_COLUMN(StatusInvMassCascade, statusInvMassCascade, bool);
DECLARE_SOA_COLUMN(StatusInvMassCharmBaryon, statusInvMassCharmBaryon, bool);
DECLARE_SOA_COLUMN(ResultSelections, resultSelections, bool);
DECLARE_SOA_COLUMN(PidTpcInfoStored, pidTpcInfoStored, int);
DECLARE_SOA_COLUMN(PidTofInfoStored, pidTofInfoStored, int);
DECLARE_SOA_COLUMN(TpcNSigmaPiFromCharmBaryon, tpcNSigmaPiFromCharmBaryon, float);
DECLARE_SOA_COLUMN(TpcNSigmaPiFromCasc, tpcNSigmaPiFromCasc, float);
DECLARE_SOA_COLUMN(TpcNSigmaPiFromLambda, tpcNSigmaPiFromLambda, float);
DECLARE_SOA_COLUMN(TpcNSigmaPrFromLambda, tpcNSigmaPrFromLambda, float);
DECLARE_SOA_COLUMN(TofNSigmaPiFromCharmBaryon, tofNSigmaPiFromCharmBaryon, float);
DECLARE_SOA_COLUMN(TofNSigmaPiFromCasc, tofNSigmaPiFromCasc, float);
DECLARE_SOA_COLUMN(TofNSigmaPiFromLambda, tofNSigmaPiFromLambda, float);
DECLARE_SOA_COLUMN(TofNSigmaPrFromLambda, tofNSigmaPrFromLambda, float);

} // namespace full

DECLARE_SOA_TABLE(HfToXiPiEvs, "AOD", "HFTOXIPIEV",
                  full::IsEventSel8, full::IsEventSelZ);

DECLARE_SOA_TABLE(HfToXiPiFulls, "AOD", "HFTOXIPIFULL",
                  full::XPv, full::YPv, full::ZPv, full::Centrality, collision::NumContrib, collision::Chi2,
                  full::XDecayVtxCharmBaryon, full::YDecayVtxCharmBaryon, full::ZDecayVtxCharmBaryon,
                  full::XDecayVtxCascade, full::YDecayVtxCascade, full::ZDecayVtxCascade,
                  full::XDecayVtxV0, full::YDecayVtxV0, full::ZDecayVtxV0,
                  full::SignDecay,
                  full::CovVtxCharmBaryonXX, full::CovVtxCharmBaryonYY, full::CovVtxCharmBaryonZZ,
                  full::PxCharmBaryon, full::PyCharmBaryon, full::PzCharmBaryon,
                  full::PxCasc, full::PyCasc, full::PzCasc,
                  full::PxPiFromCharmBaryon, full::PyPiFromCharmBaryon, full::PzPiFromCharmBaryon,
                  full::PxLambda, full::PyLambda, full::PzLambda,
                  full::PxPiFromCasc, full::PyPiFromCasc, full::PzPiFromCasc,
                  full::PxPosV0Dau, full::PyPosV0Dau, full::PzPosV0Dau,
                  full::PxNegV0Dau, full::PyNegV0Dau, full::PzNegV0Dau,
                  full::ImpactParCascXY, full::ImpactParPiFromCharmBaryonXY,
                  full::ImpactParCascZ, full::ImpactParPiFromCharmBaryonZ,
                  full::ErrImpactParCascXY, full::ErrImpactParPiFromCharmBaryonXY,
                  full::InvMassLambda, full::InvMassCascade, full::InvMassCharmBaryon,
                  full::CosPAV0, full::CosPACharmBaryon, full::CosPACasc, full::CosPAXYV0, full::CosPAXYCharmBaryon, full::CosPAXYCasc,
                  full::CTauOmegac, full::CTauCascade, full::CTauV0, full::CTauXic,
                  full::EtaV0PosDau, full::EtaV0NegDau, full::EtaPiFromCasc, full::EtaPiFromCharmBaryon,
                  full::EtaCharmBaryon, full::EtaCascade, full::EtaV0,
                  full::DcaXYToPvV0Dau0, full::DcaXYToPvV0Dau1, full::DcaXYToPvCascDau,
                  full::DcaZToPvV0Dau0, full::DcaZToPvV0Dau1, full::DcaZToPvCascDau,
                  full::DcaCascDau, full::DcaV0Dau, full::DcaCharmBaryonDau,
                  full::DecLenCharmBaryon, full::DecLenCascade, full::DecLenV0, full::ErrorDecayLengthCharmBaryon, full::ErrorDecayLengthXYCharmBaryon,
                  full::NormImpParCascade, full::NormImpParPiFromCharmBar, full::NormDecayLenCharmBar, full::IsPionGlbTrkWoDca, full::PionItsNCls,
                  full::NTpcRowsPion, full::NTpcRowsPiFromCasc, full::NTpcRowsPosV0Dau, full::NTpcRowsNegV0Dau,
                  full::StatusPidLambda, full::StatusPidCascade, full::StatusPidCharmBaryon,
                  full::StatusInvMassLambda, full::StatusInvMassCascade, full::StatusInvMassCharmBaryon, full::ResultSelections, full::PidTpcInfoStored, full::PidTofInfoStored,
                  full::TpcNSigmaPiFromCharmBaryon, full::TpcNSigmaPiFromCasc, full::TpcNSigmaPiFromLambda, full::TpcNSigmaPrFromLambda,
                  full::TofNSigmaPiFromCharmBaryon, full::TofNSigmaPiFromCasc, full::TofNSigmaPiFromLambda, full::TofNSigmaPrFromLambda,
                  full::FlagMcMatchRec, full::DebugMcRec, full::OriginMcRec, full::CollisionMatched);

DECLARE_SOA_TABLE(HfToXiPiLites, "AOD", "HFTOXIPILITE",
                  full::XPv, full::YPv, full::ZPv, full::Centrality, collision::NumContrib, collision::Chi2,
                  full::XDecayVtxCharmBaryon, full::YDecayVtxCharmBaryon, full::ZDecayVtxCharmBaryon,
                  full::XDecayVtxCascade, full::YDecayVtxCascade, full::ZDecayVtxCascade,
                  full::XDecayVtxV0, full::YDecayVtxV0, full::ZDecayVtxV0,
                  full::SignDecay,
                  full::PxCharmBaryon, full::PyCharmBaryon, full::PzCharmBaryon,
                  full::PxPiFromCharmBaryon, full::PyPiFromCharmBaryon, full::PzPiFromCharmBaryon,
                  full::PxPiFromCasc, full::PyPiFromCasc, full::PzPiFromCasc,
                  full::PxPosV0Dau, full::PyPosV0Dau, full::PzPosV0Dau,
                  full::PxNegV0Dau, full::PyNegV0Dau, full::PzNegV0Dau,
                  full::ImpactParCascXY, full::ImpactParPiFromCharmBaryonXY,
                  full::ErrImpactParCascXY, full::ErrImpactParPiFromCharmBaryonXY,
                  full::InvMassLambda, full::InvMassCascade, full::InvMassCharmBaryon,
                  full::EtaV0PosDau, full::EtaV0NegDau, full::EtaPiFromCasc, full::EtaPiFromCharmBaryon,
                  full::DcaXYToPvV0Dau0, full::DcaXYToPvV0Dau1, full::DcaXYToPvCascDau,
                  full::DcaCascDau, full::DcaV0Dau, full::DcaCharmBaryonDau,
                  full::ErrorDecayLengthCharmBaryon, full::NormImpParCascade, full::NormImpParPiFromCharmBar,
                  full::IsPionGlbTrkWoDca, full::PionItsNCls,
                  full::NTpcRowsPion, full::NTpcRowsPiFromCasc, full::NTpcRowsPosV0Dau, full::NTpcRowsNegV0Dau,
                  full::PidTpcInfoStored, full::PidTofInfoStored,
                  full::TpcNSigmaPiFromCharmBaryon, full::TpcNSigmaPiFromCasc, full::TpcNSigmaPiFromLambda, full::TpcNSigmaPrFromLambda,
                  full::TofNSigmaPiFromCharmBaryon, full::TofNSigmaPiFromCasc, full::TofNSigmaPiFromLambda, full::TofNSigmaPrFromLambda,
                  full::FlagMcMatchRec, full::OriginMcRec, full::CollisionMatched);

} // namespace o2::aod

/// Writes the full information in an output TTree
struct HfTreeCreatorToXiPi {

  Produces<o2::aod::HfToXiPiFulls> rowCandidateFull;
  Produces<o2::aod::HfToXiPiLites> rowCandidateLite;
  Produces<o2::aod::HfToXiPiEvs> rowEv;

  Configurable<float> zPvCut{"zPvCut", 10., "Cut on absolute value of primary vertex z coordinate"};

  using Cents = soa::Join<aod::CentFV0As, aod::CentFT0Ms, aod::CentFT0As, aod::CentFT0Cs, aod::CentFDDMs>;
  using MyTrackTable = soa::Join<aod::Tracks, aod::TrackSelection, aod::TracksExtra>;
  using MyEventTable = soa::Join<aod::Collisions, aod::EvSels>;
  using MyEventTableWithFT0C = soa::Join<aod::Collisions, aod::EvSels, aod::CentFT0Cs>;
  using MyEventTableWithFT0M = soa::Join<aod::Collisions, aod::EvSels, aod::CentFT0Ms>;
  using MyEventTableWithNTracksPV = soa::Join<aod::Collisions, aod::EvSels, aod::CentNTPVs>;

  void init(InitContext const&)
  {
    if ((doprocessMcLiteXic0 && doprocessMcLiteOmegac0) || (doprocessMcFullXic0 && doprocessMcFullOmegac0)) {
      LOGF(fatal, "Both Xic0 and Omegac0 MC processes enabled, please choose ONLY one!");
    }
  }

  template <bool useCentrality, typename T>
  void fillEvent(const T& collision, float cutZPv)
  {
    rowEv(
      collision.sel8(), std::abs(collision.posZ()) < cutZPv);
  }

  template <bool useCentrality, typename MyEventTableType, typename T>
  void fillCandidate(const T& candidate, int8_t flagMc, int8_t debugMc, int8_t originMc, bool collisionMatched)
  {

    float centrality = -999.f;
    if constexpr (useCentrality) {
      auto const& collision = candidate.template collision_as<MyEventTableType>();
      centrality = o2::hf_centrality::getCentralityColl(collision);
    }

    rowCandidateFull(
      candidate.xPv(),
      candidate.yPv(),
      candidate.zPv(),
      centrality,
      candidate.template collision_as<MyEventTableType>().numContrib(),
      candidate.template collision_as<MyEventTableType>().chi2(),
      candidate.xDecayVtxCharmBaryon(),
      candidate.yDecayVtxCharmBaryon(),
      candidate.zDecayVtxCharmBaryon(),
      candidate.xDecayVtxCascade(),
      candidate.yDecayVtxCascade(),
      candidate.zDecayVtxCascade(),
      candidate.xDecayVtxV0(),
      candidate.yDecayVtxV0(),
      candidate.zDecayVtxV0(),
      candidate.signDecay(),
      candidate.covVtxCharmBaryon0(),
      candidate.covVtxCharmBaryon3(),
      candidate.covVtxCharmBaryon5(),
      candidate.pxCharmBaryon(),
      candidate.pyCharmBaryon(),
      candidate.pzCharmBaryon(),
      candidate.pxCasc(),
      candidate.pyCasc(),
      candidate.pzCasc(),
      candidate.pxBachFromCharmBaryon(),
      candidate.pyBachFromCharmBaryon(),
      candidate.pzBachFromCharmBaryon(),
      candidate.pxLambda(),
      candidate.pyLambda(),
      candidate.pzLambda(),
      candidate.pxBachFromCasc(),
      candidate.pyBachFromCasc(),
      candidate.pzBachFromCasc(),
      candidate.pxPosV0Dau(),
      candidate.pyPosV0Dau(),
      candidate.pzPosV0Dau(),
      candidate.pxNegV0Dau(),
      candidate.pyNegV0Dau(),
      candidate.pzNegV0Dau(),
      candidate.impactParCascXY(),
      candidate.impactParBachFromCharmBaryonXY(),
      candidate.impactParCascZ(),
      candidate.impactParBachFromCharmBaryonZ(),
      candidate.errImpactParCascXY(),
      candidate.errImpactParBachFromCharmBaryonXY(),
      candidate.invMassLambda(),
      candidate.invMassCascade(),
      candidate.invMassCharmBaryon(),
      candidate.cosPAV0(),
      candidate.cosPACharmBaryon(),
      candidate.cosPACasc(),
      candidate.cosPAXYV0(),
      candidate.cosPAXYCharmBaryon(),
      candidate.cosPAXYCasc(),
      candidate.cTauOmegac(),
      candidate.cTauCascade(),
      candidate.cTauV0(),
      candidate.cTauXic(),
      candidate.etaV0PosDau(),
      candidate.etaV0NegDau(),
      candidate.etaBachFromCasc(),
      candidate.etaBachFromCharmBaryon(),
      candidate.etaCharmBaryon(),
      candidate.etaCascade(),
      candidate.etaV0(),
      candidate.dcaXYToPvV0Dau0(),
      candidate.dcaXYToPvV0Dau1(),
      candidate.dcaXYToPvCascDau(),
      candidate.dcaZToPvV0Dau0(),
      candidate.dcaZToPvV0Dau1(),
      candidate.dcaZToPvCascDau(),
      candidate.dcaCascDau(),
      candidate.dcaV0Dau(),
      candidate.dcaCharmBaryonDau(),
      candidate.decLenCharmBaryon(),
      candidate.decLenCascade(),
      candidate.decLenV0(),
      candidate.errorDecayLengthCharmBaryon(),
      candidate.errorDecayLengthXYCharmBaryon(),
      candidate.impactParCascXY() / candidate.errImpactParCascXY(),
      candidate.impactParBachFromCharmBaryonXY() / candidate.errImpactParBachFromCharmBaryonXY(),
      candidate.decLenCharmBaryon() / candidate.errorDecayLengthCharmBaryon(),
      candidate.template bachelorFromCharmBaryon_as<MyTrackTable>().isGlobalTrackWoDCA(),
      candidate.template bachelorFromCharmBaryon_as<MyTrackTable>().itsNCls(),
      candidate.template bachelorFromCharmBaryon_as<MyTrackTable>().tpcNClsCrossedRows(),
      candidate.template bachelor_as<MyTrackTable>().tpcNClsCrossedRows(),
      candidate.template posTrack_as<MyTrackTable>().tpcNClsCrossedRows(),
      candidate.template negTrack_as<MyTrackTable>().tpcNClsCrossedRows(),
      candidate.statusPidLambda(),
      candidate.statusPidCascade(),
      candidate.statusPidCharmBaryon(),
      candidate.statusInvMassLambda(),
      candidate.statusInvMassCascade(),
      candidate.statusInvMassCharmBaryon(),
      candidate.resultSelections(),
      candidate.pidTpcInfoStored(),
      candidate.pidTofInfoStored(),
      candidate.tpcNSigmaPiFromCharmBaryon(),
      candidate.tpcNSigmaPiFromCasc(),
      candidate.tpcNSigmaPiFromLambda(),
      candidate.tpcNSigmaPrFromLambda(),
      candidate.tofNSigmaPiFromCharmBaryon(),
      candidate.tofNSigmaPiFromCasc(),
      candidate.tofNSigmaPiFromLambda(),
      candidate.tofNSigmaPrFromLambda(),
      flagMc,
      debugMc,
      originMc,
      collisionMatched);
  }

  template <bool useCentrality, typename MyEventTableType, typename T>
  void fillCandidateLite(const T& candidate, int8_t flagMc, int8_t originMc, bool collisionMatched)
  {
    if (candidate.resultSelections() && candidate.statusPidCharmBaryon() && candidate.statusInvMassLambda() && candidate.statusInvMassCascade() && candidate.statusInvMassCharmBaryon()) {

      float centrality = -999.f;
      if constexpr (useCentrality) {
        auto const& collision = candidate.template collision_as<MyEventTableType>();
        centrality = o2::hf_centrality::getCentralityColl(collision);
      }

      rowCandidateLite(
        candidate.xPv(),
        candidate.yPv(),
        candidate.zPv(),
        centrality,
        candidate.template collision_as<MyEventTableType>().numContrib(),
        candidate.template collision_as<MyEventTableType>().chi2(),
        candidate.xDecayVtxCharmBaryon(),
        candidate.yDecayVtxCharmBaryon(),
        candidate.zDecayVtxCharmBaryon(),
        candidate.xDecayVtxCascade(),
        candidate.yDecayVtxCascade(),
        candidate.zDecayVtxCascade(),
        candidate.xDecayVtxV0(),
        candidate.yDecayVtxV0(),
        candidate.zDecayVtxV0(),
        candidate.signDecay(),
        candidate.pxCharmBaryon(),
        candidate.pyCharmBaryon(),
        candidate.pzCharmBaryon(),
        candidate.pxBachFromCharmBaryon(),
        candidate.pyBachFromCharmBaryon(),
        candidate.pzBachFromCharmBaryon(),
        candidate.pxBachFromCasc(),
        candidate.pyBachFromCasc(),
        candidate.pzBachFromCasc(),
        candidate.pxPosV0Dau(),
        candidate.pyPosV0Dau(),
        candidate.pzPosV0Dau(),
        candidate.pxNegV0Dau(),
        candidate.pyNegV0Dau(),
        candidate.pzNegV0Dau(),
        candidate.impactParCascXY(),
        candidate.impactParBachFromCharmBaryonXY(),
        candidate.errImpactParCascXY(),
        candidate.errImpactParBachFromCharmBaryonXY(),
        candidate.invMassLambda(),
        candidate.invMassCascade(),
        candidate.invMassCharmBaryon(),
        candidate.etaV0PosDau(),
        candidate.etaV0NegDau(),
        candidate.etaBachFromCasc(),
        candidate.etaBachFromCharmBaryon(),
        candidate.dcaXYToPvV0Dau0(),
        candidate.dcaXYToPvV0Dau1(),
        candidate.dcaXYToPvCascDau(),
        candidate.dcaCascDau(),
        candidate.dcaV0Dau(),
        candidate.dcaCharmBaryonDau(),
        candidate.errorDecayLengthCharmBaryon(),
        candidate.impactParCascXY() / candidate.errImpactParCascXY(),
        candidate.impactParBachFromCharmBaryonXY() / candidate.errImpactParBachFromCharmBaryonXY(),
        candidate.template bachelorFromCharmBaryon_as<MyTrackTable>().isGlobalTrackWoDCA(),
        candidate.template bachelorFromCharmBaryon_as<MyTrackTable>().itsNCls(),
        candidate.template bachelorFromCharmBaryon_as<MyTrackTable>().tpcNClsCrossedRows(),
        candidate.template bachelor_as<MyTrackTable>().tpcNClsCrossedRows(),
        candidate.template posTrack_as<MyTrackTable>().tpcNClsCrossedRows(),
        candidate.template negTrack_as<MyTrackTable>().tpcNClsCrossedRows(),
        candidate.pidTpcInfoStored(),
        candidate.pidTofInfoStored(),
        candidate.tpcNSigmaPiFromCharmBaryon(),
        candidate.tpcNSigmaPiFromCasc(),
        candidate.tpcNSigmaPiFromLambda(),
        candidate.tpcNSigmaPrFromLambda(),
        candidate.tofNSigmaPiFromCharmBaryon(),
        candidate.tofNSigmaPiFromCasc(),
        candidate.tofNSigmaPiFromLambda(),
        candidate.tofNSigmaPrFromLambda(),
        flagMc,
        originMc,
        collisionMatched);
    }
  }

  void processDataFull(MyEventTable const& collisions, MyTrackTable const&,
                       soa::Join<aod::HfCandToXiPi, aod::HfSelToXiPi> const& candidates)
  {
    // Filling event properties
    rowEv.reserve(collisions.size());
    for (const auto& collision : collisions) {
      fillEvent<false>(collision, zPvCut);
    }

    // Filling candidate properties
    rowCandidateFull.reserve(candidates.size());
    for (const auto& candidate : candidates) {
      fillCandidate<false, MyEventTable>(candidate, -7, -7, RecoDecay::OriginType::None, false);
    }
  }
  PROCESS_SWITCH(HfTreeCreatorToXiPi, processDataFull, "Process data with full information w/o centrality", true);

  void processMcFullXic0(MyEventTable const& collisions, MyTrackTable const&,
                         soa::Join<aod::HfCandToXiPi, aod::HfSelToXiPi, aod::HfXicToXiPiMCRec> const& candidates)
  {
    // Filling event properties
    rowEv.reserve(collisions.size());
    for (const auto& collision : collisions) {
      fillEvent<false>(collision, zPvCut);
    }

    // Filling candidate properties
    rowCandidateFull.reserve(candidates.size());
    for (const auto& candidate : candidates) {
      fillCandidate<false, MyEventTable>(candidate, candidate.flagMcMatchRec(), candidate.debugMcRec(), candidate.originMcRec(), candidate.collisionMatched());
    }
  }
  PROCESS_SWITCH(HfTreeCreatorToXiPi, processMcFullXic0, "Process MC with full information for xic0 w/o centrality", false);

  void processMcFullOmegac0(MyEventTable const& collisions, MyTrackTable const&,
                            soa::Join<aod::HfCandToXiPi, aod::HfSelToXiPi, aod::HfOmegacToXiPiMCRec> const& candidates)
  {
    // Filling event properties
    rowEv.reserve(collisions.size());
    for (const auto& collision : collisions) {
      fillEvent<false>(collision, zPvCut);
    }

    // Filling candidate properties
    rowCandidateFull.reserve(candidates.size());
    for (const auto& candidate : candidates) {
      fillCandidate<false, MyEventTable>(candidate, candidate.flagMcMatchRec(), candidate.debugMcRec(), candidate.originMcRec(), candidate.collisionMatched());
    }
  }
  PROCESS_SWITCH(HfTreeCreatorToXiPi, processMcFullOmegac0, "Process MC with full information for omegac0", false);

  void processDataLite(MyEventTable const& collisions, MyTrackTable const&,
                       soa::Join<aod::HfCandToXiPi, aod::HfSelToXiPi> const& candidates)
  {
    // Filling event properties
    rowEv.reserve(collisions.size());
    for (const auto& collision : collisions) {
      fillEvent<false>(collision, zPvCut);
    }

    // Filling candidate properties
    rowCandidateLite.reserve(candidates.size());
    for (const auto& candidate : candidates) {
      fillCandidateLite<false, MyEventTable>(candidate, -7, RecoDecay::OriginType::None, false);
    }
  }
  PROCESS_SWITCH(HfTreeCreatorToXiPi, processDataLite, "Process data and produce lite table version", false);

  void processDataLiteWithFT0M(MyEventTableWithFT0M const& collisions, MyTrackTable const&,
                               soa::Join<aod::HfCandToXiPi, aod::HfSelToXiPi> const& candidates)
  {
    // Filling event properties
    rowEv.reserve(collisions.size());
    for (const auto& collision : collisions) {
      fillEvent<true>(collision, zPvCut);
    }

    // Filling candidate properties
    rowCandidateLite.reserve(candidates.size());
    for (const auto& candidate : candidates) {
      fillCandidateLite<true, MyEventTableWithFT0M>(candidate, -7, RecoDecay::OriginType::None, false);
    }
  }
  PROCESS_SWITCH(HfTreeCreatorToXiPi, processDataLiteWithFT0M, "Process data and produce lite table version with FT0M", false);

  void processDataLiteWithFT0C(MyEventTableWithFT0C const& collisions, MyTrackTable const&,
                               soa::Join<aod::HfCandToXiPi, aod::HfSelToXiPi> const& candidates)
  {
    // Filling event properties
    rowEv.reserve(collisions.size());
    for (const auto& collision : collisions) {
      fillEvent<true>(collision, zPvCut);
    }

    // Filling candidate properties
    rowCandidateLite.reserve(candidates.size());
    for (const auto& candidate : candidates) {
      fillCandidateLite<true, MyEventTableWithFT0C>(candidate, -7, RecoDecay::OriginType::None, false);
    }
  }
  PROCESS_SWITCH(HfTreeCreatorToXiPi, processDataLiteWithFT0C, "Process data and produce lite table version with FT0C", false);

  void processDataLiteWithNTracksPV(MyEventTableWithNTracksPV const& collisions, MyTrackTable const&,
                                    soa::Join<aod::HfCandToXiPi, aod::HfSelToXiPi> const& candidates)
  {
    // Filling event properties
    rowEv.reserve(collisions.size());
    for (const auto& collision : collisions) {
      fillEvent<true>(collision, zPvCut);
    }

    // Filling candidate properties
    rowCandidateLite.reserve(candidates.size());
    for (const auto& candidate : candidates) {
      fillCandidateLite<true, MyEventTableWithNTracksPV>(candidate, -7, RecoDecay::OriginType::None, false);
    }
  }
  PROCESS_SWITCH(HfTreeCreatorToXiPi, processDataLiteWithNTracksPV, "Process data and produce lite table version with NTracksPV", false);

  void processMcLiteXic0(MyEventTable const& collisions, MyTrackTable const&,
                         soa::Join<aod::HfCandToXiPi, aod::HfSelToXiPi, aod::HfXicToXiPiMCRec> const& candidates)
  {
    // Filling event properties
    rowEv.reserve(collisions.size());
    for (const auto& collision : collisions) {
      fillEvent<false>(collision, zPvCut);
    }

    // Filling candidate properties
    rowCandidateLite.reserve(candidates.size());
    for (const auto& candidate : candidates) {
      fillCandidateLite<false, MyEventTable>(candidate, candidate.flagMcMatchRec(), candidate.originMcRec(), candidate.collisionMatched());
    }
  }
  PROCESS_SWITCH(HfTreeCreatorToXiPi, processMcLiteXic0, "Process MC and produce lite table version for xic0", false);

  void processMcLiteXic0WithFT0C(MyEventTableWithFT0C const& collisions, MyTrackTable const&,
                                 soa::Join<aod::HfCandToXiPi, aod::HfSelToXiPi, aod::HfXicToXiPiMCRec> const& candidates)
  {
    // Filling event properties
    rowEv.reserve(collisions.size());
    for (const auto& collision : collisions) {
      fillEvent<true>(collision, zPvCut);
    }

    // Filling candidate properties
    rowCandidateLite.reserve(candidates.size());
    for (const auto& candidate : candidates) {
      fillCandidateLite<true, MyEventTableWithFT0C>(candidate, candidate.flagMcMatchRec(), candidate.originMcRec(), candidate.collisionMatched());
    }
  }
  PROCESS_SWITCH(HfTreeCreatorToXiPi, processMcLiteXic0WithFT0C, "Process MC and produce lite table version for Xic0 with FT0C", false);

  void processMcLiteXic0WithFT0M(MyEventTableWithFT0M const& collisions, MyTrackTable const&,
                                 soa::Join<aod::HfCandToXiPi, aod::HfSelToXiPi, aod::HfXicToXiPiMCRec> const& candidates)
  {
    // Filling event properties
    rowEv.reserve(collisions.size());
    for (const auto& collision : collisions) {
      fillEvent<true>(collision, zPvCut);
    }

    // Filling candidate properties
    rowCandidateLite.reserve(candidates.size());
    for (const auto& candidate : candidates) {
      fillCandidateLite<true, MyEventTableWithFT0M>(candidate, candidate.flagMcMatchRec(), candidate.originMcRec(), candidate.collisionMatched());
    }
  }
  PROCESS_SWITCH(HfTreeCreatorToXiPi, processMcLiteXic0WithFT0M, "Process MC and produce lite table version for Xic0 with FT0M", false);

  void processMcLiteXic0WithNTracksPV(MyEventTableWithNTracksPV const& collisions, MyTrackTable const&,
                                      soa::Join<aod::HfCandToXiPi, aod::HfSelToXiPi, aod::HfXicToXiPiMCRec> const& candidates)
  {
    // Filling event properties
    rowEv.reserve(collisions.size());
    for (const auto& collision : collisions) {
      fillEvent<true>(collision, zPvCut);
    }

    // Filling candidate properties
    rowCandidateLite.reserve(candidates.size());
    for (const auto& candidate : candidates) {
      fillCandidateLite<true, MyEventTableWithNTracksPV>(candidate, candidate.flagMcMatchRec(), candidate.originMcRec(), candidate.collisionMatched());
    }
  }
  PROCESS_SWITCH(HfTreeCreatorToXiPi, processMcLiteXic0WithNTracksPV, "Process MC and produce lite table version for Xic0 with NTracksPV", false);

  void processMcLiteOmegac0(MyEventTable const& collisions, MyTrackTable const&,
                            soa::Join<aod::HfCandToXiPi, aod::HfSelToXiPi, aod::HfOmegacToXiPiMCRec> const& candidates)
  {
    // Filling event properties
    rowEv.reserve(collisions.size());
    for (const auto& collision : collisions) {
      fillEvent<false>(collision, zPvCut);
    }

    // Filling candidate properties
    rowCandidateLite.reserve(candidates.size());
    for (const auto& candidate : candidates) {
      fillCandidateLite<false, MyEventTable>(candidate, candidate.flagMcMatchRec(), candidate.originMcRec(), candidate.collisionMatched());
    }
  }
  PROCESS_SWITCH(HfTreeCreatorToXiPi, processMcLiteOmegac0, "Process MC and produce lite table version for omegac0", false);

}; // end of struct

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<HfTreeCreatorToXiPi>(cfgc)};
}
