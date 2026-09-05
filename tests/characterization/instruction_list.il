(* Representative Instruction List program for generated-output characterization. *)

FUNCTION add_one : INT
  VAR_INPUT
    value : INT;
  END_VAR

  LD value
  ADD 1
  ST add_one
END_FUNCTION
