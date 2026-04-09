#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum TermType TermType;
enum TermType {
  Zero,
  Var,
  Succ,
  Sum,
  Product,
};

typedef enum FormulaType FormulaType;
enum FormulaType {
  Atom,
  Negation,
  And,
  Or,
  Implies,
  Exists,
  Forall,
  // TODO: Add biconditional(iff), NOR, and NAND, with intro and elim rules.
};

typedef struct Term Term;
struct Term {
  TermType type;
  union {
    char var;    // Variable
    Term *child; // Successor
    struct {
      Term *l;
      Term *r;
    } binary_op; // Sum, Product
  };
};

typedef struct Formula Formula;
struct Formula {
  FormulaType type;
  union {
    struct {
      Term *l;
      Term *r;
    } atom;         // Atom
    Formula *child; // Negation
    struct {
      Formula *l;
      Formula *r;
    } binary_op; // And, Or, Implies
    struct {
      char var;
      Formula *body;
    } quantifier; // There exists, For all
  };
};

// TODO: Have every Term and Formula store it's own string representation.
void print_term(Term *t) {
  switch (t->type) {
  case Zero:
    printf("0");
    break;
  case Var:
    printf("%c", t->var);
    break;
  case Succ:
    printf("S");
    print_term(t->child);
    break;
  case Sum:
    printf("(");
    print_term(t->binary_op.l);
    printf("+");
    print_term(t->binary_op.r);
    printf(")");
    break;
  case Product:
    printf("(");
    print_term(t->binary_op.l);
    printf("*");
    print_term(t->binary_op.r);
    printf(")");
    break;
  }
}

void print_formula(Formula *f) {
  switch (f->type) {
  case Atom:
    printf("(");
    print_term(f->atom.l);
    printf("=");
    print_term(f->atom.r);
    printf(")");
    break;
  case Negation:
    printf("¬");
    printf("(");
    print_formula(f->child);
    printf(")");
    break;
  case And:
    printf("(");
    print_formula(f->binary_op.l);
    printf("∧");
    print_formula(f->binary_op.r);
    printf(")");
    break;
  case Or:
    printf("(");
    print_formula(f->binary_op.l);
    printf("∨");
    print_formula(f->binary_op.r);
    printf(")");
    break;
  case Implies:
    printf("(");
    print_formula(f->binary_op.l);
    printf("->");
    print_formula(f->binary_op.r);
    printf(")");
    break;
  case Exists:
    printf("(");
    printf("∃%c:", f->quantifier.var);
    print_formula(f->quantifier.body);
    printf(")");
    break;
  case Forall:
    printf("(");
    printf("∀%c:", f->quantifier.var);
    print_formula(f->quantifier.body);
    printf(")");
    break;
  }
}

Term *make_zero(void) {
  Term *t = malloc(sizeof(Term));
  t->type = Zero;
  return t;
}

Term *make_var(char v) {
  Term *t = malloc(sizeof(Term));
  t->type = Var;
  t->var = v;
  return t;
}

Term *make_succ(Term *child) {
  Term *t = malloc(sizeof(Term));
  t->type = Succ;
  t->child = child;
  return t;
}

Term *make_sum(Term *l, Term *r) {
  Term *t = malloc(sizeof(Term));
  t->type = Sum;
  t->binary_op.l = l;
  t->binary_op.r = r;
  return t;
}

Term *make_product(Term *l, Term *r) {
  Term *t = malloc(sizeof(Term));
  t->type = Product;
  t->binary_op.l = l;
  t->binary_op.r = r;
  return t;
}

Formula *make_atom(Term *l, Term *r) {
  Formula *f = malloc(sizeof(Formula));
  f->type = Atom;
  f->atom.l = l;
  f->atom.r = r;
  return f;
}

Formula *make_negation(Formula *child) {
  Formula *f = malloc(sizeof(Formula));
  f->type = Negation;
  f->child = child;
  return f;
}

