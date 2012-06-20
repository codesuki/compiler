

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"


extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val,
    True,
    False;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
    True        = idtable.add_string("true");
    False       = idtable.add_string("false");
}



ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {

    /* Fill this in */
    install_basic_classes();
                                 
    classes_ = append_Classes(classes_, classes);

    SymbolTable <Symbol, Entry> class_set;
    class_set.enterscope();

    class_set.addid(Object, Object);
    class_set.addid(Int, Int);
    class_set.addid(Str, Str);
    class_set.addid(IO, IO);
    class_set.addid(SELF_TYPE, SELF_TYPE);
    
    for (int i = classes->first(); classes->more(i); i = classes->next(i))
    {
        Class_ c = classes->nth(i);

        if (class_set.lookup(c->get_name()) != NULL)
        {
            semant_error(c) << c->get_name() << " already defined" << std::endl;
            return;
        }

        class_set.addid(c->get_name(), c->get_name());
        
        if (check_class(c))
        {
            semant_error(c) << "cyclic dependency detected" << std::endl;
            return;
        }
    }

    if (get_class(Main) == NULL)
    {
        semant_error() << "Class Main is not defined." << std::endl;
        return;
    }
}

Class_ ClassTable::get_class(Symbol name)
{ 
    if (name == SELF_TYPE || name == self)
        name = symbols_.lookup(self);

    for (int i = classes_->first(); classes_->more(i); i = classes_->next(i))
    {
        Class_ c = classes_->nth(i);
        if (name == c->get_name()) 
            return c;
    }

    return NULL;
}

/* 
 * This method uses a bruteforce approach to check for cycles. 
 * In addition it checks for invalid parents like Int, Str, Bool and undefined classes.
 */

bool ClassTable::check_class(Class_ c)
{
    int n = classes_->len();

    int i = 0;

    Class_ parent = get_class(c->get_parent());
     
    if (parent == NULL)
    {
        semant_error(c) << "undefined parent" << std::endl; 
        return true;
    }

    while (parent->get_name() != Object)
    {
        if (parent->get_name() == Str 
                || parent->get_name() == Bool 
                || parent->get_name() == Int
                || parent->get_name() == SELF_TYPE)
            semant_error(c) << "not allowed to derive from basic classes" << std::endl;

        i++;
        
        if (i > n) 
            return true;
        
        parent = get_class(parent->get_parent());
        
        if (parent == NULL)
        {
            semant_error(c) << "undefined parent" << std::endl;   
            return true;
        }
    }

    return false;
}

Symbol ClassTable::get_return_type(Symbol c, Symbol m)
{
    
}

method_class* ClassTable::get_method(Class_ c, Symbol m)
{
    for (int i = c->get_features()->first(); c->get_features()->more(i); i = c->get_features()->next(i))
    {
        method_class *method = dynamic_cast<method_class*>(c->get_features()->nth(i));
        
        if (method == NULL) 
            continue;

        if (method->get_name() == m)
        {
            return method;

        }
    }
    
    Class_ p = get_class(c->get_parent());
    return (p == NULL) ? NULL : get_method(p, m);
}

bool ClassTable::is_subclass(Class_ c1, Class_ c2)
{
    if (c1->get_name() == c2->get_name())
        return true;

    Class_ p = get_class(c1->get_parent());

    return (p == NULL) ? false : is_subclass(p, c2);
}

/*
 * One-off error was here, had to add and check the class itself, too...
 * This method should be changed to a good graph-algorithm for efficiency.
 */
Symbol ClassTable::get_lub(Class_ c1, Class_ c2)
{
    if (c1->get_name() == c2->get_name())
        return c1->get_name();
    
    SymbolTable<Symbol, Entry> parents;
    parents.enterscope();
    parents.addid(c1->get_name(), c1->get_name());

    Class_ p1 = get_class(c1->get_parent());
    while (p1 != NULL)
    {
        parents.addid(p1->get_name(), p1->get_name());
        p1 = get_class(p1->get_parent());
    }

    Class_ p2 = c2;
    while (p2 != NULL)
    {
        if (parents.lookup(p2->get_name()) != NULL)
            return p2->get_name();

        p2 = get_class(p2->get_parent());
    }

    return Object;
}

