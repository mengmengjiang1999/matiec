TYPE
  Point : STRUCT X : INT; END_STRUCT;
  PointArray : ARRAY [1..2] OF Point;
END_TYPE

FUNCTION_BLOCK Cell
  VAR Value : INT; END_VAR
  Value := Value;
END_FUNCTION_BLOCK

FUNCTION Adjust : INT
  VAR_INPUT Source : INT; END_VAR
  VAR_OUTPUT Copy : INT; END_VAR
  VAR_IN_OUT Target : INT; END_VAR
  Copy := Source;
  Target := Target + Source;
  Adjust := Target;
END_FUNCTION

PROGRAM LookupMatrix
  VAR
    Item : Point;
    Items : PointArray;
    Block : Cell;
    InputValue : INT;
    OutputValue : INT;
  END_VAR
  Item.X := InputValue;
  Items[1].X := Item.X;
  Block.Value := Items[1].X;
  OutputValue := Adjust(Source := Block.Value, Copy => OutputValue,
                        Target := Items[2].X);
END_PROGRAM