Formula *make_and(Formula *l, Formula *r) {
  Formula *f = malloc(sizeof(Formula));
  f->type = And;
  f->binary_op.l = l;
  f->binary_op.r = r;
  return f;
}

Formula *make_or(Formula *l, Formula *r) {
  Formula *f = malloc(sizeof(Formula));
  f->type = Or;
  f->binary_op.l = l;
  f->binary_op.r = r;
  return f;
}

Formula *make_implies(Formula *l, Formula *r) {
  Formula *f = malloc(sizeof(Formula));
  f->type = Implies;
  f->binary_op.l = l;
  f->binary_op.r = r;
  return f;
}

Formula *make_forall(char var, Formula *body) {
  Formula *f = malloc(sizeof(Formula));
  f->type = Forall;
  f->quantifier.var = var;
  f->quantifier.body = body;
  return f;
}

Formula *make_exists(char var, Formula *body) {
  Formula *f = malloc(sizeof(Formula));
  f->type = Exists;
  f->quantifier.var = var;
  f->quantifier.body = body;
  return f;
}

bool term_equal(Term *a, Term *b) {
  if (a->type != b->type) {
    return false;
  }
  switch (a->type) {
  case Zero:
    return true;
  case Var:
    return a->var == b->var;
  case Succ:
    return term_equal(a->child, b->child);
  case Sum:
  case Product:
    return term_equal(a->binary_op.l, b->binary_op.l) &&
           term_equal(a->binary_op.r, b->binary_op.r);
  }
}

bool formula_equal(Formula *a, Formula *b) {
  if (a->type != b->type) {
    return false;
  }
  switch (a->type) {
  case Atom:
    return term_equal(a->atom.l, b->atom.l) && term_equal(a->atom.r, b->atom.r);
  case Negation:
    return formula_equal(a->child, b->child);
  case And:
  case Or:
  case Implies:
    return formula_equal(a->binary_op.l, b->binary_op.l) &&
           formula_equal(a->binary_op.r, b->binary_op.r);
  case Exists:
  case Forall:
    return a->quantifier.var == b->quantifier.var &&
           formula_equal(a->quantifier.body, b->quantifier.body);
  }
}

Formula *intro_and(Formula *l, Formula *r) { return make_and(l, r); }

Formula *elim_and(Formula *f, int n) {
  if (f->type != And) {
    printf("Formula is not of the form ⟨p ∧ q⟩");
    return NULL;
  }
  if (n == 1) {
    return f->binary_op.l;
  }
  if (n == 2) {
    return f->binary_op.r;
  }
  printf("n is out of range 1-2");
  return NULL;
}

Formula *elim_implies(Formula *p, Formula *t) {
  if (t->type != Implies) {
    printf("Elim implies: second argument is not an implication\n");
    return NULL;
  }
  if (!formula_equal(p, t->binary_op.l)) {
    printf("Elim implies: premise does not match antecedent\n");
    return NULL;
  }
  return t->binary_op.r;
}

Formula *symmetry(Formula *f) {
  if (f->type != Atom) {
    printf("Symmetry: formula is not an atom\n");
    return NULL;
  }
  return make_atom(f->atom.r, f->atom.l);
}

Formula *transitivity(Formula *a, Formula *b) {
  if (a->type != Atom) {
    printf("Formula a is not an Atom\n");
    return NULL;
  }
  if (b->type != Atom) {
    printf("Formula b is not an Atom\n");
    return NULL;
  }
  if (!term_equal(a->atom.r, b->atom.l)) {
    printf("Formula a RHS and formula b LHS are not equal\n");
    return NULL;
  }
  return make_atom(a->atom.l, b->atom.r);
}

Formula *intro_succ(Formula *f) {
  if (f->type != Atom) {
    printf("Formula is not an Atom\n");
    return NULL;
  }
  return make_atom(make_succ(f->atom.l), make_succ(f->atom.r));
}