void ClassTable::publish_variables(Class_ c)
{
    Features features = c->get_features();
    for (int i = features->first(); features->more(i); i = features->next(i))
    {
        attr_class *attr = dynamic_cast<attr_class*>(features->nth(i));
        
        if (attr == NULL)
            continue;

        attr->publish(this);
    }

    Class_ p = get_class(c->get_parent());
    if (p != NULL) 
    {
        publish_variables(p);
    }
}

void ClassTable::publish_variables(method_class* m)
{
    
}

void method_class::publish(ClassTableP classtable)
{
    SymbolTable<Symbol, Entry> params;
    params.enterscope();

    for (int i = formals->first(); formals->more(i); i = formals->next(i))
    {
        Formal_class *formal = formals->nth(i);

        if (params.lookup(formal->get_name()) != NULL)
        {
            classtable->semant_error(classtable->get_current_class()) << 
                "Duplicated parameter name " << formal->get_name() 
                << " in method " << name << std::endl;
            return; 
        }

        params.addid(formal->get_name(), formal->get_name());

        formal->publish(classtable);
    }

    params.exitscope();
}

void attr_class::publish(ClassTableP classtable)
{
    if (classtable->symbols_.lookup(name) != NULL)
    {    
        classtable->semant_error(classtable->get_current_class()) << 
            "Attribute " << name << " already defined" 
            << std::endl;
        return;    
    }

    classtable->symbols_.addid(name, type_decl);
}

void formal_class::publish(ClassTableP classtable)
{
    if (name == self)
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Using keyword as parameter name: " << name 
            << std::endl;
        return;          
    }
    classtable->symbols_.addid(name, type_decl);
}

void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);  

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);

    classes_ = append_Classes(nil_Classes(), single_Classes(Object_class));
    classes_ = append_Classes(classes_, single_Classes(IO_class));
    classes_ = append_Classes(classes_, single_Classes(Int_class));
    classes_ = append_Classes(classes_, single_Classes(Bool_class));
    classes_ = append_Classes(classes_, single_Classes(Str_class));
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()                  
{                                                 
    semant_errors++;                            
    return error_stream;
} 



/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);

    if (classtable->errors()) {
	    cerr << "Compilation halted due to static semantic errors." << endl;
	    exit(1);
    }  
    
    classtable->symbols_.enterscope();

    classtable->symbols_.addid(True, Bool);
    classtable->symbols_.addid(False, Bool);

    /* some semantic analysis code may go here */
    for (int i = classes->first(); classes->more(i); i = classes->next(i))
    {
        Class_ c = classes->nth(i);
        Symbol name = c->get_name();
        classtable->symbols_.addid(name, name);
        c->semant(classtable);
    }

    classtable->symbols_.exitscope();

    if (classtable->errors()) {
	cerr << "Compilation halted due to static semantic errors." << endl;
	exit(1);
    } 
}

void class__class::semant(ClassTableP classtable)
{
    if (parent == SELF_TYPE)
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Not a valid parent " << parent <<  std::endl; 
        return;
    }

    if (classtable->get_class(parent) == NULL)
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Not a valid parent " << parent <<  std::endl; 
        return;
    }

    classtable->symbols_.enterscope();
    classtable->set_current_class(this);
    classtable->symbols_.addid(self, classtable->get_current_class()->get_name());
    classtable->publish_variables(this);
    for (int i = features->first(); features->more(i); i = features->next(i))
    {
        features->nth(i)->semant(classtable);
    }
    classtable->symbols_.exitscope();
}

void method_class::semant(ClassTableP classtable)
{
    classtable->symbols_.enterscope();
    publish(classtable);

    Symbol type = expr->semant(classtable);
    
    method_class *parent_method = classtable->get_method
        (classtable->get_class(classtable->get_current_class()->get_parent()), name); 

    Formals parent_formals = (parent_method != NULL) ? parent_method->get_formals() : NULL;
    
    if (return_type == SELF_TYPE && type != SELF_TYPE)
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Wrong return type in method " << name << std::endl;
        goto exit; 
    }

    if (type == SELF_TYPE)
        type = classtable->symbols_.lookup(self);

    if (classtable->get_class(return_type) == NULL)
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Undefined return type " << return_type << " in method " << name << "." << std::endl;
        goto exit;
    }

    if (!classtable->is_subclass(classtable->get_class(type), classtable->get_class(return_type)))
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Wrong return type in method " << name << " expected " << return_type 
            << " found " << type <<  std::endl;
        goto exit;
    }

    if (parent_method && parent_method->get_formals()->len() != formals->len())
    {    
        classtable->semant_error(classtable->get_current_class()) << 
            "Wrong number of parameters in method override " << name <<  std::endl;
        goto exit; 
    }
    
    for (int i = formals->first(); formals->more(i); i = formals->next(i))
    {          
        if (formals->nth(i)->get_type() == SELF_TYPE)
        { 
            classtable->semant_error(classtable->get_current_class()) << 
                "Formal parameter " << name << " cannot have type SELF_TYPE"
                << std::endl;
            goto exit;    
        }

        if (parent_method && parent_formals->nth(i)->get_type() != formals->nth(i)->get_type())
        {
            classtable->semant_error(classtable->get_current_class()) << 
                "Wrong parameter type in method override."
                << std::endl;
            goto exit; 
        }
    }          

