#ifndef NODE_H_INCLUDED
#	define NODE_H_INCLUDED

#include <stdbool.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "mem.h"
#include "realtype.h"

#define CIRCUIT_EXPORT    static inline


enum {
	COMP_INVALID = 0,
	COMP_WIRE=1,         /// no real component, just abstract wire.
	COMP_DC_VOLTAGE_SRC, /// V - 
	COMP_AC_VOLTAGE_SRC, /// v - 
	COMP_DC_CURRENT_SRC, /// I - 
	COMP_AC_CURRENT_SRC, /// i - 
	COMP_RESISTOR,       /// R - 
	COMP_CAPACITOR,      /// C - I = C * ∂V(t)/∂t
	COMP_INDUCTOR,       /// L - V = L * ∂I(t)/∂t
	COMP_VCCS,           /// G - Voltage Controlled Current Source
	COMP_VCVS,           /// E - Voltage Controlled Voltage Source
	COMP_CCVS,           /// H - Current Controlled Voltage Source
	COMP_CCCS,           /// F - Current Controlled Current Source
	MAX_COMP_TYPES,
};

enum {
	MAX_NODES = 10,
	GND_IDX   =  0,
};


CIRCUIT_EXPORT void skip_ws(char const text[const restrict static 1], size_t *const restrict i_ref) {
	while( text[*i_ref] != 0 && isspace(text[*i_ref]) ) {
		(*i_ref)++;
	}
}
/// Parse tokens like "3k", "1e-3", "5m", "10", "2.2u"
CIRCUIT_EXPORT rat parse_si_scalar(char const tok[const static 1]) {
	size_t len = 0;
	while( tok[len] != 0 && tok[len] != '\n' && tok[len] != '\r' ) {
		len++;
	}
	if( len==0 ) {
		return rat_zero();
	}
	
	char last = tok[len - 1];
	rat scale = rat_pos1();
	switch( last ) {
		case 'k': case 'K': scale = float_to_rat(1E+3);   break;
		case 'M':           scale = float_to_rat(1E+6);   break;
		case 'G':           scale = float_to_rat(1E+9);   break;
		case 'm':           scale = float_to_rat(1E-3);   break;
		case 'u':           scale = float_to_rat(1E-6);   break;
		case 'n':           scale = float_to_rat(1E-9);   break;
		default:            scale = rat_pos1(); last = 0; break;
	}
	
	char buf[48] = {0};
	if( last != 0 && len < sizeof buf ) {
		memcpy(buf, tok, len-1);
		buf[len-1] = 0;
		return rat_mul(str_to_rat(buf), scale);
	}
	return str_to_rat(tok);
}

CIRCUIT_EXPORT uint8_t kind_from_letter(char const c) {
	switch( c ) {
		case 'R': case 'r': return COMP_RESISTOR;
		case 'V':           return COMP_DC_VOLTAGE_SRC;
		case 'v':           return COMP_DC_VOLTAGE_SRC;
		case 'I':           return COMP_DC_CURRENT_SRC;
		case 'i':           return COMP_AC_CURRENT_SRC;
		case 'C': case 'c': return COMP_CAPACITOR;
		case 'L': case 'l': return COMP_INDUCTOR;
		case 'G': case 'g': return COMP_VCCS;
		case 'E': case 'e': return COMP_VCVS;
		case 'H': case 'h': return COMP_CCVS;
		case 'F': case 'f': return COMP_CCCS;
		case 'W': case 'w': return COMP_WIRE;
		default:            return COMP_INVALID;
	}
}

CIRCUIT_EXPORT NO_NULLS rat lex_decimal(size_t *const restrict i_ref, char const text[const restrict static 1]) {
	bool has_dot = false;
	size_t n = 0;
	enum { MAX_NUM_CSTR = 24 };
	char num_cstr[MAX_NUM_CSTR] = {0};
	while( text[*i_ref] != 0 && (isalnum(text[*i_ref]) || text[*i_ref]=='.' || text[*i_ref]=='-') ) {
		switch( text[*i_ref] ) {
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9': {
				if( n < MAX_NUM_CSTR ) {
					num_cstr[n++] = text[*i_ref];
					++*i_ref;
				}
				break;
			}
			case '.': {
				if( n < MAX_NUM_CSTR && !has_dot ) {
					num_cstr[n++] = text[*i_ref];
					++*i_ref;
					has_dot = true;
				}
				break;
			}
			default: {
				if( n < MAX_NUM_CSTR ) {
					num_cstr[n++] = text[*i_ref];
					++*i_ref;
				}
				return false;
			}
		}
	}
	return str_to_rat(num_cstr);
}