Formula *elim_succ(Formula *f) {
  if (f->type != Atom) {
    printf("Formula is not an Atom\n");
    return NULL;
  }
  if (f->atom.l->type != Succ || f->atom.r->type != Succ) {
    printf("Both sides must begin with S\n");
    return NULL;
  }
  return make_atom(f->atom.l->child, f->atom.r->child);
}

Formula *induction(Formula *s, Formula *t) {
  if (s->type != Forall) {
    printf("Formula s must be of the form ∀a: ⟨p ⊃ q⟩\n");
    return NULL;
  }
  Formula *imp = s->quantifier.body;
  if (imp->type != Implies) {
    printf("Body must be an implication\n");
    return NULL;
  }
}

Formula *intro_axiom_one(void) {
  // clang-format off
  Formula *axiom_one = make_forall('a',
                        make_negation(
                          make_atom(
                            make_succ(
                              make_var('a')), 
                            make_zero())));
  return axiom_one;
  // clang-format on
}

Formula *intro_axiom_two(void) {
  // clang-format off
  Formula *axiom_two = make_forall('a', 
                        make_atom(
                          make_sum(
                            make_var('a'), 
                            make_zero()), 
                          make_var('a')));
  return axiom_two;
  // clang-format on
}

Formula *intro_axiom_three(void) {
  // clang-format off
  Formula *axiom_three = make_forall('a',
                          make_forall('b',
                            make_atom(
                              make_sum(
                                make_var('a'), 
                                make_succ(make_var('b'))),
                              make_succ(
                                make_sum(
                                  make_var('a'),
                                  make_var('b'))))));
  return axiom_three;
  // clang-format on
}

Formula *intro_axiom_four(void) {
  // clang-format off
  Formula *axiom_four = make_forall('a', 
                          make_atom(
                            make_product(
                              make_var('a'), 
                              make_zero()),
                            make_zero()));
  return axiom_four;
  // clang-format on
}

Formula *intro_axiom_five(void) {
  // clang-format off
  Formula *axiom_five = make_forall('a',
                          make_forall('b',
                            make_atom(
                              make_product(
                                make_var('a'),
                                make_succ(
                                  make_var('b'))),
                              make_sum(
                                make_product(
                                  make_var('a'),
                                  make_var('b')),
                                make_var('a')))));
  return axiom_five;
  // clang-format on
}

int main(void) {
  printf("woah TNT!\n");
  Term my_term;
  Term my_term_two;
  Term my_term_three;
  my_term.type = Zero;
  my_term_two.type = Succ;
  my_term_two.child = &my_term;
  my_term_three.type = Succ;
  my_term_three.child = &my_term_two;
  print_term(&my_term_three);
  printf("\n");
  Formula my_formula;
  my_formula.type = Atom;
  my_formula.atom.l = &my_term_two;
  my_formula.atom.r = &my_term_three;
  print_formula(&my_formula);
  printf("\n");
  Formula *axiom_one = make_forall(
      'a', make_negation(make_atom(make_succ(make_var('a')), make_zero())));
  print_formula(axiom_one);
  printf("\n");
  Formula my_formula_two;
  my_formula_two = *symmetry(&my_formula);
  print_formula(&my_formula_two);
  printf("\n");
  print_formula(elim_succ(&my_formula_two));
  printf("\n");
  Formula my_formula_four;
  // clang-format off
  my_formula_four = *make_forall('a',
                       make_forall('b', 
                         make_atom(
                             make_sum(
                               make_var('a'), 
                               make_var('b')), 
                             make_sum(
                               make_var('b'), 
                               make_var('a')))));
  // clang-format on
  print_formula(&my_formula_four);
  printf("\n\n");
  Formula *my_axiom = intro_axiom_one();
  print_formula(my_axiom);
  printf("\n");
  my_axiom = intro_axiom_two();
  print_formula(my_axiom);
  printf("\n");
  my_axiom = intro_axiom_three();
  print_formula(my_axiom);
  printf("\n");
  my_axiom = intro_axiom_four();
  print_formula(my_axiom);
  printf("\n");
  my_axiom = intro_axiom_five();
  print_formula(my_axiom);
  printf("\n");
}
