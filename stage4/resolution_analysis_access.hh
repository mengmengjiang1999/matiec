#ifndef MATIEC_STAGE4_RESOLUTION_ANALYSIS_ACCESS_HH
#define MATIEC_STAGE4_RESOLUTION_ANALYSIS_ACCESS_HH

#include "stage4.hh"
#include "../compiler/analysis_store.hh"

inline const matiec::ResolutionAnalysisRecord *stage4_resolution_record(
    const stage4out_c &output, const symbol_c *symbol) {
  const matiec::AnalysisStore *analysis = output.analysis_store();
  if (analysis == NULL) return NULL;
  const matiec::AnalysisEntry<matiec::ResolutionAnalysisRecord> *entry =
      analysis->resolution(symbol);
  return entry == NULL ? NULL : &entry->value;
}

#endif