exit:
    classtable->symbols_.exitscope();
}

void attr_class::semant(ClassTableP classtable)
{
    if (name == self)
    { 
        classtable->semant_error(classtable->get_current_class()) << 
            "Using keyword as attribute name: " << name 
            << std::endl;
        return; 
    }

    Symbol type = init->semant(classtable);
    
    if (type == No_type)
    {
        return;
    }

    if (!classtable->is_subclass(classtable->get_class(type), classtable->get_class(type_decl)))
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Wrong type in attribute initialization: " << name 
            << " expected " << type_decl << " was " << type << std::endl;
        return; 
    }
}

Symbol assign_class::semant(ClassTableP classtable)
{
    Symbol type = classtable->symbols_.lookup(name);
                              
    if (type == NULL) 
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Identifier not declared: " << name << std::endl;
        set_type(Object);
        return Object;
    } 

    Symbol type2 = expr->semant(classtable);

    if (type != type2)
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Wrong type in assign statement" << std::endl;
        set_type(Object);
        return Object; 
    }

    set_type(type);
    return type;
}

Symbol static_dispatch_class::semant(ClassTableP classtable)
{    
    Symbol c_type = expr->semant(classtable);

    Class_ c;

    if (c_type == No_type || c_type == SELF_TYPE) 
        c = classtable->get_current_class();
    else
        c = classtable->get_class(c_type);

    if (c == NULL)
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Tried to dispatch method on non-class" << std::endl;
        set_type(Object);
        return Object;
    }

    Class_ p = classtable->get_class(c->get_parent());

    if (!classtable->is_subclass(c, p))
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Tried static dispatch to non-parent class" << std::endl;
        set_type(Object);
        return Object;
    }
    
    method_class *m = classtable->get_method(p, name);
    
    if (m == NULL)
    { 
        classtable->semant_error(classtable->get_current_class()) << 
            "Class " << c_type << " doesnt have method " << name << std::endl;
        set_type(Object);
        return Object;
    }

    if (actual->len() != m->get_formals()->len())
    {
        classtable->semant_error(classtable->get_current_class()) << 
            name << " expects " << m->get_formals()->len() << " parameters " << 
            actual->len() << " given" << std::endl;
        set_type(Object);
        return Object;
    }

    Formals formals = m->get_formals();
    int j = formals->first();

    for (int i = actual->first(); 
            actual->more(i); i = actual->next(i), j = formals->next(j))
    {
        Symbol type = actual->nth(i)->semant(classtable);
        
        if (!classtable->is_subclass(classtable->get_class(type), 
                    classtable->get_class(formals->nth(j)->get_type())))
        {
            classtable->semant_error(classtable->get_current_class()) << 
                name << " parameter " << i << " expected " << formals->nth(j)->get_type()
                <<  " given " << type << std::endl; 
            set_type(Object);
            return Object; 
        }
    }

    Symbol return_type = m->get_return_type();

    if (return_type == SELF_TYPE)
    {
        return_type = c_type;
    }

    set_type(return_type);
    return return_type; 
}

