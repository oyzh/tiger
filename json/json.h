
typedef char* String;

typedef struct J_Value_* J_Value;
typedef struct J_Object_* J_Object;
typedef struct J_Pair_* J_Pair;
typedef struct J_Array_* J_Array;
typedef struct J_Number_* J_Number;

struct J_Value_{
  enum {K_False, K_Null, K_True, K_Object, K_Array, K_Number, K_String }kind;
  union{
    J_Object obj;
    J_Array arr;
    J_Number num;
    String str;
  }u;
};


struct J_Object_{
  J_Pair head;
  J_Object tail;
};

struct J_Pair_{
  String key;
  J_Value value;
};

struct J_Array_{
  J_Value head;
  J_Array tail;
};

struct J_Number_{
  enum{Double,Int}kind;
  union{
    int i;
    double d;
  }u;
};



J_Value V_False(void);
J_Value V_Null(void);
J_Value V_True(void);
J_Value V_Object(J_Object o);
J_Value V_Array(J_Array a );
J_Value V_Number(J_Number n);
J_Value V_String(String s);

J_Object O_Object(J_Pair h, J_Object t);
J_Pair P_Pair(String k,J_Value v);
J_Array A_Array(J_Value h, J_Array t);
J_Number N_Double(double d);
J_Number N_Int(int i);
