#ifndef TIDE_MATCH_SET_H
#define TIDE_MATCH_SET_H

#define  BOOST_DATE_TIME_NO_LIB
#include <boost/thread.hpp>
#include <vector>
#include "raw_proteins.pb.h"
#include "tide/records.h"
#include "tide/active_peptide_queue.h"  // no include guard
#include "tide/fixed_cap_array.h"
#include "tide/peptide.h"
#include "tide/sp_scorer.h"
#include "tide/spectrum_collection.h"

#include "model/Modification.h"
#include "model/PostProcessProtein.h"

using namespace std;

typedef vector<const pb::Protein*> ProteinVec;

class TideMatchSet {

 public:
  bool exact_pval_search_;
  int elution_window_;
  SCORE_FUNCTION_T cur_score_function_;
  double max_mz_;

  typedef pair<int, int> Pair2;
  typedef FixedCapacityArray<Pair2> Arr2;

//  typedef pair<pair<double, double>, int> Pair;   //store results for exact_pval calculations
//  typedef FixedCapacityArray<Pair> Arr;

  struct Scores {
    double xcorr_score;
    double xcorr_pval;
    int resEv_score;
    double resEv_pval;
    double combinedPval;
    int rank;
    double tailor;  //Added by AKF   
    int by_ion_matched; 
    int by_ion_total;     

    /*// added by Yang
    double prec_intensity_logrank;
    double rt_diff;
    double frag_pval;
    double prec_frag_coelu;
    double ensemble_score;*/
  };
  typedef FixedCapacityArray<Scores> Arr;

  // Matches will be an array of pairs, (score, counter), where counter refers
  // to the index within the ActivePeptideQueue, counting from the back.  This
  // slight complication is due to the way the generated machine code fills the
  // counter in the matches buffer by decrementing the counter.
  TideMatchSet(
    Arr* matches,
    double max_mz
  );
  TideMatchSet(
    Peptide* peptide,
    double max_mz
  );

  ~TideMatchSet();

    /**
   * Write peptide centric matches to output files
   */
  void report(
    ofstream* target_file,  ///< target file to write to
    ofstream* decoy_file, ///< decoy file to write to
    int top_matches,
    const ActivePeptideQueue* peptides, ///< peptide queue
    const ProteinVec& proteins, ///< proteins corresponding with peptides
    bool compute_sp ///< whether to compute sp or not
  );

  /**
   * Write spectrum centric to output files
   */
  void report(
    ofstream* target_file,  ///< target file to write to
    ofstream* decoy_file, ///< decoy file to write to
    int top_n,  ///< number of matches to report
    int decoys_per_target,
    const string& spectrum_filename, ///< name of spectrum file
    const Spectrum* spectrum, ///< spectrum for matches
    int charge, ///< charge for matches
    const ActivePeptideQueue* peptides, ///< peptide queue
    const ProteinVec& proteins, ///< proteins corresponding with peptides
    bool compute_sp, ///< whether to compute sp or not
    bool highScoreBest, //< indicates semantics of score magnitude
    boost::mutex * rwlock = NULL
  );

  static void colPrint(
    bool* printTab,
    ofstream* file,
    const char* myString
  );
 
  static void writeHeaders(
    ofstream* file,
    bool decoyFile,
    bool multiDecoy,
    bool compute_sp
  );

  // added by Yang
  static void writeHeadersDIA(ofstream* file, bool compute_sp);

  void writeToFileDIA(
    ofstream* file,
    int top_n,
    const vector<Arr::iterator>& vec,
    const string& spectrum_filename,
    const Spectrum* spectrum,
    int charge,
    const ActivePeptideQueue* peptides,
    const ProteinVec& proteins,
    const map<Arr::iterator, FLOAT_T>* delta_cn_map,
    const map<Arr::iterator, FLOAT_T>* delta_lcn_map,
    const map<Arr::iterator, pair<const SpScorer::SpScoreData, int> >* sp_map,
    const map<Arr::iterator, boost::tuple<double, double, double>>* intensity_map,
    const map<Arr::iterator, boost::tuple<double, double, double>>* logrank_map,
    const map<Arr::iterator, boost::tuple<double, double, double>>* coelute_map,
    const map<Arr::iterator, boost::tuple<double, double>>* ms2pval_map,
    map<string, double>* peptide_predrt_map
  );


