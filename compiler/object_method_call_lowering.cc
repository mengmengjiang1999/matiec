#include "compiler/object_method_call_lowering.hh"

#include "absyntax/absyntax.hh"
#include "absyntax/visitor.hh"
#include "absyntax_utils/absyntax_utils.hh"
#include "compiler/diagnostic_engine.hh"
#include "compiler/ast_arena.hh"
#include "compiler/object_method_ast_analysis.hh"

#include <algorithm>
#include <cctype>
#include <map>
#include <string>

namespace matiec {
namespace {

std::string uppercase(std::string value) {
  std::transform(value.begin(), value.end(), value.begin(),
                 [](unsigned char ch) { return static_cast<char>(std::toupper(ch)); });
  return value;
}

SourceRange source_range(symbol_c *symbol) {
  if (symbol == nullptr) return {};
  return {{symbol->first_file == nullptr ? "" : symbol->first_file,
           static_cast<std::size_t>(symbol->first_line),
           static_cast<std::size_t>(symbol->first_column), 0},
          {symbol->last_file == nullptr ? "" : symbol->last_file,
           static_cast<std::size_t>(symbol->last_line),
           static_cast<std::size_t>(symbol->last_column), 0}};
}

template <typename Symbol>
Symbol *located_token(const std::string &value, symbol_c *location) {
  return new Symbol(retain_ast_string(value.c_str()), location->first_line,
                    location->first_column,
                    location->first_file, location->first_order,
                    location->last_line, location->last_column,
                    location->last_file, location->last_order);
}

symbol_c *field_reference(symbol_c *receiver,
                          const std::string &field,
                          symbol_c *location) {
  identifier_c *field_name = located_token<identifier_c>(field, location);
  symbol_c *receiver_parent = receiver->parent;
  structured_variable_c *reference = new structured_variable_c(
      receiver, field_name, location->first_line, location->first_column,
      location->first_file, location->first_order, location->last_line,
      location->last_column, location->last_file, location->last_order);
  receiver->parent = receiver_parent;
  return reference;
}

class method_call_lowering_c : public iterator_visitor_c {
 public:
  method_call_lowering_c(const ObjectMethodAnalysisResult &model,
                         DiagnosticEngine &diagnostics)
      : model_(model), diagnostics_(diagnostics) {}

  void *visit(object_method_invocation_c *symbol) override {
    token_c *receiver_name = get_var_name_c::get_name(symbol->receiver);
    token_c *method_name = dynamic_cast<token_c *>(symbol->method_name);
    if (receiver_name == nullptr || method_name == nullptr) {
      diagnostics_.error("Unsupported method receiver", source_range(symbol));
      return nullptr;
    }

    const std::string instance_key = uppercase(receiver_name->value);
    const auto instance = model_.instance_types.find(instance_key);
    if (instance == model_.instance_types.end()) {
      diagnostics_.error("Unknown function-block method receiver: " +
                             std::string(receiver_name->value),
                         source_range(symbol));
      return nullptr;
    }

    const ObjectMethodAst *method = nullptr;
    for (const ObjectMethodAst &candidate : model_.methods) {
      if (uppercase(candidate.owner) == uppercase(instance->second) &&
          uppercase(candidate.name) == uppercase(method_name->value)) {
        method = &candidate;
        break;
      }
    }
    if (method == nullptr) {
      diagnostics_.error("Unknown method " + std::string(method_name->value) +
                             " for function block " + instance->second,
                         source_range(symbol));
      return nullptr;
    }

    param_assignment_list_c *arguments = new param_assignment_list_c(
        symbol->first_line, symbol->first_column, symbol->first_file,
        symbol->first_order, symbol->last_line, symbol->last_column,
        symbol->last_file, symbol->last_order);
    list_c *explicit_arguments = dynamic_cast<list_c *>(
        symbol->formal_param_list != nullptr ? symbol->formal_param_list
                                             : symbol->nonformal_param_list);
    if (explicit_arguments != nullptr) {
      for (int index = 0; index < explicit_arguments->n; ++index) {
        symbol_c *argument = explicit_arguments->get_element(index);
        symbol_c *parent = argument->parent;
        arguments->add_element(argument);
        argument->parent = parent;
      }
    }

    const bool formal = symbol->formal_param_list != nullptr;
    for (const auto &field : method->owner_fields) {
      symbol_c *reference = field_reference(symbol->receiver, field.first, symbol);
      if (formal) {
        identifier_c *parameter = located_token<identifier_c>(
            "MATIECSELF" + field.first, symbol);
        arguments->add_element(new input_variable_param_assignment_c(
            parameter, reference, symbol->first_line, symbol->first_column,
            symbol->first_file, symbol->first_order, symbol->last_line,
            symbol->last_column, symbol->last_file, symbol->last_order));
      } else {
        arguments->add_element(reference);
      }
    }

    poutype_identifier_c *lowered_name =
        located_token<poutype_identifier_c>(method->lowered_name, symbol);
    symbol_c *compatibility_arguments = arguments->n == 0 ? nullptr : arguments;
    symbol->compatibility_invocation = new function_invocation_c(
        lowered_name, formal ? compatibility_arguments : nullptr,
        formal ? nullptr : compatibility_arguments,
        symbol->first_line, symbol->first_column,
        symbol->first_file, symbol->first_order, symbol->last_line,
        symbol->last_column, symbol->last_file, symbol->last_order);

    return iterator_visitor_c::visit(symbol);
  }

 private:
  const ObjectMethodAnalysisResult &model_;
  DiagnosticEngine &diagnostics_;
};

}  // namespace

bool lower_object_method_calls(
    symbol_c *tree_root, const ObjectMethodAnalysisResult &model,
    DiagnosticEngine &diagnostics) {
  if (tree_root == nullptr) return false;
  method_call_lowering_c lowering(model, diagnostics);
  tree_root->accept(lowering);
  return !diagnostics.has_errors();
}

}  // namespace matiec
