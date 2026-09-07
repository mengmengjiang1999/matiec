#ifndef MATIEC_STAGE3_DATATYPE_ANALYSIS_STORE_HH
#define MATIEC_STAGE3_DATATYPE_ANALYSIS_STORE_HH

class symbol_c;

namespace matiec {
class AnalysisStore;
}

bool publish_datatype_candidates(symbol_c *tree_root,
                                 matiec::AnalysisStore &analysis);
void materialize_datatype_candidates(symbol_c *tree_root,
                                     const matiec::AnalysisStore &analysis);

#endif
