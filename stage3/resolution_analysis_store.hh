#ifndef MATIEC_STAGE3_RESOLUTION_ANALYSIS_STORE_HH
#define MATIEC_STAGE3_RESOLUTION_ANALYSIS_STORE_HH

class symbol_c;

namespace matiec {
class AnalysisStore;
}

bool publish_declaration_resolution(symbol_c *tree_root,
                                    matiec::AnalysisStore &analysis);
void materialize_declaration_resolution(
    symbol_c *tree_root, const matiec::AnalysisStore &analysis);

#endif