CIRCUIT_EXPORT size_t idx_2_to_1(size_t const idx_dim1, size_t const idx_dim2, size_t const dim_size) {
	return (idx_dim1 * dim_size) + idx_dim2;
}

CIRCUIT_EXPORT NO_NULLS rat *alloc_vec(struct TIBiStack *const s, size_t const n) {
	rat *const v = bistack_alloc_front_vec(s, n, sizeof *v);
	for( size_t i=0; i < n; i++ ) {
		v[i] = rat_zero();
	}
	return v;
}

CIRCUIT_EXPORT bool node_is_ground(uint8_t const n) { return n==GND_IDX; }

#ifndef TICE_H
CIRCUIT_EXPORT void print_matrix(size_t const n, rat const G[const static n*n], rat const v[const static n]) {
	enum { NUM_CSTR_LEN=12 };
	for( size_t i=0; i < n; i++ ) {
		printf("| ");
		for( size_t j=0; j < n; j++ ) {
			printf("%10s", rat_to_cstr(G[idx_2_to_1(i,j,n)], NUM_CSTR_LEN, (char[NUM_CSTR_LEN]){0}));
			if( j+1 < n ) {
				printf(", ");
			}
		}
		printf(" | I[V%zu]: %10s\n", i+1, rat_to_cstr(v[i], NUM_CSTR_LEN, (char[NUM_CSTR_LEN]){0}));
	}
}
#endif

CIRCUIT_EXPORT NO_NULLS size_t setup_matrix_ids(size_t const node_bits, uint8_t(*const restrict n_to_m)[MAX_NODES], uint8_t(*const restrict m_to_n)[MAX_NODES]) {
	size_t n = 0;
	/// we start at 1 because we're ignoring the ground node.
	for( size_t i=1; i < MAX_NODES; i++ ) {
		if( node_bits & (1<<i) ) {
			(*n_to_m)[i] = n;
			(*m_to_n)[n] = i;
			n++;
		}
	}
	return n;
}

/**
 * Gauss–Jordan RREF on [A | v], in place.
 * - A: n x n stored row-major via idx_2_to_1(i,j,n).
 * - v: length n RHS vector (augmented column).
 * On return, A is in reduced row-echelon form; v is transformed accordingly.
 * 
 * Returns:
 *        RREFResultOk -> OK, unique solution (rank==n, A becomes identity)
 *  RREFResultFreeVars -> OK, free variables (rank < n, consistent)
 * RREFResultBadMatrix -> Inconsistent system (some 0==nonzero row)
 */
