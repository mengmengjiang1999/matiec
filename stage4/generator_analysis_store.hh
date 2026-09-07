#ifndef MATIEC_STAGE4_GENERATOR_ANALYSIS_STORE_HH
#define MATIEC_STAGE4_GENERATOR_ANALYSIS_STORE_HH

class symbol_c;

namespace matiec {
class AnalysisStore;
}

bool publish_generator_analysis(symbol_c *tree_root,
                                matiec::AnalysisStore &analysis);
void materialize_generator_analysis(symbol_c *tree_root,
                                    const matiec::AnalysisStore &analysis);

#endif