Symbol dispatch_class::semant(ClassTableP classtable)
{
    Symbol c_type = expr->semant(classtable);

    Class_ c;

    if (c_type == No_type || c_type == SELF_TYPE) 
        c = classtable->get_current_class();
    else
        c = classtable->get_class(c_type);

    if (c == NULL)
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Tried to dispatch method on non-class" << std::endl;
        set_type(Object);
        return Object;
    }
    
    method_class *m = classtable->get_method(c, name);
    
    if (m == NULL)
    { 
        classtable->semant_error(classtable->get_current_class()) << 
            "Class " << c_type << " doesnt have method " << name << std::endl;
        set_type(Object);
        return Object;
    }

    if (actual->len() != m->get_formals()->len())
    {
        classtable->semant_error(classtable->get_current_class()) << 
            name << " expects " << m->get_formals()->len() << " parameters " << 
            actual->len() << " given" << std::endl;
        set_type(Object);
        return Object;
    }

    Formals formals = m->get_formals();
    int j = formals->first();

    for (int i = actual->first(); 
            actual->more(i); i = actual->next(i), j = formals->next(j))
    {
        Symbol type = actual->nth(i)->semant(classtable);
       
        if (!classtable->is_subclass(classtable->get_class(type), 
                    classtable->get_class(formals->nth(j)->get_type())))
        {
            classtable->semant_error(classtable->get_current_class()) << 
                name << " parameter " << i << " expected " << formals->nth(j)->get_type()
                <<  " given " << type << std::endl; 
            set_type(Object);
            return Object; 
        }
    }

    Symbol return_type = m->get_return_type();

    if (return_type == SELF_TYPE)
    {
        return_type = c_type;
    }

    set_type(return_type);
    return return_type; 
}

Symbol cond_class::semant(ClassTableP classtable)
{
    Symbol type1 = pred->semant(classtable);

    if (type1 != Bool)
    { 
        classtable->semant_error(classtable->get_current_class()) << 
            "Loop predicate must be boolean, " << type << " given." << std::endl; 
        set_type(Object);
        return Object; 
    }

    Symbol type2 = then_exp->semant(classtable);
    Symbol type3 = else_exp->semant(classtable);

    Symbol type4 = classtable->get_lub(classtable->get_class(type2), classtable->get_class(type3));

    set_type(type4);
    return type4;
}

Symbol loop_class::semant(ClassTableP classtable)
{
    Symbol type = pred->semant(classtable);

    body->semant(classtable);

    if (type != Bool)
    {
       classtable->semant_error(classtable->get_current_class()) << 
            "Loop predicate must be boolean, " << type << " given." << std::endl; 
       set_type(Object);
       return Object; 
    }

    set_type(Object);
    return Object;
}

Symbol typcase_class::semant(ClassTableP classtable)
{
    Symbol type1 = expr->semant(classtable);
    Symbol type2;

    SymbolTable<Symbol, Entry> distinct_set;
    distinct_set.enterscope();

    Symbol lub = NULL;

    for (int i = cases->first(); cases->more(i); i = cases->next(i))
    {
        Symbol type_decl = cases->nth(i)->get_type();

        type2 = cases->nth(i)->semant(classtable);
        
        if (distinct_set.lookup(type_decl) != NULL)
        {
            classtable->semant_error(classtable->get_current_class()) << 
                "Branches in Case statement must be of different types" << std::endl; 
            set_type(Object);
            return Object;
        }

        distinct_set.addid(type_decl, type_decl);
        
        if (lub == NULL)
            lub = type2;

        lub = classtable->get_lub(classtable->get_class(lub), classtable->get_class(type2));
    }

    set_type(lub);
    return lub;
}
 
Symbol branch_class::semant(ClassTableP classtable)
{
    classtable->symbols_.enterscope();
    classtable->symbols_.addid(name, type_decl);

    Symbol type = expr->semant(classtable);

    classtable->symbols_.exitscope();

    return type;
}

Symbol block_class::semant(ClassTableP classtable)
{
    Symbol type;
    for (int i = body->first(); body->more(i); i = body->next(i))
    {
        type = body->nth(i)->semant(classtable);
    }

    set_type(type);
    return type;
}

Symbol let_class::semant(ClassTableP classtable)
{
    if (identifier == self)
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Can not use self as let variable" << std::endl;
        set_type(Object);
        return Object;    
    }

    Symbol type = init->semant(classtable);
    
    if (type != No_type && type != type_decl)
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Wrong type in let initialization" << std::endl;
        set_type(Object);
        return Object; 
    } 
    
    classtable->symbols_.enterscope();
    classtable->symbols_.addid(identifier, type_decl);

    Symbol type2 = body->semant(classtable);

    classtable->symbols_.exitscope();

    set_type(type2);
    return type2;
}

Symbol plus_class::semant(ClassTableP classtable)
{
    Symbol type1 = e1->semant(classtable);
    Symbol type2 = e2->semant(classtable);
    
    if (type1 != Int || type2 != Int)
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Addition used with non Integer variable" << std::endl;
        set_type(Object);
        return Object;
    }

    set_type(Int);
    return Int;
}