enum RREFResult {
	RREFResultOk = 0,
	RREFResultFreeVars,
	RREFResultBadMatrix,
};
CIRCUIT_EXPORT enum RREFResult gaussian_rref(size_t const matrix_len, rat A[const restrict static matrix_len*matrix_len], rat v[const restrict static matrix_len]) {
	size_t curr_pivot_row = 0;
	rat const eps = rat_epsilon();
	for( size_t c=0; c < matrix_len && curr_pivot_row < matrix_len; c++ ) {
		size_t curr_pivot = curr_pivot_row;
		size_t const rc = idx_2_to_1(curr_pivot_row, c, matrix_len);
		rat cur_max = rat_abs(A[rc]);
		for( size_t i = curr_pivot_row+1; i < matrix_len; i++ ) {
			size_t const ic  = idx_2_to_1(i, c, matrix_len);
			rat  const mag = rat_abs(A[ic]);
			if( rat_lt(cur_max, mag) ) {
				cur_max = mag;
				curr_pivot = i;
			}
		}
		if( rat_lt(cur_max, eps) ) {
			continue;
		}
		
		if( curr_pivot != curr_pivot_row ) {
			rat const tmpv = v[curr_pivot_row];
			v[curr_pivot_row] = v[curr_pivot];
			v[curr_pivot] = tmpv;
			for( size_t j=0; j < matrix_len; j++ ) {
				size_t const rj   = idx_2_to_1(curr_pivot_row, j, matrix_len);
				size_t const pj   = idx_2_to_1(curr_pivot, j, matrix_len);
				rat    const tmpA = A[rj];
				A[rj] = A[pj];
				A[pj] = tmpA;
			}
		}
		
		size_t const rc2 = idx_2_to_1(curr_pivot_row, c, matrix_len);
		rat  const piv = A[rc2];
		for( size_t j=0; j < matrix_len; j++ ) {
			size_t const rj = idx_2_to_1(curr_pivot_row, j, matrix_len);
			A[rj] = rat_div(A[rj], piv);
		}
		v[curr_pivot_row] = rat_div(v[curr_pivot_row], piv);
		
		for( size_t i=0; i < matrix_len; i++ ) {
			if( i==curr_pivot_row ) {
				continue;
			}
			size_t const ic = idx_2_to_1(i, c, matrix_len);
			rat const factor = A[ic];
			if( rat_lt(rat_abs(factor), eps) ) {
				continue;
			}
			for( size_t j=0; j < matrix_len; j++ ) {
				size_t const ij = idx_2_to_1(i, j, matrix_len);
				size_t const rj = idx_2_to_1(curr_pivot_row, j, matrix_len);
				A[ij] = rat_sub(A[ij], rat_mul(factor, A[rj]));
			}
			v[i] = rat_sub(v[i], rat_mul(factor, v[curr_pivot_row]));
		}
		curr_pivot_row++;
	}
	
	for( size_t i=0; i < matrix_len; i++ ) {
		bool all_zero = true;
		for( size_t j=0; j < matrix_len; j++ ) {
			rat const aij = A[idx_2_to_1(i, j, matrix_len)];
			if( !rat_lt(rat_abs(aij), eps) ) {
				all_zero = false;
				break;
			}
		}
		if( all_zero && !rat_lt(rat_abs(v[i]), eps) ) {
			return RREFResultBadMatrix;    /// inconsistent
		}
	}
	return curr_pivot_row==matrix_len? RREFResultOk : RREFResultFreeVars;
}



enum {
	ERR_NODE_OOB  = -2,
	ERR_OOM       = -1,
	ERR_SELF_LOOP = +0,
	ERR_OK        = +1,
};

/// represents a component that's connected to between two nodes.
/// 24 bytes.
/// Using linked list to save memory (ironically).
/// rat is 9 bytes.
struct Comp {
	union {
		rat current;
		struct { uint8_t np, nn, _pad; } dep;
	} aux;
	struct Comp *next; /// ptrs are 3 bytes on the TI8*.
	rat        value;
	uint8_t      kind, node, owner;
};

CIRCUIT_EXPORT bool should_stamp_once(struct Comp const *const comp) { return comp->owner < comp->node; }

CIRCUIT_EXPORT NO_NULLS struct Comp *component_new(struct TIBiStack *const s, rat const value, uint8_t const kind, uint8_t const node) {
	struct Comp *const comp = bistack_alloc_back(s, sizeof *comp);
	if( comp==NULL ) {
		return NULL;
	}
	comp->value = value;
	comp->node  = node;
	comp->kind  = kind;
	comp->aux.current = rat_zero();
	return comp;
}


struct Circuit {
	struct TIBiStack bistack;
	
	/// Electrical circuits are basically a network of components with components in between.
	/// The best way to represent an electrical circuit is a graph.
	/// Specifically, an adjacency list type graph where vertices/components contain an
	/// edge/weight that represents an electrical component.
	struct Comp     *components[MAX_NODES];
	size_t           active_nodes; /// used as a bitflag.
};

CIRCUIT_EXPORT NO_NULLS void circuit_connect_component(
	struct Circuit *const c,
	uint8_t         const n1,
	uint8_t         const n2,
	struct Comp    *const comp
) {
	comp->owner = n1;
	comp->next = c->components[n1];
	c->components[n1] = comp;
	c->active_nodes |= (1 << n1) | (1 << n2);
}

CIRCUIT_EXPORT NO_NULLS int circuit_add_component(
	struct Circuit *const c,
	uint8_t         const n1,
	uint8_t         const n2,
	uint8_t         const comp_type,
	rat             const value
) {
	if( n1 >= MAX_NODES || n2 >= MAX_NODES ) {
		return ERR_NODE_OOB;
	} else if( n1==n2 ) {
		return ERR_SELF_LOOP;
	}
	
	struct Comp *const comp = component_new(&c->bistack, value, comp_type, n2);
	if( comp==NULL ) {
		return ERR_OOM;
	}
	circuit_connect_component(c, n1, n2, comp);
	return ERR_OK;
}


