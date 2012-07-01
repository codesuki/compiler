# start of generated code
	.data
	.align	2
	.globl	class_nameTab
	.globl	Main_protObj
	.globl	Int_protObj
	.globl	String_protObj
	.globl	bool_const0
	.globl	bool_const1
	.globl	_int_tag
	.globl	_bool_tag
	.globl	_string_tag
_int_tag:
	.word	5
_bool_tag:
	.word	6
_string_tag:
	.word	7
	.globl	_MemMgr_INITIALIZER
_MemMgr_INITIALIZER:
	.word	_NoGC_Init
	.globl	_MemMgr_COLLECTOR
_MemMgr_COLLECTOR:
	.word	_NoGC_Collect
	.globl	_MemMgr_TEST
_MemMgr_TEST:
	.word	0
	.word	-1
str_const15:
	.word	7
	.word	5
	.word	String_dispTab
	.word	int_const1
	.byte	0	
	.align	2
	.word	-1
str_const14:
	.word	7
	.word	5
	.word	String_dispTab
	.word	int_const0
	.ascii	"D"
	.byte	0	
	.align	2
	.word	-1
str_const13:
	.word	7
	.word	5
	.word	String_dispTab
	.word	int_const0
	.ascii	"B"
	.byte	0	
	.align	2
	.word	-1
str_const12:
	.word	7
	.word	5
	.word	String_dispTab
	.word	int_const0
	.ascii	"C"
	.byte	0	
	.align	2
	.word	-1
str_const11:
	.word	7
	.word	6
	.word	String_dispTab
	.word	int_const2
	.ascii	"Main"
	.byte	0	
	.align	2
	.word	-1
str_const10:
	.word	7
	.word	5
	.word	String_dispTab
	.word	int_const0
	.ascii	"A"
	.byte	0	
	.align	2
	.word	-1
str_const9:
	.word	7
	.word	6
	.word	String_dispTab
	.word	int_const3
	.ascii	"String"
	.byte	0	
	.align	2
	.word	-1
str_const8:
	.word	7
	.word	6
	.word	String_dispTab
	.word	int_const2
	.ascii	"Bool"
	.byte	0	
	.align	2
	.word	-1
str_const7:
	.word	7
	.word	5
	.word	String_dispTab
	.word	int_const4
	.ascii	"Int"
	.byte	0	
	.align	2
	.word	-1
str_const6:
	.word	7
	.word	5
	.word	String_dispTab
	.word	int_const5
	.ascii	"IO"
	.byte	0	
	.align	2
	.word	-1
str_const5:
	.word	7
	.word	6
	.word	String_dispTab
	.word	int_const3
	.ascii	"Object"
	.byte	0	
	.align	2
	.word	-1
str_const4:
	.word	7
	.word	7
	.word	String_dispTab
	.word	int_const6
	.ascii	"_prim_slot"
	.byte	0	
	.align	2
	.word	-1
str_const3:
	.word	7
	.word	7
	.word	String_dispTab
	.word	int_const7
	.ascii	"SELF_TYPE"
	.byte	0	
	.align	2
	.word	-1
str_const2:
	.word	7
	.word	7
	.word	String_dispTab
	.word	int_const7
	.ascii	"_no_class"
	.byte	0	
	.align	2
	.word	-1
str_const1:
	.word	7
	.word	8
	.word	String_dispTab
	.word	int_const8
	.ascii	"<basic class>"
	.byte	0	
	.align	2
	.word	-1
str_const0:
	.word	7
	.word	6
	.word	String_dispTab
	.word	int_const9
	.ascii	"test.cl"
	.byte	0	
	.align	2
	.word	-1
int_const9:
	.word	5
	.word	4
	.word	Int_dispTab
	.word	7
	.word	-1
int_const8:
	.word	5
	.word	4
	.word	Int_dispTab
	.word	13
	.word	-1
int_const7:
	.word	5
	.word	4
	.word	Int_dispTab
	.word	9
	.word	-1
int_const6:
	.word	5
	.word	4
	.word	Int_dispTab
	.word	10
	.word	-1
int_const5:
	.word	5
	.word	4
	.word	Int_dispTab
	.word	2
	.word	-1
