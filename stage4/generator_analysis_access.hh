#ifndef MATIEC_STAGE4_GENERATOR_ANALYSIS_ACCESS_HH
#define MATIEC_STAGE4_GENERATOR_ANALYSIS_ACCESS_HH

#include "stage4.hh"
#include "../compiler/analysis_store.hh"

#include <string>

inline bool stage4_set_generator_symbol(stage4out_c &output,
                                        const symbol_c *symbol,
                                        const std::string &name,
                                        symbol_c *value) {
  matiec::AnalysisStore *analysis = output.mutable_analysis_store();
  return analysis != nullptr &&
         analysis->set_generator_symbol(symbol, name, value);
}

inline symbol_c *stage4_generator_symbol(const stage4out_c &output,
                                         const symbol_c *symbol,
                                         const std::string &name) {
  const matiec::AnalysisStore *analysis = output.analysis_store();
  return analysis == nullptr ? nullptr :
      analysis->generator_symbol(symbol, name);
}

#endif