CIRCUIT_EXPORT int circuit_add_from_line(struct Circuit *const restrict c, char const line[const restrict static 1]) {
	size_t i=0;
	skip_ws(line, &i);
	if( line[i]==0 ) {
		return ERR_OK;
	}
	
	char letter = line[i];
	if( !isalpha(letter) ) {
		return ERR_OK;
	}
	i++;
	uint8_t const kind = kind_from_letter(letter);
	
	uint8_t n1 = 0, n2 = 0;
	char val_tok[48] = {0};
	if( letter=='E'||letter=='e'||letter=='G'||letter=='g'||letter=='F'||letter=='f' ) {
		uint8_t nc1 = 0, nc2 = 0;
		skip_ws(line, &i);
		if( kind==COMP_INVALID || n1>=MAX_NODES || n2>=MAX_NODES || nc1>=MAX_NODES || nc2>=MAX_NODES || n1==n2 ) {
			return ERR_OK;
		}
		
		rat const value = parse_si_scalar(val_tok);
		struct Comp *const comp = component_new(&c->bistack, value, kind, n2);
		if( comp==NULL ) {
			return ERR_OOM;
		}
		comp->aux.dep.np = nc1;
		comp->aux.dep.nn = nc2;
		circuit_connect_component(c, n1, n2, comp);
		return ERR_OK;
	}
	
	if( sscanf(line, " %c %hhu %hhu %47s", &letter, &n1, &n2, val_tok) < 4 ) {
		return ERR_OK;
	}
	
	uint8_t const comp_type = kind_from_letter(letter);
	//printf("got letter: %c, comp type: %u\n", letter, comp_type);
	if( comp_type==COMP_INVALID || n1 >= MAX_NODES || n2 >= MAX_NODES || n1==n2 ) {
		return ERR_OK;
	}
	rat const value = parse_si_scalar(val_tok);
	//printf("circuit_add_from_line.value :: %s\n", rat_to_cstr(value, 64, (char[64]){0}));
	return circuit_add_component(c, n1, n2, comp_type, value);
}

CIRCUIT_EXPORT void circuit_add_from_string(struct Circuit *const restrict c, char const text[const restrict static 1]) {
	size_t i = 0;
	for( size_t p=0; text[p] != 0; ) {
		i = p;
		while( text[p] != 0 && text[p] != '\n' ) {
			p++;
		}
		size_t len = p - i;
		char buf[128] = {0};
		if( len >= sizeof buf ) {
			len = sizeof buf - 1;
		}
		memcpy(buf, &text[i], len);
		buf[len] = 0;
		( void )(circuit_add_from_line(c, buf));
		if( text[p]=='\n' ) {
			p++;
			continue;
		}
	}
}

CIRCUIT_EXPORT struct Circuit circuit_make(size_t const memory_size, uint8_t memory[const static memory_size]) {
	struct Circuit circ = {0};
	for( size_t i=0; i < MAX_NODES; i++ ) {
		circ.components[i] = NULL;
	}
	circ.bistack = bistack_make(memory, memory_size);
	return circ;
}

