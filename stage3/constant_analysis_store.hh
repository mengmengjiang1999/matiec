#ifndef MATIEC_STAGE3_CONSTANT_ANALYSIS_STORE_HH
#define MATIEC_STAGE3_CONSTANT_ANALYSIS_STORE_HH

class symbol_c;

namespace matiec {
class AnalysisStore;
}

bool publish_constant_analysis(symbol_c *tree_root,
                               matiec::AnalysisStore &analysis);

#endif