  void gatherTargetsAndDecoys(
    const ActivePeptideQueue* peptides,
    const ProteinVec& proteins,
    vector<Arr::iterator>& targetsOut,
    vector<Arr::iterator>& decoysOut,
    int top_n,
    int numDecoys,
    bool highScoreBest // indicates semantics of score magnitude
  );

  static void computeDeltaCns(
    const vector<Arr::iterator>& vec, // xcorr*100000000.0, high to low
    map<Arr::iterator, FLOAT_T>* delta_cn_map, // map to add delta cn scores to
    map<Arr::iterator, FLOAT_T>* delta_lcn_map
  );

  static void computeSpData(
    const vector<Arr::iterator>& vec,
    map<Arr::iterator, pair<const SpScorer::SpScoreData, int> >* sp_rank_map,
    SpScorer* sp_scorer,
    const ActivePeptideQueue* peptides
  );

  static void initModMap(const pb::ModTable& modTable, ModPosition position);
  static std::vector<Crux::Modification> getMods(const Peptide* peptide);

  static string CleavageType;
  static string decoy_prefix_;

 protected:
  Arr* matches_;
  Arr2* matches2_;
  Peptide* peptide_;

  // For allocation
  static char match_collection_loc_[sizeof(MatchCollection)];
  static char decoy_match_collection_loc_[sizeof(MatchCollection)];

  static bool lessXcorrScore(const Scores& x, const Scores& y) {
    return x.xcorr_score < y.xcorr_score;
  }

  static bool moreXcorrScore(const Scores& x, const Scores& y) {
    return x.xcorr_score > y.xcorr_score;
  }

  static bool lessXcorrPvalScore(const Scores& x, const Scores& y) {
    return x.xcorr_pval < y.xcorr_pval;
  }

  static bool moreXcorrPvalScore(const Scores& x, const Scores& y) {
    return x.xcorr_pval > y.xcorr_pval;
  }

  static bool lessResEvScore(const Scores& x, const Scores& y) {
    return x.resEv_score < y.resEv_score;
  }

  static bool moreResEvScore(const Scores& x, const Scores& y) {
    return x.resEv_score > y.resEv_score;
  }

  static bool lessResEvPvalScore(const Scores& x, const Scores& y) {
    return x.resEv_pval < y.resEv_pval;
  }

  static bool moreResEvPvalScore(const Scores& x, const Scores& y) {
    return x.resEv_pval > y.resEv_pval;
  }

  static bool lessCombinedPvalScore(const Scores& x, const Scores& y) {
    return x.combinedPval < y.combinedPval;
  }

  static bool moreCombinedPvalScore(const Scores& x, const Scores& y) {
    return x.combinedPval > y.combinedPval;
  }

/**
   * Helper function for tab delimited report function for peptide centric
   */
  void writeToFile(
    ofstream* file,
    const ActivePeptideQueue* peptides,
    const ProteinVec& proteins,
    bool compute_sp ///< whether to compute sp or not
  );

  /**
   * Helper function for tab delimited report function
   */
  void writeToFile(
    ofstream* file,
    int top_n,
    int decoys_per_target,
    const vector<Arr::iterator>& vec,
    const string& spectrum_filename,
    const Spectrum* spectrum,
    int charge,
    const ActivePeptideQueue* peptides,
    const ProteinVec& proteins,
    const map<Arr::iterator, FLOAT_T>& delta_cn_map,
    const map<Arr::iterator, FLOAT_T>& delta_lcn_map,
    const map<Arr::iterator, pair<const SpScorer::SpScoreData, int> >* sp_map,
    boost::mutex * rwlock
  );

  Crux::Peptide getCruxPeptide(const Peptide* peptide);

  struct spGreater {
    inline bool operator() (const pair<Arr::iterator, SpScorer::SpScoreData>& lhs,
                            const pair<Arr::iterator, SpScorer::SpScoreData>& rhs) {
      return lhs.second.sp_score > rhs.second.sp_score;
    }
  };
};

#endif

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 2
 * End:
 */