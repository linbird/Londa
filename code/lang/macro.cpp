#define JOINT(type, name) type##name
#define ENUM2STR(type, value) (JOINT(type, _Name)( static_cast<type>( value ) ))
#define CONCAT(arg0, arg1) arg0 ## arg1


int main() {
  //ENUM2STR( HWIOAssignmentStatus_AssignmentType, AnValue.value );
  CONCAT(Hello, World)                        // -> HelloWorld
    CONCAT(Hello, CONCAT(World, !))             // -> HelloCONCAT(World, !)
    CONCAT(CONCAT(Hello, World) C, CONCAT(!))    // -> CONCAT(Hello, World) CONCAT(!) ##拼接，阻止继续展开
    return 0;
}