Symbol sub_class::semant(ClassTableP classtable)
{ 
    Symbol type1 = e1->semant(classtable);
    Symbol type2 = e2->semant(classtable);
    
    if (type1 != Int || type2 != Int)
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Substract used with non Integer variable" << std::endl;
        set_type(Object);
        return Object;
    }

    set_type(Int);
    return Int;
}

Symbol mul_class::semant(ClassTableP classtable)
{ 
    Symbol type1 = e1->semant(classtable);
    Symbol type2 = e2->semant(classtable);
    
    if (type1 != Int || type2 != Int)
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Multiply used with non Integer variable" << std::endl;
        set_type(Object);
        return Object;
    }

    set_type(Int);
    return Int;
}

Symbol divide_class::semant(ClassTableP classtable)
{
    Symbol type1 = e1->semant(classtable);
    Symbol type2 = e2->semant(classtable);
    
    if (type1 != Int || type2 != Int)
    {                 
        classtable->semant_error(classtable->get_current_class()) << 
            "Divide used with non Integer variable" << std::endl;
        set_type(Object);
        return Object;
    }

    set_type(Int);
    return Int;
}

Symbol neg_class::semant(ClassTableP classtable)
{
    Symbol type = e1->semant(classtable);

    if (type != Int) 
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Neg used with non Integer variable" << std::endl;
        set_type(Object);
        return Object;
    }

    set_type(Int);
    return Int;
}

Symbol lt_class::semant(ClassTableP classtable)
{
    Symbol type1 = e1->semant(classtable);
    Symbol type2 = e2->semant(classtable);

    if (type1 != Int || type2 != Int)
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Less-than comparison only valid with Integers" << std::endl;
        set_type(Object);
        return Object; 
    }

    set_type(Bool);
    return Bool;
}

Symbol eq_class::semant(ClassTableP classtable)
{
    Symbol type1 = e1->semant(classtable);
    Symbol type2 = e2->semant(classtable);

    if ((type1 == Int && type2 != Int) || 
            (type1 == Bool && type2 != Bool) ||
            (type1 == Str && type2 != Str))
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "compare only supports standard types" << std::endl;
        set_type(Object);
        return Object;
    }

    set_type(Bool);
    return Bool;
}

Symbol leq_class::semant(ClassTableP classtable)
{
    Symbol type1 = e1->semant(classtable);
    Symbol type2 = e2->semant(classtable);

    if (type1 != Int || type2 != Int)
    {
        classtable->semant_error(classtable->get_current_class()) << 
            "Less-than-equal comparison only valid with Integers" << std::endl;
        set_type(Object);
        return Object; 
    }

    set_type(Bool);
    return Bool; 
}

Symbol comp_class::semant(ClassTableP classtable)
{
    Symbol type = e1->semant(classtable);
    if (type != Bool)
    {
        classtable->semant_error(classtable->get_current_class()) 
            << "Not requires Boolean" << std::endl; 
        set_type(Object);
        return Object;
    }

    set_type(Bool);
    return Bool;
}

Symbol int_const_class::semant(ClassTableP classtable)
{
    set_type(Int);
    return Int;
}

Symbol bool_const_class::semant(ClassTableP classtable)
{
    set_type(Bool);
    return Bool;
}

Symbol string_const_class::semant(ClassTableP classtable)
{
    set_type(Str);
    return Str;
}

Symbol new__class::semant(ClassTableP classtable)
{
    if (classtable->get_class(type_name) == NULL)
    { 
        classtable->semant_error(classtable->get_current_class()) 
            << "'new' used with undefined class " << type_name << "." << std::endl; 
        set_type(Object);
        return Object;
    }
        
    set_type(type_name);
    return type_name;
}

Symbol isvoid_class::semant(ClassTableP classtable)
{
    e1->semant(classtable);
    set_type(Bool);
    return Bool;
}

Symbol no_expr_class::semant(ClassTableP classtable)
{
    set_type(No_type);
    return No_type;
}

Symbol object_class::semant(ClassTableP classtable)
{
    if (name == self) // handle SELF_TYPE
    {
        set_type(SELF_TYPE);
        return SELF_TYPE;
    }
    
    Symbol type = classtable->symbols_.lookup(name);
                              
    if (type == NULL) 
    {
        classtable->semant_error(classtable->get_current_class()) 
            << "identifier not declared: " << name << std::endl;
        set_type(Object);
        return Object;
    }

    set_type(type);
    return type;
}