CIRCUIT_EXPORT size_t circuit_build_dc(
	struct Circuit *const restrict c,
	uint8_t       (*const restrict matrix_id_to_node_out)[MAX_NODES],
	rat                        **G_out,
	rat                        **I_out
) {
	uint8_t node_to_matrix_id[MAX_NODES];
	memset(node_to_matrix_id, -1, sizeof node_to_matrix_id);
	size_t const n = setup_matrix_ids(c->active_nodes, &node_to_matrix_id, matrix_id_to_node_out);
	if( n==0 ) {
		*G_out = NULL;
		*I_out = NULL;
		return 0;
	}
	/// allocate our matrices.
	*G_out = alloc_vec(&c->bistack, n*n);
	*I_out = alloc_vec(&c->bistack, n);
	rat const eps = rat_epsilon();
	for( uint8_t node=1; node < MAX_NODES; node++ ) {
		for( struct Comp *cmp = c->components[node]; cmp != NULL; cmp=cmp->next ) {
			uint8_t const a = node_to_matrix_id[node];
			switch( cmp->kind ) {
				case COMP_RESISTOR: {
					if( rat_lt(rat_abs(cmp->value), eps) ) {
						/// TODO: ideal wire
						continue;
					}
					rat const g = rat_div(rat_pos1(), cmp->value);
					if( node_is_ground(cmp->node) ) {
						(*G_out)[idx_2_to_1(a,a,n)] = rat_add((*G_out)[idx_2_to_1(a,a,n)], g);
					} else {
						uint8_t const b = node_to_matrix_id[cmp->node];
						(*G_out)[idx_2_to_1(a,a,n)] = rat_add((*G_out)[idx_2_to_1(a,a,n)], g);
						(*G_out)[idx_2_to_1(b,b,n)] = rat_add((*G_out)[idx_2_to_1(b,b,n)], g);
						(*G_out)[idx_2_to_1(a,b,n)] = rat_sub((*G_out)[idx_2_to_1(a,b,n)], g);
						(*G_out)[idx_2_to_1(b,a,n)] = rat_sub((*G_out)[idx_2_to_1(b,a,n)], g);
					}
					break;
				}
				case COMP_DC_CURRENT_SRC: {
					rat const amps = cmp->value;
					/// I A+ B- amps ==> A --I-> B
					/// 0 = vA/R + amps
					(*I_out)[a] = rat_sub((*I_out)[a], amps);
					if( !node_is_ground(cmp->node) ) {
						uint8_t const b = node_to_matrix_id[cmp->node];
						(*I_out)[b] = rat_add((*I_out)[b], amps);
					}
					break;
				}
				case COMP_DC_VOLTAGE_SRC: { /// TODO:
					bool const b_g = node_is_ground(cmp->node);
					rat const voltage = cmp->value;
					if( !node_is_ground(cmp->node) ) {
						/// supernode.
						uint8_t const b = node_to_matrix_id[cmp->node];
					} else {
						
					}
					break;
				}
				case COMP_VCCS: { /// TODO:
					/*
					uint8_t const cp = cmp->aux.ctrl_p;
					uint8_t const cm = cmp->aux.ctrl_m;
					rat   const gm = cmp->value;
					//*/
					break;
				}
				case COMP_VCVS: { /// TODO:
					/*
					uint8_t const cp = cmp->aux.ctrl_p;
					uint8_t const cm = cmp->aux.ctrl_m;
					bool const a_g = node_is_ground(a);
					bool const b_g = node_is_ground(b);
					if( a_g && b_g ) {
						/// degenerate
						break;
					}
					//*/
					break;
				}
				case COMP_CCVS: { /// TODO:
					/*
					uint8_t const cp = cmp->aux.ctrl_p;
					uint8_t const cm = cmp->aux.ctrl_m;
					/// if device is shorted to itself or control pins invalid, skip.
					if( a==b || cp==cm ) {
						break;
					}
					//*/
					break;
				}
				case COMP_CCCS: { /// TODO:
					rat const coef = cmp->value;
					uint8_t const cp = cmp->aux.dep.np;
					uint8_t const cm = cmp->aux.dep.nn;
					if( a==b || cp==cm ) {
						/// self-loop
						continue;
					}
					if( !node_is_ground(cmp->node) ) {
						uint8_t const b = node_to_matrix_id[cmp->node];
						
					}
					break;
				}
			}
		}
	}
	return n;
}

CIRCUIT_EXPORT NO_NULLS void circuit_solve_dc(struct Circuit *const c) {
	uint8_t matrix_id_to_node[MAX_NODES] = {0};
	rat *G = NULL;
	rat *V = NULL;
	size_t const n = circuit_build_dc(c, &matrix_id_to_node, &G, &V);
	if( n==0 || G==NULL || V==NULL ) {
		bistack_reset_front(&c->bistack);
		return;
	}
	print_matrix(n, G, V);
	/**
R 1 0 -2E3
R 1 3 2E3
R 2 0 4E3
R 2 3 4E3
R 3 0 1E3
I 1 2 4E-3
I 2 0 2E-3
	 */
	if( gaussian_rref(n, G, V) != RREFResultBadMatrix ) {
		for( size_t i=0; i < n; i++ ) {
			char num[48] = {0};
			printf("V%u = %s\n", matrix_id_to_node[i], rat_to_cstr(V[i], sizeof num, num));
		}
	}
	bistack_reset_front(&c->bistack);
}
#endif