int_const4:
	.word	5
	.word	4
	.word	Int_dispTab
	.word	3
	.word	-1
int_const3:
	.word	5
	.word	4
	.word	Int_dispTab
	.word	6
	.word	-1
int_const2:
	.word	5
	.word	4
	.word	Int_dispTab
	.word	4
	.word	-1
int_const1:
	.word	5
	.word	4
	.word	Int_dispTab
	.word	0
	.word	-1
int_const0:
	.word	5
	.word	4
	.word	Int_dispTab
	.word	1
	.word	-1
bool_const0:
	.word	6
	.word	4
	.word	Bool_dispTab
	.word	0
	.word	-1
bool_const1:
	.word	6
	.word	4
	.word	Bool_dispTab
	.word	1
class_nameTab:
	.word	str_const5
	.word	str_const6
	.word	str_const7
	.word	str_const8
	.word	str_const9
	.word	str_const10
	.word	str_const12
	.word	str_const14
	.word	str_const13
	.word	str_const11
class_objTab:
	.word	Object_dispTab
	.word	Object_init
	.word	IO_dispTab
	.word	IO_init
	.word	Int_dispTab
	.word	Int_init
	.word	Bool_dispTab
	.word	Bool_init
	.word	String_dispTab
	.word	String_init
	.word	A_dispTab
	.word	A_init
	.word	C_dispTab
	.word	C_init
	.word	D_dispTab
	.word	D_init
	.word	B_dispTab
	.word	B_init
	.word	Main_dispTab
	.word	Main_init
	.word	-1
Object_protObj:
	.word	3
	.word	get_size()
	.word	Object_dispTab
	.word	-1
IO_protObj:
	.word	4
	.word	get_size()
	.word	IO_dispTab
	.word	-1
Int_protObj:
	.word	5
	.word	get_size()
	.word	Int_dispTab
	.word	0
	.word	-1
Bool_protObj:
	.word	6
	.word	get_size()
	.word	Bool_dispTab
	.word	0
	.word	-1
String_protObj:
	.word	7
	.word	get_size()
	.word	String_dispTab
	.word	int_const1
	.word	0
	.word	-1
A_protObj:
	.word	8
	.word	get_size()
	.word	A_dispTab
	.word	-1
C_protObj:
	.word	10
	.word	get_size()
	.word	C_dispTab
	.word	-1
D_protObj:
	.word	12
	.word	get_size()
	.word	D_dispTab
	.word	-1
B_protObj:
	.word	11
	.word	get_size()
	.word	B_dispTab
	.word	-1
Main_protObj:
	.word	9
	.word	get_size()
	.word	Main_dispTab
	.word	int_const1
	.word	str_const15
	.word	bool_const0
	.word	0
	.word	0
Object_dispTab:
	.word	Object.abort
	.word	Object.type_name
	.word	Object.copy
IO_dispTab:
	.word	Object.abort
	.word	Object.type_name
	.word	Object.copy
	.word	IO.out_string
	.word	IO.out_int
	.word	IO.in_string
	.word	IO.in_int
Int_dispTab:
	.word	Object.abort
	.word	Object.type_name
	.word	Object.copy
Bool_dispTab:
	.word	Object.abort
	.word	Object.type_name
	.word	Object.copy
String_dispTab:
	.word	Object.abort
	.word	Object.type_name
	.word	Object.copy
	.word	String.length
	.word	String.concat
	.word	String.substr
A_dispTab:
	.word	Object.abort
	.word	Object.type_name
	.word	Object.copy
C_dispTab:
	.word	Object.abort
	.word	Object.type_name
	.word	Object.copy
D_dispTab:
	.word	Object.abort
	.word	Object.type_name
	.word	Object.copy
B_dispTab:
	.word	Object.abort
	.word	Object.type_name
	.word	Object.copy
Main_dispTab:
	.word	Object.abort
	.word	Object.type_name
	.word	Object.copy
	.word	Main.main
	.globl	heap_start
heap_start:
	.word	0
	.text
	.globl	Main_init
	.globl	Int_init
	.globl	String_init
	.globl	Bool_init
	.globl	Main.main

# end of generated code
